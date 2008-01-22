#include "OccupancyGrid.h"

CellValue::CellValue()
{
	prob=0.5;
	min_height=10.0;
	max_height=-10.0;
	height_diff=-20.0;  // negative height difference indicates cell is unknown or max<min
}

CellValue::CellValue(float default_value)
{
	prob=default_value;
	min_height=10.0;
	max_height=-10.0;
	height_diff=-20.0;  // negative height difference indicates cell is unknown or max<min
}

void CellValue::UpdateHeight(float height)
{
	if(height>max_height)
		max_height=height;
	if(height<min_height)
		min_height=height;
	height_diff=max_height-min_height;
}

void CellValue::UpdateProbability(int value, float prob_falsepos, float prob_falseneg)
{
	float cell_value, normalize;
	if(value==0)
	{
		cell_value=prob;
		normalize=(1.0-prob_falsepos)*(1.0-cell_value)+prob_falseneg*cell_value;
		prob=1.0-(1.0-prob_falsepos)*(1.0-cell_value)/normalize;
		if(normalize<0.001)
			cout << "Warning: normalize close to zero! cell=" << cell_value << endl;
		if(prob<MIN_PROBABILITY)
			prob=MIN_PROBABILITY;  	// Min out at 0.1 probability - allow objects to move
	}
	else if(value==1)
	{
		cell_value=prob;
		normalize=(1-prob_falseneg)*cell_value+prob_falsepos*(1-cell_value);
		prob=(1-prob_falseneg)*cell_value/normalize;
		if(normalize<0.001)
			cout << "Warning: normalize close to zero! cell=" << cell_value << endl;
		if(prob>MAX_PROBABILITY)
			prob=MAX_PROBABILITY;  // Max out at 0.9 probability - allow objects to move
		if(prob>0.5)
		{// Reset the height value when the cell enters the grid
			min_height=10.0;
			max_height=-10.0;
			height_diff=-20.0;
		}
	}
	else
		cout << "Illegal cell update" << endl;
	
	
}

OccupancyGrid::OccupancyGrid()// ***** NOTE THAT THIS CONSTRUCTOR IS NOT USED AT PRESENT
{
	default_value=0.5;  // default value (probability) is 0.5
	resolution=0.5;		// default resolution is 0.5m
	dimension=121;		// default size is 60mx60m
	worldoffset_x=0;	// World origin is initial position
	worldoffset_y=0;
	prob_falseneg=0.45; // Although not representative of the true probabilities,
	prob_falsepos=0.1;  // these values work well when the update rate is very high	
	vector <CellValue> temprow;
	CellValue tempcell(default_value);
	temprow.assign(dimension,tempcell);
	for(int i=0;i<dimension;i++)
		cell.push_back(temprow);
}

	
OccupancyGrid::OccupancyGrid(float res,int dim) // **** NOTE CHANGE PARAMETERS HERE
{
	default_value=0.5;  // default value (probability) is 0.5
	resolution=res;
	dimension=dim;
	worldoffset_x=0;	// World origin is initial position
	worldoffset_y=0;
	prob_falseneg=0.45;
	prob_falsepos=0.1;
	vector <CellValue> temprow;
	CellValue tempcell(default_value);
	temprow.assign(dimension,tempcell);
	for(int i=0;i<dimension;i++)
		cell.push_back(temprow);
}

OccupancyGrid::~OccupancyGrid()
{
	cell.clear();
}
	
