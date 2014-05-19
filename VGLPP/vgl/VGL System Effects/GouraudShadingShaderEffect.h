//
//  VGLPP
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
