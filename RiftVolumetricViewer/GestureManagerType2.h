#pragma once
#include "AbstractLeapGestureManager.h"
#include <vtkPlaneCollection.h>
class GestureManagerType2 :
	public AbstractLeapGestureManager
{
public:
	GestureManagerType2();
	~GestureManagerType2();
	void manageRotation();
	void manageScale(){};
	void manageTransformation(){};
	void manageBoxBorder();
	void update();

private:
	int state;
	enum states{rotation,sectioning};
	void checkForStateChange();
	bool stateChanged;

	
	struct limitsHand{
		int xmin, xmax, ymin, ymax, zmin, zmax;
	};
	struct limits{
		limitsHand left,right;
	}limit;
	
};

