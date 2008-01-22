#include "CarModel.h"

CarModel::CarModel() // Default car is 3x2x2m, with origin centre 1m from the rear
{
	rear[0].setXYZ(-1.0,-1.0,0.0);rear[1].setXYZ(-1.0, 1.0,0.0);
	rear[2].setXYZ(-1.0,-1.0,2.0);rear[3].setXYZ(-1.0, 1.0,2.0);
	rear[4].setXYZ( 1.0,-1.0,0.0);rear[5].setXYZ( 1.0, 1.0,0.0);
	rear[6].setXYZ( 1.0,-1.0,2.0);rear[7].setXYZ( 1.0, 1.0,2.0);
	front[0].setXYZ( 1.0,-1.0,0.0);front[1].setXYZ( 1.0, 1.0,0.0);
	front[2].setXYZ( 1.0,-1.0,1.0);front[3].setXYZ( 1.0, 1.0,1.0);
	front[4].setXYZ( 2.0,-1.0,0.0);front[5].setXYZ( 2.0, 1.0,0.0);
	front[6].setXYZ( 2.0,-1.0,1.0);front[7].setXYZ( 2.0, 1.0,1.0);
	orientation.x=0.0;orientation.y=0.0;orientation.z=0.0;
	position.x=0.0;position.y=0.0;position.z=0.0;
	LaserMountPosition.x=0.0;LaserMountPosition.y=0.0;LaserMountPosition.z=0.0;
	LaserMountOrientation.x=0.0;LaserMountOrientation.y=0.0;LaserMountOrientation.z=0.0;
}

CarModel::~ CarModel()
{
}

void CarModel::glDraw(float transparency)
{
	Point worldrear[8];
	Point worldfront[8];
	for(int i=0;i<8;i++)
	{
		worldrear[i]=rear[i];
		worldrear[i].rotate(orientation.x,orientation.y,orientation.z);
		//worldrear[i].translate(position.x,position.y,position.z);
		worldfront[i]=front[i];
		worldfront[i].rotate(orientation.x,orientation.y,orientation.z);
		//worldfront[i].translate(position.x,position.y,position.z);
	}
	glBegin(GL_QUADS); //Draw representation of car
	glColor4f(0.7f,0.2f,0.2f,transparency);			// Red
	glVertex3f(worldrear[0].x,worldrear[0].y,worldrear[0].z);glVertex3f(worldrear[1].x,worldrear[1].y,worldrear[1].z);glVertex3f(worldrear[5].x,worldrear[5].y,worldrear[5].z);glVertex3f(worldrear[4].x,worldrear[4].y,worldrear[4].z);
	glVertex3f(worldrear[2].x,worldrear[2].y,worldrear[2].z);glVertex3f(worldrear[3].x,worldrear[3].y,worldrear[3].z);glVertex3f(worldrear[7].x,worldrear[7].y,worldrear[7].z);glVertex3f(worldrear[6].x,worldrear[6].y,worldrear[6].z);
	glVertex3f(worldrear[0].x,worldrear[0].y,worldrear[0].z);glVertex3f(worldrear[1].x,worldrear[1].y,worldrear[1].z);glVertex3f(worldrear[3].x,worldrear[3].y,worldrear[3].z);glVertex3f(worldrear[2].x,worldrear[2].y,worldrear[2].z);
	glVertex3f(worldrear[4].x,worldrear[4].y,worldrear[4].z);glVertex3f(worldrear[5].x,worldrear[5].y,worldrear[5].z);glVertex3f(worldrear[7].x,worldrear[7].y,worldrear[7].z);glVertex3f(worldrear[6].x,worldrear[6].y,worldrear[6].z);
	glVertex3f(worldrear[0].x,worldrear[0].y,worldrear[0].z);glVertex3f(worldrear[4].x,worldrear[4].y,worldrear[4].z);glVertex3f(worldrear[6].x,worldrear[6].y,worldrear[6].z);glVertex3f(worldrear[2].x,worldrear[2].y,worldrear[2].z);
	glVertex3f(worldrear[1].x,worldrear[1].y,worldrear[1].z);glVertex3f(worldrear[5].x,worldrear[5].y,worldrear[5].z);glVertex3f(worldrear[7].x,worldrear[7].y,worldrear[7].z);glVertex3f(worldrear[3].x,worldrear[3].y,worldrear[3].z);

	glColor4f(0.2f,0.2f,0.7f,transparency);			// blue
	glVertex3f(worldfront[0].x,worldfront[0].y,worldfront[0].z);glVertex3f(worldfront[1].x,worldfront[1].y,worldfront[1].z);glVertex3f(worldfront[5].x,worldfront[5].y,worldfront[5].z);glVertex3f(worldfront[4].x,worldfront[4].y,worldfront[4].z);
	glVertex3f(worldfront[2].x,worldfront[2].y,worldfront[2].z);glVertex3f(worldfront[3].x,worldfront[3].y,worldfront[3].z);glVertex3f(worldfront[7].x,worldfront[7].y,worldfront[7].z);glVertex3f(worldfront[6].x,worldfront[6].y,worldfront[6].z);
	glVertex3f(worldfront[0].x,worldfront[0].y,worldfront[0].z);glVertex3f(worldfront[1].x,worldfront[1].y,worldfront[1].z);glVertex3f(worldfront[3].x,worldfront[3].y,worldfront[3].z);glVertex3f(worldfront[2].x,worldfront[2].y,worldfront[2].z);
	glVertex3f(worldfront[4].x,worldfront[4].y,worldfront[4].z);glVertex3f(worldfront[5].x,worldfront[5].y,worldfront[5].z);glVertex3f(worldfront[7].x,worldfront[7].y,worldfront[7].z);glVertex3f(worldfront[6].x,worldfront[6].y,worldfront[6].z);
	glVertex3f(worldfront[0].x,worldfront[0].y,worldfront[0].z);glVertex3f(worldfront[4].x,worldfront[4].y,worldfront[4].z);glVertex3f(worldfront[6].x,worldfront[6].y,worldfront[6].z);glVertex3f(worldfront[2].x,worldfront[2].y,worldfront[2].z);
	glVertex3f(worldfront[1].x,worldfront[1].y,worldfront[1].z);glVertex3f(worldfront[5].x,worldfront[5].y,worldfront[5].z);glVertex3f(worldfront[7].x,worldfront[7].y,worldfront[7].z);glVertex3f(worldfront[3].x,worldfront[3].y,worldfront[3].z);
	glEnd();
}
