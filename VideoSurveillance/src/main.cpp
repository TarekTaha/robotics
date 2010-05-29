 
/**
* Display the video from webcam and automatically save the frame
* every 3 seconds.
*/

#include <stdio.h>
#include "cv.h"
#include "highgui.h"

int main(int argc, char** argv)
{

    CvCapture *capture;
    IplImage  *frame;
    double    t, ms = 0;

    bool finished = false;
    /* initialize webcam */
    capture = cvCaptureFromCAM(0);

    //CvFont myFont;

    //Initialise font
    //cvInitFont(&myFont,CV_FONT_HERSHEY_COMPLEX_SMALL ,1.5f,1.5f,0,1,8);

    while (!finished)
    {
        t = (double)cvGetTickCount();

        /* display video */
        frame = cvQueryFrame(capture);
       //cvPutText( frame, "Palestine", cvPoint(50,50), &myFont , cvScalar(128) );
        if(!frame)
            continue;
        /* get elapsed time */
        t = (double)cvGetTickCount() - t;
        ms += t/((double)cvGetTickFrequency() * 1000.0);

        /* autosave every 3 seconds */
        if (ceil(ms) >= 2000)
        {
            cvSaveImage("../webClient/img.jpg", frame);
            ms = 0;
        }
    }

    /* free memory */
    cvReleaseCapture(&capture);
    return 0;
}
