//#include <glew.h>

#include <FFGL.h>
#include <FFGLLib.h>
//#include "../common/opengl/include/glext.h"
//#include "../common/opengl/include/GLU.H"
#include "FFGLColorWrapMixer.h"
#include "utilities.h"

//#include <glut.h>
//#include <glu.h>

#ifdef _WIN32
//#include "../common/opengl/include/glut.h"
#define NOMINMAX
#include <windows.h>
#else	//all other platforms
#include <OpenGL/glu.h>
#endif
#ifdef __APPLE__	//OS X
#include <TargetConditionals.h>
#include <Carbon.h>
#endif
#include <stdio.h>

#define	FFPARAM_Blend		(0)

bool hastime = false;	//workaround for hosts without Time support

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo(
	FFGLColorWrapMixer::CreateInstance,				// Create method
	"MCWP",								// Plugin unique ID
	"ColorWrap",      				// Plugin name
	1,						   			// API major version number 													
	500,								// API minor version number
	1,									// Plugin major version number
	000,								// Plugin minor version number
	FF_EFFECT,							// Plugin type
	"Blends images by wrapping the colors around",	// Plugin description
	"by Matias Wilkman" // About
);

std::string vertexShaderCode = STRINGIFY(
void main()
{
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[1] = gl_MultiTexCoord1;
  gl_FrontColor = gl_Color;
});

std::string fragmentShaderCode = STRINGIFY(
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float blend;
void main( void )
{
    vec2 coords = gl_TexCoord[0].st;
    vec4 col = texture2D(tex0, coords);
    vec4 col2 = texture2D(tex1, coords);
    vec4 color = mix(max(mod(col + blend, 1.0), col), col2, blend);
    gl_FragColor = color;
});

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLColorWrapMixer::FFGLColorWrapMixer()
:CFreeFrameGLPlugin(),
 m_initResources(1)
{
	// Input properties
	SetMinInputs(2);
	SetMaxInputs(2);

	// parameters:
	SetParamInfo(FFPARAM_Blend, "Blend", FF_TYPE_STANDARD, 0.0f);	
	m_blend = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

FFResult FFGLColorWrapMixer::InitGL(const FFGLViewportStruct *vp)
{
	m_initResources = 0;

	//initialize gl shader
	m_shader.Compile(vertexShaderCode, fragmentShaderCode);

	//activate our shader
	m_shader.BindShader();

	//to assign values to parameters in the shader, we have to lookup
	//the "location" of each value.. then call one of the glUniform* methods
	//to assign a value
	m_sampler0location = m_shader.FindUniform("tex0");
	m_sampler1location = m_shader.FindUniform("tex1");
	m_blendlocation = m_shader.FindUniform("blend");

	glUniform1iARB(m_sampler0location, 0);
	glUniform1iARB(m_sampler1location, 1);

	m_shader.UnbindShader();

	return FF_SUCCESS;

}

FFResult FFGLColorWrapMixer::DeInitGL()
{
  m_shader.FreeGLResources();
  return FF_SUCCESS;
}

FFResult FFGLColorWrapMixer::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	if (pGL->numInputTextures < 2) return 
		FF_FAIL;

	if (pGL->inputTextures[0]==NULL || pGL->inputTextures[1]==NULL) 
		return FF_FAIL;

	//activate our shader
	m_shader.BindShader();

	glUniform1fARB(m_blendlocation, m_blend);

	FFGLTextureStruct &textureSrc = *(pGL->inputTextures[0]);
	FFGLTexCoords maxCoordsSrc = GetMaxGLTexCoords(textureSrc);

	FFGLTextureStruct &textureDest = *(pGL->inputTextures[1]);
	FFGLTexCoords maxCoordsDest = GetMaxGLTexCoords(textureDest);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureSrc.Handle);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureDest.Handle);
	
	//draw the quad that will be painted by the shader/textures
	//note that we are sending texture coordinates to texture unit 1..
	//the vertex shader and fragment shader refer to this when querying for
	//texture coordinates of the inputTexture
	glBegin(GL_QUADS);

	//lower left
	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, 0.0f);
	glMultiTexCoord2f(GL_TEXTURE1, 0.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);

	//upper left
	glMultiTexCoord2f(GL_TEXTURE0, 0.0f, maxCoordsDest.t);
	glMultiTexCoord2f(GL_TEXTURE1, 0.0f, maxCoordsSrc.t);
	glVertex2f(-1.0f, 1.0f);

	//upper right
	glMultiTexCoord2f(GL_TEXTURE0, maxCoordsDest.s, maxCoordsDest.t);
	glMultiTexCoord2f(GL_TEXTURE1, maxCoordsSrc.s, maxCoordsSrc.t);
	glVertex2f(1.0f, 1.0f);

	//lower right
	glMultiTexCoord2f(GL_TEXTURE0, maxCoordsDest.s, 0.0f);
	glMultiTexCoord2f(GL_TEXTURE1, maxCoordsSrc.s, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//unbind the shader
	m_shader.UnbindShader();

	return FF_SUCCESS;
}

float FFGLColorWrapMixer::GetFloatParameter(unsigned int dwIndex)
{
	float retValue = 0.0;

	switch (dwIndex)
	{
	case FFPARAM_Blend:
		retValue = m_blend;
		return retValue;
	default:
		return retValue;
	}
}

FFResult FFGLColorWrapMixer::SetFloatParameter(unsigned int dwIndex, float value)
{
	switch (dwIndex)
	{
	case FFPARAM_Blend:
		m_blend = value;
		break;
	default:
		return FF_FAIL;
	}

	return FF_SUCCESS;
}
