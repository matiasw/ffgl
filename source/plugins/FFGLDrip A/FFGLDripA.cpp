#include <FFGL.h>
#include <FFGLLib.h>

#include "FFGLDripA.h"
#include "../../lib/ffgl/utilities/utilities.h"

#define FFPARAM_MixVal  (0)

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLDripA::CreateInstance,				// Create method
	"DRIP",								// Plugin unique ID
	"Drip A",      				// Plugin name
	1,						   			// API major version number 													
	500,								// API minor version number
	1,									// Plugin major version number
	000,								// Plugin minor version number
	FF_EFFECT,							// Plugin type
	"Screen drip mixer A",// Plugin description
	"by Matias Wilkman"	// About
);


FFGLDripA::FFGLDripA()
:CFreeFrameGLPlugin(),
 m_initResources(1)
{
	// Input properties
	SetMinInputs(2);
	SetMaxInputs(2);

	// Parameters
	SetParamInfo(FFPARAM_MixVal, "Mixer Value", FF_TYPE_STANDARD, 0.0f);
	m_blend = 0.0f;

	for (int i = 0; i < RESOLUTION; i++)
		m_speeds[i] = random(1.0, 1.2);
}

FFGLDripA::~FFGLDripA()
{
	
}

FFResult FFGLDripA::InitGL(const FFGLViewportStruct *vp)
{
    return FF_SUCCESS;
}

FFResult FFGLDripA::DeInitGL()
{

    return FF_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////



FFResult FFGLDripA::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	if (pGL->numInputTextures<2) return FF_FAIL;
	if (pGL->inputTextures[0] == NULL || pGL->inputTextures[1] == NULL) return FF_FAIL;

	FFGLTextureStruct &TextureA = *(pGL->inputTextures[0]);
	FFGLTextureStruct &TextureB = *(pGL->inputTextures[1]);

	//get the max s,t that correspond to the 
	//width,height of the used portion of the allocated texture space
	FFGLTexCoords maxCoordsA = GetMaxGLTexCoords(TextureA);
	FFGLTexCoords maxCoordsB = GetMaxGLTexCoords(TextureB);

	glEnable(GL_TEXTURE_2D);
	
	//source B
	glBindTexture(GL_TEXTURE_2D, TextureB.Handle);
	glBegin(GL_QUADS);
		//lower left
		glTexCoord2f(0, 0);
		glVertex2i(-1, -1);
		//upper left
		glTexCoord2f(0, 1.0f*maxCoordsB.t);
		glVertex2i(-1, 1);
		//upper right
		glTexCoord2f(1.0f*maxCoordsB.s, 1.0f*maxCoordsB.t);
		glVertex2i(1, 1);
		//lower right
		glTexCoord2f(1.0f*maxCoordsB.s, 0);
		glVertex2i(1, -1);
	glEnd();
	
	//source A
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureA.Handle);
	for (int i = 0; i < RESOLUTION; i++)
	{
		glBegin(GL_QUADS);
		//lower left
		glTexCoord2f(i*1.0 / RESOLUTION*maxCoordsA.s, 0.0);
		glVertex2f(-1 + i * 2.0 / RESOLUTION, -1.0 + m_blend * 2.0*m_speeds[i]);
		//upper left
		glTexCoord2f(i*1.0 / RESOLUTION*maxCoordsA.s, 1.0f*maxCoordsA.t);
		glVertex2f(-1 + i * 2.0 / RESOLUTION, 1.0 + m_blend * 2.0*m_speeds[i]);
		//upper right
		glTexCoord2f((i + 1)*1.0 / RESOLUTION*maxCoordsA.s, 1.0f*maxCoordsA.t);
		glVertex2f(-1 + (i + 1)*2.0 / RESOLUTION, 1.0 + m_blend * 2.0*m_speeds[i]);
		//lower right
		glTexCoord2f((i + 1)*1.0 / RESOLUTION*maxCoordsA.s, 0.0);
		glVertex2f(-1 + (i + 1)*2.0 / RESOLUTION, -1.0 + m_blend * 2.0*m_speeds[i]);
		glEnd();
	}
	
	return FF_SUCCESS;
}

float FFGLDripA::GetFloatParameter(unsigned int dwIndex)
{
	float retValue = 0.0;

	switch (dwIndex)
	{
	case FFPARAM_MixVal:
		retValue = m_blend;
		return retValue;
	default:
		return retValue;
	}
}

FFResult FFGLDripA::SetFloatParameter(unsigned int dwIndex, float value)
{
	switch (dwIndex)
	{
	case FFPARAM_MixVal:
		m_blend = value;
		break;
	default:
		return FF_FAIL;
	}

	return FF_SUCCESS;
}
