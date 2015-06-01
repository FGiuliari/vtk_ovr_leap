#include "SmartVolume.h"

#define VERBOSE 1

//nasty hack to allow method as argument
SmartVolume* smvol;

//constructor method
SmartVolume *SmartVolume::New(){
	return new SmartVolume;
}

//prints the command line arguments
void PrintUsage()
{
	cout << "Usage: " << endl;
	cout << endl;
	cout << "  GPURenderDemo <options>" << endl;
	cout << endl;
	cout << "where options may include: " << endl;
	cout << endl;
	cout << "  -DICOM <directory>" << endl;
	cout << "  -VTI <filename>" << endl;
	cout << "  -MHA <filename>" << endl;
	cout << "  -DependentComponents" << endl;
	cout << "  -Clip" << endl;
	cout << "  -MIP <window> <level>" << endl;
	cout << "  -CompositeRamp <window> <level>" << endl;
	cout << "  -CompositeShadeRamp <window> <level>" << endl;
	cout << "  -CT_Skin" << endl;
	cout << "  -CT_Bone" << endl;
	cout << "  -CT_Muscle" << endl;
	cout << "  -FrameRate <rate>" << endl;
	cout << "  -DataReduction <factor>" << endl;
	cout << endl;
	cout << "You must use either the -DICOM option to specify the directory where" << endl;
	cout << "the data is located or the -VTI or -MHA option to specify the path of a .vti file." << endl;
	cout << endl;
	cout << "By default, the program assumes that the file has independent components," << endl;
	cout << "use -DependentComponents to specify that the file has dependent components." << endl;
	cout << endl;
	cout << "Use the -Clip option to display a cube widget for clipping the volume." << endl;
	cout << "Use the -FrameRate option with a desired frame rate (in frames per second)" << endl;
	cout << "which will control the interactive rendering rate." << endl;
	cout << "Use the -DataReduction option with a reduction factor (greater than zero and" << endl;
	cout << "less than one) to reduce the data before rendering." << endl;
	cout << "Use one of the remaining options to specify the blend function" << endl;
	cout << "and transfer functions. The -MIP option utilizes a maximum intensity" << endl;
	cout << "projection method, while the others utilize compositing. The" << endl;
	cout << "-CompositeRamp option is unshaded compositing, while the other" << endl;
	cout << "compositing options employ shading." << endl;
	cout << endl;
	cout << "Note: MIP, CompositeRamp, CompositeShadeRamp, CT_Skin, CT_Bone," << endl;
	cout << "and CT_Muscle are appropriate for DICOM data. MIP, CompositeRamp," << endl;
	cout << "and RGB_Composite are appropriate for RGB data." << endl;
	cout << endl;
	cout << "Example: GPURenderDemo -DICOM CTNeck -MIP 4096 1024" << endl;
	cout << endl;
}

