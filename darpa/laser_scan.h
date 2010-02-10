#ifndef LASERSCAN_H_
#define LASERSCAN_H_

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <time.h>
#include <cstring>
#include <cstdlib>

using namespace std;

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>



#include "zpr.h"
#include "LaserBuffer.h"
#include "OccupancyGrid.h"
#include "CarModel.h"

#define USAGE \
  "USAGE: -i timestamp will use the dataset timestamp_localize3d0.log etc" \

#ifndef MIN
#define MIN(a,b) ((a < b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a > b) ? (a) : (b))
#endif


#define absMax3(a) fmax(fabs((a).x),fmax(fabs((a).y),fabs((a).z)))

#define maxDiff(a,b,c) fmax(fmax(fabs((a).x-(b).x),fmax(fabs((a).y-(b).y),fabs((a).z-(b).z))), \
			fmax(fmax(fabs((c).x-(b).x),fmax(fabs((c).y-(b).y),fabs((c).z-(b).z))), \
			fmax(fabs((a).x-(c).x),fmax(fabs((a).y-(c).y),fabs((a).z-(c).z)))))

#define PI M_PI

#define TRANSPARENCY 0
#define MESH 1
#define OPAQUE 2
#define MAX_POINTSIZE 10

struct displayinfo
{
	bool light;
	int view;
	bool points;	// Display points or not
	bool height;	// Display height map or now
	bool prob;	// Display probability map or not
	int sparsity;
	int pointsize;
	float transparency;
};


struct localizeinfo
{
	double x; // longitude: degrees east
	double y; // latitude: degrees north
	double z; // m above sea level
	double pitch; //degrees
	double roll; //degrees
	double yaw; //degrees
};

displayinfo display;
// laseroffsetinfo laseroffset;
localizeinfo localize,localize_last,localize_initial;

static GLfloat light_ambient[]  = { 0.3, 0.3, 0.3, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_position[] = { 0.0, 0.0, 3.0, 1.0 };

static GLfloat mat_ambient[]    = { 0.7, 0.7, 0.7, 1.0 };
static GLfloat mat_diffuse[]    = { 0.8, 0.8, 0.8, 1.0 };
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat high_shininess[] = { 100.0 };

// Stipple bitmap
GLubyte mesh[] = { 0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55,
	0x55, 0x55, 0x55, 0x55 };

static bool keystate[256];
LaserScan currentScan(361), lastScan(361);//, car(14);
OccupancyGrid grid(0.5,121);
CarModel car;

fstream laserLogfile, localizeLogfile;
string laserbuffer, localizebuffer, dummybuffer;
char out_filename[128]="output";
char laser_filename[128]="070610_103525_laserscanner2d0.log";
char localize_filename[128]="070610_103525_localise3d0.log";
double timestamp_laserCurrent=0.0, timestamp_laserLast=0.0, timestamp_localizeCurrent=0.0, timestamp_localizeLast=0.0;
clock_t current_clock,previous_clock;
			
#endif /*LASERSCAN_H_*/
