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

#include <QMainWindow>
#include <QStringListModel>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "analitzaplot/planecurve.h"
#include "analitzaplot/planecurvesmodel.h"
#include "analitzaplot/plotview2d.h"


int main(int argc, char *argv[])
{
    KAboutData aboutData("PlotView2DTest",
                         0,
                         ki18n("PlotView2DTest"),
                         "1.0",
                         ki18n("PlotView2DTest"),
                         KAboutData::License_LGPL_V3,
                         ki18n("(c) 2012 Percy Camilo T. Aucahuasi"),
                         ki18n("PlotView2DTest"),
                         "http://www.kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    QMainWindow *mainWindow = new QMainWindow();
    mainWindow->setMinimumSize(640, 480);
    
    //BEGIN test calls

    PlaneCurvesModel *model = new PlaneCurvesModel(mainWindow);
    QItemSelectionModel *selection = new QItemSelectionModel(model);
    
    Graph2D *view2d = new Graph2D(mainWindow);
//     view2d->setReadOnly(true);
    view2d->setSquares(false);
    view2d->setModel(model);
    view2d->setSelectionModel(selection);
    
    model->addCurve(Analitza::Expression("x->x*x"), "Hola", Qt::cyan);
    model->addCurve(Analitza::Expression("q->q+2"), "Hola", Qt::green);
    model->addCurve(Analitza::Expression("t->vector{t*t, t}"), "Hola", Qt::yellow);
    model->addCurve(Analitza::Expression("(x,y)->5*(x**2+y**2)**3-15*(x*y*72)**2"), "chau", Qt::blue);

    if (model->rowCount()>0)
    {
        selection->setCurrentIndex(model->index(model->rowCount()-1), QItemSelectionModel::Select);
    }

    //END test calls
    
    mainWindow->setCentralWidget(view2d);

    mainWindow->show();

    return app.exec();
}