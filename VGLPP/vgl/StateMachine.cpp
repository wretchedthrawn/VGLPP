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

#include <stdexcept>
#include "StateMachine.h"
#include "ShaderEffect.h"
#include "SolidColorShaderEffect.h"
#include "GouraudShadingShaderEffect.h"
#include "BaseState.h"

using namespace std;

namespace vgl
{
  //static inline float RAD(float n)  { return (n * (M_PI/180.0f)); }
  //static inline float DEG(float n)  { return (n * (180.0f/M_PI)); }
  
  static const float2 texRect[4] =
  {
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 0, 1 }
  };
  static const float2 texRectTc[4] =
  {
    { 0, 0 },
    { 1, 0 },
    { 1, 1 },
    { 0, 1 }
  };

  StateMachine &StateMachine::machine()
  {
    //let the system handle this
    return *(System::system().currentStateMachine());
  }
  
  StateMachine::StateMachine()
  {
    //setup state model object
    state = shared_ptr<BaseState>(new BaseState);
    
    //create system shader effects
    solidColorEffect = shared_ptr<ShaderEffect>(new SolidColorShaderEffect);
    gouraudEffect = shared_ptr<ShaderEffect>(new GouraudShadingShaderEffect);
    shaderEffectStack.push(defaultEffectForDevice());
    
    //init the transform stacks
    transform.modelviewMatrix.push(mat4Identity);
    transform.projectionMatrix.push(mat4Identity);
    transform.textureMatrix.push(mat4Identity);
    
    //setup the utility vertex buffers & arrays
    glGenBuffers(5, utilityVBOs);
    glGenBuffers(2, texRectVBOs);
    utilityVBO = utilityVBOs[0];
    
    glGenVertexArrays(1, &texRectVAO);
    glBindVertexArray(texRectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, texRectVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float2)*4, texRect, GL_STATIC_DRAW);
    glVertexAttribPointer(VertexAttribPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VertexAttribPosition);
    glBindBuffer(GL_ARRAY_BUFFER, texRectVBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float2)*4, texRectTc, GL_STATIC_DRAW);
    glVertexAttribPointer(VertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VertexAttribTexCoord0);
    
    glGenVertexArrays(1, &defaultVAO);
    glBindVertexArray(defaultVAO);
  }
  
  StateMachine::~StateMachine()
  {
    if(texRectVAO)
      glDeleteVertexArrays(1, &texRectVAO);
    if(defaultVAO)
      glDeleteVertexArrays(1, &defaultVAO);
    glDeleteBuffers(5, utilityVBOs);
    glDeleteBuffers(2, texRectVBOs);
  }
  
  void StateMachine::bindDefaultVAO()
  {
    glBindVertexArray(defaultVAO);
  }
  
  void StateMachine::bindTexRectVAO()
  {
    glBindVertexArray(texRectVAO);
  }
  
  void StateMachine::bindDefaultFramebuffer()
  {
    System::system().bindDefaultFramebuffer();
  }
  
  GLuint StateMachine::getCurrentShaderProgram()
  {
    return currentShaderProgram;
  }
  
  void StateMachine::useShaderProgram(GLuint shaderProgram)
  {
    //Efficiently only change GL state when necessary
    if(currentShaderProgram != shaderProgram)
    {
      currentShaderProgram = shaderProgram;
      glUseProgram(shaderProgram);
    }
  }

  void StateMachine::setShaderEffect(shared_ptr<ShaderEffect> effect)
  {
    shaderEffectStack.top() = effect;
  }
  
  void StateMachine::pushShaderEffect(shared_ptr<ShaderEffect> effect)
  {
    shaderEffectStack.push(effect);
  }
  
  void StateMachine::popShaderEffect()
  {
    shaderEffectStack.pop();
  }
  
  shared_ptr<ShaderEffect> StateMachine::defaultEffectForDevice()
  {
    return gouraudEffect;
  }
  
  void StateMachine::pushModelView()
  {
    transform.modelviewMatrix.push(transform.modelviewMatrix.top());
#ifdef DEBUG
    if(transform.modelviewMatrix.size() > 32)
      throw runtime_error("modelview transorm stack overflow");
#endif
  }
  
  void StateMachine::popModelView()
  {
    transform.modelviewMatrix.pop();
    updateMatrixState();
  }
  
  void StateMachine::updateMatrixState()
  {
    state->getTransform()->setModelviewMatrix(transform.modelviewMatrix.top());
    state->getTransform()->setProjectionMatrix(transform.projectionMatrix.top());

    shaderEffect->setTransformDirty();
  }

  void StateMachine::setDiffuseMaterialColor(float4 diff)
  {
    state->getMaterial()->setDiffuse(diff);
    shaderEffect->setMaterialDiffuseDirty();
  }
  
  void StateMachine::setAmbientMaterialColor(float4 amb)
  {
    state->getMaterial()->setAmbient(amb);
    shaderEffect->setMaterialAmbientDirty();
  }
  
  void StateMachine::setEmissiveMaterialColor(float4 em)
  {
    state->getMaterial()->setEmissive(em);
    shaderEffect->setMaterialEmissiveDirty();
  }
  
  void StateMachine::setSpecularMaterialColor(float4 spec)
  {
    state->getMaterial()->setSpecular(spec);
    shaderEffect->setMaterialSpecularDirty();
  }
  
  void StateMachine::setShininessMaterialValue(float shininess)
  {
    state->getMaterial()->setShininess(shininess);
    shaderEffect->setMaterialShininessDirty();
  }

  void StateMachine::setLightPosition(float4 v, int lightIndex)
  {
    //transform it first
    float4 transV = mat4MultiplyFloat4(transform.modelviewMatrix.top(), v);
    shared_ptr<EffectPropertyLight> light = state->lightAtIndex(lightIndex);
    
    float oldW = light->getPosition().w;
    
    light->setWorldPosition(v);
    light->setPosition(transV);
    shaderEffect->setLightDirty(lightIndex);
    
    if(v.w != oldW)
    {
      //for now just rebuild every internal shader that depends on this.
      GouraudShadingShaderEffect *gouraud = (GouraudShadingShaderEffect *)gouraudEffect.get();
      gouraud->setLightingVeryDirty();
    }
  }

  void StateMachine::setLightAmbientColor(float4 v, int lightIndex)
  {
    state->lightAtIndex(lightIndex)->setAmbient(v);
    shaderEffect->setLightDirty(lightIndex);
  }
  
  void StateMachine::setLightDiffuseColor(float4 v, int lightIndex)
  {
    state->lightAtIndex(lightIndex)->setDiffuse(v);
    shaderEffect->setLightDirty(lightIndex);
  }
  
  void StateMachine::setLightSpecularColor(float4 v, int lightIndex)
  {
    state->lightAtIndex(lightIndex)->setSpecular(v);
    shaderEffect->setLightDirty(lightIndex);
  }
  
  void StateMachine::setLightSpotDirection(float3 v, int lightIndex)
  {
    float4 glkV = make_float4(v.x, v.y, v.z, 0);
    glkV = mat4MultiplyFloat4(transform.modelviewMatrix.top(), glkV);
    float3 transV = make_float3(glkV.x, glkV.y, glkV.z);
    shared_ptr<EffectPropertyLight> light = state->lightAtIndex(lightIndex);

    light->setSpotDirection(transV);
    light->setWorldSpotDirection(v);
    shaderEffect->setLightDirty(lightIndex);
  }
  
  void StateMachine::setLightSpotExponent(float exp, int lightIndex)
  {
    state->lightAtIndex(lightIndex)->setSpotExponent(exp);
    shaderEffect->setLightDirty(lightIndex);
  }
  
  void StateMachine::setLightSpotCutoff(float cutoff, int lightIndex)
  {
    shared_ptr<EffectPropertyLight> light = state->lightAtIndex(lightIndex);

    float oldCutoff = light->getSpotCutoff();
    bool oldIsSpot = (oldCutoff != 180.0f);

    light->setSpotCutoff(cutoff);
    shaderEffect->setLightDirty(lightIndex);

    if((cutoff != 180.0f) != oldIsSpot)
    {
      //for now just rebuild every internal shader that depends on this.
      GouraudShadingShaderEffect *gouraud = (GouraudShadingShaderEffect *)gouraudEffect.get();
      gouraud->setLightingVeryDirty();
    }
  }
  
  void StateMachine::setLightAttenuationFactors(float3 v, int lightIndex)
  {
    shared_ptr<EffectPropertyLight> light = state->lightAtIndex(lightIndex);
    
    float3 oldAtten = light->getAttenuation();
    bool oldState = (oldAtten.x != 1.0 || oldAtten.y != 0.0 || oldAtten.z != 0.0);
    
    light->setAttenuation(v.x, v.y, v.z);
    shaderEffect->setLightDirty(lightIndex);
    
    bool lState = (v.x != 1.0 || v.y != 0.0 || v.z != 0.0);
    if(lState != oldState)
    {
      //for now just rebuild every internal shader that depends on this.
      GouraudShadingShaderEffect *gouraud = (GouraudShadingShaderEffect *)gouraudEffect.get();
      gouraud->setLightingVeryDirty();
    }
  }
  
  void StateMachine::enableLight(bool b, int lightIndex)
  {
    shared_ptr<EffectPropertyLight> light = state->lightAtIndex(lightIndex);

    bool oldState = light->isEnabled(), lState = b ? true : false;
    light->setEnabled(b);
    shaderEffect->setLightDirty(lightIndex);

    if(lState != oldState)
    {
      //for now just rebuild every internal shader that depends on this.
      GouraudShadingShaderEffect *gouraud = (GouraudShadingShaderEffect *)gouraudEffect.get();
      gouraud->setLightingVeryDirty();
    }
  }

  
}

