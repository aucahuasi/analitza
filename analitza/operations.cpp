/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "operations.h"

#include <math.h>

#include <cmath>
#include <complex>
#include <QCoreApplication>

#include "value.h"
#include "vector.h"
#include "expression.h"
#include "list.h"
#include "expressiontypechecker.h"
#include "customobject.h"

#include "matrix.h"
#include "container.h"

using namespace std;
using namespace Analitza;

Cn* Operations::reduceRealReal(enum Operator::OperatorType op, Cn *oper, const Cn *oper1, QString** correct)
{
	int residu;
	const double a=oper->value(), b=oper1->value();
	
	switch(op) {
		case Operator::plus:
			oper->setValue(a+b);
			break;
		case Operator::times:
			oper->setValue(a*b);
			break;
		case Operator::divide:
			if(Q_LIKELY(b!=0.))
				oper->setValue(a / b);
			else
				*correct=new QString(QCoreApplication::tr("Cannot divide by 0."));
			break;
		case Operator::minus:
			oper->setValue(a - b);
			break;
		case Operator::power:
			oper->setValue( b==2 ? a*a
					: b<1 && b>-1 && a<0 ? pow(complex<double>(a), complex<double>(b)).real()
								 : pow(a, b));
			break;
		case Operator::rem:
			if(Q_LIKELY(floor(b)!=0.))
				oper->setValue(int(remainder(a, b)));
			else
				*correct=new QString(QCoreApplication::tr("Cannot calculate the remainder on 0."));
			break;
		case Operator::quotient:
			oper->setValue(int(floor(a / b)));
			break;
		case Operator::factorof: {
			int fb = int(floor(b));
			if(Q_LIKELY(fb!=0))
				oper->setValue((int(floor(a)) % fb)==0);
			else
				*correct=new QString(QCoreApplication::tr("Cannot calculate the factor on 0."));
		}	break;
		case Operator::min:
			oper->setValue(a < b? a : b);
			break;
		case Operator::max:
			oper->setValue(a > b? a : b);
			break;
		case Operator::gt:
			oper->setValue(a > b);
			break;
		case Operator::lt:
			oper->setValue(a < b);
			break;
		case Operator::eq:
			oper->setValue(a == b);
			break;
		case Operator::approx:
			oper->setValue(fabs(a-b)<0.001);
			break;
		case Operator::neq:
			oper->setValue(a != b);
			break;
		case Operator::geq:
			oper->setValue(a >= b);
			break;
		case Operator::leq:
			oper->setValue(a <= b);
			break;
		case Operator::_and:
			oper->setValue(a && b);
			break;
		case Operator::_or:
			oper->setValue(a || b);
			break;
		case Operator::_xor:
			oper->setValue((a || b) && !(a&&b));
			break;
		case Operator::implies:
			oper->setValue(a || !b);
			break;
		case Operator::gcd:  {
			//code by michael cane aka kiko :)
			int ia=floor(a), ib=floor(b);
			while (ib > 0) {
				residu = ia % ib;
				ia = ib;
				ib = residu;
			}
			oper->setValue(ia);
		}	break;
		case Operator::lcm:
			//code by michael cane aka kiko :)
			if(Q_UNLIKELY(floor(a)==0. || floor(b)==0.))
				*correct=new QString(QCoreApplication::tr("Cannot calculate the lcm of 0."));
			else {
				int ia=floor(a), ib=floor(b);
				int ic=ia*ib;
				while (ib > 0) {
					residu = ia % ib;
					ia = ib;
					ib = residu;
				}
				ia=ic/ia;
				oper->setValue(ia);
			}
			break;
		case Operator::root:
			oper->setValue(			  b==2.0 ? sqrt(a)
					  : (b>1 || b<-1) && a<0 ? pow(complex<double>(a), complex<double>(1./b)).real()
											 : pow(a, 1.0/b));
			break;
		default:
			break;
	}
	return oper;
}

