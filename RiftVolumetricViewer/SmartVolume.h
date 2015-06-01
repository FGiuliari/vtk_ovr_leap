#ifndef SMART_H
#define SMART_H
#include "vtkBoxWidget.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkColorTransferFunction.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkImageResample.h"
#include "vtkMetaImageReader.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlanes.h"
#include "vtkProperty.h"
#include "vtkOpenGLRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"
#include "vtkXMLImageDataReader.h"
#include "vtkSmartVolumeMapper.h"
#include <vtkSmartPointer.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkTransform.h>
#include <vtkLinearTransform.h>
#include "vtkBoxWidgetCallback.h"
#include <vtkInteractorStyleTrackballActor.h>


#define VTI_FILETYPE 1
#define MHA_FILETYPE 2
#define DEBUG 1
#define opwinstep 256
#define oplvlstep 128

class SmartVolume{
public:
	vtkVolume *volume;
	vtkRenderWindow *renWin;
	vtkOpenGLRenderer *renderer;
	vtkTransform *boxtrans;
	vtkCamera *initCam;
	vtkBoxWidget *box;
	vtkRenderWindowInteractor *iren;
	vtkCallbackCommand *keypressCallback;
	vtkImageResample *resample;
	vtkSmartVolumeMapper *mapper;
	vtkBoxWidgetCallback *callback;
	vtkColorTransferFunction *colorFun;
	vtkPiecewiseFunction *opacityFun;
	vtkVolumeProperty *property;
	vtkInteractorStyleTrackballActor *style;
	double opacityWindow;
	double opacityLevel;
	int blendType;

	static SmartVolume *New();
	virtual void Init(int argc, char *argv[]);
	virtual void Start(int w, int h);

protected:
	SmartVolume();

private:
	//virtual void PrintUsage();
	static void KeypressCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* vtkNotUsed(callData));
	virtual void setBlendingMode(int);
};

#endif
