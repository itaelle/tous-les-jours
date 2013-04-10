//
//  main.cpp
//  HeadTrackingApiMacPort
//
//  Created by 김밀한 on 13. 4. 10..
//  Copyright (c) 2013년 TLJ. All rights reserved.
//

#include "DualCamManager.h"
#include  <iostream>


int main(int argc, const char * argv[])
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

