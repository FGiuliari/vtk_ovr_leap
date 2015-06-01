
#include "vtkStereoDistortPass.h"
#include <vtkObjectFactory.h>
#include <cassert>
#include <vtkRenderState.h>
#include <vtkRenderer.h>
#include <vtkgl.h>
#include <vtkFrameBufferObject.h>
#include <vtkTextureObject.h>
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>
#include <vtkUniformVariables.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkTextureUnitManager.h>
#include <vtkOpenGLError.h>
#include <vtkMath.h>

// to be able to dump intermediate passes into png files for debugging.
// only for vtkStereoDistortPass developers.
//#define VTK_GAUSSIAN_BLUR_PASS_DEBUG

#include <vtkPNGWriter.h>
#include <vtkImageImport.h>
#include <vtkPixelBufferObject.h>
#include <vtkPixelBufferObject.h>
#include <vtkImageExtractComponents.h>
#include <vtkCamera.h>
#include <vtkMath.h>

vtkStandardNewMacro(vtkStereoDistortPass);

char *vtkStereoDistortPassShader_fs;


// ----------------------------------------------------------------------------
vtkStereoDistortPass::vtkStereoDistortPass()
{
	this->FrameBufferObject = 0;
	this->Pass1 = 0;
	this->Pass2 = 0;
	this->Pass3 = 0;
	this->BlurProgram = 0;
	this->Supported = false;
	this->SupportProbed = false;

	std::ifstream ifs("distortion.fs");
	std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	vtkStereoDistortPassShader_fs = (char*)malloc(sizeof(char)*std::strlen(content.c_str()));
	std::strcpy(vtkStereoDistortPassShader_fs, content.c_str());
}

