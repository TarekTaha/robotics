#include "EHMMObjDatabase.h"
#include "ImgObjDatabase.h"
#include "EHMMObjRecognition.h"
#include <iostream>
#include <string>
#include "highgui.h"

using namespace std;

string PATH_CV_IMG_DB    = "/home/BlackCoder/workspace/FaceRecognition/DATABASE/CvImgDB/";
string PATH_CV_EHMM_DB   = "/home/BlackCoder/workspace/FaceRecognition/DATABASE/CvEhmmDB/";
const char* cascade_name = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";

//Training/Recognition parameters
const int IMG_WIDTH = 100;
const int IMG_HEIGHT = 0;
const int OBS_WIDTH = 12;
const int OBS_HEIGHT = 12;
const int NO_DCT_COEFF_X = 4;
const int NO_DCT_COEFF_Y = 4;
const int STEP_X = 4;
const int STEP_Y = 4;
const bool SUPPRESS_INTESITY = true;

string recognizedPersonName = "Unknown";

//*****************************************************************************

void CreateOpenCVTeamImgDb( const string &pathCvImgDb,
						   const string &imgCvDbName )
{
        const string &PATH_IMAGES = pathCvImgDb + "Images/";

	ImgObjDatabase imgDb;

	//Create the database
	imgDb.Create( imgCvDbName );

	//Add all the users
	imgDb.AddObj( "Sergey.Molinov" );
	imgDb.AddObj( "Vadim.Pisarevsky" );
	imgDb.AddObj( "Valery.Cherepennikov" );
	imgDb.AddObj( "Valery.Mosyagin" );
	imgDb.AddObj( "Victor.Eruhimov" );
        imgDb.AddObj( "Tarek.Taha" );

	//Add all the images of the users
	imgDb.GetObj( 0 ).AddImage( PATH_IMAGES + "Sergey.Molinov0000.bmp" );
	imgDb.GetObj( 0 ).AddImage( PATH_IMAGES + "Sergey.Molinov0001.bmp" );
	imgDb.GetObj( 0 ).AddImage( PATH_IMAGES + "Sergey.Molinov0002.bmp" );
	imgDb.GetObj( 0 ).AddImage( PATH_IMAGES + "Sergey.Molinov0003.bmp" );
	imgDb.GetObj( 0 ).AddImage( PATH_IMAGES + "Sergey.Molinov0004.bmp" );
	imgDb.GetObj( 0 ).AddImage( PATH_IMAGES + "Sergey.Molinov0005.bmp" );

	imgDb.GetObj( 1 ).AddImage( PATH_IMAGES + "Vadim.Pisarevsky0000.bmp" );
	imgDb.GetObj( 1 ).AddImage( PATH_IMAGES + "Vadim.Pisarevsky0001.bmp" );
	imgDb.GetObj( 1 ).AddImage( PATH_IMAGES + "Vadim.Pisarevsky0002.bmp" );
	imgDb.GetObj( 1 ).AddImage( PATH_IMAGES + "Vadim.Pisarevsky0003.bmp" );
	imgDb.GetObj( 1 ).AddImage( PATH_IMAGES + "Vadim.Pisarevsky0004.bmp" );
	imgDb.GetObj( 1 ).AddImage( PATH_IMAGES + "Vadim.Pisarevsky0005.bmp" );

	imgDb.GetObj( 2 ).AddImage( PATH_IMAGES + "Valery.Cherepennikov0000.bmp" );
	imgDb.GetObj( 2 ).AddImage( PATH_IMAGES + "Valery.Cherepennikov0001.bmp" );
	imgDb.GetObj( 2 ).AddImage( PATH_IMAGES + "Valery.Cherepennikov0002.bmp" );
	imgDb.GetObj( 2 ).AddImage( PATH_IMAGES + "Valery.Cherepennikov0003.bmp" );
	imgDb.GetObj( 2 ).AddImage( PATH_IMAGES + "Valery.Cherepennikov0004.bmp" );
	imgDb.GetObj( 2 ).AddImage( PATH_IMAGES + "Valery.Cherepennikov0005.bmp" );

	imgDb.GetObj( 3 ).AddImage( PATH_IMAGES + "Valery.Mosyagin0000.bmp" );
	imgDb.GetObj( 3 ).AddImage( PATH_IMAGES + "Valery.Mosyagin0001.bmp" );
	imgDb.GetObj( 3 ).AddImage( PATH_IMAGES + "Valery.Mosyagin0002.bmp" );
	imgDb.GetObj( 3 ).AddImage( PATH_IMAGES + "Valery.Mosyagin0003.bmp" );
	imgDb.GetObj( 3 ).AddImage( PATH_IMAGES + "Valery.Mosyagin0004.bmp" );
	imgDb.GetObj( 3 ).AddImage( PATH_IMAGES + "Valery.Mosyagin0005.bmp" );

	imgDb.GetObj( 4 ).AddImage( PATH_IMAGES + "Victor.Eruhimov0000.bmp" );
	imgDb.GetObj( 4 ).AddImage( PATH_IMAGES + "Victor.Eruhimov0001.bmp" );
	imgDb.GetObj( 4 ).AddImage( PATH_IMAGES + "Victor.Eruhimov0002.bmp" );
	imgDb.GetObj( 4 ).AddImage( PATH_IMAGES + "Victor.Eruhimov0003.bmp" );
	imgDb.GetObj( 4 ).AddImage( PATH_IMAGES + "Victor.Eruhimov0004.bmp" );
	imgDb.GetObj( 4 ).AddImage( PATH_IMAGES + "Victor.Eruhimov0005.bmp" );

        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0006.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0007.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0008.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0009.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0010.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0011.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0012.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0013.bmp" );
        imgDb.GetObj( 5 ).AddImage( PATH_IMAGES + "Tarek.Taha0014.bmp" );

	cout << endl << "CreateOpenCVTeamImgDb: Created '" << imgCvDbName << "'." << endl << endl;
	cout << imgDb;

	imgDb.Save( pathCvImgDb );
}

