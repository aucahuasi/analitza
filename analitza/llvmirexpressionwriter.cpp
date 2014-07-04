/*************************************************************************************
 *  Copyright (C) 2014 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 2                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "llvmirexpressionwriter.h"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "value.h"
#include "vector.h"
#include "operator.h"
#include "container.h"
#include <QStringList>
#include "list.h"
#include "variable.h"
#include "apply.h"
#include "analitzautils.h"
#include "matrix.h"

Q_DECLARE_METATYPE(llvm::Value*);

using namespace Analitza;

template <class T>
QStringList LLVMIRExpressionWriter::allValues(T it, const T& itEnd, AbstractExpressionVisitor* writer)
{
	QStringList elements;
	for(; it!=itEnd; ++it)
		elements += (*it)->accept(writer).toString();
	
	return elements;
}

QMap<Operator::OperatorType, QString> llvminitOperators()
{
	QMap<Operator::OperatorType, QString> ret;
	ret.insert(Operator::plus, "+");
	ret.insert(Operator::times, "*");
	ret.insert(Operator::divide, "/");
	ret.insert(Operator::eq, "=");
	ret.insert(Operator::neq, "!=");
	ret.insert(Operator::lt, "<");
	ret.insert(Operator::leq, "<=");
	ret.insert(Operator::gt, ">");
	ret.insert(Operator::geq, ">=");
	ret.insert(Operator::power, "^");
	ret.insert(Operator::minus, "-");
	return ret;
}

const QMap<Operator::OperatorType, QString> LLVMIRExpressionWriter::s_operators=llvminitOperators();

LLVMIRExpressionWriter::LLVMIRExpressionWriter(const Object* o)
{
    if (o)
        m_result=o->accept(this);
}

QVariant LLVMIRExpressionWriter::visit(const Ci* var)
{
	return var->name();
}

QVariant LLVMIRExpressionWriter::visit(const Operator* op)
{
	return op->name();
}

QVariant LLVMIRExpressionWriter::visit(const Vector* vec)
{
	return QString("vector { %1 }").arg(allValues<Vector::const_iterator>(vec->constBegin(), vec->constEnd(), this).join(QString(", ")));
}

QVariant LLVMIRExpressionWriter::visit(const Matrix* m)
{
	return QString("matrix { %1 }").arg(allValues(m->constBegin(), m->constEnd(), this).join(QString(", ")));
}

QVariant LLVMIRExpressionWriter::visit(const MatrixRow* mr)
{
	return QString("matrixrow { %1 }").arg(allValues(mr->constBegin(), mr->constEnd(), this).join(QString(", ")));
}

QVariant LLVMIRExpressionWriter::visit(const List* vec)
{
	if(!vec->isEmpty() && vec->at(0)->type()==Object::value && static_cast<Cn*>(vec->at(0))->format()==Cn::Char)
		return QString("\""+AnalitzaUtils::listToString(vec)+"\"");
	else
		return QString("list { %1 }").arg(allValues<List::const_iterator>(vec->constBegin(), vec->constEnd(), this).join(QString(", ")));
}

QVariant LLVMIRExpressionWriter::visit(const Cn* val)
{
	llvm::Value *ret = 0;
	
	switch(val->format()) {
		case Cn::Boolean: ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(bool)*8, val->intValue(), false)); break;
		case Cn::Char: ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(char)*8, val->intValue(), false)); break;
		case Cn::Integer: ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(int)*8, val->intValue(), true)); break;
		case Cn::Real: ret = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(val->value())); break;
		case Cn::Complex: {
			llvm::Type *rawtypes[2] = {llvm::Type::getDoubleTy(llvm::getGlobalContext()), llvm::Type::getDoubleTy(llvm::getGlobalContext())};
			llvm::ArrayRef<llvm::Type*> types = llvm::ArrayRef<llvm::Type*>(rawtypes, 2);
			llvm::StructType *complextype = llvm::StructType::get(llvm::getGlobalContext(), types, true);
			
			std::complex<double> complexval = val->complexValue();
			llvm::Constant *rawvalues[2] = {llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(complexval.real())),
				llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(complexval.imag()))};
			llvm::ArrayRef<llvm::Constant*> values = llvm::ArrayRef<llvm::Constant*>(rawvalues, 2);
			
			ret = llvm::ConstantStruct::get(complextype,values);
		}	break;
	}
	
	return QVariant::fromValue((llvm::Value*)ret); //TODO better casting using LLVM API
}

int LLVMIRExpressionWriter::weight(const Operator* op, int size, int pos)
{
	switch(op->operatorType()) {
		case Operator::lt:
		case Operator::gt:
		case Operator::eq:
		case Operator::neq:
		case Operator::leq:
		case Operator::geq:
			return 1;
		case Operator::plus:
			return 2;
		case Operator::minus:
			return size==1 ? 8 : 3;
		case Operator::times:
			return 4;
		case Operator::divide:
			return 5 + (pos>0 ? 0 : 1);
		case Operator::_and:
		case Operator::_or:
		case Operator::_xor:
			return 6;
		case Operator::power:
			return 7 + (pos>0 ? 0 : 1);
		default:
			return 1000;
	}
}

QVariant LLVMIRExpressionWriter::visit(const Analitza::Apply* a)
{
	Operator op=a->firstOperator();
	QStringList ret;
	QString toret;
	QString bounds;
	QStringList bvars=a->bvarStrings();
	
	if(a->ulimit() || a->dlimit()) {
		bounds += '=';
		if(a->dlimit())
			bounds += a->dlimit()->accept(this).toString();
		bounds += "..";
		if(a->ulimit())
			bounds += a->ulimit()->accept(this).toString();
	}
	else if(a->domain())
		bounds += '@'+a->domain()->accept(this).toString();
	
	int i = 0;
	foreach(Object* o, a->m_params) {
		Object::ObjectType type=o->type();
		switch(type) {
			case Object::oper:
				Q_ASSERT(false);
				break;
			case Object::variable:
				ret << static_cast<const Ci*>(o)->accept(this).toString();
				break;
			case Object::apply: {
				const Apply *c = (const Apply*) o;
				QString s = c->accept(this).toString();
				if(s_operators.contains(op.operatorType()) && !c->isUnary()) {
					Operator child_op = c->firstOperator();
					
					if(child_op.operatorType() && weight(&op, c->countValues(), -1)>weight(&child_op, c->countValues(), i))
						s=QString("(%1)").arg(s);
				}
				ret << s;
			}	break;
			default:
				ret << o->accept(this).toString();
				break;
		}
		++i;
	}
	
	bool func=op.operatorType()==Operator::function;
	if(func) {
		QString n = ret.takeFirst();
		if(a->m_params.first()->type()!=Object::variable)
			n='('+n+')';
		
		toret += QString("%1(%2)").arg(n).arg(ret.join(", "));
	} else if(op.operatorType()==Operator::selector) {
		if(a->m_params.last()->isApply()) {
			const Apply* a1=static_cast<const Apply*>(a->m_params.last());
			if(s_operators.contains(a1->firstOperator().operatorType()))
				ret.last()='('+ret.last()+')';
		}
		
		toret += QString("%1[%2]").arg(ret.last()).arg(ret.first());
	} else if(ret.count()>1 && s_operators.contains(op.operatorType())) {
		toret += ret.join(s_operators.value(op.operatorType()));
	} else if(ret.count()==1 && op.operatorType()==Operator::minus)
		toret += '-'+ret[0];
	else {
		QString bounding;
		if(!bounds.isEmpty() || !bvars.isEmpty()) {
			if(bvars.count()!=1) bounding +='(';
			bounding += bvars.join(", ");
			if(bvars.count()!=1) bounding +=')';
			
			bounding = ':'+bounding +bounds;
		}
			
		toret += QString("%1(%2%3)").arg(op.accept(this).toString()).arg(ret.join(", ")).arg(bounding);
	}
	
	return toret;
}

QVariant LLVMIRExpressionWriter::visit(const Container* var)
{
	QStringList ret = allValues(var->constBegin(), var->constEnd(), this);
	
	QString toret;
	switch(var->containerType()) {
		case Container::declare:
			toret += ret.join(":=");
			break;
		case Container::lambda: {
			QString last=ret.takeLast();
			QStringList bvars = var->bvarStrings();
			if(bvars.count()!=1) toret +='(';
			toret += bvars.join(", ");
			if(bvars.count()!=1) toret +=')';
			toret += "->" + last;
		}	break;
		case Container::math:
			toret += ret.join("; ");
			break;
		case Container::uplimit: //x->(n1..n2) is put at the same time
		case Container::downlimit:
			break;
		case Container::bvar:
			if(ret.count()>1) toret += '(';
			toret += ret.join(", ");
			if(ret.count()>1) toret += ')';
			break;
		case Container::piece:
			toret += ret[1]+" ? "+ret[0];
			break;
		case Container::otherwise:
			toret += "? "+ret[0];
			break;
		default:
			toret += var->tagName()+" { "+ret.join(", ")+" }";
			break;
	}
	return toret;
}

QVariant LLVMIRExpressionWriter::visit(const CustomObject*)
{
	return "CustomObject";
}

QVariant LLVMIRExpressionWriter::visit(const None* )
{
	return QString();
}