// ----------------------------------------------------------------------------
vtkStereoDistortPass::~vtkStereoDistortPass()
{
	if (this->FrameBufferObject != 0)
	{
		vtkErrorMacro(<< "FrameBufferObject should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass1 != 0)
	{
		vtkErrorMacro(<< "Pass1 should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->Pass2 != 0)
	{
		vtkErrorMacro(<< "Pass2 should have been deleted in ReleaseGraphicsResources().");
	}
	if (this->BlurProgram != 0)
	{
		this->BlurProgram->Delete();
	}
}

// ----------------------------------------------------------------------------
void vtkStereoDistortPass::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

void vtkStereoDistortPass::SetDistortionParams(float myLensCenter[4], float myScreenCenter[2], float myScale[2], float myScaleIn[2], float myWarp[4]){
	memcpy(this->myLensCenter, myLensCenter, sizeof(float) * 4);
	memcpy(this->myScreenCenter, myScreenCenter, sizeof(float) * 2);
	memcpy(this->myScale, myScale, sizeof(float) * 2);
	memcpy(this->myScaleIn, myScaleIn, sizeof(float) * 2);
	memcpy(this->myWarp, myWarp, sizeof(float) * 4);
}


void vtkStereoDistortPass::getEyeFromCenter(bool isLeft, double pos[3], double foc[3], double up[3], double output[3]){
	double temp[3];
	//camera position to focus point vector
	vtkMath::Subtract(foc, pos, temp);
	//cross product and normalization to determine shift direction
	vtkMath::Cross(temp, up, output);
	vtkMath::Normalize(output);
	//multiplication based on IPD
	vtkMath::MultiplyScalar(output, isLeft ? -this->IPD / 2.0 : this->IPD / 2.0);
}

// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkStereoDistortPass::Render(const vtkRenderState *s)
{
	assert("pre: s_exists" && s != 0);

	vtkOpenGLClearErrorMacro();

	this->NumberOfRenderedProps = 0;

	vtkRenderer *r = s->GetRenderer();

	if (this->DelegatePass != 0)
	{
		if (!this->SupportProbed)
		{
			this->SupportProbed = true;
			// Test for Hardware support. If not supported, just render the delegate.
			bool supported = vtkFrameBufferObject::IsSupported(r->GetRenderWindow());

			if (!supported)
			{
				vtkErrorMacro("FBOs are not supported by the context. Cannot blur the image.");
			}
			if (supported)
			{
				supported = vtkTextureObject::IsSupported(r->GetRenderWindow());
				if (!supported)
				{
					vtkErrorMacro("Texture Objects are not supported by the context. Cannot blur the image.");
				}
			}

			if (supported)
			{
				supported =
					vtkShaderProgram2::IsSupported(static_cast<vtkOpenGLRenderWindow *>(
					r->GetRenderWindow()));
				if (!supported)
				{
					vtkErrorMacro("GLSL is not supported by the context. Cannot blur the image.");
				}
			}

			if (supported)
			{
				// FBO extension is supported. Is the specific FBO format supported?
				if (this->FrameBufferObject == 0)
				{
					this->FrameBufferObject = vtkFrameBufferObject::New();
					this->FrameBufferObject->SetContext(r->GetRenderWindow());
				}
				if (this->Pass1 == 0)
				{
					this->Pass1 = vtkTextureObject::New();
					this->Pass1->SetContext(r->GetRenderWindow());
				}
				if (this->Pass3 == 0)
				{
					this->Pass3 = vtkTextureObject::New();
					this->Pass3->SetContext(r->GetRenderWindow());
				}
				this->Pass1->Create2D(64, 64, 4, VTK_UNSIGNED_CHAR, false);
				this->Pass3->Create2D(64, 64, 4, VTK_UNSIGNED_CHAR, false);
				this->FrameBufferObject->SetColorBuffer(0, this->Pass1);
				this->FrameBufferObject->SetNumberOfRenderTargets(1);
				this->FrameBufferObject->SetActiveBuffer(0);
				this->FrameBufferObject->SetDepthBufferNeeded(true);

				GLint savedCurrentDrawBuffer;
				glGetIntegerv(GL_DRAW_BUFFER, &savedCurrentDrawBuffer);
				supported = this->FrameBufferObject->StartNonOrtho(64, 64, false);
				if (!supported)
				{
					vtkErrorMacro("The requested FBO format is not supported by the context. Cannot blur the image.");
				}
				else
				{
					this->FrameBufferObject->UnBind();
					glDrawBuffer(static_cast<GLenum>(savedCurrentDrawBuffer));
				}
			}

			this->Supported = supported;
		}

		if (!this->Supported)
		{
			this->DelegatePass->Render(s);
			this->NumberOfRenderedProps +=
				this->DelegatePass->GetNumberOfRenderedProps();
			return;
		}

		GLint savedDrawBuffer;
		glGetIntegerv(GL_DRAW_BUFFER, &savedDrawBuffer);

		// 1. Create a new render state with an FBO.

		int width;
		int height;
		int size[2];
		s->GetWindowSize(size);
		width = size[0];
		height = size[1];

		int w = width;
		int h = height;

		if (this->Pass1 == 0)
		{
			this->Pass1 = vtkTextureObject::New();
			this->Pass1->SetContext(r->GetRenderWindow());
		}

		if (this->Pass3 == 0)
		{
			this->Pass3 = vtkTextureObject::New();
			this->Pass3->SetContext(r->GetRenderWindow());
		}

		if (this->FrameBufferObject == 0)
		{
			this->FrameBufferObject = vtkFrameBufferObject::New();
			this->FrameBufferObject->SetContext(r->GetRenderWindow());
		}
		//*********************************************
		//Get Central eye position so i can restore it at the end
		double posCen[3], focCen[3], upCen[3], dirL[3],dirR[3];
		s->GetRenderer()->GetActiveCamera()->GetPosition(posCen);
		s->GetRenderer()->GetActiveCamera()->GetFocalPoint(focCen);
		s->GetRenderer()->GetActiveCamera()->GetViewUp(upCen);
		/*cout << "***************************************************\n";
		cout << "PosCen: " << posCen[0] << "//" << posCen[1] << "//" << posCen[2] << "//\n";
		cout << "focCen: " << focCen[0] << "//" << focCen[1] << "//" << focCen[2] << "//\n";
		cout << "upCen: " << upCen[0] << "//" << upCen[1] << "//" << upCen[2] << "//\n";*/
		//Get "central eye to left eye" vector
		this->getEyeFromCenter(true, posCen, focCen, upCen, dirL);
		//calculate left eye position and focus
		double posL[3], focL[3];
		std::memcpy(posL, posCen, sizeof(double) * 3);
		std::memcpy(focL, focCen, sizeof(double) * 3);
		vtkMath::Add(posL, dirL, posL);
		vtkMath::Add(focL, dirL, focL);
		//Get "central eye to right eye" vector
		this->getEyeFromCenter(false, posCen, focCen, upCen, dirR);
		//calculate left eye position and focus
		double posR[3], focR[3];
		std::memcpy(posR, posCen, sizeof(double) * 3);
		std::memcpy(focR, focCen, sizeof(double) * 3);
		vtkMath::Add(posR, dirR, posR);
		vtkMath::Add(focR, dirR, focR);



		s->GetRenderer()->GetActiveCamera()->SetPosition(posL);
		s->GetRenderer()->GetActiveCamera()->SetFocalPoint(focL);
		//s->GetRenderer()->SetBackground(1.0, 0.0, 0.0);
		this->RenderDelegate(s, width, height, w, h, this->FrameBufferObject,this->Pass1);
		s->GetRenderer()->GetActiveCamera()->SetPosition(posR);
		s->GetRenderer()->GetActiveCamera()->SetFocalPoint(focR);
		//s->GetRenderer()->SetBackground(0.0, 1.0, 0.0);
		this->RenderDelegate(s, width, height, w, h, this->FrameBufferObject, this->Pass3);

		//Restore central eye original position
		s->GetRenderer()->GetActiveCamera()->SetPosition(posCen);
		s->GetRenderer()->GetActiveCamera()->SetFocalPoint(focCen);
		s->GetRenderer()->GetActiveCamera()->SetViewUp(upCen);
		//*********************************************
		
		//cout << this->Pass1->GetWidth() << "\n";

		// 3. Same FBO, but new color attachment (new TO).
		if (this->Pass2 == 0)
		{
			this->Pass2 = vtkTextureObject::New();
			this->Pass2->SetContext(this->FrameBufferObject->GetContext());
		}

		if (this->Pass2->GetWidth() != static_cast<unsigned int>(width) ||
			this->Pass2->GetHeight() != static_cast<unsigned int>(height))
		{
			this->Pass2->Create2D(static_cast<unsigned int>(width),
				static_cast<unsigned int>(height), 4,
				VTK_UNSIGNED_CHAR, false);
		}

		this->FrameBufferObject->SetColorBuffer(0, this->Pass2);
		this->FrameBufferObject->Start(width, height, false);

		// Use a blur shader, do it horizontally. this->Pass1 is the source
		// (this->Pass2 is the fbo render target)

		if (this->BlurProgram == 0)
		{
			this->BlurProgram = vtkShaderProgram2::New();
			this->BlurProgram->SetContext(
				static_cast<vtkOpenGLRenderWindow *>(
				this->FrameBufferObject->GetContext()));
			vtkShader2 *shader = vtkShader2::New();
			shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
			shader->SetSourceCode(vtkStereoDistortPassShader_fs);
			shader->SetContext(this->BlurProgram->GetContext());
			this->BlurProgram->GetShaders()->AddItem(shader);
			shader->Delete();
		}

		this->BlurProgram->Build();

		if (this->BlurProgram->GetLastBuildStatus() != VTK_SHADER_PROGRAM2_LINK_SUCCEEDED)
		{
			vtkErrorMacro("Couldn't build the shader program. At this point , it can be an error in a shader or a driver bug.");

			// restore some state.
			this->FrameBufferObject->UnBind();
			glDrawBuffer(static_cast<GLenum>(savedDrawBuffer));
			return;
		}

		vtkUniformVariables *var = this->BlurProgram->GetUniformVariables();
		vtkTextureUnitManager *tu =
			static_cast<vtkOpenGLRenderWindow *>(r->GetRenderWindow())->GetTextureUnitManager();

		int sourceId = tu->Allocate();
		int sourceId3 = tu->Allocate();
		//cout << sourceId << "//" << sourceId3 << "\n";
		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + static_cast<GLenum>(sourceId));
		this->Pass1->Bind();
		vtkgl::ActiveTexture(vtkgl::TEXTURE0 + static_cast<GLenum>(sourceId3));
		this->Pass3->Bind();
		//cout << this->Pass1->GetWidth() << "\n";
		//cout << this->Pass2->GetWidth() << "\n";
		//cout << this->Pass3->GetWidth() << "\n";
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		var->SetUniformi("source", 1, &sourceId);
		var->SetUniformi("source3", 1, &sourceId3);
		//********************************
		var->SetUniformf("leftLensCenter", 2, this->myLensCenter);
		var->SetUniformf("rightLensCenter", 2, &(this->myLensCenter[2]));
		var->SetUniformf("screenCenter", 2, this->myScreenCenter);
		var->SetUniformf("scale", 2, this->myScale);
		var->SetUniformf("scaleIn", 2, this->myScaleIn);
		var->SetUniformf("hmdWarpParam", 4, this->myWarp);
		//********************************
		this->BlurProgram->Use();
		this->FrameBufferObject->UnBind();
		glDrawBuffer(static_cast<GLenum>(savedDrawBuffer));

		this->FrameBufferObject->RenderQuad(0, width - 1, 0, height - 1);

		this->Pass1->UnBind();
		this->Pass3->UnBind();

		tu->Free(sourceId);
		tu->Free(sourceId3);
		vtkgl::ActiveTexture(vtkgl::TEXTURE0);

		this->BlurProgram->Restore();
	}
	else
	{
		vtkWarningMacro(<< " no delegate.");
	}

	vtkOpenGLCheckErrorMacro("failed after Render");
}

// ----------------------------------------------------------------------------
// Description:
// Release graphics resources and ask components to release their own
// resources.
// \pre w_exists: w!=0
void vtkStereoDistortPass::ReleaseGraphicsResources(vtkWindow *w)
{
	assert("pre: w_exists" && w != 0);

	this->Superclass::ReleaseGraphicsResources(w);

	if (this->BlurProgram != 0)
	{
		this->BlurProgram->ReleaseGraphicsResources();
	}
	if (this->FrameBufferObject != 0)
	{
		this->FrameBufferObject->Delete();
		this->FrameBufferObject = 0;
	}
	if (this->Pass1 != 0)
	{
		this->Pass1->Delete();
		this->Pass1 = 0;
	}
	if (this->Pass2 != 0)
	{
		this->Pass2->Delete();
		this->Pass2 = 0;
	}
}


void vtkStereoDistortPass::SetIPD(double val){
	this->IPD = val;
}