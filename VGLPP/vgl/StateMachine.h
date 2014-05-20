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

#ifndef __VGLPP__StateMachine__
#define __VGLPP__StateMachine__

#include <stack>
#include <memory>
#include "vgl.h"
#include "System.h"
#include "VecTypes.h"
#include "MatTypes.h"

using std::shared_ptr;

namespace vgl
{
  class ShaderEffect;
  class BaseState;
  
  // Named vertex attributes for mapping VGLEffects logic to client vertex attrib enables
  typedef enum
  {
    VertexAttribPosition,
    VertexAttribNormal,
    VertexAttribTangent,
    VertexAttribColor,
    VertexAttribTexCoord0,
  } VertoGLVertexAttrib;
  
  class Transform
  {
  public:
    std::stack<mat4> modelviewMatrix;
    std::stack<mat4> projectionMatrix;
    std::stack<mat4> textureMatrix;
  };
  
  class StateMachine
  {
  public:
    ///Access to the current vgl state machine object
    static StateMachine &machine();
    
    StateMachine();
    ~StateMachine();
    
    inline shared_ptr<BaseState> getState() { return state; }
    
    void bindDefaultVAO();
    void bindTexRectVAO();
    void bindDefaultFramebuffer();
    
    //Shaders and effects///////////////////////
    
    ///Used to efficiently determine the last bound GL shader program
    GLuint getCurrentShaderProgram();
    void useShaderProgram(GLuint shaderProgram);

    inline shared_ptr<ShaderEffect> getShaderEffect() { return shaderEffect; }
    void setShaderEffect(shared_ptr<ShaderEffect> effect);
    void pushShaderEffect(shared_ptr<ShaderEffect> effect);
    void popShaderEffect();
    shared_ptr<ShaderEffect> defaultEffectForDevice();

    //transforms////////////////////////////////
    void pushModelView();
    void popModelView();
    
    //materials/////////////////////////////////
    void setDiffuseMaterialColor(float4 diff);
    void setAmbientMaterialColor(float4 amb);
    void setEmissiveMaterialColor(float4 em);
    void setSpecularMaterialColor(float4 spec);
    void setShininessMaterialValue(float shininess);
    
    //lights///////////////////////////////////
    void setLightPosition(float4 v, int lightIndex);
    void setLightAmbientColor(float4 v, int lightIndex);
    void setLightDiffuseColor(float4 v, int lightIndex);
    void setLightSpecularColor(float4 v, int lightIndex);
    void setLightSpotDirection(float3 v, int lightIndex);
    void setLightSpotExponent(float exp, int lightIndex);
    void setLightSpotCutoff(float cutoff, int lightIndex);
    void setLightAttenuationFactors(float3 v, int lightIndex);
    void enableLight(bool b, int lightIndex);
    
  private:
    
    void updateMatrixState();
    
    //core state
    shared_ptr<BaseState> state;
    Transform transform;
    
    //effects
    shared_ptr<ShaderEffect> gouraudEffect;
    shared_ptr<ShaderEffect> solidColorEffect;
    
    shared_ptr<ShaderEffect> shaderEffect;
    std::stack<shared_ptr<ShaderEffect>> shaderEffectStack;
    
    GLuint currentShaderProgram;

    GLuint defaultVAO, texRectVAO;
    GLuint utilityVBO, utilityVBOs[5], texRectVBOs[2];
  };
}

#endif /* defined(__VGLPP_Demo__StateMachine__) */
