#ifndef OCCGRID_H_
#define OCCGRID_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <vector>
#include "LaserBuffer.h"
#include "CarModel.h"
// #include "laser_scan.h"

#define MAX_GRIDSIZE 256
// Probability thresholds for occupancy grids
#define MIN_PROBABILITY 0.1
#define MAX_PROBABILITY 0.9

class CellValue
{
public:
	float prob;
	double min_height;
	double max_height;
	double height_diff;
	CellValue();
	CellValue(float default_value);
	void UpdateHeight(float height);
	void UpdateProbability(int value, float prob_falsepos, float prob_falseneg);
};

class OccupancyGrid
{
public:
	float 	resolution; 			//Resolution in m
	int 	dimension;				// Width and height of grid in number of cells.
									// If the origin is to be in the centre of a cell, choose an odd number (highly recommended)
									// If the origin is to be on gridlines, choose an even number
	//float	cell[MAX_GRIDSIZE][MAX_GRIDSIZE];	// Being slack and defining it statically
	vector <vector <CellValue> > cell;
	int		worldoffset_x;			// Offset from world origin in number of cells
	int		worldoffset_y;			//
	float default_value;  			//  Default OG value

	float prob_falseneg; 			// Prior probability of false negative 
	float prob_falsepos; 			// Prior probability of false positive
	
	OccupancyGrid();
	
	OccupancyGrid(float res,int dim);
	
	~OccupancyGrid();
	
	void glDraw(bool display_prob, bool display_height);
	
	void shift(float x, float y);  // Shift the map according to the motion of the vehicle
	
	void update(LaserScan scan, CarModel car);	// Update the map according to the laser scan and car pose
};
#endif
