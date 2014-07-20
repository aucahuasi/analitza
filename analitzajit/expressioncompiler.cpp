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

#include "expressioncompiler.h"

#include <llvm/Analysis/Verifier.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "operationscompiler.h"
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
Q_DECLARE_METATYPE(llvm::Function*);

//TODO better names
static llvm::IRBuilder<> buildr(llvm::getGlobalContext());
static std::map<QString, llvm::Value*> NamedValues;

using namespace Analitza;

ExpressionCompiler::ExpressionCompiler(const Object* o, llvm::Module *mod, const QMap< QString, llvm::Type* >& bvartypes, Variables* v)
	: m_bvartypes(bvartypes)
	, m_mod(mod)
{
    if (o)
        m_result=o->accept(this);
}

QVariant ExpressionCompiler::visit(const Analitza::Ci* var)
{
	//TODO chack in variables too ... since we are playing by efault with stack vars
// 	NamedValues[var->name()]->dump();
	return QVariant::fromValue<llvm::Value*>(NamedValues[var->name()]);
}

QVariant ExpressionCompiler::visit(const Analitza::Operator* op)
{
// 	switch(op->operatorType()) {
// 		case Operator::lt:
// 		case Operator::gt:
// 		case Operator::eq:
// 		case Operator::neq:
// 		case Operator::leq:
// 		case Operator::geq:
// 			return 1;
// 		case Operator::plus:
// 			return 2;
// 		case Operator::minus:
// 			return size==1 ? 8 : 3;
// 		case Operator::times:
// 			return 4;
// 		case Operator::divide:
// 			return 5 + (pos>0 ? 0 : 1);
// 		case Operator::_and:
// 		case Operator::_or:
// 		case Operator::_xor:
// 			return 6;
// 		case Operator::power:
// 			return 7 + (pos>0 ? 0 : 1);
// 		default:
// 			return 1000;
// 	}
	return op->name(); //TODO
}

QVariant ExpressionCompiler::visit(const Analitza::Vector* vec)
{
	return QString();
}

QVariant ExpressionCompiler::visit(const Analitza::Matrix* m)
{
	return QString();
}

QVariant ExpressionCompiler::visit(const Analitza::MatrixRow* mr)
{
	return QString();
}

QVariant ExpressionCompiler::visit(const Analitza::List* vec)
{
	return QString();
}

