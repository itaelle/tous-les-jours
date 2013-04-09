//
//  DualCamManager.cpp
//  HeadTrackingApiMacPort
//
//  Created by 김밀한 on 13. 4. 10..
//  Copyright (c) 2013년 TLJ. All rights reserved.
//

#include "DualCamManager.h"
#include <fstream>
#include <iostream>

DualCamManager::DualCamManager(std::string _fileName, CvSize sizeCam1, CvSize sizeCam2, int nx, int ny, int useUncalibrated, double _squareSize) : fileName(_fileName), SizeCam1(sizeCam1), SizeCam2(sizeCam2)
{
	FILE * f = fopen(this->fileName.c_str(), "rt");
	iscalibrated = true;
	calibManager = new CalibrationManager(fileName, nx, ny, useUncalibrated, _squareSize);
    
	if (!f)
		iscalibrated = false;
	else
	{
		if (checkFileQuality(f))
			fclose(f);
		else
			iscalibrated = false;
	}
	cam1Resize = cvCreateImage(SizeCam1, 8, 3);
	cam2Resize = cvCreateImage(SizeCam2, 8, 3);
	cam1Gray = cvCreateImage(SizeCam1, 8, 1);
	cam2Gray = cvCreateImage(SizeCam2, 8, 1);
}


DualCamManager::~DualCamManager(void)
{
	cvReleaseImage(&cam1Resize);
	cvReleaseImage(&cam2Resize);
	cvReleaseImage(&cam1Gray);
	cvReleaseImage(&cam2Gray);
	cvReleaseCapture(&cap1);
	cvReleaseCapture(&cap2);
}

bool DualCamManager::isCalibrated()const
{
	return iscalibrated;
}


void DualCamManager::saveCalibrationImages(int nx, int ny)
{
	chessX = nx;
	chessY = ny;
	CvPoint2D32f * corners1 = (CvPoint2D32f*)malloc(sizeof(CvPoint2D32f) * (chessX * chessY) + 1);
	CvPoint2D32f * corners2 = (CvPoint2D32f*)malloc(sizeof(CvPoint2D32f) * (chessX * chessY) + 1);
	int key = -1;
	std::ofstream file;
	int count = 0;
	file.open(this->fileName);
	std::cout << "in save calib images"<<std::endl;
	while (key != 27)
	{
		cam1 = cvQueryFrame(cap1);
		cam2 = cvQueryFrame(cap2);
        
		if (cam1 && cam2)
		{
			cvResize(cam1, cam1Resize);
			cvResize(cam2, cam2Resize);
			cvCvtColor(cam1Resize, cam1Gray, CV_RGB2GRAY);
			cvCvtColor(cam2Resize, cam2Gray, CV_RGB2GRAY);
            
			int found1 = cvFindChessboardCorners(cam1Gray, cvSize(chessX, chessY), corners1);
			int found2 = cvFindChessboardCorners(cam2Gray, cvSize(chessX, chessY), corners2);
			cvDrawChessboardCorners(cam1Resize, cvSize(chessX, chessY), corners1, chessX * chessY, 1);
			cvDrawChessboardCorners(cam2Resize, cvSize(chessX, chessY), corners2, chessX * chessY, 1);
			if (found1 == 1 && found2 == 1 && count < 42)
			{
				if (count % 3)
				{
					std::string imageNameLeft = "ImageLeft";
					std::string imageNameRight = "ImageRight";
					std::ostringstream ss1;
					ss1 << count;
					imageNameLeft += ss1.str();
					imageNameRight += ss1.str();
					imageNameLeft += ".jpg";
					imageNameRight += ".jpg";
					cvSaveImage(imageNameLeft.c_str(), cam1Gray);
					cvSaveImage(imageNameRight.c_str(), cam2Gray);
					file << imageNameLeft;
					file << "\n";
					file << imageNameRight;
					file << "\n";
					std::cout << "saving " << imageNameLeft << std::endl;
				}
				++count;
			}
			if (count >= 42)
			{
				std::cout << "Done!"<<std::endl;
				break;
			}
			cvShowImage("Cam1", cam1Resize);
			cvShowImage("Cam2", cam2Resize);
		}
		else
			break;
		key = cvWaitKey(1);
	}
	file.close();
	std::cout << "out of saving calib images"<<std::endl;
}

bool DualCamManager::checkFileQuality(FILE * f)
{
	if (f)
	{
		char * buff = (char*)malloc(sizeof(char*) * 1024);
		for (int i = 0 ; i < 1024 - 1; ++i)
			buff[i] = 0;
		fgets(buff, 1024 - 3, f);
		
		if (buff[0] != 'I')
		{
			fclose(f);
			remove(fileName.c_str()); // ???
			return false;//bad
		}
		return true;//good
	}
	return false;
}

