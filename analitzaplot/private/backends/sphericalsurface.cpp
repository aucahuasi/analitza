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




#include "private/abstractsurface.h"
// #include "private/surfacefactory.h"
#include "private/functiongraphfactory.h"


#include "analitza/value.h"
#include <analitza/vector.h>

//TODO macros para las prop e abajo


class Fap : public AbstractSurface/*, static class? better macros FooClass*/
{
public:
    explicit Fap(const Analitza::Expression& e);
    Fap(const Analitza::Expression& e, Analitza::Variables* v);
    
    
    TYPE_NAME("esfe")
    EXPRESSION_TYPE(Analitza::ExpressionType(Analitza::ExpressionType::Lambda).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)).addParameter(
                        Analitza::ExpressionType(Analitza::ExpressionType::Value)))
    COORDDINATE_SYSTEM(Cartesian)
    PARAMETERS("t,p") // t azimuth p polar
    ICON_NAME("none")
    EXAMPLES("")

    //Own
    virtual bool setInterval(const QString& argname, const Analitza::Expression& min, const Analitza::Expression& max);
    virtual bool setInterval(const QString& argname, double min, double max);
    
    QVector3D fromParametricArgs(double u, double v);
    void update(const Box3D& viewport);
    
    
};

bool Fap::setInterval(const QString& argname, const Analitza::Expression& min, const Analitza::Expression& max)
{
    
    Analitza::Analyzer *intervalsAnalizer = new Analitza::Analyzer(analyzer->variables());

    QPair<Analitza::Expression, Analitza::Expression> ival = interval(argname, true);
    
    double min_val = ival.first.toReal().value();
    double max_val = ival.second.toReal().value();

    delete intervalsAnalizer;
    
    if (min_val<0 || max_val < 0) // el radio es distancia por tanto es positivo, el angulo va de 0 hasta +inf
        return false; 
    
    if (argname == QString("a") && max_val >= 2*M_PI)
        return false;
    
    if (argname == QString("p") && max_val > M_PI)
        return false;
    
    return AbstractFunctionGraph::setInterval(argname, min, max);
}

bool Fap::setInterval(const QString& argname, double min, double max)
{
    if (min<0 || max < 0) // el radio es distancia por tanto es positivo, el angulo va de 0 hasta +inf
        return false; 
    
    
    if (argname == QString("a") && max >= 2*M_PI)
        return false;
    
    if (argname == QString("p") && max > M_PI)
        return false;
    
    return AbstractFunctionGraph::setInterval(argname, min, max);
}


Fap::Fap(const Analitza::Expression& e): AbstractSurface(e)
{
    setInterval("a", 0, M_PI);
    setInterval("p", 0, M_PI);
}

Fap::Fap(const Analitza::Expression& e, Analitza::Variables* v): AbstractSurface(e)
{

}


QVector3D Fap::fromParametricArgs(double a, double p)
{
    
    arg("a")->setValue(a);
    arg("p")->setValue(p);    

    double r = analyzer->calculateLambda().toReal().value();

    return sphericalToCartesian(r,a,p);
}

void Fap::update(const Box3D& viewport)
{
    buildParametricSurface();
}


REGISTER_SURFACE(Fap)
