/*************************************************************************************
 *  Copyright (C) 2009 by Aleix Pol <aleixpol@kde.org>                               *
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

#include "viewportwidget.h"

#include <QFormLayout>
#include <KLocale>
#include <QDoubleSpinBox>
#include <KPushButton>
#include <QBoxLayout>
#include <limits>

ViewportWidget::ViewportWidget(QWidget * parent, Qt::WFlags flags)
	: QWidget (parent, flags)
{
	const double LIMIT=std::numeric_limits<double>::max();
	
	m_top=new QDoubleSpinBox(this);
	m_left=new QDoubleSpinBox(this);
	m_width=new QDoubleSpinBox(this);
	m_height=new QDoubleSpinBox(this);
	
	m_top->setRange(-LIMIT, LIMIT);
	m_left->setRange(-LIMIT, LIMIT);
	m_width->setRange(0, LIMIT);
	m_height->setRange(0, LIMIT);
	
	QVBoxLayout* upperLayout=new QVBoxLayout;
	setLayout(upperLayout);
	QFormLayout* layout=new QFormLayout;
	layout->addRow(i18n("Left:"), m_left);
	layout->addRow(i18n("Top:"), m_top);
	layout->addRow(i18n("Width:"), m_width);
	layout->addRow(i18n("Height:"), m_height);
	
	QPushButton *apply=new KPushButton(KIcon("dialog-ok-apply"), i18n("Apply"), this);
	connect(apply, SIGNAL(clicked()), SLOT(emitViewport()));
	
	upperLayout->addLayout(layout);
	upperLayout->addWidget(apply);
}

QRectF ViewportWidget::viewport() const
{
	return QRectF(m_left->value(), m_top->value(),
				  m_width->value(), -m_height->value());
}

void ViewportWidget::setViewport(const QRectF& current)
{
	m_top->setValue(current.top());
	m_left->setValue(current.left());
	m_width->setValue(current.width());
	m_height->setValue(-current.height());
}

void ViewportWidget::emitViewport ()
{
	emit viewportChange(viewport());
}