Cn* Operations::reduceUnaryReal(enum Operator::OperatorType op, Cn *val, QString** correct)
{
	const double a=val->value();
	
	switch(op) {
		case Operator::minus:
			val->setValue(-a);
			break;
		case Operator::factorial: {
			//Use gamma from math.h?
			uint res=1;
			for(int i=a; i>1.; i--) {
				res*=floor(i);
			}
			val->setValue(res);
		}	break;
		case Operator::sin:
			val->setValue(sin(a));
			break;
		case Operator::cos:
			val->setValue(cos(a));
			break;
		case Operator::tan:
			val->setValue(tan(a));
			break;
		case Operator::sec:
			val->setValue(1./cos(a));
			break;
		case Operator::csc:
			val->setValue(1./sin(a));
			break;
		case Operator::cot:
			val->setValue(1./tan(a));
			break;
		case Operator::sinh:
			val->setValue(sinh(a));
			break;
		case Operator::cosh:
			val->setValue(cosh(a));
			break;
		case Operator::tanh:
			val->setValue(tanh(a));
			break;
		case Operator::sech:
			val->setValue(1.0/cosh(a));
			break;
		case Operator::csch:
			val->setValue(1.0/sinh(a));
			break;
		case Operator::coth:
			val->setValue(cosh(a)/sinh(a));
			break;
		case Operator::arcsin:
			val->setValue(asin(a));
			break;
		case Operator::arccos:
			val->setValue(acos(a));
			break;
		case Operator::arctan:
			val->setValue(atan(a));
			break;
		case Operator::arccot:
			val->setValue(log(a+pow(a*a+1., 0.5)));
			break;
		case Operator::arcsinh: //see http://en.wikipedia.org/wiki/Inverse_hyperbolic_function
			val->setValue(asinh(a));
			break;
		case Operator::arccosh:
			val->setValue(acosh(a));
			break;
		case Operator::arccsc:
			val->setValue(1/asin(a));
			break;
		case Operator::arccsch:
			val->setValue(log(1/a+sqrt(1/(a*a)+1)));
			break;
		case Operator::arcsec:
			val->setValue(1/(acos(a)));
			break;
		case Operator::arcsech:
			val->setValue(log(1/a+sqrt(1/a+1)*sqrt(1/a-1)));
			break;
		case Operator::arctanh:
			val->setValue(atanh(a));
			break;
		case Operator::exp:
			val->setValue(exp(a));
			break;
		case Operator::ln:
			val->setValue(log(a));
			break;
		case Operator::log:
			val->setValue(log10(a));
			break;
		case Operator::abs:
			val->setValue(a>=0. ? a : -a);
			break;
		//case Object::conjugate:
		//case Object::arg:
		//case Object::real:
		//case Object::imaginary:
		case Operator::floor:
			val->setValue(floor(a));
			break;
		case Operator::ceiling:
			val->setValue(ceil(a));
			break;
		case Operator::_not:
			val->setValue(!a);
			break;
		default:
			*correct=new QString(QCoreApplication::tr("Could not calculate a value %1").arg(Operator(op).toString()));
			break;
	}
	return val;
}

Object* Operations::reduceContainerReal(Operator::OperatorType op, Container* cntr, Cn* oper, QString** correct)
{
	return errorCase("error en escalar con none", cntr, correct);
}

Object* Operations::reduceRealContainer(Operator::OperatorType op, Cn* oper, Container* cntr, QString** correct)
{
	return errorCase("error en escalar con none", cntr, correct);
}

Object * Operations::reduceRealVector(Operator::OperatorType op, Cn * oper, Vector * v1, QString** correct)
{
	switch(op) {
		case Operator::selector: {
			int select=oper->intValue();
			delete oper;
			Object* ret=0;
			if(select<1 || (select-1) >= v1->size()) {
				*correct=new QString(QCoreApplication::tr("Invalid index for a container"));
				ret=new Cn(0.);
			} else {
				ret=v1->at(select-1)->copy();
			}
			return ret;
		}	break;
		default: {
			Vector *ret = v1->copy();
			for(Vector::iterator it=ret->begin(); it!=ret->end(); ++it)
			{
				*it=reduce(op, new Cn(*oper), *it, correct);
			}
			
			delete oper;
			return ret;
		}
	}
}

Object * Operations::reduceVectorReal(Operator::OperatorType op, Vector * v1, Cn * oper, QString** correct)
{
	for(Vector::iterator it=v1->begin(); it!=v1->end(); ++it)
	{
		*it=reduce(op, *it, new Cn(*oper), correct);
	}
	return v1;
}

Object * Operations::reduceVectorVector(Operator::OperatorType op, Vector * v1, Vector * v2, QString** correct)
{
	if(v1->size()!=v2->size()) { //FIXME: unneeded? ... aucahuasi: I think is needed ...
		*correct=new QString(QCoreApplication::tr("Cannot operate on different sized vectors."));
		return new Container(Container::none); // uniform error value ... empty string
	}
	
	if(op==Operator::scalarproduct)
		op=Operator::times;
	Vector::iterator it2=v2->begin();
	for(Vector::iterator it1=v1->begin(); it1!=v1->end(); ++it1, ++it2)
	{
		*it1=reduce(op, *it1, *it2, correct);
	}
	return v1;
}