//keyboard function callback
void SmartVolume::KeypressCallbackFunction(vtkObject* caller, long unsigned int vtkNotUsed(eventId), void* vtkNotUsed(clientData), void* vtkNotUsed(callData)){

	vtkRenderWindowInteractor *iren = static_cast<vtkRenderWindowInteractor*>(caller);
#if VERBOSE
	std::cout << "Pressed: " << iren->GetKeySym() << std::endl;
#endif

	//blend type selection
	if (strcmp(iren->GetKeySym(), "0") >= 0 && strcmp(iren->GetKeySym(), "5") <= 0){
		int blendType = atoi(iren->GetKeySym());
		// Depending on the blend type selected
		// adjust the transfer function
		//more details in the method comments
		smvol->setBlendingMode(blendType);
	}
	//resets volume and bounding box position and sections
	if (strcmp(iren->GetKeySym(), "r") == 0){
		smvol->renderer->GetActiveCamera()->DeepCopy(smvol->initCam);
		smvol->box->SetTransform(smvol->boxtrans);
		vtkPlanes *planes = vtkPlanes::New();
		smvol->box->GetPlanes(planes);
		smvol->volume->GetMapper()->SetClippingPlanes(planes);
		planes->Delete();
	}
	//enable/disable box
	if (strcmp(iren->GetKeySym(), "c") == 0){
		smvol->box->SetEnabled(!smvol->box->GetEnabled());
	}

	//CHANGE THESE
	//opacity window and opacity level control
	if (strcmp(iren->GetKeySym(), "Right") == 0){
		smvol->opacityWindow += opwinstep;
		smvol->setBlendingMode(smvol->blendType);
		std::cout << "NEW VALUE - Opacity Window: " << smvol->opacityWindow << std::endl;
	}
	else if (strcmp(iren->GetKeySym(), "Left") == 0){
		smvol->opacityWindow -= opwinstep;
		smvol->setBlendingMode(smvol->blendType);
		std::cout << "NEW VALUE - Opacity Window: " << smvol->opacityWindow << std::endl;
	}
	else if (strcmp(iren->GetKeySym(), "Up") == 0){
		smvol->opacityLevel += oplvlstep;
		smvol->setBlendingMode(smvol->blendType);
		std::cout << "NEW VALUE - Opacity Level: " << smvol->opacityLevel << std::endl;
	}
	else if (strcmp(iren->GetKeySym(), "Down") == 0){
		smvol->opacityLevel -= oplvlstep;
		smvol->setBlendingMode(smvol->blendType);
		std::cout << "NEW VALUE - Opacity Level: " << smvol->opacityLevel << std::endl;
	}




	smvol->renWin->Render();
}

