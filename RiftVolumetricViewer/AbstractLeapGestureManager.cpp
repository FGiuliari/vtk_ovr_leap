#include "AbstractLeapGestureManager.h"


AbstractLeapGestureManager::AbstractLeapGestureManager()
{

}


AbstractLeapGestureManager::~AbstractLeapGestureManager()
{
}

void AbstractLeapGestureManager::config(SmartVolume *smvol){
	sav = smvol;
	volMan.init(sav->volume);
	boxInitialTrans = vtkTransform::New();
	sav->box->GetTransform(boxInitialTrans);
}

void AbstractLeapGestureManager::init(){
	/*left = NULL;
	pastLeft = NULL;
	right = NULL;
	pastRight = NULL;
	*/
	frame = controller.frame();
	pastFrame = frame;

}
void AbstractLeapGestureManager::update(){
	cout << "update\n";
	pastFrame = frame;
	frame = controller.frame();
	HandList hands = frame.hands();
		pastLeft = left;
		pastRight = right;
		for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl){
			Hand hand = *hl;
			if (hand.isLeft()){
				left = hand;

			}
			else{
				right = hand;

			}
		
	}
	
}
