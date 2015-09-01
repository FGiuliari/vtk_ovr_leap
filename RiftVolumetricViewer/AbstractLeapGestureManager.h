#ifndef AbstractLeapGestureManager_H
#define AbstractLeapGestureManager_H
#include <Leap.h>
#include "SmartVolume.h"
#include "VolumeTransformManager.h"

using namespace Leap;
class AbstractLeapGestureManager
{
public:
	AbstractLeapGestureManager();
	~AbstractLeapGestureManager();
	virtual void config(SmartVolume*);
	virtual void manageRotation(){ cout << "managerotation abstract\n"; };
	virtual void manageScale(){};
	virtual void manageTransformation(){};
	virtual void manageBoxBorder(){};
	virtual void update();
protected:
	void init();
	Controller controller;
	Frame frame, pastFrame;
	Hand right, pastRight, left, pastLeft;
	SmartVolume* sav;
	vtkTransform *boxInitialTrans;
	VolumeTransformManager volMan;


};

#endif