all: 3dscanner

clean:
	-rm -f *.o *.so *.*~
3dscanner: 
	#echo 
	g++ -c laser_scan.cc
	g++ -c zpr.c
	g++ -c LaserBuffer.cpp
	g++ -c OccupancyGrid.cpp
	g++ -c CarModel.cpp
	g++  CarModel.o OccupancyGrid.o LaserBuffer.o laser_scan.o zpr.o -o laser_scan -lglut `pkg-config --libs --cflags playercore`
