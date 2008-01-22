#include "laser_scan.h"

void ReduceToUnit(Point & vector)					// Reduces A Normal Vector (3 Coordinates)
{									// To A Unit Normal Vector With A Length Of One.
	float length;							// Holds Unit Length
	// Calculates The Length Of The Vector
	length = (float)sqrt((vector.x*vector.x) + (vector.y*vector.y) + (vector.z*vector.z));

	if(length == 0.0f)						// Prevents Divide By 0 Error By Providing
		length = 1.0f;						// An Acceptable Value For Vectors To Close To 0.

	vector.x /= length;						// Dividing Each Element By
	vector.y /= length;						// The Length Results In A
	vector.z /= length;						// Unit Normal Vector.
}

void transformMult(float out[4], float mat[4][4], float in[4])  //Matrix multiplication: out=mat*in  
{
	out[0]=mat[0][0]*in[0]+mat[0][1]*in[1]+mat[0][2]*in[2]+mat[0][3]*in[3];
	out[1]=mat[1][0]*in[0]+mat[1][1]*in[1]+mat[1][2]*in[2]+mat[1][3]*in[3];
	out[2]=mat[2][0]*in[0]+mat[2][1]*in[1]+mat[2][2]*in[2]+mat[2][3]*in[3];
	out[3]=mat[3][0]*in[0]+mat[3][1]*in[1]+mat[3][2]*in[2]+mat[3][3]*in[3];
}

inline void drawGlPoint(Point p)
{
	glVertex3f((float) p.x,(float)p.y,(float)p.z);
}

