#include "SmartVolume.h"
#include "AbstractLeapGestureManager.h"

using namespace Leap;

class LeapCallback : public vtkCommand
{
public:

	static LeapCallback* New();
	void Configure(vtkRenderWindow*, SmartVolume*, AbstractLeapGestureManager*);
	virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId, void *vtkNotUsed(callData));

	

private:
	int TimerCount;
	vtkSmartPointer<vtkRenderWindow> renWin;
	SmartVolume* sav;
	AbstractLeapGestureManager *manager;


};

