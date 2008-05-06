/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@gmail.com>                             *
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

#include "kalgebraplasma.h"

#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QGraphicsProxyWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <KLineEdit>

#include <plasma/theme.h>
#include <plasma/dataengine.h>
// #include <plasma/widgets/label.h>
// #include <plasma/widgets/flash.h>
// #include <plasma/layouts/boxlayout.h>
// #include <plasma/widgets/icon.h>

#include "expression.h"

KAlgebraPlasmoid::KAlgebraPlasmoid(QObject *parent, const QVariantList &args)
	: Plasma::Applet(parent, args)
{
	//this will get us the standard applet background, for free!
	//setDrawStandardBackground(true);
	resize(200, 200);
}
 
 
KAlgebraPlasmoid::~KAlgebraPlasmoid() {}

void KAlgebraPlasmoid::init()
{
    QGraphicsProxyWidget * graphicsWidget = new QGraphicsProxyWidget(this);
    QFrame *widget = new QFrame;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    graphicsWidget->setWidget(widget);

    // FIXME real position
    graphicsWidget->setPos(10,10);
    graphicsWidget->resize(size());
    
// 	Plasma::VBoxLayout* m_layout = new Plasma::VBoxLayout( this );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );
	
	m_input = new KLineEdit(widget);
    layout->addWidget(m_input);
// 	m_input->setGeometry(QRectF(QPointF(0,0),size()));
    m_input->setClickMessage(i18n("Enter the expression..."));
// 	m_input->setMultiLine(false);
// 	m_layout->addItem(m_input);
	
    m_output = new QLabel(widget);
    m_output->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_output);
    
// 	m_output->setGeometry(QRectF(QPointF(0,0),size()));
// 	m_layout->addItem(m_output);
	
	connect(m_input, SIGNAL(editingFinished()), this, SLOT(addOperation()));
	connect(m_input, SIGNAL(changed()), this, SLOT(simplify()));
}

void KAlgebraPlasmoid::constraintsEvent(Plasma::Constraints constraints)
{
    if (constraints & Plasma::SizeConstraint) {
        // FIXME resize
    }
}

void KAlgebraPlasmoid::addOperation()
{
// 	m_input->selectAll();
	Expression res;
	a.setExpression(Expression(m_input->text(), false));
	if(a.isCorrect()) {
		res=a.evaluate();
	}
	
	if(a.isCorrect()) {
// 		m_output->setPen(QPen(Qt::white));
        QPalette palette = m_output->palette();
        palette.setColor(QPalette::WindowText, Qt::white);
        m_output->setPalette(palette);

		m_output->setText(res.toString());
	} else {
// 		m_output->setPen(QPen(Qt::red));
        QPalette palette = m_output->palette();
        palette.setColor(QPalette::WindowText, Qt::red);
        m_output->setPalette(palette);

		m_output->setText(a.errors().join("\n"));
	}
}

void KAlgebraPlasmoid::simplify()
{
	Expression res;
	a.setExpression(Expression(m_input->text(), false));
	
	if(a.isCorrect()) {
		a.simplify();
		res=*a.expression();

		QPalette palette = m_output->palette();
        palette.setColor(QPalette::WindowText, Qt::blue);
        m_output->setPalette(palette);
// 		m_output->setPen(QPen(Qt::blue));
		m_output->setText(res.toString());
	} else
		m_output->setText(QString());
}

#include "kalgebraplasma.moc"