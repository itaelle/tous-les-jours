// DualCamCalibrateOOP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DualCamManager.h"
#include  <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	DualCamManager * camManager = new DualCamManager();

	camManager->findVideoDevices();
	camManager->startStream();
	if (camManager->isCalibrated())
	{
		camManager->run();
	}
	else
	{
		bool succeed = camManager->calibrate();
		if (succeed)
			camManager->run();
	}
	delete camManager;
	return 0;
}

