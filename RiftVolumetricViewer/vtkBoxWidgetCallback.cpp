#include "vtkBoxWidgetCallback.h"
#include <vtkBoxWidget.h>
#include <vtkPlanes.h>


vtkBoxWidgetCallback *vtkBoxWidgetCallback::New()
{
	return new vtkBoxWidgetCallback;
}
void vtkBoxWidgetCallback::Execute(vtkObject *caller, unsigned long, void*)
{
	vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
	if (this->Mapper)
	{
		vtkPlanes *planes = vtkPlanes::New();
		widget->GetPlanes(planes);
		this->Mapper->SetClippingPlanes(planes);
		planes->Delete();
	}
}
void vtkBoxWidgetCallback::SetMapper(vtkSmartVolumeMapper* m)
{
	this->Mapper = m;
}


vtkBoxWidgetCallback::vtkBoxWidgetCallback()
{
	this->Mapper = 0;
}

