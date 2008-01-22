#ifndef LASERBUFFER_H_
#define LASERBUFFER_H_
#include <vector>
#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define LASER_MAXRANGE 80.0
#define MIN_OBSTACLEHEIGHT 0.4

using namespace std;
class Point
{	
public : 
	float x;
	float y;
	float z;
	Point();
	Point(float x ,float y,float z);
	void setXYZ(float x ,float y,float z);
	void rotate(float pitch,float roll,float yaw);  // rotation about the world centred axes: yaw,pitch,roll
	void translate(float trans_x,float trans_y,float trans_z);  // translation
	Point & operator=(const Point &ps);
	bool operator==(const Point& point) const;
};


class LaserScan
{	
public : 
	vector <Point>	points; 	// 3D cartesian locations of the points in world frame
	vector <float>	ranges; 	// raw range data
	float resolution; 			// angular resolution in degrees
	float start_angle;			// scan initial angle in degrees
	LaserScan();
	~LaserScan();
	LaserScan(int laserMaxPoints);
	void rotate(float pitch, float roll, float yaw); // rotate about world centred axes
	void translate(float x, float y, float z); // translate
	void glDraw();
};

#endif /*LASERBUFFER_H_*/
