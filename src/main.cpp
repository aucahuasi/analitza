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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include "algebra.h"

int main(int argc, char *argv[])
{
	KAboutData about("kalgebra", ("KAlgebra"), "0.7", "A calculator",
			 KAboutData::License_GPL, "(C) 2006 Aleix Pol Gonzalez", 0, 0, "aleixpol@gmail.com");
	about.addAuthor( "Aleix Pol Gonzalez", 0, "aleixpol@gmail.com" );
	KCmdLineArgs::init(argc, argv, &about);
	KApplication app;
	
	/*if(app.isSessionRestored()) {
		RESTORE(KAlgebra);
	} else {*/
		KAlgebra* widget = new KAlgebra;
		widget->show();
	//}
	
	return app.exec();
}
