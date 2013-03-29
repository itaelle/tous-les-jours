#pragma once
#include <string>
#include <vector>
#include "CalibrationManager.h"

class DualCamManager
{
public:
	DualCamManager(std::string fileName = "list.txt", CvSize sizeCam1 = cvSize(320, 240), CvSize sizeCam2 = cvSize(320, 240), int nx = 8, int ny = 6, int useUncalibrated = 0, double squareSize = 5.0);
	~DualCamManager(void);
	bool calibrate(int nx = 8, int ny = 6, int useUncalibrated = 0, float _squareSize = 2.5);
	bool isCalibrated()const;
	void startStream(int cam$Id1 = 0, int camId2 = 2);
	void findVideoDevices();
	void run();
private :
	CalibrationManager * calibManager;
	std::string fileName;
	std::vector<std::string> deviceList;
	bool checkFileQuality(FILE *);
	void saveCalibrationImages(int, int);
	int chessX, chessY;

	int camId1, camId2;

	bool iscalibrated;
	CvCapture * cap1, * cap2;
	IplImage * cam1, * cam2;
	IplImage * cam1Resize, * cam2Resize;
	IplImage * cam1Gray, * cam2Gray;
	CvSize SizeCam1;
	CvSize SizeCam2;
};

