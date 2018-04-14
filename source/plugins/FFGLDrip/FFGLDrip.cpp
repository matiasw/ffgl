#ifdef _WIN32
//#include "../common/opengl/include/glut.h"
#define NOMINMAX
#include <windows.h>
#else	//all other platforms
#include <OpenGL/glu.h>
#include <sys/time.h>
#endif
#ifdef __APPLE__	//OS X
#include <TargetConditionals.h>
#endif
#include <FFGL.h>
#include <FFGLLib.h>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <algorithm>

#include "FFGLDrip.h"
#include "utilities.h"

#define FFPARAM_Blend	(0)

using namespace std;

bool hastime = false;	//workaround for hosts without Time support

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLDrip::CreateInstance,	// Create method
	"DRIP",								// Plugin unique ID											
	"Drip A",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"Screen drip mixer",	// Plugin description
	"by Matias Wilkman" // About
);

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLDrip::FFGLDrip()
:CFreeFrameGLPlugin(),
 m_initResources(1)
{
	// Input properties
	SetMinInputs(2);
	SetMaxInputs(2);

#ifdef _WIN32
	srand(GetTickCount());
#else
	srand(time(NULL));
#endif
	// Parameters
	SetParamInfo(FFPARAM_Blend, "Blend", FF_TYPE_STANDARD, 1.0f);
	m_blend = 0.0f;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

FFResult FFGLDrip::InitGL(const FFGLViewportStruct *vp)
{

	return FF_SUCCESS;
}

FFResult FFGLDrip::DeInitGL()
{
	return FF_SUCCESS;
}

FFResult FFGLDrip::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	if (pGL->numInputTextures<2) return FF_FAIL;
	if (pGL->inputTextures[0]==NULL || pGL->inputTextures[1]==NULL) return FF_FAIL;

	FFGLTextureStruct &TextureA = *(pGL->inputTextures[0]);
	FFGLTextureStruct &TextureB = *(pGL->inputTextures[1]);

	glEnable(GL_TEXTURE_2D);
	//get the max s,t that correspond to the 
	//width,height of the used portion of the allocated texture space
	FFGLTexCoords maxCoords = GetMaxGLTexCoords(TextureA);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity(); 
	glScalef(maxCoords.s, maxCoords.t, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	
	//Store current texture parameters:
	GLint minfilter, maxfilter, wraps, wrapt;
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minfilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &maxfilter);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wraps);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapt);
	
	//Set minification, magnification & wrap modes:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//source A
	glBindTexture(GL_TEXTURE_2D, TextureA.Handle);
	glBegin(GL_QUADS);
		//lower left
		glTexCoord2f(0, 0);
		glVertex2i(-1, -0.5);
		//upper left
		glTexCoord2f(0, 1.0f);
		glVertex2i(-1,1);
		//upper right
		glTexCoord2f(1.0f, 1.0f);
		glVertex2i(1,1);
		//lower right
		glTexCoord2f(1.0f, 0);
		glVertex2i(1, -0.5);
	glEnd();
	
	//source B
	glBindTexture(GL_TEXTURE_2D, TextureB.Handle);
	glBegin(GL_QUADS);
		//lower left
		glTexCoord2f(0, 0);
		glVertex2i(-1, -1+2.0-2.0*m_blend);
		//upper left
		glTexCoord2f(0, 1.0f);
		glVertex2i(-1, 1+2.0-2.0*m_blend);
		//upper right
		glTexCoord2f(1.0f, 1.0f+2.0-2.0*m_blend);
		glVertex2i(1, 1);
		//lower right
		glTexCoord2f(1.0f, 0);
		glVertex2i(1, -1+2.0-2.0*m_blend);
	glEnd();
	

	//Restore old texture parameters:
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minfilter);
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &maxfilter);
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wraps);
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapt);

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	return FF_SUCCESS;
}

float FFGLDrip::GetFloatParameter(DWORD dwIndex)
{

	switch (dwIndex) {
		case FFPARAM_Blend:
			return m_blend;
		default:
			return FF_FAIL;
	}
}

FFResult FFGLDrip::SetFloatParameter(unsigned int dwIndex, float value)
{
	switch (dwIndex) {
		case FFPARAM_Blend:
			m_blend = value;
		default:
			return FF_FAIL;
	}
	return FF_SUCCESS;
}


char* FFGLDrip::GetParameterDisplay(DWORD dwIndex) 
{	
	memset(m_DisplayValue, 0, 15);
	
	switch (dwIndex) {
		case FFPARAM_Blend:
		{
			sprintf(m_DisplayValue, "%d", m_blend);
			return m_DisplayValue;
		}
		default:
			return m_DisplayValue;
	}
	return NULL;
}