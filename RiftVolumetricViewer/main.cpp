#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
#include "SmartVolume.h"
#include <vtkLightsPass.h>
#include <vtkCameraPass.h>
#include <vtkRenderPassCollection.h>
#include <vtkDefaultPass.h>
#include <vtkSequencePass.h>
#include "vtkStereoDistortPass.h"
#include "rift_middleware.h"
#include <vtkRendererCollection.h>
#include <vtkConeSource.h>
#include <vtkPolyDataMapper.h>
#include "LeapCallback.h"
#include "GestureManagerType1.h"
#include "GestureManagerType2.h"


class vtkTimerCallback : public vtkCommand{
public:
	static vtkTimerCallback *New(){
		vtkTimerCallback *cb = new vtkTimerCallback;
		cb->TimerCount = 0;
		return cb;
	}

	void Configure(vtkRenderWindow* window,SmartVolume *sv){
		this->renWin = window;
		this->sav = sv;
		last_yaw = last_pitch = last_roll = 0.0;
		
	}

	virtual void Execute(vtkObject *vtkNotUsed(caller),	unsigned long eventId,void *vtkNotUsed(callData)){
		if (vtkCommand::TimerEvent == eventId){
			sav->volume->RotateWXYZ(.0, 1.0, 0.0, 0.0);
			++this->TimerCount;
			Headpose hp = this->rift.getHeadpose();
			vtkSmartPointer<vtkCamera> camera = this->renWin->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
			if (hp.roll != 0){
				camera->SetRoll((double)-hp.roll);
			}

			camera->Yaw((double)hp.yaw - last_yaw);

			camera->Pitch((double)hp.pitch - last_pitch);

			last_yaw = hp.yaw; last_pitch = hp.pitch; last_roll = hp.roll;

			// camera_r_->Azimuth(1);  camera_l_->Azimuth(1);
			//cout << "*****\nPitch: " << hp.pitch << "\n" << "Yaw: " << hp.yaw << "\n" << "Roll: " << hp.roll << "\n******\n";

			renWin->Render();
		}
	}
private:
	int TimerCount;
	vtkSmartPointer<vtkRenderWindow> renWin;
	SmartVolume *sav;
	rift_middleware rift;
	float last_yaw, last_pitch, last_roll;

};

vtkStereoDistortPass *blurP;

void KeypressIPDCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* vtkNotUsed(callData)){

	vtkRenderWindowInteractor *iren = static_cast<vtkRenderWindowInteractor*>(caller);

	//blend type selection
	if (strcmp(iren->GetKeySym(), "o") == 0){
		blurP->SetIPD(blurP->IPD + 5.0);
	}
	if (strcmp(iren->GetKeySym(), "l") == 0){
		blurP->SetIPD(blurP->IPD - 5.0);
	}

	cout << "IPD: " << blurP->IPD << "\n";
}

int main(int argc, char *argv[]){
	SmartVolume *sv = SmartVolume::New();
	sv->Init(argc, argv);
	double* center = sv->volume->GetCenter();
	sv->volume->SetOrigin(center);

	//********************************************
	// The elementary passes.
	vtkLightsPass *lights = vtkLightsPass::New();
	vtkDefaultPass *default = vtkDefaultPass::New();

	// Put them in a sequence.
	vtkRenderPassCollection *passes = vtkRenderPassCollection::New();
	passes->AddItem(lights);
	passes->AddItem(default);

	vtkSequencePass *seq = vtkSequencePass::New();
	seq->SetPasses(passes);

	// Make the sequence the delegate of a camera pass.
	vtkCameraPass *cameraP = vtkCameraPass::New();

	cameraP->SetDelegatePass(seq);
	// Create the post-processing pass and attach the camera pass as its delegate

	blurP = vtkStereoDistortPass::New();
	blurP->SetDelegatePass(cameraP);

	float myLensCenter[4] = { 0.287994117*2.0, 0.500000000, 1.0 - 0.287994117*2.0, 0.500000000 }; //centro occhio con offset pupille// 0.287994117
	float myScreenCenter[2] = { 0.50000000, 0.500000000 }; //centro occhio senza offset
	float myScale[2] = { 0.50000000 / 1.15, 0.5 * (8.0 / 10.0) / 1.5 }; //metà aspect ratio ; metà aspect ratio * aspect ratio
	float myScaleIn[2] = { 2.00000000, 2 / (8.0 / 10.0) }; //1 ; 1/aspectratio
	float myWarp[4] = { 1.00000000, 0.18, 0.115, 0.0 };

	blurP->SetDistortionParams(myLensCenter, myScreenCenter, myScale, myScaleIn, myWarp);
	blurP->SetIPD(200);
	// The main pass is the blur pass, attach it to the renderer
	sv->renderer->SetPass(blurP);
	//******************************************
	sv->renderer->SetBackground(0.1, 0.1, 0.3);
	//sv->renWin->SetFullScreen(1);
	//****************************************

	//****************************************
	sv->renWin->FullScreenOn();
	//sv->Start(1366, 768);
	//sv->renWin->SetSize(1280, 768);
	sv->iren->Initialize();

	vtkSmartPointer<vtkTimerCallback> tcb = vtkSmartPointer<vtkTimerCallback>::New();
	sv->iren->AddObserver(vtkCommand::TimerEvent, tcb);
	int timerId = sv->iren->CreateRepeatingTimer(2);
	tcb->Configure(sv->renWin,sv);

	vtkCallbackCommand *keypressCallback = vtkCallbackCommand::New();
	keypressCallback->SetCallback(KeypressIPDCallbackFunction);
	sv->iren->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);

	vtkSmartPointer<LeapCallback> leap = vtkSmartPointer<LeapCallback>::New();
	sv->iren->AddObserver(vtkCommand::TimerEvent, leap);
	int timerId1 = sv->iren->CreateRepeatingTimer(2);
	GestureManagerType2 man;
	leap->Configure(sv->renWin, sv, &man);

	// Add the volume to the scene
	//volume->SetPosition(0.0, 0.0, 0.0);
	sv->renderer->AddVolume(sv->volume);
	sv->renderer->ResetCamera();
	double pos[3];
	sv->renderer->GetActiveCamera()->GetPosition(pos);
	pos[2] += 500;
	sv->renderer->GetActiveCamera()->SetPosition(pos);
	sv->renderer->GetActiveCamera()->SetClippingRange(0.1, 10000);
	sv->initCam->DeepCopy(sv->renderer->GetActiveCamera());


	// interact with data
	vtkPlanes *planes = vtkPlanes::New();
	sv->box->GetPlanes(planes);
	sv->volume->GetMapper()->SetClippingPlanes(planes);
	sv->renWin->GlobalWarningDisplayOff();
	sv->iren->Start();
	//sv->renWin->BordersOff();
	//sv->Start(1228, 768);
	return 0;
}