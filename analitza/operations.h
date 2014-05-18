/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "operator.h"
#include <expressiontype.h>

namespace Analitza
{

class Container;

class List;
class Cn;
class Vector;
class Matrix;
class CustomObject;
class ExpressionType;

class Operations
{
	public:
		typedef Cn* (*UnaryOp)(Operator::OperatorType, Object*, QString**);
		typedef Object * (*BinaryOp)(Operator::OperatorType op, Object *, Object *, QString** correct);
		
		static Object* reduce(Operator::OperatorType op, Object* oper, Object* oper1, QString** correct);
		static Object* reduceUnary(Operator::OperatorType op, Object* oper, QString** correct);
		
		static QList<ExpressionType> infer(Analitza::Operator::OperatorType op);
		static QList<ExpressionType> inferUnary(Operator::OperatorType op);
		
	private:
		static UnaryOp opsUnary[Object::custom+1];
		static BinaryOp opsBinary[Object::custom+1][Object::custom+1];
		
		static Cn* reduceRealReal(Operator::OperatorType op, Cn *oper, const Cn* oper1, QString** correct);
		static Cn* reduceUnaryReal(Operator::OperatorType op, Cn *oper, QString** correct);
		static Object* reduceRealContainer(Operator::OperatorType op, Cn* oper, Container *cntr, QString** correct);
		static Object* reduceContainerReal(Operator::OperatorType op, Container *cntr, Cn* oper, QString** correct);
		
		static Object* reduceRealVector(Operator::OperatorType op, Cn *oper, Vector* vector, QString** correct);
		static Object* reduceVectorReal(Operator::OperatorType op, Vector* vector, Cn *oper, QString** correct);
		static Object* reduceVectorVector(Operator::OperatorType op, Vector* v1, Vector* v2, QString** correct);
		static Cn* reduceUnaryVector(Operator::OperatorType op, Vector* c, QString** correct);
		
		static Object* reduceRealList(Operator::OperatorType op, Cn *oper, List* vector, QString** correct);
		static Object* reduceListList(Operator::OperatorType op, List* l1, List* l2, QString** correct);
		static Cn* reduceUnaryList(Operator::OperatorType op, List* l, QString** correct);
		
		static Object* reduceRealMatrix(Analitza::Operator::OperatorType op, Analitza::Cn* v, Analitza::Matrix* m1, QString** correct);
		static Object* reduceMatrixMatrix(Operator::OperatorType op, Matrix* m1, Matrix* m2, QString** correct);
		static Object* reduceUnaryMatrix(Analitza::Operator::OperatorType op, Analitza::Matrix* m, QString** correct);
		static Object* reduceMatrixContainer(Operator::OperatorType op, Matrix* m, Container *cntr, QString** correct);
		static Object* reduceContainerMatrix(Operator::OperatorType op, Container *cntr, Matrix* m, QString** correct);
		
		static Object* reduceCustomCustom(Operator::OperatorType op, CustomObject* v1, CustomObject* v2, QString** correct);
		
		static Object* errorCase(const QString &error, Container *errorcntr, QString** correct); // errorcntr is none container
};

}
#endif