void SmartVolume::setBlendingMode(int blendType){
	this->blendType = blendType;
	//create the 2 functions as new
	vtkColorTransferFunction *colorFun2 = vtkColorTransferFunction::New();
	vtkPiecewiseFunction *opacityFun2 = vtkPiecewiseFunction::New();
	switch (blendType){
		// MIP
		// Create an opacity ramp from the window and level values.
		// Color is white. Blending is MIP.
	case 0:

		colorFun2->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0);
		opacityFun2->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0,
			opacityLevel + 0.5*opacityWindow, 1.0);
		mapper->SetBlendModeToMaximumIntensity();
		break;

		// CompositeRamp
		// Create a ramp from the window and level values. Use compositing
		// without shading. Color is a ramp from black to white.
	case 1:
		colorFun2->AddRGBSegment(0.0, 0.0, 0.0, 0.0,
			255.0, 1.0, 1.0, 1.0);
		opacityFun2->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0,
			opacityLevel + 0.5*opacityWindow, 1.0);
		mapper->SetBlendModeToComposite();
		property->ShadeOff();
		break;

		// CompositeShadeRamp
		// Create a ramp from the window and level values. Use compositing
		// with shading. Color is white.
	case 2:
		colorFun2->AddRGBSegment(0.0, 1.0, 1.0, 1.0, 255.0, 1.0, 1.0, 1.0);
		opacityFun2->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0,
			opacityLevel + 0.5*opacityWindow, 1.0);
		mapper->SetBlendModeToComposite();
		property->ShadeOn();
		break;

		// CT_Skin
		// Use compositing and functions set to highlight skin in CT data
		// Not for use on RGB data
	case 3:
		colorFun2->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
		colorFun2->AddRGBPoint(-1000, .62, .36, .18, 0.5, 0.0);
		colorFun2->AddRGBPoint(-500, .88, .60, .29, 0.33, 0.45);
		colorFun2->AddRGBPoint(3071, .83, .66, 1, 0.5, 0.0);

		opacityFun2->AddPoint(-3024, 0, 0.5, 0.0);
		opacityFun2->AddPoint(-1000, 0, 0.5, 0.0);
		opacityFun2->AddPoint(-500, 1.0, 0.33, 0.45);
		opacityFun2->AddPoint(3071, 1.0, 0.5, 0.0);

		mapper->SetBlendModeToComposite();
		volume->GetProperty()->ShadeOn();
		volume->GetProperty()->SetAmbient(0.1);
		volume->GetProperty()->SetDiffuse(0.9);
		volume->GetProperty()->SetSpecular(0.2);
		volume->GetProperty()->SetSpecularPower(10.0);
		volume->GetProperty()->SetScalarOpacityUnitDistance(0.8919);

		break;

		// CT_Bone
		// Use compositing and functions set to highlight bone in CT data
		// Not for use on RGB data
	case 4:
		colorFun2->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
		colorFun2->AddRGBPoint(-16, 0.73, 0.25, 0.30, 0.49, .61);
		colorFun2->AddRGBPoint(641, .90, .82, .56, .5, 0.0);
		colorFun2->AddRGBPoint(3071, 1, 1, 1, .5, 0.0);

		opacityFun2->AddPoint(-3024, 0, 0.5, 0.0);
		opacityFun2->AddPoint(-16, 0, .49, .61);
		opacityFun2->AddPoint(641, .72, .5, 0.0);
		opacityFun2->AddPoint(3071, .71, 0.5, 0.0);

		mapper->SetBlendModeToComposite();
		volume->GetProperty()->ShadeOn();
		volume->GetProperty()->SetAmbient(0.1);
		volume->GetProperty()->SetDiffuse(0.9);
		volume->GetProperty()->SetSpecular(0.2);
		volume->GetProperty()->SetSpecularPower(10.0);
		volume->GetProperty()->SetScalarOpacityUnitDistance(0.8919);
		break;

		// CT_Muscle
		// Use compositing and functions set to highlight muscle in CT data
		// Not for use on RGB data
	case 5:
		colorFun2->AddRGBPoint(-3024, 0, 0, 0, 0.5, 0.0);
		colorFun2->AddRGBPoint(-155, .55, .25, .15, 0.5, .92);
		colorFun2->AddRGBPoint(217, .88, .60, .29, 0.33, 0.45);
		colorFun2->AddRGBPoint(420, 1, .94, .95, 0.5, 0.0);
		colorFun2->AddRGBPoint(3071, .83, .66, 1, 0.5, 0.0);

		opacityFun2->AddPoint(-3024, 0, 0.5, 0.0);
		opacityFun2->AddPoint(-155, 0, 0.5, 0.92);
		opacityFun2->AddPoint(217, .68, 0.33, 0.45);
		opacityFun2->AddPoint(420, .83, 0.5, 0.0);
		opacityFun2->AddPoint(3071, .80, 0.5, 0.0);

		mapper->SetBlendModeToComposite();
		volume->GetProperty()->ShadeOn();
		volume->GetProperty()->SetAmbient(0.1);
		volume->GetProperty()->SetDiffuse(0.9);
		volume->GetProperty()->SetSpecular(0.2);
		volume->GetProperty()->SetSpecularPower(10.0);
		volume->GetProperty()->SetScalarOpacityUnitDistance(0.8919);
		break;
	default:
		break;
	}
	//set the new functions
	property->SetColor(colorFun2);
	property->SetScalarOpacity(opacityFun2);

	//delete the old ones
	colorFun->Delete();
	opacityFun->Delete();
	//set new ones
	this->colorFun = colorFun2;
	this->opacityFun = opacityFun2;
}

