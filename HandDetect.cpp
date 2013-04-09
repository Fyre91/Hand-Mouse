#include "cv.h"

#include "cxcore.h"

#include "highgui.h"

#include "math.h"

#include <iostream>

#include <stdio.h>

#include <string.h>

#include <conio.h>

#include <sstream>

#include <time.h>

#include<windows.h>



using namespace std;

using namespace cv;



int main()

{

  int c = 0;

	CvSeq* a = 0;

	CvCapture* capture = cvCaptureFromCAM(0);

	if(!cvQueryFrame(capture)){ cout<<"Video capture failed, please check the camera."<<endl;}else{cout<<"Video camera capture status: Successfull"<<endl;};

    	CvSize sz = cvGetSize(cvQueryFrame( capture));

	IplImage* src = cvCreateImage( sz, 8, 3 );

	IplImage* src1 = cvCreateImage( sz, 8, 3 );

	IplImage* hsv_image = cvCreateImage( sz, 8, 3);

	IplImage* hsv_mask = cvCreateImage( sz, 8, 1);

	IplImage* hsv_edge = cvCreateImage( sz, 8, 1);



	CvScalar  hsv_min = cvScalar(0, 30, 80, 0);

	CvScalar  hsv_max = cvScalar(20, 150, 255, 0);

	

	CvMemStorage* storage = cvCreateMemStorage(0);

	CvMemStorage* areastorage = cvCreateMemStorage(0);

	CvMemStorage* minStorage = cvCreateMemStorage(0);

	CvMemStorage* dftStorage = cvCreateMemStorage(0);

	CvSeq* contours = NULL;

	

	cvNamedWindow( "Main",1);

	cvNamedWindow( "HSV Transformed",1);

	cvNamedWindow( "Contour",1);



	while( c != 27)

	{

		IplImage* bg = cvCreateImage( sz, 8, 3);

		cvRectangle( bg, cvPoint(0,0), cvPoint(bg->width,bg->height), CV_RGB( 0, 0, 0), -1, 8, 0 );



		//Draw grid in the window

		for(int b = 0; b< int(bg->width/10); b++)

		{

			cvLine( bg, cvPoint(b*20, 0), cvPoint(b*20, bg->height), CV_RGB( 255, 255, 255), 1, 8, 0 );

			cvLine( bg, cvPoint(0, b*20), cvPoint(bg->width, b*20), CV_RGB( 0, 0, 0), 1, 8, 0 );

		}



		//Capture frame from webcam

     		src = cvQueryFrame( capture);

		cvCvtColor(src, hsv_image, CV_BGR2HSV);

		cvInRangeS (hsv_image, hsv_min, hsv_max, hsv_mask);

		//cvSmooth( hsv_mask, hsv_mask, CV_MEDIAN, 27, 0, 0, 0 );

		//cvCanny(hsv_mask, hsv_edge, 1, 3, 5);



		//Find the contours in the frame

		cvFindContours( hsv_mask, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );

		CvSeq* contours2 = NULL;

		double result = 0, result2 = 0;





		//Find the largest contour in the frame(expected Hand)

		while(contours)

		{

			result = fabs( cvContourArea( contours, CV_WHOLE_SEQ ) );

			if ( result >= result2) {result2 = result; contours2 = contours;};

			contours  =  contours->h_next;

		}



		//Apply convex hull method to estimate the shape of the hand

		if ( contours2 )

		{

			//cout << "contours2: " << contours2->total << endl;

			CvRect rect = cvBoundingRect( contours2, 0 );

			cvRectangle( bg, cvPoint(rect.x, rect.y + rect.height), cvPoint(rect.x + rect.width, rect.y), CV_RGB(255, 0, 0), 1, 8, 0 );

			//cout << "Ratio: " << rect.width << ", " << rect.height << ", " << (float)rect.width / rect.height << endl;

			int checkcxt = cvCheckContourConvexity( contours2 );

				//cout << checkcxt <<endl;

			CvSeq* hull = cvConvexHull2( contours2, 0, CV_CLOCKWISE, 0 );

			CvSeq* defect = cvConvexityDefects( contours2, hull, dftStorage );

			if( defect->total >=20 ) {cout << " Closed Palm " << endl;}

			else if(  defect->total <20) {cout << " Open Palm " << endl;}

			//else{ cout << " Fist " << endl;}

			//cout << "defect: " << defect->total << endl;



			CvBox2D box = cvMinAreaRect2( contours2, minStorage );

			//cout << "box angle: " << (int)box.angle << endl;

			cvCircle( bg, cvPoint(box.center.x, box.center.y), 3, CV_RGB(200, 0, 200), 2, 8, 0 );

			cout<<endl<<"Area"<<box.size.height*box.size.width<<endl;

			cout<<box.center.x<<endl<<box.center.y<<endl;

			//if(box.size.height*box.size.width<=60000 && box.size.height*box.size.width>=50000)

			SetCursorPos(box.center.x,box.center.y);



    			//mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);



   			//mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

    			//cout << "Ratio: " << (float)box.size.width/box.size.height <<endl;

		}

		cvShowImage( "hsv-msk", hsv_mask);

		//IplImage* contour = cvCreateImage( sz, 8, 3 );



		//cvDrawContours( bg, contours2,  CV_RGB( 0, 200, 0), CV_RGB( 0, 100, 0), 1, 1, 8, cvPoint(0,0));



		cvDrawContours( bg, contours2,  CV_RGB( 255, 255,255), CV_RGB( 0, 100, 0), 1, 1, 8, cvPoint(0,0));



		cvShowImage( "Main", src);



		cvNamedWindow("bg",0);



		//Display the image

		cvShowImage("bg",bg);

		//Release the image

		cvReleaseImage( &bg);







		//Wait for some time

		c = cvWaitKey( 10);

	}



	cvReleaseCapture( &capture);

	cvDestroyAllWindows();

	return 0;



}
