/*************************************************************************************
 *  Copyright (C) 2010-2012 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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


#ifndef _ANALITZAPLOT_FUNCTIONGRAPH_H
#define _ANALITZAPLOT_FUNCTIONGRAPH_H

#include "abstractfunctiongraph.h"

#define CONSTRUCTORS(name) \
name (const Analitza::Expression &functionExpression) : AbstractSpaceCurve(functionExpression) { } \
name (const Analitza::Expression &functionExpression, Analitza::Variables *variables) :AbstractSpaceCurve(functionExpression, variables) { }

namespace Analitza
{
    class Variables;
}

class ANALITZAPLOT_EXPORT AbstractSpaceCurve : public AbstractFunctionGraph 
{
public:
    AbstractSpaceCurve(const Analitza::Expression& e, Analitza::Variables* v);
    AbstractSpaceCurve(const Analitza::Expression& e);
    virtual ~AbstractSpaceCurve();

    //Curve ... los expongo como publicos tanto para planecurve como para los backend (
    //para los backends por un tema de performance y flexibilidad) 
    // al final en planecurve todo estara expuesto consistentemente 
    QVector<QVector3D> points;
    QVector<int> jumps;

    //Own
    virtual void update(const Box& viewport) = 0;

protected:
    AbstractSpaceCurve() {}
    AbstractSpaceCurve(const AbstractSpaceCurve& other) {}
    
    bool addPoint(const QVector3D& p);
};


#endif // ANALITZAPLOT_FUNCTIONGRAPH_H