Cn* Operations::reduceUnaryVector(Operator::OperatorType op, Vector * c, QString** correct)
{
	Cn *ret=0;
	switch(op) {
		case Operator::card:
			ret=new Cn(c->size());
			break;
		default:
			//Should be dealt by typechecker. not necessary
			*correct=new QString(QCoreApplication::tr("Could not calculate a vector's %1").arg(Operator(op).toString()));
			ret=new Cn(0.);
			break;
	}
	delete c;
	return ret;
}

Object* Operations::reduceListList(Operator::OperatorType op, List* l1, List* l2, QString** correct)
{
	Object* ret=0;
	switch(op) {
		case Operator::_union: {
			List::iterator itEnd=l2->end();
			for(List::iterator it=l2->begin(); it!=itEnd; ++it) {
				l1->appendBranch((*it)->copy());
			}
			
			ret=l1;
		}	break;
		default:
			//Should be dealt by typechecker. not necessary
			*correct=new QString(QCoreApplication::tr("Could not calculate a list's %1").arg(Operator(op).toString()));
			delete l1;
			ret=new Cn(0.);
			break;
	}
	return ret;
}

Cn* Operations::reduceUnaryList(Operator::OperatorType op, List* l, QString** correct)
{
	Cn *ret=0;
	switch(op) {
		case Operator::card:
			ret=new Cn(l->size());
			break;
		default:
			*correct=new QString(QCoreApplication::tr("Could not calculate a list's %1").arg(Operator(op).toString()));
			ret=new Cn(0.);
			break;
	}
	delete l;
	return ret;
}

Object* Operations::reduceRealList(Operator::OperatorType op, Cn* oper, List* v1, QString** correct)
{
	switch(op) {
		case Operator::selector: {
			int select=oper->intValue();
			Object* ret=0;
			if(select<1 || (select-1) >= v1->size()) {
				*correct=new QString(QCoreApplication::tr("Invalid index for a container"));
				ret=new Cn(0.);
			} else {
				ret=v1->at(select-1);
				v1->setAt(select-1, 0);
			}
			delete oper;
			return ret;
		}	break;
		default:
			break;
	}
	return 0;
}

Object* Operations::reduceCustomCustom(Operator::OperatorType op, CustomObject* v1, CustomObject* v2, QString** )
{
	switch(op) {
		case Operator::neq:
			return new Cn(v1->value()!=v2->value());
		case Operator::eq:
			return new Cn(v1->value()==v2->value());
		default:
			break;
	}
	
	Q_ASSERT(false && "not implemented, please report");
	return 0;
}

Object* Operations::reduceMatrixMatrix(Operator::OperatorType op, Matrix* m1, Matrix* m2, QString** correct)
{
	Object* ret = 0;
	switch(op) {
		//TODO see if we can use here and or xor for matrix too
		case Operator::plus:
		case Operator::minus: {
			if(m1->rowCount() == m2->rowCount() && m1->columnCount() == m2->columnCount()) {
				Matrix::iterator it2=m2->begin();
				for(Matrix::iterator it1=m1->begin(); it1!=m1->end(); ++it1, ++it2)
				{
					*it1 = static_cast<MatrixRow*>(reduceVectorVector(op, *it1, *it2, correct));
				}
				ret = m1;
			} else {
				*correct=new QString(QCoreApplication::tr("Addition of two matrices is allowed provided that both matrices have the same number of rows and the same number of columns"));
				ret=new Matrix();
			}
		}	break;
		case Operator::times: {
			//TODO feature 2
		}	break;
		default:
			break;
	}

	return ret;
}

Object* Operations::reduceRealMatrix(Operator::OperatorType op, Cn* v, Matrix* m1, QString** correct)
{
	Object* ret = 0;
// 	qDebug() << "que op: " << op;
	switch(op) {
		case Operator::selector: {
			int select=v->intValue();
			if(select<1 || (select-1) >= m1->rowCount()) {
				*correct=new QString(QCoreApplication::tr("Invalid index for a container"));
				ret=new Vector(1);
			} else {
				MatrixRow* row = static_cast<MatrixRow*>(m1->rows()[select-1]);
				Vector* nv = new Vector(row->size());
				for(Vector::iterator it=row->begin(); it!=row->end(); ++it) {
					nv->appendBranch((*it));
					*it = 0;
				}
				ret = nv;
			}
			delete v;
		}	break;
		case Operator::times: {
			Matrix *nm = new Matrix();
// 			aqui marca el error solo 
			for(Matrix::iterator it=m1->begin(); it!=m1->end(); ++it)
				nm->appendBranch(static_cast<MatrixRow*>(reduceRealVector(op, static_cast<Cn*>(v->copy()), static_cast<MatrixRow*>(*it), correct)));
			ret = nm;
		}	break;
		default:
			break;
	}
	return ret;
}

