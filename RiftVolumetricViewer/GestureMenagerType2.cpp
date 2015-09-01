#include "GestureManagerType2.h"


GestureManagerType2::GestureManagerType2()
{
	AbstractLeapGestureManager::init();
	state = states::rotation;
	stateChanged = false;
	limit.left.xmin = -200;
	limit.left.xmax = -140;
	limit.left.ymin = 160;
	limit.left.ymax = 220;
	limit.left.zmin = -30;
	limit.left.zmax = 30;

	limit.right.xmin = 140;
	limit.right.xmax = 200;
	limit.right.ymin = 160;
	limit.right.ymax = 220;
	limit.right.zmin = -30;
	limit.right.zmax = 30;
}


GestureManagerType2::~GestureManagerType2()
{
}

void GestureManagerType2::manageRotation(){
	if (state == states::rotation){
		cout << "entrato nel ciclo managerotation\n " << right.grabStrength();
		if (right.fingers()[1].isExtended() && right.fingers()[2].isExtended() && right.fingers()[3].isExtended() && right.fingers()[4].isExtended()){
			Leap::Vector palmPos = right.palmPosition();
			cout << palmPos.y;
			float xRot = 0;
			if (palmPos.y <= limit.right.ymin)
				xRot = (palmPos.y - limit.right.ymin) / 10;
			if (palmPos.y >= limit.right.ymax)
				xRot = (palmPos.y - limit.right.ymax) / 10;
			float yRot = 0;

			if (palmPos.x <= limit.right.xmin)
				yRot = (palmPos.x - limit.right.xmin) / 10;
			if (palmPos.x >= limit.right.xmax)
				yRot = (palmPos.x - limit.right.xmax) / 10;
			

			float zRot = 0;
			if (palmPos.y <220 && palmPos.y>160 && palmPos.x > 100 && palmPos.x < 160){
				if (right.palmNormal().roll()> PI / 15)
					zRot = (right.palmNormal().roll() - PI / 15) / 7 * 180 / PI;
				if (right.palmNormal().roll() < -PI / 15)
					zRot = (right.palmNormal().roll() + PI / 15) / 7 * 180 / PI;

			}

			volMan.volumeRotation(xRot, -yRot, -zRot);


		}
	}
}

void GestureManagerType2::manageBoxBorder(){
	if (state == states::sectioning){
		if (left.grabStrength()<=0.4){
			if (left.palmNormal().angleTo(Vector::down()) <= PI / 6){
				Leap::Vector palmPos = left.palmPosition();
				cout << palmPos.y;
				float ySect = 0;
				if (palmPos.y <= limit.left.ymin)
					ySect = (palmPos.y - limit.left.ymin) / 10;
				if (palmPos.y >= limit.left.ymax)
					ySect = (palmPos.y - limit.left.ymax) / 10;
				volMan.volumeBounds(2, ySect);
				
				float xSect = 0;
				if (palmPos.x <= limit.left.xmin)
					xSect = (palmPos.x - limit.left.xmin) / 10;
				if (palmPos.x >= limit.left.xmax)
					xSect = (palmPos.x - limit.left.xmax) / 10;
				volMan.volumeBounds(0, xSect);

				float zSect = 0;
				if (palmPos.z <= limit.left.zmin)
					zSect = (palmPos.z - limit.left.zmin) / 10;
				if (palmPos.z >= limit.left.zmax)
					zSect = (palmPos.z - limit.left.zmax) / 10;
				volMan.volumeBounds(5, -zSect);
			}
		}
		if (right.grabStrength() <= 0.4){
			if (right.palmNormal().angleTo(Vector::down()) <= PI / 6){
				Leap::Vector palmPos = right.palmPosition();
				cout << palmPos.y;
				float ySect = 0;
				if (palmPos.y <= limit.right.ymin)
					ySect = (palmPos.y - limit.right.ymin) / -10;
				if (palmPos.y >= limit.right.ymax)
					ySect = (palmPos.y - limit.right.ymax) / -10;
				volMan.volumeBounds(3, ySect);

				float xSect = 0;
				if (palmPos.x <= limit.right.xmin)
					xSect = (palmPos.x - limit.right.xmin) / -10;
				if (palmPos.x >= limit.right.xmax)
					xSect = (palmPos.x - limit.right.xmax) / -10;
				volMan.volumeBounds(1, xSect);

				float zSect = 0;
				if (palmPos.z <= limit.right.zmin)
					zSect = (palmPos.z - limit.right.zmin) / 10;
				if (palmPos.z >= limit.right.zmax)
					zSect = (palmPos.z - limit.right.zmax) / 10;
				volMan.volumeBounds(4, -zSect);
			}
		}
	}
}

void GestureManagerType2::update(){
	AbstractLeapGestureManager::update();
	checkForStateChange();
}


void GestureManagerType2::checkForStateChange(){
	if (!stateChanged){
		if (left.grabStrength() >=0.9 && right.grabStrength() >= 0.9){
			stateChanged = true;
			switch (state){
				case states::rotation : state = states::sectioning;
										break;
				case states::sectioning : state = states::rotation;
										break;
			}
		}
	}
	else {
		if (left.grabStrength() <= 0.4 && right.grabStrength() <= 0.4){
			stateChanged = false;
		}
	
	}
}