void OccupancyGrid::glDraw(bool display_prob,bool display_height)
{
	int i,j;
	glColor4f(0.4f,0.4f,0.4f,0.4);
	float intensity;
	glBegin(GL_LINES);
	for(i=0;i<dimension+1;i++)
	{
		glVertex2f((i-dimension/2.0)*resolution,-dimension*resolution/2.0);
		glVertex2f((i-dimension/2.0)*resolution,dimension*resolution/2.0);
		glVertex2f(-dimension*resolution/2.0,(i-dimension/2.0)*resolution);
		glVertex2f(dimension*resolution/2.0,(i-dimension/2.0)*resolution);
	}
	glEnd();
	glBegin(GL_QUADS);
//	cout << cell[0][0] << endl;
	for(i=0;i<cell.size();i++)
		for(j=0;j<cell[i].size();j++)
	{
		if(display_prob)
		{	
			glColor4f(cell[i][j].prob,cell[i][j].prob,cell[i][j].prob,0.5);
			glVertex2f((i-dimension/2.0)*resolution,(j-dimension/2.0)*resolution);
			glVertex2f((i-dimension/2.0)*resolution,(j+1.0-dimension/2.0)*resolution);
			glVertex2f((i+1.0-dimension/2.0)*resolution,(j+1.0-dimension/2.0)*resolution);
			glVertex2f((i+1.0-dimension/2.0)*resolution,(j-dimension/2.0)*resolution);
		}
		if(display_height)
		{
			if(cell[i][j].height_diff>0.0)
			{
				if(cell[i][j].height_diff>MIN_OBSTACLEHEIGHT)
					intensity=1.0;
				else
					intensity=cell[i][j].height_diff/MIN_OBSTACLEHEIGHT;
				glColor4f(0.0f,intensity,intensity,0.3); // transparent cyan
				glVertex2f((i-dimension/2.0)*resolution,(j-dimension/2.0)*resolution);
				glVertex2f((i-dimension/2.0)*resolution,(j+1.0-dimension/2.0)*resolution);
				glVertex2f((i+1.0-dimension/2.0)*resolution,(j+1.0-dimension/2.0)*resolution);
				glVertex2f((i+1.0-dimension/2.0)*resolution,(j-dimension/2.0)*resolution);
			}
		}
	}
	glEnd();
}
	
void OccupancyGrid::shift(float x, float y)
{
	int x_int=(int)round(x/resolution);
	int y_int=(int)round(y/resolution);
	int i,j;
	vector <CellValue> temprow;
	CellValue tempcell(default_value);
	temprow.assign(dimension,tempcell);
	if((x_int-worldoffset_x)>0)
		for(i=0;i<x_int-worldoffset_x;i++)
		{
			cell.push_back(temprow);
			cell.erase(cell.begin());
		}
	else if((x_int-worldoffset_x)<0)
		for(i=0;i<worldoffset_x-x_int;i++)
		{
			cell.insert(cell.begin(),temprow);
			cell.pop_back();
		}
	if((y_int-worldoffset_y)>0)
		for(j=0;j<cell.size();j++)
			for(i=0;i<y_int-worldoffset_y;i++)
			{
				cell[j].push_back(tempcell);
				cell[j].erase(cell[j].begin());
			}
	else if((y_int-worldoffset_y)<0)
		for(j=0;j<cell.size();j++)
			for(i=0;i<worldoffset_y-y_int;i++)
			{
				cell[j].insert(cell[j].begin(),tempcell);
				cell[j].pop_back();
			}
	worldoffset_x=x_int;
	worldoffset_y=y_int;
}

