#include "VolumeTransformManager.h"


VolumeTransformManager::VolumeTransformManager()
{
}


VolumeTransformManager::~VolumeTransformManager()
{
}

void VolumeTransformManager::init(vtkVolume* vol){
	volume = vol;
}

void VolumeTransformManager::volumeRotation(double rotation[3]){
	volumeRotation(rotation[0], rotation[1], rotation[2]);
}

void VolumeTransformManager::volumeRotation(double x, double y, double z){


	volume->RotateWXYZ(-x, 1.0, 0.0, 0.0);
	volume->RotateWXYZ(-y, 0.0, 1.0, 0.0);
	volume->RotateWXYZ(-z, 0.0, 0.0, 1.0);

	vtkTransform *mat = vtkTransform::New();
	double center[3];
	volume->GetOrigin(center);
	mat->Identity();
	mat->Translate(center[0], center[1], center[2]);
	mat->RotateWXYZ(-z, 0.0, 0.0, 1.0);
	mat->RotateWXYZ(-y, 0.0, 1.0, 0.0);
	mat->RotateWXYZ(-x, 1.0, 0.0, 0.0);
	mat->Translate(-center[0], -center[1], -center[2]);


	vtkPlaneCollection *planes;
	vtkPlane *plane;
	planes = volume->GetMapper()->GetClippingPlanes();
	for (int i = 0; i < 6; i++){
		plane = planes->GetItem(i);
		double *origin;
		double *normal = NULL;
		normal = plane->GetNormal();
		origin = plane->GetOrigin();

		mat->TransformNormal(normal, normal);
		mat->TransformPoint(origin, origin);
		plane->SetNormal(normal);
		plane->SetOrigin(origin);
	}
	volume->GetMapper()->SetClippingPlanes(planes);
	mat->Delete();
}

void VolumeTransformManager::volumeBounds(int side, double amount){
	vtkPlaneCollection *planes;
	vtkPlane *plane;
	planes = volume->GetMapper()->GetClippingPlanes();
	plane = planes->GetItem(side);
	double *origin;
	double *normal;
	origin = plane->GetOrigin();
	normal = plane->GetNormal();


	//sposto l'origine del piano nel verso della sua normale
	origin[0] += amount * normal[0];
	origin[1] += amount * normal[1];
	origin[2] += amount * normal[2];

	plane->SetOrigin(origin);
	volume->GetMapper()->SetClippingPlanes(planes);
}