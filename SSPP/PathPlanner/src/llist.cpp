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
#include "llist.h"

namespace CasPlanner
{
//! Returns the head of the Linked List.
Node * LList::getHead()
{
	return this->Start;
};
//! Returns the next Node in the list from the current Node.
void LList::next()
{
	Start = Start->next;
	if(Start != NULL)
		Start->prev = NULL;
};
//! Returns the previous Node in the Linked list from the current Node.
void LList::prev()
{
	Start = Start->prev;
};
//! Finds if a node is already in the Linked list
Node * LList::has(Node * q)
{
	Node *p = this->Start;
	while (p)
	{
//    	cout<<"\n HAS"; fflush(stdout);
//    	cout<<"\n p X:"<<p->pose.p.x()<<" Y:"<<p->pose.p.y()<<" q X:"<<q->pose.p.x()<<" Y:"<<q->pose.p.y();
		if(*q==*p)
		{
			return p;
//			cout<<"Yes HAS works ";
		}
		p = p->next;				
	}
	return NULL;
};
//! Finds if a node is already in the Linked list according to it's ID
Node * LList::has(int id)
{
	Node *p = this->Start;
	while (p)
	{
		if(p->id ==id )
		{
			return p;
		}
		p = p->next;				
	}
	return NULL;
};
//! Empty Constructor
LList::LList()
{
	Start = NULL;
};
//! Destructor of the Linked List
LList::~LList()
{
	Node *p;
	while (Start != NULL)
	{
		p = Start->next;
		delete Start;
		Start = p;
	}		
};
//! Free the allocated memory and delete the Nodes.
void LList::free()
{
	Node *p;
	while (Start != NULL)
	{
		//cout <<"\n	--->>> Freeing Node <<<---"; fflush(stdout);
		p = Start->next;
		delete Start;
		Start = p;
	}		
};
//! Add a Node to the child list of a node and sort ascending.
void LList::add(Node * curChild)
{
	Node * p,* q ;
	p = this->Start;
	q = p;
	// now insert the child into the open list according to the f value
	while (p) 
	{
		// insert before p, sorted ascending
		if (p->f_value > curChild->f_value) 	       
		{
//			cout<<"\n Added 1"; fflush(stdout);
			// test head of the list case
			if (p == Start)
			{
				assert(p->next!=NULL);
				Start = curChild;
//				cout<<"\n Added at Start"; fflush(stdout);
				curChild->prev = NULL;
			}
			else
				curChild->prev = p->prev;
			curChild->next = p;			
			if (p->prev)
				(p->prev)->next = curChild;			
			p->prev = curChild;
			break;
		}
		q = p;
		p = p->next;
	}
	if (p == NULL)       
	{
		if (q != NULL) // insert at the end
		{
//			cout<<"\n Added 2"; fflush(stdout);
      		q->next = curChild;
			curChild->prev = q;
			curChild->next = NULL;
		}
		else	      // insert at the beginning
		{
//			cout<<"\n Added to the front";
			this->Start = curChild;
			this->Start->prev = NULL;
			this->Start->next = NULL;
		}
	}			
	p= Start;
	while(p)
	{
		cout<<"\n Pose X:"<<p->pose.p.x()<<" Y:"<<p->pose.p.y();
		p = p->next;
	}
	//cout <<"\n	--->>> Node Added Successfully <<<---"; fflush(stdout);
};
//! Remove a Node from the Linked List
bool LList::remove(Node *q)
{
	Node *p;
	p = Start;
	while (p)
	{
		if(*q==*p)
		{
			//qDebug("I actually removed something");
			if (p->prev != NULL)	        
		   		(p->prev)->next = p->next;
			if (p->next != NULL)	        
				(p->next)->prev = p->prev;								
			if (p == Start)
				Start = p->next;
			delete p;
			return 1;
		}
		p = p->next;
	}
	return 0;	
};
//! Remove a Node from the Linked List according to id
bool LList::remove(int id)
{
	Node *p;
	p = Start;
	while (p)
	{
		if(p->id==id)
		{
			//qDebug("I actually removed something");
			if (p->prev != NULL)	        
		   		(p->prev)->next = p->next;
			if (p->next != NULL)	        
				(p->next)->prev = p->prev;								
			if (p == Start)
				Start = p->next;
			delete p;
			return 1;
		}
		p = p->next;
	}
	return 0;	
};
//! Remove a Node from the Linked List but keep in memory
bool LList::unlink(Node *q)
{
	Node *p;
	p = Start;
	while (p)
	{
		cout<<"\n Unlink"; fflush(stdout);
		if(*q==*p)
		{
			//qDebug("I actually removed something");
			if (p->prev != NULL)	        
		    	(p->prev)->next = p->next;
			if (p->next != NULL)	        
				(p->next)->prev = p->prev;								
			if (p == Start)
				Start = p->next;
			return 1;
		}
		p = p->next;
	}
	return 0;	
};
//! Remove a Node from the Linked List but keep in memory according to id
bool LList::unlink(int id)
{
	Node *p;
	p = Start;
	while (p)
	{
		cout<<"\n Unlink"; fflush(stdout);
		if(p->id==id)
		{
			//qDebug("I actually removed something");
			if (p->prev != NULL)	        
		    	(p->prev)->next = p->next;
			if (p->next != NULL)	        
				(p->next)->prev = p->prev;								
			if (p == Start)
				Start = p->next;
			return 1;
		}
		p = p->next;
	}
	return 0;	
};
//! Print the values of the Nodes in the Linked List.
void LList::print()
{
	Node *p;
	int i=0;
	p = Start;
	while(p)
	{
		cout<<"\n Node["<<++i<<"] X="<<p->pose.p.x()<<" Y="<<p->pose.p.y();
		p = p->next;
	}
};
}
