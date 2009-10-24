/*************************************************************************************
 *  Copyright (C) 2007-2009 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef ANALITZAUTILS_H
#define ANALITZAUTILS_H

#include <QStringList>
class Object;
class Container;
class Variables;

namespace AnalitzaUtils
{
	bool hasTheVar(const QSet<QString>& vars, const Object *o);
	bool hasTheVar(const QSet<QString>& vars, const Container* c);
	bool isLambda(const Object* o);
	QStringList dependencies(const Object* o, const QStringList& scope);
	
	/** Returns whether there is any @p var variable in the @p o tree.
			@p bvars tells the already defined variables (which won't return true). */
	bool hasVars(const Object* o, const QString &var=QString(),
					const QStringList& bvars=QStringList(), const Variables* vars=0);
	
	/** Returns whether @p o1 is equal to @p o2. */
	bool equalTree(const Object * o1, const Object * o2);
	
	/** Prints an expression tree from a node @p o. @p prefix will be the start of every item line */
	void objectWalker(const Object* o, const QByteArray& prefix=QByteArray());
};

#endif // ANALITZAUTILS_H