//*****************************************************************************

void LoadImgDb( const string &pathCvImgDb, const string &imgDbName )
{
	ImgObjDatabase imgDb;

	imgDb.Load( imgDbName, pathCvImgDb );

	cout << endl <<"LoadImgDb: Loaded '" << imgDbName << "'." << endl << endl;
	cout << imgDb;

	for ( size_t i = 0; i < imgDb.GetNoObjs( ); i++ )
	{
		for ( size_t j = 0; j < imgDb.GetObj( i ).GetNoImages( ); j++ )
		{
			IplImage *img = imgDb.GetObj( i ).GetGrayScaleImage( j, 0, 0, true );

			cvReleaseImage( &img );
		}
	}
}

//*****************************************************************************

void CreateOpenCVTeamEHMMDb( const string &pathEhmmDb, const string &ehmmCvDbName )
{
	EHMMObjDatabase ehmmDb;
	int noStates[] = { 5, 3, 6, 6, 6, 3 };
	int numMixtures[] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
	int vecSize = 15; //because we suppress the first coefficient DCT_SIZE_X * DCT_SIZE_Y - 1

	//Create the database
	ehmmDb.Create( ehmmCvDbName, noStates, numMixtures, vecSize );

	//Add all the users
	ehmmDb.AddObj( "Sergey.Molinov" );
	ehmmDb.AddObj( "Vadim.Pisarevsky" );
	ehmmDb.AddObj( "Valery.Cherepennikov" );
	ehmmDb.AddObj( "Valery.Mosyagin" );
	ehmmDb.AddObj( "Victor.Eruhimov" );
        ehmmDb.AddObj( "Tarek.Taha" );

	cout << endl << "CreateOpenCVTeamEHMMDb: Created '" << ehmmCvDbName << "'." << endl << endl;
	cout << ehmmDb;

	ehmmDb.Save( pathEhmmDb );
}

