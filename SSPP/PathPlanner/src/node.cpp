/***************************************************************************
 *   Copyright (C) 2007 by Tarek Taha                                      *
 *   tataha@eng.uts.edu.au                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "node.h"

namespace CasPlanner
{
//! Node Constructor
Node :: Node ()
{
		parent = next = prev = NULL;
};
//! Node Destructor
Node :: ~Node ()
{
		parent = next = prev = NULL;
};
//! Node == Operator override
bool Node ::operator == (Node a)
{
	return ( isEqual(this->pose.p.x(),a.pose.p.x()) && isEqual(this->pose.p.y(),a.pose.p.y()));
}
//! Node != Operator override
bool Node ::operator != (Node a)
{
	return ( !isEqual(this->pose.p.x(),a.pose.p.x()) && isEqual(this->pose.p.y(),a.pose.p.y()));
}
bool Node ::operator < (Node a)
{
	return (this->f_value < a.f_value);
}
}
