//
//  VGLPP
//  A very limited port of the Verto Core GL Layer from Verto Studio to C++
//  http://vertostudio.com
//
//
//  This code comes at absolutely NO WARRANTY.
//  The author accepts no liability whatsoever for any effects,
//  adverse or otherwise, that may arise as a result of using this software.
//  By using this code, or any derivitive works thereof, you
//  are stating that you have read and accept these terms, and that you are
//  using this software at YOUR OWN RISK.
//

#ifndef __VGLPP_Demo__GouraudShadingShaderEffect__
#define __VGLPP_Demo__GouraudShadingShaderEffect__

#include <map>
#include <string>
#include "ShaderEffect.h"

using std::string;

namespace vgl
{
  /**
   This shader shows an example of how to near-perfectly emulate the
   original fixed-functionality system that legacy OpenGL runs on top of
   it targets OpenGLES performance over readability.  Because it's so bloated
   and feature-heavy, It's not strongly recommended to heavily rely on this shader 
   for production games, but it'll get the job done for testing and in cases where
   you want a shader that just works properly.
   */
  
  class GouraudShadingShaderEffect : public ShaderEffect
  {
  public:
    GouraudShadingShaderEffect();
    virtual ~GouraudShadingShaderEffect();
    
    virtual void prepareToDrawFromState(StateMachine *machine);
    virtual void setTextureDirty(unsigned int texture);
    
    void setPointSize(float p);
    
    /// Causes a full recompilation of lighting shader state.
    void setLightingVeryDirty();

  private:
    
    string genFragmentShaderForTextureMode(bool tex);
    string genVertexShaderForTextureMode(bool tex);
    bool updateVertexShaderForLightingAndTextureMode(bool tex);
    bool updateVertexShaderForLighting();
    
    GLuint shaderProgramTex;
    
    float pointSize;
    unsigned int lightingRecompilationDirtyFlags, builtLightFlags;
    
    std::map<string, int> uniformLookupTableTex;
    
    GLuint *shaders;
    GLuint *shadersTex;
  };
}

#endif /* defined(__VGLPP_Demo__GouraudShadingShaderEffect__) */
