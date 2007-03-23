/*************************************************************************************
 *  Copyright (C) 2007 by Aleix Pol <aleixpol@gmail.com>                             *
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

#ifndef FUNCTION_H
#define FUNCTION_H

#include <QColor>
#include "analitza.h"
#include "expression.h"

/** Defines a function axe type. */
enum Axe { Cartesian=0, Polar};

/**
 *	Calculates 2D functions.
 *	@author Aleix Pol i Gonzalez
 */
class function
{
	friend class Graph2D;
public:
	/** Constructor. Create an empty function. */
	function();
	
	/** Copy constructor. */
	function(const function& f);
	
	/** Constructor. Creates a new function.
		@param name the function name.
		@param newExp the function expression tree.
		@param color the function representation color.
		@param selec defines if the function is selected.
	*/
	function(const QString& name, const Expression& newExp, const QColor& color, bool selec=false);
	
	/** Destructor. */
	~function();
	
	/** Constructor. Creates a new function.
		@param name the function name.
		@param newExp the function expression tree.
		@param color the function representation color.
		@param selec defines if the function is selected.
	 */
	void setFunction(const QString& name, const Expression& newExp, const QColor& color, bool selec=false);
	
	/** Defines a new function behaviour.
		@param viewport sets the coordinates the function will fit to.
		@param resolution sets how many points will the function have.
	*/
	void update_points(QRect viewport, unsigned int resolution);
	
	/** Retrieves the color of the function. */
	QColor color() const { return m_color; }
	
	/** Sets the color of the function. */
	void setColor(const QColor& newColor) { m_color=newColor; }
	
	/** Returns the number of points it has. */
	unsigned int npoints() const { return m_last_resolution; }
	
	/** Sets whether it is selected. */
	void setSelected(bool newSelec) { m_selected=newSelec; }
	
	/** Retrieves whether it is selected. */
	bool selected() const { return m_selected; }
	
	/** Sets whether the function has to be shown. */
	void setShown(bool newShow) { m_show=newShow; }
	
	/** Returns whether the function can be drawn. */
	bool isShown() const { return m_show && func->isCorrect(); }
	
	/** Equal operator. */
	bool operator==(const function& f) const { return f.m_name==m_name; }
	
	/** Retrieves the function's name. */
	QString name() const { return m_name; }
	
	/** Copies a function */
	function operator=(const function& f);
	
	/** Retrieves the function evaluator. */
	Analitza* analitza() const { return func; }
	
	Axe axeType() const;
protected:
	/** Here we store the calculated points of the function. */
	QPointF *points;
private:
	Analitza *func;
	bool m_show;
	bool m_selected;
	QString m_firstlambda;
	QColor m_color;
	QRect m_last_viewport;
	unsigned int m_last_resolution;
	unsigned int m_last_max_res;
	QStringList err; //function errors
	void update_pointsY(QRect viewport, unsigned int resolucio); //for functions such as y=f(x)
	void update_pointsX(QRect viewport, unsigned int resolucio); //for functions such as x=f(y)
	void update_pointsPolar(QRect viewport, unsigned int resolucio); //for functions such as r=f(sigma)
	QPair<QPointF, QString> calc(const QPointF& dp);
	
	inline QPointF fromPolar(double r, double th) { return QPointF(r*cos(th), r*sin(th)); }
	QString m_name;
};

#endif
