#pragma once
#include <cv.h>
#include <highgui.h>


class CalibrationManager
{
	friend class DualCamManager;
public:
	CalibrationManager(std::string = "list.txt", int nx = 8, int ny = 6, int useUncalibrated = 0, float _squareSize = 5);
	~CalibrationManager(void);
	bool runCalibration();
private :
	void loadData(CvSize);
	void InitMatrices();
	void InitMatricesFile(CvSize);
	void saveFiles(CvMat, CvMat, CvMat, CvMat);
	std::string fileName;
	int nx, ny;
	int useUncalibrated;
	float squareSize;

	double avgError;

	CvMat _M1;
	CvMat _M2;
	CvMat _D1;
	CvMat _D2;
	CvMat _R;
	CvMat _T;
	CvMat _E;
	CvMat _F;
	CvMat _Q;
	CvMat *_Q2;
	CvMat *mx1;
	CvMat *my1;
	CvMat *mx2;
	CvMat *my2;
	double Q[4][4];

	double M1[3][3], M2[3][3], D1[5], D2[5];
    double R[3][3], T[3], E[3][3], F[3][3];
	
	CvStereoBMState * BMState;
};

