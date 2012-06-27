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

#include "planecurvesmodel.h"

#include <analitza/localize.h>
#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QIcon>

#include "analitza/expression.h"

///

PlaneCurvesModel::PlaneCurvesModel(Analitza::Variables *v, QObject * parent)
    : FunctionGraphModel(v, parent)
{
}

PlaneCurvesModel::~PlaneCurvesModel()
{
    qDeleteAll(m_items.begin(), m_items.end());
    m_items.clear();
}

QVariant PlaneCurvesModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid() || index.row()>=m_items.count())
        return QVariant();

    int var=index.row();

    PlaneCurve *tmpcurve = m_items.at(var);

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column()) {
        case 0:
            return tmpcurve->name();
            break;
        case 1:
            return tmpcurve->expression().toString();
            break;
        }
        break;
    case Qt::DecorationRole:
        if(index.column()==0) {
            QPixmap ico(15, 15);
            ico.fill(tmpcurve->color());
            return  QIcon(ico);
        } else {
            return QIcon::fromTheme(tmpcurve->iconName());
        }
        break;
    }

    return QVariant();
}

int PlaneCurvesModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid())
        return 0;
    else
        return m_items.size();
}

//agrego item al model y no como un puntero ... esto para manejar que el model maneje el scope del planecurve internamente
void PlaneCurvesModel::addCurve(const Analitza::Expression& functionExpression, const QString& name, const QColor& col)
{
    //no se permiten items invalidos
    Q_ASSERT(PlaneCurve::canDraw(functionExpression));
    
    beginInsertRows (QModelIndex(), m_items.count(), m_items.count());

    m_items.append(new PlaneCurve(functionExpression, variablesModule, name, col));

    endInsertRows();
}

void PlaneCurvesModel::removeCurve(int row)
{
    Q_ASSERT(row<m_items.size());

    beginRemoveRows(QModelIndex(), row, row);

    PlaneCurve *tmpcurve = m_items[row];
    delete tmpcurve;
        
    m_items.removeAt(row);

    endRemoveRows();
}

const PlaneCurve* PlaneCurvesModel::curve(int curveIndex) const
{
    Q_ASSERT(curveIndex<m_items.count());

    return m_items[curveIndex];
}

bool PlaneCurvesModel::setCurve(int curveIndex, const Analitza::Expression &functionExpression, const QString &name, const QColor& col)
{
//                 if (PlaneCurve::canDraw(fexp))
//                 {
//                     m_items[index.row()]->reset(fexp);
// 
//                     emit dataChanged(index, index);
// 
//                     return true;
//                 }
return false;
}

void PlaneCurvesModel::setCurveParameterInterval(int curveIndex, const QString &argname, const Analitza::Expression &min, const Analitza::Expression &max)
{
    
}


void PlaneCurvesModel::setCurveParameterInterval(int curveIndex, const QString &argname, double min, double max)
{
    
}

    
void PlaneCurvesModel::updateCurve(int curveIndex, const QRect& viewport)
{
    m_items[curveIndex]->update(viewport);

    emit dataChanged(index(curveIndex), index(curveIndex));
}


QPair< QPointF, QString > PlaneCurvesModel::curveImage(int row, const QPointF& mousepos)
{
    Q_ASSERT(row<m_items.count());

    return m_items[row]->image(mousepos);
}

QLineF PlaneCurvesModel::curveTangent(int row, const QPointF& mousepos)
{
    Q_ASSERT(row<m_items.count());

    return m_items[row]->tangent(mousepos);
}