Object* Operations::reduceUnaryMatrix(Operator::OperatorType op, Matrix* m, QString** )
{
	Object* ret = 0;
	switch(op) {
		case Operator::transpose: {
			int sizeA = m->rowCount(), sizeB = static_cast<MatrixRow*>(*m->constBegin())->size();
			Matrix* mret = new Matrix;
			for(int i=0; i<sizeB; ++i) {
				MatrixRow* row = new MatrixRow(sizeA);
				for(int j=0; j<sizeA; ++j) {
					row->appendBranch(m->at(j, i)->copy());
				}
				mret->appendBranch(row);
			}
			ret = mret;
		}	break;
		default:
			break;
	}
	return ret;
}

Object* Operations::reduceMatrixContainer(Operator::OperatorType op, Matrix* m, Container* cntr, QString** correct)
{
	return errorCase("erroror en mat oprations", cntr, correct);
}

Object* Operations::reduceContainerMatrix(Operator::OperatorType op, Container* cntr, Matrix* m, QString** correct)
{
	return reduceMatrixContainer(op, m, cntr, correct);
}

ExpressionType TypeTriplet(const ExpressionType& a,const ExpressionType& b,const ExpressionType& c) { return ExpressionType(ExpressionType::Lambda).addParameter(a).addParameter(b).addParameter(c); }

//TODO: test that there's one output per input
QList<ExpressionType> Operations::infer(Operator::OperatorType op)
{
	QList<ExpressionType> ret;
	
	switch(op) {
		case Operator::plus:
		case Operator::minus:
			ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
			ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
			ret << TypeTriplet(ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
							   ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
							   ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
			break;
		case Operator::divide:
			ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
			ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Value),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
			ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
			break;
		case Operator::times:
			ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
			ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Value),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
			ret << TypeTriplet(ExpressionType(ExpressionType::Value),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
			ret << TypeTriplet(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
			ret << TypeTriplet(ExpressionType(ExpressionType::Value),
							   ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
							   ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1));
			break;
		case Operator::eq:
		case Operator::neq:
			ret << TypeTriplet(ExpressionType(ExpressionType::Any, 1),
							   ExpressionType(ExpressionType::Any, 1),
							   ExpressionType(ExpressionType::Bool));
			break;
		case Operator::scalarproduct:
			ret << TypeTriplet(
								ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
								ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1),
								ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1));
			break;
		case Operator::power:
		case Operator::rem:
		case Operator::quotient:
		case Operator::factorof:
		case Operator::min:
		case Operator::max:
		case Operator::gcd:
		case Operator::lcm:
		case Operator::root:
			ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
			break;
		case Operator::gt:
		case Operator::lt:
		case Operator::approx:
		case Operator::geq:
		case Operator::leq:
			ret << TypeTriplet(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Bool));
			break;
		case Operator::_and:
		case Operator::_or:
		case Operator::_xor:
		case Operator::implies:
			ret << TypeTriplet(ExpressionType(ExpressionType::Bool), ExpressionType(ExpressionType::Bool), ExpressionType(ExpressionType::Bool));
			break;
		case Operator::selector:
			ret << TypeTriplet(ExpressionType(ExpressionType::Value),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1),
							   ExpressionType(ExpressionType::Any, 1));
			ret << TypeTriplet(ExpressionType(ExpressionType::Value),
							   ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
							   ExpressionType(ExpressionType::Any, 1));
			ret << TypeTriplet(ExpressionType(ExpressionType::Value),
							   ExpressionType(ExpressionType::Matrix, ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), -2),
							   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1));
			break;
		case Operator::_union:
			ret << TypeTriplet(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
							   ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
							   ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)));
			break;
		case Operator::map:
			ret << TypeTriplet(ExpressionType(ExpressionType::Lambda).addParameter(ExpressionType(ExpressionType::Any, 1)).addParameter(ExpressionType(ExpressionType::Any, 2)),
							   ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
							   ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 2)));
			break;
		case Operator::filter:
			ret << TypeTriplet(ExpressionType(ExpressionType::Lambda).addParameter(ExpressionType(ExpressionType::Any, 1)).addParameter(ExpressionType::Bool),
							   ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)),
							   ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)));
			break;
		default:
			break;
	}
	return ret;
}

ExpressionType TypePair(const ExpressionType& a, const ExpressionType& b) { return ExpressionType(ExpressionType::Lambda).addParameter(a).addParameter(b); }

