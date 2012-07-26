/*************************************************************************************
 *  Copyright (C) 2007-2008 by Aleix Pol <aleixpol@kde.org>                          *
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

#include "plotsview2d.h"

#include <QSvgGenerator>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QList>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFile>

#include <analitza/localize.h>
#include <QDebug>

#include <analitza/analyzer.h>
#include "plotter2d.h"
#include "plotsmodel.h"
#include "plotsproxymodel.h"
#include <cmath>
#include <QtGui/qitemselectionmodel.h>

PlotsView2D::PlotsView2D(QWidget *parent,PlotsProxyModel* fm)
:QWidget(parent), Plotter2D(size(), fm),
    valid(false), mode(None),
    m_framed(false), m_readonly(false), m_selection(0)
{
    this->setFocusPolicy(Qt::ClickFocus);
    this->setCursor(Qt::CrossCursor);
    
    this->setMouseTracking(!m_readonly);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    defViewport = QRectF(QPointF(-12., 10.), QSizeF(24., -20.));
    resetViewport();
    
    this->setAutoFillBackground(false);
    
    //NOTE remove next iter
//     connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
//         this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
//     connect(model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
//         this, SLOT(addFuncs(QModelIndex,int,int)));
//     connect(model(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
//         this, SLOT(removeFuncs(QModelIndex,int,int)));
}

PlotsView2D::~PlotsView2D() {}

void PlotsView2D::drawFunctions(QPaintDevice* pd)
{
    if(buffer.isNull() || buffer.size()!=size())
        buffer = QPixmap(size());
    buffer.fill(palette().color(QPalette::Active, QPalette::Base));
    
    Plotter2D::drawFunctions(pd);
    
    valid=true;
}

void PlotsView2D::paintEvent(QPaintEvent * )
{
    if(!valid)
        drawFunctions(&buffer);
    
    QPainter p(this);
    p.drawPixmap(QRect(QPoint(0,0), size()), buffer);
    QPen ccursor;
    
//  finestra.setRenderHint(QPainter::Antialiasing, true);
    
    if(!m_readonly && mode==None) {
        QPointF ultim = toWidget(mark);
        
        //Draw derivative
        ccursor.setColor(palette().color(QPalette::Active, QPalette::Link));
        ccursor.setStyle(Qt::SolidLine);
        QLineF sl=slope(mark);
        sl.translate(mark);
        
        p.setPen(ccursor);
        p.setRenderHint(QPainter::Antialiasing, true);
#ifndef Q_CC_MSVC
        if(!sl.isNull() && !std::isnan(sl.length()))
#else
        if(!sl.isNull() && !_isnan(sl.length()))
#endif
            p.drawLine(toWidget(sl));
        p.setRenderHint(QPainter::Antialiasing, false);
        //EOderivative
        
        ccursor.setColor(QColor(0xc0,0,0));
        ccursor.setStyle(Qt::SolidLine);
        
        p.setPen(ccursor);
        p.drawLine(QPointF(0.,ultim.y()), QPointF(size().width(), ultim.y()));
        p.drawLine(QPointF(ultim.x(),0.), QPointF(ultim.x(), size().height()));
        
        int w=p.fontMetrics().width(m_posText)+15, h=p.fontMetrics().height();
        
        if(ultim.x()+w > size().width())
            ultim.setX(size().width()-w);
        if(ultim.y()+h > size().height())
            ultim.setY(size().height()-h);
        if(ultim.y() < 0.)
            ultim.setY(0.);
        
        p.setPen(QPen(Qt::black));
        p.drawText(QPointF(ultim.x()+15., ultim.y()+15.), m_posText);
    } else if(!m_readonly && mode==Selection) {
        ccursor.setColor(QColor(0xc0,0,0));
        ccursor.setStyle(Qt::SolidLine);
        p.setPen(ccursor);
        p.setBrush(QColor(0xff,0xff, 0,0x90));
        p.drawRect(QRect(press,last));
    }
    
    if(m_framed) {
        QPen bord(Qt::black);
        p.setPen(bord);
        p.drawRect(QRect(QPoint(0,0), size()-QSize(2,2)));
    }
    p.end();
    
//  qDebug() << "xxx2 " << viewport;
}

void PlotsView2D::wheelEvent(QWheelEvent *e)
{
    QRectF viewport=lastViewport();
    int steps = e->delta()/(8*15);
    qreal d = (-0.03*steps) + 1;
    
    if(d>0 || (viewport.width()+d > 2 && viewport.height()+d < 2)) {
        scaleViewport(d, e->pos());
    }
}

void PlotsView2D::mousePressEvent(QMouseEvent *e)
{
    if(!m_readonly && (e->button()==Qt::LeftButton || e->button()==Qt::MidButton)) {
        last = press = e->pos();
        ant = toViewport(e->pos());
        this->setCursor(QCursor(Qt::PointingHandCursor));
        if(e->button()==Qt::MidButton || (e->button()==Qt::LeftButton && e->modifiers()&Qt::ControlModifier))
            mode=Pan;
        else if(e->button()==Qt::LeftButton)
            mode=Selection;
    }
}

void PlotsView2D::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_readonly)
        this->setCursor(Qt::ArrowCursor);
    else
        this->setCursor(Qt::CrossCursor);
    
    if(!m_readonly && mode==Selection) {
        QPointF pd = toViewport(e->pos())-toViewport(press);
        
        QPoint pd2 = e->pos()-press;
        QRect rr(press, QSize(pd2.x(), pd2.y()));
        
        if(rr.width()>20 && rr.height()>20) { //if selection is not very small
            QRectF r(fromWidget(press), QSizeF(pd.x(), pd.y()));
            
            if(r.top()<r.bottom()) {
                double aux = r.bottom();
                r.setBottom(r.top());
                r.setTop(aux);
            }
            
            if(r.right()<r.left()) {
                double aux = r.left();
                r.setLeft(r.right());
                r.setRight(aux);
            }
            
            setViewport(r);
        } else
            sendStatus(i18n("Selected viewport too small"));
    }
    
    mode = None;
    this->repaint();
}

void PlotsView2D::mouseMoveEvent(QMouseEvent *e)
{
    mark=calcImage(fromWidget(e->pos()));
    
    if(!m_readonly && mode==Pan && ant != toViewport(e->pos())){
        moveViewport(e->pos() - press);
        
        press = e->pos();
    } else if(e->buttons()&Qt::LeftButton) {
        last = e->pos();
    } else if(e->buttons()==0)
        sendStatus(QString("x=%1 y=%2").arg(mark.x(),3,'g',5).arg(mark.y(),3,'g',5));
    
    this->repaint();
}

void PlotsView2D::keyPressEvent(QKeyEvent * e)
{
    const double xstep=lastViewport().width()/12., ystep=lastViewport().height()/10.;
    
    switch(e->key()) {
        case Qt::Key_Right:
            setViewport(lastUserViewport().translated(xstep, 0));
            break;
        case Qt::Key_Left:
            setViewport(lastUserViewport().translated(-xstep, 0));
            break;
        case Qt::Key_Down:
            setViewport(lastUserViewport().translated(0, -ystep));
            break;
        case Qt::Key_Up:
            setViewport(lastUserViewport().translated(0, ystep));
            break;
        case Qt::Key_Minus:
            zoomOut();
            break;
        case Qt::Key_Plus:
            zoomIn();
            break;
        default:
            return;
    }
}

void PlotsView2D::resizeEvent(QResizeEvent *)
{
    buffer=QPixmap(size());
    setPaintedSize(size());
}

bool PlotsView2D::toImage(const QString &path, Format f)
{
    Q_ASSERT(!path.isEmpty());
    bool b=false;
    
    switch(f) {
        case SVG: {
            QFile f(path);
            QSvgGenerator gen;
            gen.setOutputDevice(&f);
            gen.setSize(this->size());
            drawFunctions(&gen);
            b=true;
            forceRepaint();
        }   break;
        case PNG:
            this->repaint();
            b=buffer.save(path, "PNG");
            break;
    }
    
    return b;
}

void PlotsView2D::zoomIn()
{
    QRectF userViewport = lastUserViewport();
    if(userViewport.height() < -3. && userViewport.width() > 3.){
        setViewport(QRect(userViewport.left() +1., userViewport.top() -1.,
                    userViewport.width() -2., userViewport.height() +2.));
    }
}

void PlotsView2D::zoomOut()
{
    QRectF userViewport = lastUserViewport();
    //FIXME:Bad solution
    //resolucio=(resolucio*viewport.width())/(viewport.width()+2.);
    setViewport(QRect(userViewport.left() -1., userViewport.top() +1.,
                userViewport.width() +2., userViewport.height() -2.));
}

void PlotsView2D::addFuncs(const QModelIndex & parent, int start, int end)
{
//     Q_ASSERT(!parent.isValid());
    
    updateFunctions(parent, start, end);
}

void PlotsView2D::removeFuncs(const QModelIndex &, int, int)
{
    forceRepaint();
}

void PlotsView2D::updateFuncs(const QModelIndex & parent, int start, int end)
{
    updateFunctions(parent,start, end);
}

void PlotsView2D::updateFuncs(const QModelIndex& start, const QModelIndex& end)
{
    updateFuncs(QModelIndex(), start.row(), end.row());
}

void PlotsView2D::setReadOnly(bool ro)
{
    m_readonly=ro;
    this->setCursor(ro ? Qt::ArrowCursor : Qt::CrossCursor);
    setMouseTracking(!ro);
}


///
void PlotsView2D::setXAxisLabel(const QString &label)
{
    m_axisXLabel = label;
    update();
}

void PlotsView2D::setYAxisLabel(const QString &label)
{
    m_axisYLabel = label;
    update();

}

void PlotsView2D::updateTickScale(QString tickScaleSymbol, qreal tickScaleSymbolValue,
        /*bool tickScaleUseSymbols,*/ int tickScaleNumerator,
        int tickScaleDenominator)
{
    m_tickScaleSymbol = tickScaleSymbol;
    m_tickScaleSymbolValue = tickScaleSymbolValue;
    //m_tickScaleUseSymbols = tickScaleUseSymbols;
    m_tickScaleNumerator = tickScaleNumerator;
    m_tickScaleDenominator = tickScaleDenominator;

    update();
}


