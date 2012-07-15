/*************************************************************************************
 *  Copyright (C) 2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com>      *
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

#ifndef SURFACE_H
#define SURFACE_H

#include "private/functiongraph.h"

#include "analitzaplotexport.h"

class AbstractSurface;

class ANALITZAPLOT_EXPORT Surface : public FunctionGraph 
{
public:
    Surface(const Analitza::Expression &functionExpression, const QString &name, const QColor& col, Analitza::Variables *variables = 0);
    virtual ~Surface();

    static bool canDraw(const Analitza::Expression &functionExpression);
    static bool canDraw(const Analitza::Expression &functionExpression, QStringList &errors);

    bool reset(const Analitza::Expression& functionExpression);

    //Own
    void update(const Box3D &viewport);
    const QVector<Triangle3D> & faces() const;
    
protected:
    Surface() {}
    Surface(const Surface &other) {}
};


#endif // SURFACE_H