void OccupancyGrid::update(LaserScan scan, CarModel car)
{
	int target_cell_x,target_cell_y,current_cell_x,current_cell_y;
	float target_point_x,target_point_y;
	float ray_angle; // Current ray angle in degrees
	float laser_origin_x,laser_origin_y;
	int quadrant; 	// Quadrant containing ray: 1,2,3,4
	float cell_value;
	float normalize;
	float rangestep;
	
	// Proper updates below
	
	laser_origin_x=(car.LaserMountPosition.x*cos(car.orientation.z*M_PI/180)-car.LaserMountPosition.y*sin(car.orientation.z*M_PI/180))/resolution+((float)dimension)/2.0;
	laser_origin_y=(car.LaserMountPosition.x*sin(car.orientation.z*M_PI/180)+car.LaserMountPosition.y*cos(car.orientation.z*M_PI/180))/resolution+((float)dimension)/2.0;

	for(int i=0;i<scan.ranges.size();i++)
	{
		ray_angle=car.orientation.z+scan.start_angle+i*scan.resolution;
		while(ray_angle>=360)
			ray_angle=ray_angle-360;
		while(ray_angle<0)
			ray_angle=ray_angle+360;
		quadrant=1+(int)floor(ray_angle/90.0);
		target_point_x=scan.points[i].x/resolution+((float)dimension)/2.0;
		target_point_y=scan.points[i].y/resolution+((float)dimension)/2.0;
		target_cell_x=(int)floor(target_point_x);
		target_cell_y=(int)floor(target_point_y);
		if((target_cell_x<0)||(target_cell_y<0)||(target_cell_x>dimension)||(target_cell_y>dimension))
			continue; //if target cell is outside the OG, continue without updating ray
		
		current_cell_x=(int)floor(laser_origin_x);
		current_cell_y=(int)floor(laser_origin_y);
		if(scan.ranges[i]<fmax(dimension*resolution/2.0,LASER_MAXRANGE))  // Update only if the scan point is inside the grid and less than MAXRANGE
		{
			while(1) // We haven't reached target cell yet
			{
				// Update the free space - bayesian update rule
				cell[current_cell_x][current_cell_y].UpdateProbability(0,prob_falsepos,prob_falseneg);

				// Move to the next cell in the ray - direction depends on which quadrant we're in
				if(quadrant==1)
				{
					if((current_cell_x+1-laser_origin_x)*(target_point_y-laser_origin_y)<(current_cell_y+1-laser_origin_y)*(target_point_x-laser_origin_x))
						current_cell_x++;
					else
						current_cell_y++;
					if((current_cell_x>=target_cell_x)&&(current_cell_y>=target_cell_y))
						break; // We have reached the target cell
				}
				else if(quadrant==2)
				{
					if((current_cell_x-laser_origin_x)*(target_point_y-laser_origin_y)<(current_cell_y+1-laser_origin_y)*(target_point_x-laser_origin_x))
						current_cell_y++;
					else
						current_cell_x--;
					if((current_cell_x<=target_cell_x)&&(current_cell_y>=target_cell_y))
						break; // We have reached the target cell
				}
				else if(quadrant==3)
				{
					if((current_cell_x-laser_origin_x)*(target_point_y-laser_origin_y)<(current_cell_y-laser_origin_y)*(target_point_x-laser_origin_x))
						current_cell_x--;
					else
						current_cell_y--;
					if((current_cell_x<=target_cell_x)&&(current_cell_y<=target_cell_y))
						break; // We have reached the target cell
				}
				else if(quadrant==4)
				{
					if((current_cell_x+1-laser_origin_x)*(target_point_y-laser_origin_y)<(current_cell_y-laser_origin_y)*(target_point_x-laser_origin_x))
						current_cell_y--;
					else
						current_cell_x++;
					if((current_cell_x>=target_cell_x)&&(current_cell_y<=target_cell_y))
						break; // We have reached the target cell
				}
				else
				{
					cout << "Error in OG update" << endl;
					cout << ray_angle << endl;
					break;
				}
				
				if((current_cell_x==0)||(current_cell_y==0)||(current_cell_x==dimension)||(current_cell_y==dimension))
				{// oops - raytrace went too far
					break;
					cout << "Ray broke OG in update" << endl;
				}
			}
		}
	}
	
	//	Then update the occupied cells if the height is above threshold
	for(int i=0;i<scan.ranges.size();i++)
	{
		if(scan.ranges[i]<fmax(dimension*resolution/2.0,LASER_MAXRANGE))  // Update only if the scan point is inside the grid and less than MAXRANGE
		{
			target_point_x=scan.points[i].x/resolution+((float)dimension-1.0)/2.0;
			target_point_y=scan.points[i].y/resolution+((float)dimension-1.0)/2.0;
			target_cell_x=(int)floor(target_point_x);
			target_cell_y=(int)floor(target_point_y);
			if((target_cell_x<0)||(target_cell_y<0)||(target_cell_x>dimension)||(target_cell_y>dimension))
				continue; //if target cell is outside the OG, continue without updating ray
			
			if(scan.points[i].z>MIN_OBSTACLEHEIGHT)
			{// Update the probabilistic map
				cell[target_cell_x][target_cell_y].UpdateProbability(1,prob_falsepos,prob_falseneg);
			}
			else
			{// Update the height map
				// cell[target_cell_x][target_cell_y].UpdateHeight(scan.points[i].z);
			}
		}
	}
}