///


QRectF PlotsView2D::definedViewport() const
{
    return lastUserViewport();
}

void PlotsView2D::viewportChanged()
{
    QRectF userViewport=lastUserViewport(), viewport=lastViewport();
    
    sendStatus(QString("(%1, %2)-(%3, %4)")
            .arg(viewport.left()).arg(viewport.top()).arg(viewport.right()).arg(viewport.bottom()));
    emit viewportChanged(userViewport);
}

int PlotsView2D::currentFunction() const
{
    if (!model()) return -1; // guard
    
    int ret=-1;
    if(m_selection) {
        ret=m_selection->currentIndex().row();
    }
    
    return ret;
}

void PlotsView2D::modelChanged()
{
    //TODO disconnect prev model
    //NOTE Estas signal son del PROXY ... para evitar que este widget reciba signals que no le interesan 
    connect(model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(updateFuncs(QModelIndex,QModelIndex)));
    connect(model(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(addFuncs(QModelIndex,int,int)));
    connect(model(), SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(removeFuncs(QModelIndex,int,int)));
}

void PlotsView2D::setSelectionModel(QItemSelectionModel* selection)
{
    Q_ASSERT(selection);
//     Q_ASSERT(selection->model() == model());

    //El item selection es del proxy
    Q_ASSERT(qobject_cast< const PlotsProxyModel* >(selection->model()));

    
    m_selection = selection;
    connect(m_selection,SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(forceRepaint()));
}
