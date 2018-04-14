#ifndef FFGLDrip_H
#define FFGLDrip_H

#include "FFGLPluginSDK.h"

class FFGLDrip :
public CFreeFrameGLPlugin
{
public:
	FFGLDrip();
	~FFGLDrip() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////
	
	FFResult SetFloatParameter(unsigned int dwIndex, float value) override;
	float	GetFloatParameter(DWORD dwIndex);
	char*	GetParameterDisplay(DWORD dwIndex);
	FFResult	ProcessOpenGL(ProcessOpenGLStruct* pGL);
	FFResult	InitGL(const FFGLViewportStruct *vp);
	FFResult	DeInitGL();

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////
	static FFResult __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLDrip();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }


protected:	
	// Parameters
	float m_blend;

	int m_initResources;
	char m_DisplayValue[15];	//buffer for parameter display value
};


#endif
