#include "GestureManagerType1.h"


GestureManagerType1::GestureManagerType1()
{
	this->init();
}


GestureManagerType1::~GestureManagerType1()
{
}

void GestureManagerType1::manageRotation(){
	//if (right != NULL){
	cout << "entrato nel ciclo managerotation\n "<<right.grabStrength();
	if (right.grabStrength() >= 1.0){
		cout << "grab\n";
		float yRot = right.rotationAngle(pastFrame, Vector::yAxis()) * 180 / PI;
		float xRot = right.rotationAngle(pastFrame, Vector::xAxis()) * 180 / PI;
		float zRot = right.rotationAngle(pastFrame, Vector::zAxis()) * 180 / PI;

		volMan.volumeRotation(xRot, yRot, zRot);
	}
}

void GestureManagerType1::manageBoxBorder(){
	if (left.grabStrength() < 0.1){
		if (left.fingers()[1].isExtended() && left.fingers()[2].isExtended() && left.fingers()[3].isExtended() && left.fingers()[4].isExtended()){
			Leap::Vector palmNormal = left.palmNormal();
			int side = -1;
			float speed = 0;
			if (palmNormal.angleTo(Leap::Vector::right()) <= (PI / 6)){
				side = 0;
				speed = left.palmVelocity()[0] / 10;
			}
			if (palmNormal.angleTo(Leap::Vector::down()) <= (PI / 6)){
				side = 2;
				speed = left.palmVelocity()[1] / 10;
			}
			if (palmNormal.angleTo(Leap::Vector::forward()) <= (PI / 6)){
				side = 5;
				speed = left.palmVelocity()[2] / -10;
			}
			if (side != -1){
				volMan.volumeBounds(side, speed);

			}

		}
	}
	if (right.grabStrength() < 0.1){
		if (right.fingers()[1].isExtended() && right.fingers()[2].isExtended() && right.fingers()[3].isExtended() && right.fingers()[4].isExtended()){
			Leap::Vector palmNormal = right.palmNormal();
			int side = -1;
			float speed = 0;
			if (palmNormal.angleTo(Leap::Vector::left()) <= (PI / 6)){
				side = 1;
				speed = right.palmVelocity()[0] / -10;
			}
			if (palmNormal.angleTo(Leap::Vector::down()) <= (PI / 6)){
				side = 3;
				speed = right.palmVelocity()[1] / -10;
			}
			if (palmNormal.angleTo(Leap::Vector::forward()) <= (PI / 6)){
				side = 4;
				speed = right.palmVelocity()[2] / -10;
			}
			if (side != -1){
				volMan.volumeBounds(side, speed);
			}
		}
	}
}

