#pragma once
#include "AbstractLeapGestureManager.h"
#include <vtkPlaneCollection.h>
class GestureManagerType1 :
	public AbstractLeapGestureManager
{
public:
	GestureManagerType1();
	~GestureManagerType1();
	void manageRotation();
	void manageScale(){};
	void manageTransformation(){};
	void manageBoxBorder();
};

