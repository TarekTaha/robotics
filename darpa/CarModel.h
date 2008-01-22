#ifndef CARMODEL_H_
#define CARMODEL_H_
#include "LaserBuffer.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

class CarModel
{
public:
	Point rear[8];
	Point front[8];
	Point LaserMountPosition;		// Laser mount position (x,y,z) in m
	Point LaserMountOrientation;	// Laser orientation in (pitch, roll, yaw) in degrees
	Point orientation;				// Car orientation wrt original position (pitch, roll, yaw) in degrees
	Point position;					// Car position wrt original position (x,y,z) in m
	CarModel();
	~CarModel();
	void glDraw(float transparency);
};

#endif