QVariant ExpressionCompiler::visit(const Analitza::Cn* val)
{
	llvm::Value *ret = 0;
	
	switch(val->format()) {
		//TODO
// 		case Analitza::Cn::Boolean: // ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(bool)*8, val->intValue(), false)); break;
// 		case Analitza::Cn::Char: //ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(char)*8, val->intValue(), false)); break;
		case Analitza::Cn::Integer: //ret = llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(sizeof(int)*8, val->intValue(), true)); break;
		case Analitza::Cn::Real: ret = llvm::ConstantFP::get(llvm::getGlobalContext(), llvm::APFloat(val->value())); break;
		case Analitza::Cn::Complex: {
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

QVariant ExpressionCompiler::visit(const Analitza::Apply* c)
{
	llvm::Value *ret = 0;
	
	const Operator::OperatorType op = c->firstOperator().operatorType();
	QString error; //TODO error management
	
	switch(c->countValues()) {
		case 1: {
			Object *val = c->at(0);
// 			qDebug() << "DUMPPPP " << apply->firstOperator().toString();
			llvm::Value *valv =val->accept(this).value<llvm::Value*>();
// 			valv->dump();
// 			valv->getType()->dump();
// 			qDebug() << "ENNNDDDD";
			
			//TODO we need to choice based on llvm::Valye::type instead of Analitza type 
			//this we could avoid use analitza::type1 for a llvm::value that is not (avoid inconsistence)
			//currently we¿ll cast all types to double (analitza value)
			//ret = OperationsCompiler::compileUnaryOperation(m_mod, buildr.GetInsertBlock(), op, val->type(), valv, error);
			ret = OperationsCompiler::compileUnaryOperation(m_mod, buildr.GetInsertBlock(), op, Object::value, valv, error);
		}	break;
		case 2: {
			Object *val1 = c->at(0);
// 			qDebug() << "obj1 " << val1->toString();
			llvm::Value *valv1 =val1->accept(this).value<llvm::Value*>();
// 			valv1->getType()->dump();
			Object *val2 = c->at(1);
// 			qDebug() << "obj2 " << val2->toString();
// 			qDebug() << "DUMPPPP " << apply->firstOperator().toString();
			llvm::Value *valv2 =val2->accept(this).value<llvm::Value*>();
// 			valv2->getType()->dump();
// 			valv->dump();
// 			valv->getType()->dump();
// 			qDebug() << "ENNNDDDD";
			
			//TODO we need to choice based on llvm::Valye::type instead of Analitza type 
			//this we could avoid use analitza::type1 for a llvm::value that is not (avoid inconsistence)
			//currently we¿ll cast all types to double (analitza value)
			//ret = OperationsCompiler::compileBinaryOperation(m_mod, buildr.GetInsertBlock(), op, val1->type(), val2->type(), valv1, valv2, error);
			ret = OperationsCompiler::compileBinaryOperation(m_mod, buildr.GetInsertBlock(), op, Object::value, Object::value, valv1, valv2, error);
		}	break;
	}
	
	return QVariant::fromValue((llvm::Value*)ret); //TODO better casting using LLVM API
}

QVariant ExpressionCompiler::visit(const Analitza::Container* c)
{
	llvm::Value *ret = 0;
	switch(c->containerType()) {
		case Analitza::Container::piecewise: {
// 			ExpressionType type=commonType(c->m_params);
// 			
// 			if(type.isError()) {
// 				addError(QCoreApplication::tr("Could not determine the type for piecewise"));
// 				current=type;
// 			} else {
// 				QList<ExpressionType> alts=type.type()==ExpressionType::Many ? type.alternatives() : QList<ExpressionType>() << type, alts2;
// 				
// 				for(QList< ExpressionType >::iterator it=alts.begin(), itEnd=alts.end(); it!=itEnd; ++it) {
// 					QMap<QString, ExpressionType> assumptions=typeIs(c->constBegin(), c->constEnd(), *it);
// 					
// 	// 				QMap<int, ExpressionType> stars;
// 	// 				bool b=ExpressionType::matchAssumptions(&stars, assumptions, type.assumptions());
// 	// 				Q_ASSERT(b);
// 	// 				type=type.starsToType(stars);
// 					
// // 					qDebug() << "suuuuu\n\t" << it->assumptions() << "\n\t" << assumptions;
// 					QMap<int, ExpressionType> stars;
// 					bool b=ExpressionType::matchAssumptions(&stars, it->assumptions(), assumptions);
// 					b&=ExpressionType::assumptionsMerge(it->assumptions(), assumptions);
// // 					qDebug() << "fefefe" << b << it->assumptions();
// 					
// 					if(b) {
// 						alts2 += *it;
// // 						qDebug() << "!!!" << it->assumptions() << b;
// 					}
// 				}
// 				current=ExpressionType(ExpressionType::Many, alts2);
// 			}
		}	break;
		case Analitza::Container::piece: {
// 			QMap<QString, ExpressionType> assumptions=typeIs(c->m_params.last(), ExpressionType(ExpressionType::Bool)); //condition check
// 			c->m_params.first()->accept(this); //we return the body
// 			QList<ExpressionType> alts=current.type()==ExpressionType::Many ? current.alternatives() : QList<ExpressionType>() << current, rets;
// 			foreach(const ExpressionType& t, alts) {
// 				QMap<int, ExpressionType> stars;
// 				ExpressionType toadd=t;
// 				bool b=ExpressionType::assumptionsMerge(toadd.assumptions(), assumptions);
// 				
// 				b&=ExpressionType::matchAssumptions(&stars, assumptions, t.assumptions());
// 				
// 				if(b) {
// 					toadd=toadd.starsToType(stars);
// 					
// 					rets += toadd;
// 				}
// 			}
// 			current=ExpressionType(ExpressionType::Many, rets);
		}	break;
		case Analitza::Container::declare:{
// 			Q_ASSERT(c->m_params.first()->type()==Object::variable);
// 			Ci* var = static_cast<Ci*>(c->m_params.first());
// 			
// 			m_calculating.append(var->name());
// 			c->m_params.last()->accept(this);
// 			m_calculating.removeLast();
// 			
// 			current=tellTypeIdentity(var->name(), current);
		}	break;
		case Analitza::Container::lambda: {
			const QStringList bvars = c->bvarStrings();
			
			std::vector<llvm::Type*> tparams = m_bvartypes.values().toVector().toStdVector();
			
// 			tparams.at(0)->dump();
// 			tparams.at(1)->dump();
			
			//TODO we need the return type
			llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), tparams, false);
			
			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "mylam", m_mod);//TODO count how many lambdas we have generated i.e mylam0, mylam1 ...
			
			//TODO better code
			int Idx = 0;
			for (llvm::Function::arg_iterator AI = F->arg_begin(); Idx != bvars.size(); ++AI, ++Idx) {
				AI->setName(bvars[Idx].toStdString());
// 				AI->dump();
				NamedValues[bvars[Idx]] = AI;
			}
			
			llvm::BasicBlock *BB = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", F);
			buildr.SetInsertPoint(BB);
			buildr.CreateRet(c->m_params.last()->accept(this).value<llvm::Value*>());
			
// 			QVariant al = QVariant::fromValue<llvm::Value*>(F);
			
// 			BB->dump();
			llvm::verifyFunction(*F);
			
			ret = F;
		}	break;
		case Analitza::Container::otherwise:
		case Analitza::Container::math:
		case Analitza::Container::none:
		case Analitza::Container::downlimit:
		case Analitza::Container::uplimit:
		case Analitza::Container::bvar:
		case Analitza::Container::domainofapplication: {
			Q_ASSERT(c->constBegin()+1==c->constEnd());
			ret = (*c->constBegin())->accept(this).value<llvm::Value*>();
		}	break;
	}
	
// 	if(current.type()==ExpressionType::Many) {
// 		if(current.alternatives().isEmpty())
// 			current=ExpressionType(ExpressionType::Error);
// 		else if(current.alternatives().count()==1)
// 			current=current.alternatives().first();
// 	}
	
	return QVariant::fromValue((llvm::Value*)ret); //TODO better casting using LLVM API
}

QVariant ExpressionCompiler::visit(const Analitza::CustomObject*)
{
	return "CustomObject";
}

QVariant ExpressionCompiler::visit(const Analitza::None* )
{
	return QString();
}