void SmartVolume::Init(int argc, char *argv[]){
	//HACK
	smvol = this;
	//arg count
	int count = 1;
	//DICOM dir
	char *dirname = NULL;
	//default levels
	opacityWindow = 4096;
	opacityLevel = 2048;
	//default blend
	blendType = 0;
	int clip = 0;
	double reductionFactor = 1.0;
	double frameRate = 60.0;
	char *fileName = NULL;
	int fileType = 0;
	while (count < argc)
	{
		if (!strcmp(argv[count], "?"))
		{
			PrintUsage();
			exit(EXIT_SUCCESS);
		}
		else if (!strcmp(argv[count], "-DICOM"))
		{
			dirname = new char[strlen(argv[count + 1]) + 1];
			sprintf(dirname, "%s", argv[count + 1]);
			count += 2;
		}
		else if (!strcmp(argv[count], "-VTI"))
		{
			fileName = new char[strlen(argv[count + 1]) + 1];
			fileType = VTI_FILETYPE;
			sprintf(fileName, "%s", argv[count + 1]);
			count += 2;
		}
		else if (!strcmp(argv[count], "-MHA"))
		{
			fileName = new char[strlen(argv[count + 1]) + 1];
			fileType = MHA_FILETYPE;
			sprintf(fileName, "%s", argv[count + 1]);
			count += 2;
		}
		else if (!strcmp(argv[count], "-Clip"))
		{
			clip = 1;
			count++;
		}
		else if (!strcmp(argv[count], "-MIP"))
		{
			opacityWindow = atof(argv[count + 1]);
			opacityLevel = atof(argv[count + 2]);
			blendType = 0;
			count += 3;
		}
		else if (!strcmp(argv[count], "-CompositeRamp"))
		{
			opacityWindow = atof(argv[count + 1]);
			opacityLevel = atof(argv[count + 2]);
			blendType = 1;
			count += 3;
		}
		else if (!strcmp(argv[count], "-CompositeShadeRamp"))
		{
			opacityWindow = atof(argv[count + 1]);
			opacityLevel = atof(argv[count + 2]);
			blendType = 2;
			count += 3;
		}
		else if (!strcmp(argv[count], "-CT_Skin"))
		{
			blendType = 3;
			count += 1;
		}
		else if (!strcmp(argv[count], "-CT_Bone"))
		{
			blendType = 4;
			count += 1;
		}
		else if (!strcmp(argv[count], "-CT_Muscle"))
		{
			blendType = 5;
			count += 1;
		}
		else if (!strcmp(argv[count], "-RGB_Composite"))
		{
			blendType = 6;
			count += 1;
		}
		else if (!strcmp(argv[count], "-FrameRate"))
		{
			frameRate = atof(argv[count + 1]);
			count += 2;
		}
		//reduces quality for better performance
		else if (!strcmp(argv[count], "-ReductionFactor"))
		{
			reductionFactor = atof(argv[count + 1]);
			if (reductionFactor <= 0.0 || reductionFactor >= 1.0)
			{
				cout << "Invalid reduction factor - use a number between 0 and 1 (exclusive)" << endl;
				cout << "Using the default of no reduction." << endl;
				reductionFactor = 1.0;
			}
			count += 2;
		}
		else
		{
			cout << "Unrecognized option: " << argv[count] << endl;
			cout << endl;
			PrintUsage();
			exit(EXIT_FAILURE);
		}
	}
	//if no input selected
	if (!dirname && !fileName)
	{
		cout << "Error: you must specify a directory of DICOM data or a .vti file or a .mha!" << endl;
		cout << endl;
		PrintUsage();
		if (DEBUG){
			dirname = "abdomen";
		}
		else{
			exit(EXIT_FAILURE);
		}
	}
	// Create the renderer, render window and interactor
	renderer = vtkOpenGLRenderer::New();
	renWin = vtkRenderWindow::New();
	renWin->AddRenderer(renderer);
	// Connect it all. Note that funny arithematic on the
	// SetDesiredUpdateRate - the vtkRenderWindow divides it
	// allocated time across all renderers, and the renderer
	// divides it time across all props. If clip is
	// true then there are two props
	iren = vtkRenderWindowInteractor::New();
	style = vtkInteractorStyleTrackballActor::New();
	iren->SetInteractorStyle(style);
	iren->SetRenderWindow(renWin);
	if (DEBUG){
		clip = 1;
		//reductionFactor=0.5;
	}
	iren->SetDesiredUpdateRate(frameRate / (1 + clip));
	//keypress
	keypressCallback = vtkCallbackCommand::New();
	keypressCallback->SetCallback(KeypressCallbackFunction);
	iren->AddObserver(vtkCommand::KeyPressEvent, keypressCallback);

	iren->GetInteractorStyle()->SetDefaultRenderer(renderer);
	// Read the data
	vtkAlgorithm *reader = 0;
	vtkImageData *input = 0;
	if (dirname)
	{
		vtkDICOMImageReader *dicomReader = vtkDICOMImageReader::New();
		dicomReader->SetDirectoryName(dirname);
		dicomReader->Update();
		input = dicomReader->GetOutput();
		reader = dicomReader;
	}
	else if (fileType == VTI_FILETYPE)
	{
		vtkXMLImageDataReader *xmlReader = vtkXMLImageDataReader::New();
		xmlReader->SetFileName(fileName);
		xmlReader->Update();
		input = xmlReader->GetOutput();
		reader = xmlReader;
	}
	else if (fileType == MHA_FILETYPE)
	{
		vtkMetaImageReader *metaReader = vtkMetaImageReader::New();
		metaReader->SetFileName(fileName);
		metaReader->Update();
		input = metaReader->GetOutput();
		reader = metaReader;
	}
	else
	{
		cout << "Error! Not VTI or MHA!" << endl;
		exit(EXIT_FAILURE);
	}

	// Verify that we actually have a volume
	int dim[3];
	input->GetDimensions(dim);
	if (dim[0] < 2 || dim[1] < 2 || dim[2] < 2)	{
		cout << "Error loading data!" << endl;
		exit(EXIT_FAILURE);
	}

	resample = vtkImageResample::New();
	if (reductionFactor < 1.0)
	{
		resample->SetInputConnection(reader->GetOutputPort());
		resample->SetAxisMagnificationFactor(0, reductionFactor);
		resample->SetAxisMagnificationFactor(1, reductionFactor);
		resample->SetAxisMagnificationFactor(2, reductionFactor);
	}

	// Create our volume and mapper
	volume = vtkVolume::New();
	mapper = vtkSmartVolumeMapper::New();
	mapper->SetRequestedRenderMode(vtkSmartVolumeMapper::DefaultRenderMode);

	// Add a box widget if the clip option was selected

	box = vtkBoxWidget::New();
	if (clip)
	{
		box->SetInteractor(iren);
		box->SetPlaceFactor(1.01);
		if (reductionFactor < 1.0)
		{
			box->SetInputConnection(resample->GetOutputPort());
		}
		else
		{
			box->SetInputData(input);
		}

		box->SetDefaultRenderer(renderer);
		box->InsideOutOn();
		box->PlaceWidget();
		callback = vtkBoxWidgetCallback::New();
		callback->SetMapper(mapper);
		box->AddObserver(vtkCommand::InteractionEvent, callback);
		callback->Delete();
		box->EnabledOff();
		box->GetSelectedFaceProperty()->SetOpacity(0.0);
	}

	if (reductionFactor < 1.0)
	{
		mapper->SetInputConnection(resample->GetOutputPort());
	}
	else
	{
		mapper->SetInputConnection(reader->GetOutputPort());
	}

	// Set the sample distance on the ray to be 1/2 the average spacing
	double spacing[3];
	if (reductionFactor < 1.0)
	{
		resample->GetOutput()->GetSpacing(spacing);
	}
	else
	{
		input->GetSpacing(spacing);
	}



	// Create our transfer function
	this->colorFun = vtkColorTransferFunction::New();
	this->opacityFun = vtkPiecewiseFunction::New();

	// Create the property and attach the transfer functions
	property = vtkVolumeProperty::New();
	property->SetIndependentComponents(true);
	property->SetColor(this->colorFun);
	property->SetScalarOpacity(this->opacityFun);
	property->SetInterpolationTypeToLinear();
	// connect up the volume to the property and the mapper
	volume->SetProperty(property);
	volume->SetMapper(mapper);
	if (DEBUG){
		blendType = 0;
	}
	this->setBlendingMode(blendType);

	boxtrans = vtkTransform::New();
	boxtrans->DeepCopy(boxtrans);
	initCam = vtkCamera::New();
	initCam->DeepCopy(renderer->GetActiveCamera());
	
}
void SmartVolume::Start(int w, int h){
	// Set the default window size
	renWin->SetSize(w, h);

	// Add the volume to the scene
	//volume->SetPosition(0.0, 0.0, 0.0);

	renderer->AddVolume(volume);
	
	renderer->ResetCamera();
	

	// interact with data
	iren->Initialize();
	iren->Start();

	opacityFun->Delete();
	colorFun->Delete();
	property->Delete();

	box->Delete();
	volume->Delete();
	mapper->Delete();
	//reader->Delete();
	resample->Delete();
	renderer->Delete();
	renWin->Delete();
	iren->Delete();
}

SmartVolume::SmartVolume(){

}