void GLDisplayLaserScan(void)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear Screen And Depth Buffer
	glEnable(GL_POINT_SMOOTH);
	glMatrixMode(GL_MODELVIEW);

	float transformMatrix[4][4];
	float transformedVec[4];
	float inputVec[4];
	
	int i=0;
	float x,y,range,theta;
	Point current_point;
	
	long seconds,useconds;
	
	glPointSize(display.pointsize);

	//Read data from the logfiles and display
	current_clock=clock();

	if((double)(current_clock-previous_clock)/CLOCKS_PER_SEC>(timestamp_laserCurrent-timestamp_laserLast))
	{
		previous_clock=current_clock;
		if(!laserLogfile.eof())
		{		
			getline(laserLogfile,dummybuffer); //Goto next line in logfile
			theta=-90;
			for(i=0;i<361;i++)
			{
				laserLogfile >> range;
				if(range>2.0)
				{
					currentScan.points[i].x=range*cos(theta*M_PI/180);
					currentScan.points[i].y=range*sin(theta*M_PI/180);
					currentScan.points[i].z=0.0;
					currentScan.ranges[i]=range;
				}
				else
				{
					currentScan.points[i].x=LASER_MAXRANGE;
					currentScan.points[i].y=LASER_MAXRANGE;
					currentScan.points[i].z=LASER_MAXRANGE;
					currentScan.ranges[i]=LASER_MAXRANGE;
				}
				theta+=0.5;
			}
			getline(laserLogfile,dummybuffer);
			getline(laserLogfile,dummybuffer); //Line of zeroes
			
			if(timestamp_localizeCurrent>timestamp_laserCurrent)
			{
				// set localize equal to the next value from the file
				getline(localizeLogfile,dummybuffer); //row containing 1
				getline(localizeLogfile,dummybuffer); //row containing 1
				localize_last.x=localize.x;localize_last.y=localize.y;localize_last.z=localize.z;localize_last.pitch=localize.pitch;localize_last.roll=localize.roll;localize_last.yaw=localize.yaw;
				localizeLogfile >> localize.x >> localize.y >> localize.z >> localize.pitch >> localize.roll >> localize.yaw;
				getline(localizeLogfile,dummybuffer);
				timestamp_localizeLast=timestamp_localizeCurrent;
				localizeLogfile >> seconds >> useconds;
				timestamp_localizeCurrent=seconds+(double)useconds/1000000;
			}
			else
			{
				// else keep getting values until timestamp_localizeCurrent>timestamp_laserCurrent
				while(timestamp_localizeCurrent<timestamp_laserCurrent)
				{
					getline(localizeLogfile,dummybuffer); //row containing 1
					getline(localizeLogfile,dummybuffer); //row containing 1
					localize_last.x=localize.x;localize_last.y=localize.y;localize_last.z=localize.z;localize_last.pitch=localize.pitch;localize_last.roll=localize.roll;localize_last.yaw=localize.yaw;
					localizeLogfile >> localize.x >> localize.y >> localize.z >> localize.pitch >> localize.roll >> localize.yaw;
					getline(localizeLogfile,dummybuffer);
					timestamp_localizeLast=timestamp_localizeCurrent;
					localizeLogfile >> seconds >> useconds;
					timestamp_localizeCurrent=seconds+(double)useconds/1000000;
				}
				// and choose the localize value according to the closest timestamp
				if((timestamp_laserCurrent-timestamp_localizeLast)>(timestamp_laserCurrent-timestamp_localizeCurrent))
				{
					getline(localizeLogfile,dummybuffer); //row containing 1
					getline(localizeLogfile,dummybuffer); //row containing 1
					localize_last.x=localize.x;localize_last.y=localize.y;localize_last.z=localize.z;localize_last.pitch=localize.pitch;localize_last.roll=localize.roll;localize_last.yaw=localize.yaw;
					localizeLogfile >> localize.x >> localize.y >> localize.z >> localize.pitch >> localize.roll >> localize.yaw;
					getline(localizeLogfile,dummybuffer);
				}	
			}
			laserLogfile >> seconds >> useconds;
			timestamp_laserLast=timestamp_laserCurrent;
			timestamp_laserCurrent=seconds+(double)useconds/1000000;
			car.orientation.x=localize.pitch;
			car.orientation.y=localize.roll;
			car.orientation.z=localize.yaw-localize_initial.yaw;
			car.position.x=(localize.x-localize_initial.x)*cos(M_PI*localize_initial.yaw/180.0)+(localize.y-localize_initial.y)*sin(M_PI*localize_initial.yaw/180.0);
			car.position.y=-(localize.x-localize_initial.x)*sin(M_PI*localize_initial.yaw/180.0)+(localize.y-localize_initial.y)*cos(M_PI*localize_initial.yaw/180.0);
			car.position.z=localize.z-localize_initial.z;

			//cout << car.position.x << "  " << car.position.y << "  " << endl;
			//cout << localize.x << "  " << localize.y << "  " << "  " << localize_initial.yaw << endl;
					
			currentScan.rotate(car.LaserMountOrientation.x,car.LaserMountOrientation.y,car.LaserMountOrientation.z); // Adjust for the position and orientation of the laser
			currentScan.translate(car.LaserMountPosition.x,car.LaserMountPosition.y,car.LaserMountPosition.z);
			currentScan.rotate(0.0,0.0,car.orientation.z);

			grid.shift(car.position.x,car.position.y);  // Shift the OG according to latest localize
			grid.update(currentScan,car);					// Update OG with latest scan
			//currentScan.rotate(car.orientation.x,car.orientation.y,car.orientation.z);
			//currentScan.translate(car.position.x,car.position.y,car.position.z); //  Only if you want a moving display
		}
	}

	currentScan.glDraw();						// Draw the laser points

	car.glDraw(display.transparency);  			// Draw the car model
		
	grid.glDraw(display.prob,display.height);								// Draw the occupancy grid outline
	
	glColor4f(0.3f,0.3f,0.3f,display.transparency);
	glBegin(GL_LINE_LOOP);						// Drawing Reference Circles
	for(i=0; i<500; i++)
	{
		theta=PI*(i-250)/250;
		x=(float)10*sin(theta);
		y=(float)10*cos(theta);
		glVertex2f((float)x,(float)y);				
	}
	glEnd();							

	glBegin(GL_LINE_LOOP);						// Drawing Reference Circles
	for(i=0; i<500; i++)
	{
		theta=PI*(i-250)/250;
		x=(float)20*sin(theta);
		y=(float)20*cos(theta);
		glVertex2f((float)x,(float)y);				
	}
	glEnd();						

	glBegin(GL_LINE_LOOP);						// Drawing Reference Circles
	for(i=0; i<500; i++)
	{
		theta=PI*(i-250)/250;
		x=(float)30*sin(theta);
		y=(float)30*cos(theta);
		glVertex2f((float)x,(float)y);				
	}
	glEnd();						

	glBegin(GL_LINES);						// Drawing Axes
	glColor4f(0.3f,0.3f,0.8f,display.transparency);
	glVertex2f(40.0,0.0);
	glVertex2f(0.0,0.0);
	glColor4f(0.3f,0.3f,0.3f,display.transparency);
	glVertex2f(0.0,0.0);
	glVertex2f(-40.0,0.0);
	glEnd();						

	glBegin(GL_LINES);						// Drawing Axes
	glVertex3f(0.0,0.0,-40.0);				
	glVertex3f(0.0,0.0,0.0);
	glColor4f(0.8f,0.3f,0.3f,display.transparency);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,0.0,40.0);
	glColor4f(0.3f,0.3f,0.3f,display.transparency);
	glEnd();

	glBegin(GL_LINES);						// Drawing Axes
	glVertex2f(0.0,40.0);				
	glVertex2f(0.0,-40.0);				
	glEnd();						
	glPopMatrix();								// Store The Modelview Matrix

	if(display.light)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	glutSwapBuffers( );//
}

void glutKeyboardUpCallback(unsigned char key, int x, int y)
{
	//printf("keyup=%c\n",(char)key);
	keystate[key]=true;
	switch(key){
		case '<':
			display.pointsize--;
			if(display.pointsize<1)
				display.pointsize=1;
			break;
		case '>':
			display.pointsize++;
			if(display.pointsize>MAX_POINTSIZE)
				display.pointsize=MAX_POINTSIZE;
			break;
	}
}

