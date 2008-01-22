#include "LaserBuffer.h"

Point::Point()
{
		//printf("Point Initialized\n"); fflush(stdout);
	this->x = this->y = this->z =0;
};

Point::Point(float x ,float y,float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Point::setXYZ(float x ,float y,float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

void Point::rotate(float pitch,float roll,float yaw)  // rotation about the world centred axes: yaw,pitch,roll
{// angles in degrees
	float temp;
	if(yaw!=0.0)
	{
		temp=(this->x)*cos(yaw*M_PI/180)-(this->y)*sin(yaw*M_PI/180);
		this->y=(this->y)*cos(yaw*M_PI/180)+(this->x)*sin(yaw*M_PI/180);
		this->x=temp;
	}
	if(pitch!=0.0)
	{
		this->x=(this->x)*cos(pitch*M_PI/180)-(this->z)*sin(pitch*M_PI/180);
		this->z=(this->z)*cos(pitch*M_PI/180)+(this->x)*sin(pitch*M_PI/180);
	}
	if(roll!=0.0)
	{
		this->z=(this->z)*cos(roll*M_PI/180)-(this->y)*sin(roll*M_PI/180);
		this->y=(this->y)*cos(roll*M_PI/180)+(this->z)*sin(roll*M_PI/180);
	}
}
	
void Point::translate(float trans_x,float trans_y,float trans_z)  // translation
{
	x=x+trans_x;
	y=y+trans_y;
	z=z+trans_z;
}

Point& Point::operator=(const Point &ps)
{
	this->x = ps.x;
	this->y = ps.y;
	this->z = ps.z;
	return * this;
}

bool Point::operator==(const Point& point) const
{
	return ((this->x== point.x) && (this->y==point.y) && (this->z==point.z));
}

LaserScan::LaserScan(){};

LaserScan::~LaserScan()
{
	points.clear();
}

LaserScan::LaserScan(int laserMaxPoints)
{
	start_angle=-90.0;
	resolution=0.5;
	for(int i=0;i<laserMaxPoints;i++)
	{
		Point p(0,0,0);
		points.push_back(p);
		ranges.push_back(0.0);
	}
}

void LaserScan::rotate(float pitch, float roll, float yaw) // rotate about world centred axes
{
	for(int i=0;i<points.size();i++)
		points[i].rotate(pitch,roll,yaw);
}
	
void LaserScan::translate(float x, float y, float z) // translate
{
	for(int i=0;i<points.size();i++)
		points[i].translate(x,y,z);
}

void LaserScan::glDraw()
{
	glBegin(GL_POINTS);		// Drawing laser points
	for(int i=0;i<361;i++)
	{
		if(points[i].z<MIN_OBSTACLEHEIGHT) // Display points below 60cm height in green, else in yellow
			glColor4f(0.3f,0.8f,0.3f,1.0f);
		else
			glColor4f(0.7f,0.7f,0.3f,1.0f);
		if(fabs(points[i].x)+fabs(points[i].y)+fabs(points[i].z)>0.1)
			glVertex3f(points[i].x,points[i].y,points[i].z);
	}
	glEnd();
}


