#include "LeapCallback.h"


LeapCallback* LeapCallback::New(){
		LeapCallback *cb = new LeapCallback;
		cb->TimerCount = 0;
		return cb;
}

	void LeapCallback::Configure(vtkRenderWindow* window, SmartVolume *sv){
		sav = sv;
	}

	 void LeapCallback::Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId, void *vtkNotUsed(callData)){
		if (vtkCommand::TimerEvent == eventId){
			lastframe = frame;
			frame = controller.frame();
			if (TimerCount = 0){
				lastframe = frame;
			}
			++TimerCount;
			HandList hands = frame.hands();
			pastLeft = left;
			pastRight = right;
		//	left = right = NULL;
			for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl){
				Hand hand = *hl;
				if (hand.isLeft()){
					left = hand;
					this->leftHand();
				}
				else{
					right = hand;
					this->rightHand();
				}
			}
		
		}
}
	 
	 void LeapCallback::leftHand(){
	 }

	 void LeapCallback::rightHand(){
		 double xl = right.fingers().extended()[0].tipPosition().x - pastRight.fingers().extended()[0].tipPosition().x;
		 double yl = right.fingers().extended()[0].tipPosition().y - pastRight.fingers().extended()[0].tipPosition().y;
		 double zl = right.fingers().extended()[0].tipPosition().z - pastRight.fingers().extended()[0].tipPosition().z;
		 sav->volume->RotateWXYZ(-yl, 1.0, 0.0, 0.0);
		 sav->volume->RotateWXYZ(xl, 0.0, 1.0, 0.0);
		 sav->volume->RotateWXYZ(zl, 0.0, 0.0, 1.0);
	 }
	 
	 ;