void glutKeyboardCallback(unsigned char key, int x, int y)
{
/*	printf("key=%d\n",key);
	printf("key=%c\n",key);*/
	keystate[key]=true;
	switch(key){
		case '[':
			display.pointsize--;
			if(display.pointsize<1)
				display.pointsize=1;
			break;
		case ']':
			display.pointsize++;
			if(display.pointsize>MAX_POINTSIZE)
				display.pointsize=MAX_POINTSIZE;
			break;
	}
}

void RefreshData(void)
{
	glutPostRedisplay( );
}

void parse_args(int argc, char** argv)
{
	int i;
	
	i=1;
	while(i<argc)
	{
		if(!strcmp(argv[i],"-o"))
		{
			if(++i<argc)
				strcpy(out_filename, argv[i]);
			else
			{
				puts(USAGE);
				exit(1);
			}
		}
		else if(!strcmp(argv[i],"-i"))
		{
			if(++i<argc)
			{
				strcpy(laser_filename, argv[i]);
				strcat(laser_filename, "_laserscanner2d0.log");
				strcpy(localize_filename, argv[i]);
				strcat(localize_filename, "_localise3d0.log");
			}
			else
			{
				puts(USAGE);
				exit(1);
			}
		}
		else
		{
			puts(USAGE);
			exit(1);
		}
		i++;
	}
}

int main(int argc, char **argv)
{
	// Setup the display data structure
	display.points=true;
	display.height=true;
	display.prob=true;
	display.light=true;
	display.view=OPAQUE;
	display.sparsity=1;
	display.transparency=0.7;
	display.pointsize=3;
	
	// Setup the laser offset data structure
	
	// Values for Sydney setup
/*	car.LaserMountPosition.x=1.18;
	car.LaserMountPosition.y=0.0;
	car.LaserMountPosition.z=1.96;
	car.LaserMountOrientation.x=-7.0; // pitch is 7 degrees towards the road
	car.LaserMountOrientation.y=0.0;
	car.LaserMountOrientation.z=0.0;*/
	
	// Values for Berkeley setup
	car.LaserMountPosition.x=1.19;
	car.LaserMountPosition.y=0.00;
	car.LaserMountPosition.z=1.92;
	car.LaserMountOrientation.x=-7.85; // pitch is 7 degrees towards the road
	car.LaserMountOrientation.y=0.0;
	car.LaserMountOrientation.z=0.0;

	//Parse command line arguments
	parse_args(argc,argv);

	long seconds, useconds;
	//Open the logfiles
	laserLogfile.open(laser_filename,fstream::in);
	localizeLogfile.open(localize_filename,fstream::in);
	for(int i=0;i<15;i++)
	{
		getline(laserLogfile,dummybuffer);
	}
	laserLogfile >> seconds >> useconds;
	timestamp_laserCurrent=seconds+(double)useconds/1000000;
	timestamp_laserLast=seconds+(double)useconds/1000000;
	
	localizeLogfile >> seconds >> useconds;
	timestamp_localizeCurrent=seconds+(double)useconds/1000000;
	timestamp_localizeLast=seconds+(double)useconds/1000000;
	getline(localizeLogfile,dummybuffer); 
	getline(localizeLogfile,dummybuffer); //row containing 1
	localizeLogfile >> localize_initial.x >> localize_initial.y >> localize_initial.z >> localize_initial.pitch >> localize_initial.roll >> localize_initial.yaw;
	localizeLogfile.seekg (0, ios::beg);
	getline(localizeLogfile,dummybuffer);
	getline(localizeLogfile,dummybuffer); //row containing 1
	localizeLogfile >> localize.x >> localize.y >> localize.z >> localize.pitch >> localize.roll >> localize.yaw;
	getline(localizeLogfile,dummybuffer);
	
	previous_clock=clock();
	

	//Setup OpenGL window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	//glOrtho(-50.0, 50.0, -50.0, 50.0, -50.0, 50.0);   // setup a 1001.0f001.0f00 viewing world
	glutInitWindowPosition(50,50);
	glutInitWindowSize(600,600);
	glutCreateWindow("LaserScan");
	glClearColor(0.0, 0.0, 0.0, 0.0);         // black background
	glutDisplayFunc(GLDisplayLaserScan);
	glutKeyboardFunc(glutKeyboardCallback);
	glutKeyboardUpFunc(glutKeyboardUpCallback);
 	glutIdleFunc(RefreshData);

	glScalef(0.03,0.03,0.03);
	gluLookAt(0.0,0.0,1.0,0.0,0.0,0.0,1.0,0.0,0.0);
	/* Configure ZPR module */

	zprInit();

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
	
	glPolygonStipple(mesh);
	//glEnable(GL_POLYGON_STIPPLE);
	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS);
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glutMainLoop( );

	laserLogfile.close();
	localizeLogfile.close();
	
	return 0;
}
