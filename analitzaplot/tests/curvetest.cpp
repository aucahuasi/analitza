/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@kde.org>                               *
 *  Copyright (C) 2012-2013 by Percy Camilo T. Aucahuasi <percy.camilo.ta@gmail.com> *
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

#include "curvetest.h"
#include "analitzaplot/curve.h"
#include "analitza/expression.h"
#include "analitza/variables.h"
#include <qtest_kde.h>
#include <cmath>
#include <QColor>

using namespace std;
using namespace Analitza;

QTEST_KDEMAIN_CORE( PlaneCurveTest )

PlaneCurveTest::PlaneCurveTest(QObject *parent)
    : QObject(parent)
{
}

PlaneCurveTest::~PlaneCurveTest()
{
}

void PlaneCurveTest::initTestCase()
{
    m_vars = new Analitza::Variables;
}

void PlaneCurveTest::cleanupTestCase()
{
    delete m_vars;
}

void PlaneCurveTest::testCopyCompare_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QColor>("color");
    
    QTest::newRow("basic_implicit") << "x*x+y*y=6" << "circle" << QColor(Qt::blue);
}

void PlaneCurveTest::testCopyCompare()
{
    QFETCH(QString, expression);
    QFETCH(QString, name);
    QFETCH(QColor, color);

    Curve curve1(Expression(expression), m_vars);
    curve1.setName(name);
    curve1.setColor(color);
    
    Curve curve2(curve1);
    Curve curve3 = curve2;
    Curve curve4;
    curve4 = curve3;

    QVERIFY(curve4.isValid());
    QCOMPARE(curve1, curve4);
}

void PlaneCurveTest::testCorrect_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("fx-diag-line") << "x->x";
    QTest::newRow("fy-diag-line") << "y->y";
    QTest::newRow("vector-diag-line") << "t->vector{t,t}";
    QTest::newRow("simple-algebraic") << "x*x+y*y=3";
}

void PlaneCurveTest::testCorrect()
{
    QFETCH(QString, input);

    Curve curve(Expression(input), m_vars);
    curve.createGeometry();
    
    QVERIFY(curve.isValid());
}

void PlaneCurveTest::testIncorrect_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("empty expression") << "";
    QTest::newRow("undefined var") << "x:=w";
    QTest::newRow("parametric-wrongvector") << "t->vector{3}";
    QTest::newRow("wrong-dimension") << "vector{2,3}";
    QTest::newRow("wrong-dimension-y") << "y->vector{2,3}";
    QTest::newRow("wrong-dimension-q") << "q->vector{2,3}";
    QTest::newRow("wrong-parametric") << "t->v";
    QTest::newRow("wrong-variable") << "x->x(x)";
    QTest::newRow("wrong-call") << "(x+1)(x+2)";
    QTest::newRow("wrong-inf") << "y->y/0";
    QTest::newRow("wrong-nan") << "x/0 + y/0 = 89";
    QTest::newRow("collision") << "(x,y)->5=x*y";

//     QTest::newRow("implicit.notindomain") << "(x,y)->3-sin(x)*sin(y)";

//     QTest::newRow("not a function") << "t";
}

void PlaneCurveTest::testIncorrect()
{
    QFETCH(QString, input);

    Curve curve(Expression(input), m_vars);
    curve.createGeometry();
    
    QVERIFY(!curve.isValid());
}

#include "curvetest.moc"