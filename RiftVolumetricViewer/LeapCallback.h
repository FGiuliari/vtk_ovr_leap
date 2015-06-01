#include "SmartVolume.h"
#include <Leap.h>

using namespace Leap;

class LeapCallback : public vtkCommand
{
public:

	static LeapCallback* New();
	void Configure(vtkRenderWindow*, SmartVolume*);
	virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId, void *vtkNotUsed(callData));

	

private:
	int TimerCount;
	vtkSmartPointer<vtkRenderWindow> renWin;
	SmartVolume* sav;
	float last_yaw, last_pitch, last_roll;
	Controller controller;
	Frame frame;
	Frame lastframe;
	Hand right, pastRight, left, pastLeft;


	void leftHand();
	void rightHand();

};