//*****************************************************************************

void TrainEHMMDb( const string &pathImgDb, const string &imgDbName,
				 const string &pathEhmmDb, const string &ehmmDbName )
{
	EHMMObjRecognition recog;
	ImgObjDatabase imgDb;
	EHMMObjDatabase ehmmDb;
	imgDb.Load( imgDbName, pathImgDb );
	ehmmDb.Load( ehmmDbName, pathEhmmDb );
	cout << endl << "TrainEHMMDb: Databases are loaded." << endl << endl;
	cout << imgDb;
	cout << ehmmDb;

	recog.Create( IMG_WIDTH, IMG_HEIGHT, OBS_WIDTH, OBS_HEIGHT, NO_DCT_COEFF_X, NO_DCT_COEFF_Y, 
		STEP_X, STEP_Y, SUPPRESS_INTESITY );

	recog.Train( imgDb, ehmmDb ); 

	ehmmDb.Save( pathEhmmDb );
}

//*****************************************************************************

void RecognizeEHMMDb( const string &image,
					 const string &pathEhmmDb, 
					 const string &ehmmDbName)
{
    EHMMObjRecognition recog;
    EHMMObjDatabase ehmmDb;
    ImgObj person;
    int index = -1;
    vector< float > likelihood;
    vector< float >::iterator it;
    cout<<"\nDatabase Name:"<<ehmmDbName;fflush(stdout);
    cout<<"\nDatabase Path:"<<pathEhmmDb;fflush(stdout);
    cout<<"\nImage to recognize:"<<image;fflush(stdout);
    ehmmDb.Load( ehmmDbName, pathEhmmDb );

    cout << endl << "RecognizeEHMMDb: Database is loaded." << endl << endl;fflush(stdout);
    cout << ehmmDb;fflush(stdout);

    person.Create( "Test" );

    person.AddImage( image );

    recog.Create( IMG_WIDTH, IMG_HEIGHT, OBS_WIDTH, OBS_HEIGHT, NO_DCT_COEFF_X, NO_DCT_COEFF_Y,
                  STEP_X, STEP_Y, SUPPRESS_INTESITY );

    IplImage *img = person.GetGrayScaleImage( 0, 100, 0 );

    index = (int)recog.ComputeLikelihood( *img, ehmmDb, likelihood );

    cvReleaseImage( &img );

    cout << endl << "All likelihoods" << endl << endl;

    for ( it = likelihood.begin( ); it < likelihood.end( ); it++ )
    {
        cout << *it << " ";
    }

    cout << endl << endl;

    if ( index >= 0 )
    {
        recognizedPersonName = ehmmDb.GetObj( index ).GetName( );
        cout << "Person is " << recognizedPersonName<< " ( "<< likelihood[ index ] << " )." << endl;
    }
}

