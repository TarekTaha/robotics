/*
        Description: Implements the recognition using embedded
        hidden markov model for face recognition
*/

#ifndef _RecognizeEHMM_H_
#define _RecognizeEHMM_H_

#include <cxcore.h>
#include <cv.h>
#include <cvaux.h>

//Forward declarations
class EHMMObj;
class EHMMObjDatabase;

float RecognizeEHMM( IplImage &img, EHMMObj &ehmmObj, int obsWidth, int obsHeight,
					int noDCTCoeffX, int noDCTCoeffY, int stepX, int stepY, 
					bool suppressIntensity = false );

size_t RecognizeEHMM( IplImage &img, EHMMObjDatabase &ehmmObjDb, int obsWidth, int obsHeight,
					 int noDCTCoeffX, int noDCTCoeffY, int stepX, int stepY, 
					 bool suppressIntensity = false );

#endif //_RecognizeEHMM_H_
