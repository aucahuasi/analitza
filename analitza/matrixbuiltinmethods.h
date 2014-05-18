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

#ifndef MATRIXBUILTINMETHODS_H
#define MATRIXBUILTINMETHODS_H

#include "builtinmethods.h"

namespace Analitza {
class Expression;
};

class FillVectorConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);
	
	static const QString id;
	static const Analitza::ExpressionType type;
};

class FillMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);
	
	static const QString id;
	static const Analitza::ExpressionType type;
};

class ZeroMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);
	
	static const QString id;
	static const Analitza::ExpressionType type;
};

class IdentityMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

class DiagonalMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

//tridiag(a, b, c, n)
class TridiagonalMatrixConstructor: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

class GetNDiagonalOfMatrix: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

class GetDiagonalOfMatrix: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

class IsZeroMatrix: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

class IsIdentityMatrix: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

class IsDiagonalMatrix: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};

/// experimental

class TestCmd: public Analitza::FunctionDefinition
{
public:
	virtual Analitza::Expression operator()(const QList< Analitza::Expression >& args);

	static const QString id;
	static const Analitza::ExpressionType type;
};


#endif // MATRIXBUILTINMETHODS_H