QList<ExpressionType> Operations::inferUnary(Operator::OperatorType op)
{
	QList<ExpressionType> ret;
	switch(op) {
		case Operator::minus:
			ret << TypePair(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
			ret << TypePair(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1));
			break;
		case Operator::card:
			ret << TypePair(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), ExpressionType(ExpressionType::Value));
			ret << TypePair(ExpressionType(ExpressionType::List, ExpressionType(ExpressionType::Any, 1)), ExpressionType(ExpressionType::Value));
			break;
		case Operator::sum:
		case Operator::product:
			ret << TypePair(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
			ret << TypePair(ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1), ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Any, 1), -1));
			break;
		case Operator::transpose:
			ret << TypePair(ExpressionType(ExpressionType::Matrix,
										   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -2), -1),
							ExpressionType(ExpressionType::Matrix,
										   ExpressionType(ExpressionType::Vector, ExpressionType(ExpressionType::Value), -1), -2));
			break;
		case Operator::factorial:
		case Operator::sin:
		case Operator::cos:
		case Operator::tan:
		case Operator::sec:
		case Operator::csc:
		case Operator::cot:
		case Operator::sinh:
		case Operator::cosh:
		case Operator::tanh:
		case Operator::sech:
		case Operator::csch:
		case Operator::coth:
		case Operator::arcsin:
		case Operator::arccos:
		case Operator::arctan:
		case Operator::arccot:
		case Operator::arcsinh:
		case Operator::arccosh:
		case Operator::arccsc:
		case Operator::arccsch:
		case Operator::arcsec:
		case Operator::arcsech:
		case Operator::arctanh:
		case Operator::exp:
		case Operator::ln:
		case Operator::log:
		case Operator::abs:
		//case Object::conjugate:
		//case Object::arg:
		//case Object::real:
		//case Object::imaginary:
		case Operator::floor:
		case Operator::ceiling:
			ret << TypePair(ExpressionType(ExpressionType::Value), ExpressionType(ExpressionType::Value));
			break;
		case Operator::_not:
		case Operator::exists:
		case Operator::forall:
			ret << TypePair(ExpressionType(ExpressionType::Bool), ExpressionType(ExpressionType::Bool));
			break;
		default:
			break;
	}
	return ret;
}

Operations::BinaryOp Operations::opsBinary[Object::custom+1][Object::custom+1] = {
	{0,0,0,0,0,0,0,0,0,0,0},
	{0, (Operations::BinaryOp) reduceRealReal, 0, (Operations::BinaryOp) reduceRealVector, (Operations::BinaryOp) reduceRealList,0,0,(Operations::BinaryOp) reduceRealContainer,(Operations::BinaryOp) reduceRealMatrix,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0, (Operations::BinaryOp) reduceVectorReal, 0, (Operations::BinaryOp) reduceVectorVector, 0,0,0,0,0,0},
	{0, 0, 0,0, (Operations::BinaryOp) reduceListList, 0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,(Operations::BinaryOp) reduceContainerReal,0,0,0,0,0,0,(Operations::BinaryOp) reduceContainerMatrix,0,0},
	{0,0,0,0,0,0,0,(Operations::BinaryOp) reduceMatrixContainer, (Operations::BinaryOp) reduceMatrixMatrix,0,0},
	{0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,(Operations::BinaryOp) reduceCustomCustom}
};

Object * Operations::reduce(Operator::OperatorType op, Object * val1, Object * val2, QString** correct)
{
	Object::ObjectType t1=val1->type(), t2=val2->type();
	
	qDebug() << "recude OPTS " << t1 << t2;

	
	BinaryOp f=opsBinary[t1][t2];
	Q_ASSERT(f);
	return f(op, val1, val2, correct);
}

Operations::UnaryOp Operations::opsUnary[] = {
	0,
	(Operations::UnaryOp) Operations::reduceUnaryReal,
	0, //variable
	(Operations::UnaryOp) Operations::reduceUnaryVector,
	(Operations::UnaryOp) Operations::reduceUnaryList,
	0, //apply
	0, //oper
	0, //container
	(Operations::UnaryOp) Operations::reduceUnaryMatrix
};

Object * Operations::reduceUnary(Operator::OperatorType op, Object * val, QString** correct)
{
	UnaryOp f=opsUnary[val->type()];
	
	Q_ASSERT(f && "using reduceUnary in a wrong way");
	return f(op, val, correct);
}

Object* Operations::errorCase(const QString& error, Container* errorcntr, QString** correct)
{
	Q_ASSERT(errorcntr->containerType() == Container::none); // this method is to handle error cases in matrix operations
	
	*correct = new QString(error);
	return new Container(Container::none); //NOTE default convention for error value ... none:empty string
}
