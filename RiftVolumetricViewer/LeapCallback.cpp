#include "LeapCallback.h"


LeapCallback* LeapCallback::New(){
		LeapCallback *cb = new LeapCallback;
		cb->TimerCount = 0;
		return cb;
}

	void LeapCallback::Configure(vtkRenderWindow* window, SmartVolume *sv,AbstractLeapGestureManager *man){
		sav = sv;
		manager = man;
		manager->config(sav);
	}

	 void LeapCallback::Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId, void *vtkNotUsed(callData)){
		 ++TimerCount;
		 manager->update();
		 manager->manageRotation();
		 manager->manageBoxBorder();
		 manager->manageTransformation();
		 manager->manageScale();

}