/*************************************************************************************
 *  Copyright (C) 2007-2011 by Aleix Pol <aleixpol@kde.org>                          *
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

#ifndef ANALITZAPLOT_FUNCTION_H
#define ANALITZAPLOT_FUNCTION_H

#include "analitza/expression.h"
#include "functionutils.h"

#include <QMap>
#include <QColor>

namespace Analitza
{
class Variables;
}

class AbstractPlaneCurve;

// this class is a sort of a functionitem ...
// it can't use for plot curves/surfaces (is a just a data holder)
class ANALITZAPLOT_EXPORT MappingGraph
{
public:
    MappingGraph();
    MappingGraph(const MappingGraph &f);
    MappingGraph(const QString &name, const QColor& col);
    virtual ~MappingGraph();

    const QString id() const { return m_id; }
    virtual const QString typeName() const = 0; // curve, linear op, isosurface etc localized
    virtual const Analitza::Expression & expression() const = 0; // why pure abstract: couse graphpres go to functionimpl

    QString name() const { return m_name; }
    void setName(const QString &newName) { m_name = newName; }
    virtual QString iconName() const = 0;
    QColor color() const { return m_color; }
    void setColor(const QColor& newColor) { m_color = newColor; }
    virtual QStringList examples() const = 0;

    virtual int spaceDimension() const = 0; // dim of the space where the item can be drawn ... IS NOT the variety dimension
    virtual CoordinateSystem coordinateSystem() const = 0;
    virtual DrawingPrecision drawingPrecision()  = 0;
    virtual void setDrawingPrecision(DrawingPrecision precs) = 0; // why pure abstract: couse graphpres go to functionimpl
    PlotStyle plotStyle() { return m_plotStyle; }
    void setPlotStyle(PlotStyle ps) { m_plotStyle = ps; }
    bool isVisible() const { return m_graphVisible; }
    void setVisible(bool f) { m_graphVisible = f; }

    virtual QStringList errors() const = 0;
    virtual bool isCorrect() const = 0;

    bool operator == (const MappingGraph& f) const { return m_id == f.m_id; }

protected:
    void setId(const QString id) { m_id = id; }

private: //TODO pimpl idiom here?
    QString m_id; // from a QUuid

    //gui
    QString m_name;
    QColor m_color;

    //graphDescription    
    PlotStyle m_plotStyle;
    bool m_graphVisible;
};

//TODO balls
//TODO Planar graphs
//TODO operator or transf

class ANALITZAPLOT_EXPORT FunctionGraph : public MappingGraph
{
public:
    FunctionGraph() : MappingGraph() {}
    FunctionGraph(const FunctionGraph &f) : MappingGraph(f) {} //interval abstrac y ademas deberia ir la data en el abscurve
    FunctionGraph(const QString &name, const QColor& col) : MappingGraph(name, col) {}
    
    virtual RealInterval argumentInterval(const QString &argname) const = 0;
    virtual void setArgumentInverval(const QString &argname, const RealInterval &interval) = 0;
    virtual QStringList arguments() const = 0;
    
    virtual void update(const QList<RealInterval> viewport) = 0;
};

//TODO class curve surface curve

class ANALITZAPLOT_EXPORT Curve : public FunctionGraph
{
public:
    Curve() : FunctionGraph() {}
    Curve(const Curve &f) : FunctionGraph(f) {}
    Curve(const QString &name, const QColor& col) : FunctionGraph(name, col) {}
    virtual ~Curve() {}

    //Own
    //TODO ... (very :p) hard Numerical analysis
    virtual double arcLength() const = 0;
    virtual bool isClosed() const = 0;
    virtual double area() const = 0; //only if is closed
    virtual QPair<bool /*yes or not*/, double /*offset*/> isParallelTo(const Curve &othercurve) = 0; // offset, either positive or negative, in the direction of the curve's normal
    virtual QList<int> jumps() const = 0;
};

//algebraic, parametric curves (//equans of this curves are functiones callet vector valued functions (with one real param) 
//... aka vectorialcurves)), polar curves etc
class ANALITZAPLOT_EXPORT PlaneCurve : public Curve 
{
//2D
    //curvature, length of arc, etc curvature
//parametricform ... implicit->parametric etc
public:
    PlaneCurve();
    PlaneCurve(const PlaneCurve &f);
    PlaneCurve(const Analitza::Expression &functionExpression, Analitza::Variables *variables, const QString &name, const QColor& col);
    virtual ~PlaneCurve();

    //MappingGraph
    const QString typeName() const;
    const Analitza::Expression &expression() const;
    QString iconName() const;
    QStringList examples() const;
    int spaceDimension() const;
    CoordinateSystem coordinateSystem() const;
    DrawingPrecision drawingPrecision();
    void setDrawingPrecision(DrawingPrecision precision); 
    QStringList errors() const;
    bool isCorrect() const;

    //FunctionGraph
    RealInterval argumentInterval(const QString &argname) const;
    void setArgumentInverval(const QString &argname, const RealInterval &interval);
    QStringList arguments() const;
    void update(const QList<RealInterval> viewport);

    //Curve
    double arcLength() const;
    bool isClosed() const;
    double area() const;
    QPair<bool, double> isParallelTo(const Curve &othercurve);
    QList<int> jumps() const;

    //Own
    const QVector<QVector2D> & points() const;
    QPair<QVector2D, QString> calc(const QPointF &mousepos);
    QLineF derivative(const QPointF &mousepos) const;

    bool isImplicit() const;
    bool isParametric() const;
    bool isAlgebraic() const; // implicit plus only polynomails analitza work :)

    PlaneCurve operator = (const PlaneCurve &curve); // copy all:members, "id" and funcimpl instance
    
    
    //TODO gsoc
    ///Region under curve (relative to zxis polar axis ... sistema of referencia = basis = 0 + vectors)
    ///parametricform ... -> expresion
    ///toparamform ->void
    /// can be parameterized ... work for analitza :)

private:
    AbstractPlaneCurve *m_planeCurve;

    QStringList m_errors;
};

// class ANALITZAPLOT_EXPORT SpaceCurve : public Curve 
// {
//     //3D
// //torsion,   
//     //curvature, length of arc, curvature
//     
// 
// };


#endif // ANALITZAPLOT_FUNCTION_H