//*****************************************************************************
CvRect detect_and_draw( IplImage* image );
int main(int argc, char* argv[])
{
    cout << endl << "Program usage:" << endl
            << " FaceRecogniton.exe" << endl
            << " Path to EHMM database with \\ at the end" << endl
            << " Name of the EHMM database" << endl
            << " Name of the image to recognize" << endl
            << endl << endl << "Example:" << endl << endl
            << " FaceRecognition d:\\DATABASE\\CvEhmmDB\\ OpenCvTeam.db d:\\Victor.Eruhimov0004.bmp" << endl << endl;
    if ( argc == 4)
    {
        RecognizeEHMMDb( argv[ 3 ], argv[ 1 ], argv[ 2 ] );
        return 0;
    }

    //Database Creation and Testing
    //CreateOpenCVTeamImgDb( PATH_CV_IMG_DB, "BlackCoder.db" );
    //LoadImgDb( PATH_CV_IMG_DB, "BlackCoder.db" );
    //CreateOpenCVTeamEHMMDb( PATH_CV_EHMM_DB, "BlackCoder.db" );
    //TrainEHMMDb( PATH_CV_IMG_DB, "BlackCoder.db", PATH_CV_EHMM_DB, "BlackCoder.db" );
    //return 0;
    IplImage *frame, *frameCopy = 0;
    CvCapture* capture = 0;
    capture = cvCreateCameraCapture(0);
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);

    //int counter=0;    char faceFileName [255];
    if( capture )
    {
        cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640);
        cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);
        // Create a new named window with title: result
        cvNamedWindow( "result", 1 );
        // Capture from the camera.
        for(;;)
        {
            // Capture the frame and load it in IplImage
            if( !cvGrabFrame( capture ))
                break;
            frame = cvRetrieveFrame( capture );

            // If the frame does not exist, quit the loop
            if( !frame )
                break;

            // Allocate framecopy as the same size of the frame
            if( !frameCopy )
                frameCopy = cvCreateImage( cvSize(frame->width,frame->height),
                                            IPL_DEPTH_8U, frame->nChannels );

            // Check the origin of image. If top left, copy the image frame to frame_copy.
            if( frame->origin == IPL_ORIGIN_TL )
                cvCopy( frame, frameCopy, 0 );
            // Else flip and copy the image
            else
                cvFlip( frame, frameCopy, 0 );
            // Call the function to detect and draw the face
            CvRect faceLocation  =  detect_and_draw( frameCopy );

            if(faceLocation.x !=0 && faceLocation.y!=0)
            {
                cvSetImageROI(frameCopy,faceLocation);
                cvSaveImage("testImage.bmp",frameCopy);
                //sprintf(faceFileName,"Tarek.Taha%04d.bmp",counter++);
                //cvSaveImage(faceFileName,frame_copy);
                cvResetImageROI(frameCopy);
                RecognizeEHMMDb( "testImage.bmp",PATH_CV_EHMM_DB, "BlackCoder.db"  );
                string displayText = "Detected Person: " + recognizedPersonName;
                cvPutText(frameCopy, displayText.c_str(), cvPoint(10, 70), &font, cvScalar(255, 255, 255, 0));
            }
            // Wait for a while before proceeding to the next frame
            if( cvWaitKey( 200 ) >= 0 )
                break;
            // Draw the rectangle in the input image
            cvRectangle( frameCopy, cvPoint(faceLocation.x,faceLocation.y),cvPoint(faceLocation.x+faceLocation.width,faceLocation.y+faceLocation.height), CV_RGB(255,0,0), 3, 8, 0 );
            cvShowImage( "result", frameCopy );
        }

        // Release the images, and capture memory
        cvReleaseImage( &frameCopy );
        cvReleaseCapture( &capture );
    }


    return 0;
}

// Function to detect and draw any faces that is present in an image
CvRect detect_and_draw( IplImage* img )
{
    CvRect faceLocation = cvRect(0,0,0,0);

    // Create memory for calculations
    static CvMemStorage* storage = 0;

    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;

    int scale = 1;

    // Create two points to represent the face locations
    CvPoint pt1, pt2;
    int i;

    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );

    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return faceLocation;
    }

    // Allocate the memory storage
    storage = cvCreateMemStorage(0);

    // Clear the memory storage which was used before
    cvClearMemStorage( storage );

    // Find whether the cascade is loaded, to find the faces. If yes, then:
    if( cascade )
    {

        // There can be more than one face in an image. So create a growable sequence of faces.
        // Detect the objects and store them in the sequence
        CvSeq* faces = cvHaarDetectObjects( img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );

        // Loop the number of faces found.
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            // Create a new rectangle for drawing the face
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
            faceLocation.x = r->x -10;faceLocation.y = r->y - 25;
            faceLocation.width = r->width + 20; faceLocation.height = r->height + 50;
            // Find the dimensions of the face,and scale it if necessary
            pt1.x = r->x*scale;
            pt2.x = (r->x+r->width)*scale;
            pt1.y = r->y*scale;
            pt2.y = (r->y+r->height)*scale;
        }
    }
    return faceLocation;
}
