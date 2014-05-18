/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@kde.org>                               *
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

#ifndef MATHMLEXPRESSIONWRITER_H
#define MATHMLEXPRESSIONWRITER_H

#include "abstractexpressionvisitor.h"

namespace Analitza
{

/**
 * \class MathMLExpressionWriter
 * 
 * \ingroup AnalitzaModule
 *
 * \brief This class represents the mathml expression writer.
 */

class MathMLExpressionWriter : public AbstractExpressionVisitor
{
	public:
		MathMLExpressionWriter(const Object* o);
		
		virtual QVariant visit(const None* var);
		virtual QVariant visit(const Ci* var);
		virtual QVariant visit(const Cn* var);
		virtual QVariant visit(const Container* var);
		virtual QVariant visit(const Operator* var);
		virtual QVariant visit(const Vector* var);
		virtual QVariant visit(const List* l);
		virtual QVariant visit(const Matrix* c);
		virtual QVariant visit(const MatrixRow* c);
		virtual QVariant visit(const Apply* a);
		virtual QVariant visit(const CustomObject* c);
		
		QVariant result() const { return m_result; }
		
	private:
		QVariant m_result;
};

}

#endif
