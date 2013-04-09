//
//  main.cpp
//  HeadTrackingApiMacPort
//
//  Created by 김밀한 on 13. 4. 10..
//  Copyright (c) 2013년 TLJ. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, const char * argv[])
{

    /* create an image */
    
    IplImage *img = cvCreateImage(cvSize(200, 100), IPL_DEPTH_8U, 3);
    
    /* draw a green line */
    
    cvLine(img,                         /* the dest image */
           
           cvPoint(10, 10),             /* start point */
           
           cvPoint(150, 80),            /* end point */
           
           cvScalar(0, 255, 0, 0),      /* the color; green */
           
           1, 8, 0);                    /* thickness, line type, shift */
    
    /* draw a blue box */
    
    cvRectangle(img,                    /* the dest image */
                
                cvPoint(20, 15),        /* top left point */
                
                cvPoint(100, 70),       /* bottom right point */
                
                cvScalar(255, 0, 0, 0), /* the color; blue */
                
                1, 8, 0);               /* thickness, line type, shift */
    
    /* draw a red circle */
    
    cvCircle(img,                       /* the dest image */
             
             cvPoint(110, 60), 35,      /* center point and radius */
             
             cvScalar(0, 0, 255, 0),    /* the color; red */
             
             1, 8, 0);                  /* thickness, line type, shift */
    
    /* display the image */
    
    cvNamedWindow("img", CV_WINDOW_AUTOSIZE);
    
    cvShowImage("img", img);
    
    cvWaitKey(0);
    
    cvDestroyWindow("img");
    
    cvReleaseImage(&img);
    
    return 0;
}

