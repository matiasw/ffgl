#pragma once

#include <FFGLShader.h>
#include "FFGLPluginSDK.h"
#include <string>

#define RESOLUTION 1024

class FFGLDripB : public CFreeFrameGLPlugin
{
public:
	FFGLDripB();
	~FFGLDripB();

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////
	
	FFResult SetFloatParameter(unsigned int dwIndex, float value) override;		
	float GetFloatParameter(unsigned int index) override;					
	FFResult ProcessOpenGL(ProcessOpenGLStruct* pGL) override;
	FFResult InitGL(const FFGLViewportStruct *vp) override;
	FFResult DeInitGL() override;

	//The mixer short name
	const char * GetShortName() override { static const char* sname = "DripB"; return sname; }

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static FFResult __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
	  *ppOutInstance = new FFGLDripB();
	  if (*ppOutInstance != NULL)
		  return FF_SUCCESS;
	  return FF_FAIL;
  }


protected:	
	// Parameters
	float m_blend;
	int m_initResources;

	float m_speeds[RESOLUTION];

};
