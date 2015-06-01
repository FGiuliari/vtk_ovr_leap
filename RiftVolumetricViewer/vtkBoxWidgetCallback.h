#include <vtkCommand.h>
#include <vtkSmartVolumeMapper.h>

class vtkBoxWidgetCallback : public vtkCommand
{
public:
	static vtkBoxWidgetCallback *New();
	virtual void Execute(vtkObject *caller, unsigned long, void*);
	void SetMapper(vtkSmartVolumeMapper* m);

protected:
	vtkBoxWidgetCallback();
	vtkSmartVolumeMapper *Mapper;
};
