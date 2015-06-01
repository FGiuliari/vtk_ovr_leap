/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkStereoDistortPass.h

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkStereoDistortPass - Implement a post-processing Gaussian blur
// render pass.
// .SECTION Description
// Blur the image renderered by its delegate. Blurring uses a Gaussian low-pass
// filter with a 5x5 kernel.
//
// This pass expects an initialized depth buffer and color buffer.
// Initialized buffers means they have been cleared with farest z-value and
// background color/gradient/transparent color.
// An opaque pass may have been performed right after the initialization.
//
// The delegate is used once.
//
// Its delegate is usually set to a vtkCameraPass or to a post-processing pass.
//
// This pass requires a OpenGL context that supports texture objects (TO),
// framebuffer objects (FBO) and GLSL. If not, it will emit an error message
// and will render its delegate and return.
//
// .SECTION Implementation
// As the filter is separable, it first blurs the image horizontally and then
// vertically. This reduces the number of texture sampling to 5 per pass.
// In addition, as texture sampling can already blend texel values in linear
// mode, by adjusting the texture coordinate accordingly, only 3 texture
// sampling are actually necessary.
// Reference: OpenGL Bloom Toturial by Philip Rideout, section
// Exploit Hardware Filtering  http://prideout.net/bloom/index.php#Sneaky

// .SECTION See Also
// vtkRenderPass

#ifndef __vtkStereoDistortPass_h
#define __vtkStereoDistortPass_h


#include <vtkImageProcessingPass.h>

class vtkOpenGLRenderWindow;
class vtkDepthPeelingPassLayerList; // Pimpl
class vtkShaderProgram2;
class vtkShader2;
class vtkFrameBufferObject;
class vtkTextureObject;

class vtkStereoDistortPass : public vtkImageProcessingPass
{
public:
	static vtkStereoDistortPass *New();
	vtkTypeMacro(vtkStereoDistortPass, vtkImageProcessingPass);
	void PrintSelf(ostream& os, vtkIndent indent);
	double IPD;
	//BTX
	// Description:
	// Perform rendering according to a render state \p s.
	// \pre s_exists: s!=0
	virtual void Render(const vtkRenderState *s);
	//ETX

	// Description:
	// Release graphics resources and ask components to release their own
	// resources.
	// \pre w_exists: w!=0
	void ReleaseGraphicsResources(vtkWindow *w);

	void SetIPD(double val);
	virtual void SetDistortionParams(float myLensCenter[4], float myScreenCenter[2], float myScale[2], float myScaleIn[2], float myWarp[4]);


protected:
	// Description:
	// Default constructor. DelegatePass is set to NULL.
	vtkStereoDistortPass();

	// Description:
	// Destructor.
	virtual ~vtkStereoDistortPass();

	// Description:
	// Graphics resources.
	vtkFrameBufferObject *FrameBufferObject;
	vtkTextureObject *Pass1; // render target for the scene
	vtkTextureObject *Pass2; // render target for the horizontal pass
	vtkTextureObject *Pass3;
	vtkShaderProgram2 *BlurProgram; // blur shader

	bool Supported;
	bool SupportProbed;

	

private:
	vtkStereoDistortPass(const vtkStereoDistortPass&);  // Not implemented.
	void operator=(const vtkStereoDistortPass&);  // Not implemented.
	void getEyeFromCenter(bool isLeft, double pos[3], double foc[3], double up[3], double output[3]);
	float myLensCenter[4], myScreenCenter[2], myScale[2], myScaleIn[2], myWarp[4];
};

#endif
