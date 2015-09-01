#pragma once
#include <vtkVolume.h>
#include <vtkTransform.h>
#include <vtkPlaneCollection.h>
#include <vtkAbstractVolumeMapper.h>
class VolumeTransformManager
{
public:
	VolumeTransformManager();
	~VolumeTransformManager();
	void volumeRotation(double rotation[3]);
	void volumeRotation(double x, double y, double z);
	void volumeBounds(int side, double amount);
	void init(vtkVolume* volume);

private: 
	vtkVolume* volume;

	
};