bool DualCamManager::calibrate(int nx, int ny, int useUncalibrated, float squareSize)
{
	std::cout << "In calibrate()"<<std::endl;
	FILE * f = fopen(fileName.c_str(), "rt");
	if (!f)
	{
		saveCalibrationImages(nx, ny);
		std::cout << fileName << std::endl;
		FILE * f2 = fopen(fileName.c_str(), "rt");
		if (checkFileQuality(f2))
		{
			fclose(f2);
			bool res = false;
			res = calibManager->runCalibration();
			while (res == false)
			{
				if (res == false)
				{
					std::cout << "Average calibration error is > 2.0, would you like to recalibrate ?" << std::endl;
					std::cout << "Y:yes N:no"<<std::endl;
					std::string str;
					std::cin >> str;
					if (str == "Y" || str == "y" || str == "yes")
					{
						saveCalibrationImages(nx, ny);
						res = calibManager->runCalibration();
					}
					else
						break;
				}
				else
					break;
			}
		}
		//else
		//	return false;
	}
	else
	{
		iscalibrated = true;
		//calibManager = new CalibrationManager();
	}
	return true;
}

void DualCamManager::startStream(int camId1, int camId2)//be careful to modify due to use of directshow !
{
	cap1 = cvCaptureFromCAM(this->camId1);
	cap2 = cvCaptureFromCAM(this->camId2);
}

void DualCamManager::findVideoDevices() //Really need to modify those codes. **
{
    int i = 0;
    int j = 0;
	camId1 = i;
	camId2 = j;
}

void DualCamManager::run()
{
	int k = -1;
	IplImage * undist1 = cvCreateImage(cvSize(320,240), 8, 1);
	IplImage * undist2 = cvCreateImage(cvSize(320,240), 8, 1);
	CvMat * disp = cvCreateMat(240,  320, CV_16SC1);
	CvMat * vdisp = cvCreateMat(240, 320, CV_8U);
	IplImage * vdispConv = cvCreateImage(cvSize(320, 240), 8, 3);
	CvMat*_3d = cvCreateMat(240, 320, CV_32FC3);
	calibManager->loadData(cvGetSize(undist1));
	while (k != 27)
	{
		cam1 = cvQueryFrame(cap1);
		cam2 = cvQueryFrame(cap2);
		switch (k)
		{
            case 's' :
                calibManager->BMState->minDisparity++;
                break;
            case 'w' :
                calibManager->BMState->minDisparity--;
                break;
		}
		if (cam1 && cam2)
		{
			cvResize(cam1, cam1Resize, CV_INTER_CUBIC);
            
			cvResize(cam2, cam2Resize, CV_INTER_CUBIC);
			cvCvtColor(cam1Resize, cam1Gray, CV_RGB2GRAY);
			cvCvtColor(cam2Resize, cam2Gray, CV_RGB2GRAY);
            
			cvRemap(cam1Gray, undist1, calibManager->mx1, calibManager->my1);
			cvRemap(cam1Gray, undist2, calibManager->mx2, calibManager->my2);
			cvFindStereoCorrespondenceBM(undist1, undist2, disp, calibManager->BMState);
			cvNormalize(disp, vdisp, 0, 256, CV_MINMAX);
			cvReprojectImageTo3D(disp, _3d, calibManager->_Q2);
			//for (int j = 0; j < 240; ++j)
			//{
			//	for (int i = 0; i < 320; ++i)
			//	{
			//
			//		vdispConv->data[(j * 3) * 320 + (i * 3)] = (int)vdisp->imageData[j * 320 + i];
			//		vdispConv->data[(j * 3) * 320 + (i * 3) + 1] = (int)vdisp->imageData[j * 320 + i];
			//		vdispConv->data[(j * 3) * 320 + (i * 3) + 2] = (int)vdisp->imageData[j * 320 + i];
			//	}
			//}
            //			cvReprojectImageTo3D(vdispConv, _3d, calibManager->_Q2);
            
			//float * ptr = _3d->data.fl; Now It's Unused.
			for (int j = 0 ; j < 240 ; ++j)
			{
				for (int i = 0 ; i < 320 ;  ++i)
				{
					//vdispConv->imageData[(3*j) * 320 + (i*3)] = ptr[(3 * j) * 320 + (i * 3) + 2];
					//vdispConv->imageData[(3*j) * 320 + (i*3) + 1]= ptr[(3 * j) * 320 + (i * 3) + 2];
					//vdispConv->imageData[(3*j) * 320 + (i*3) + 2] = ptr[(3 * j) * 320 + (i * 3) + 2];
					//std::cout << (int)ptr[ (3*j) * 320 + (i * 3) + 2] << std::endl;
					vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 0;
					vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 0;
					vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 0;
					double val = CV_MAT_ELEM(*_3d, double, j, i);
					if (val < 0)
					{
                        
                        vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 255;
                        vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 255;
                        vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 255;
					}
                    
					if (val > 0)
					{
						if (val < 10)
						{
							vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 255;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 0;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 0;
						}
						else if (val < 50)
						{
							vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 0;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 255;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 0;
						}
						else if (val < 100)
						{
							vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 0;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 0;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 255;
						}
						else if (val < 150)
						{
							vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 255;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 255;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 0;
						}
						else if (val < 200)
						{
							vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 0;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 255;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 255;
						}
						else if (val < 255)
						{
							vdispConv->imageData[(j * 3) * 320 + (i * 3)] = 255;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 1] = 0;
							vdispConv->imageData[(j * 3) * 320 + (i * 3) + 2] = 255;							
						}
					}
				}
			}
            
			cvShowImage("cam1", undist1);
			cvShowImage("cam2", undist2);
			cvShowImage("vdisp", vdispConv);
			cvShowImage("disp", vdisp);
		}
        
		k = cvWaitKey(1);
	}
}