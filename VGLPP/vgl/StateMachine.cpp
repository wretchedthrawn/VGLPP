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

#include <iostream>
#include <stdexcept>
#include "StateMachine.h"
#include "ShaderEffect.h"
#include "SolidColorShaderEffect.h"
#include "GouraudShadingShaderEffect.h"
#include "BaseState.h"
#include "Constants.h"

using namespace std;

namespace vgl
{
  static StateMachine *constructingStateMachine = nullptr;
  
  static inline float RAD(float n)  { return (n * (MathConstants::PI/180.0f)); }
  //static inline float DEG(float n)  { return (n * (180.0f/MathConstants::PI)); }
  
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
    if(constructingStateMachine)
    {
      return *constructingStateMachine;
    }
    else
    {
      //let the system handle this
      return *(System::system().currentStateMachine());
    }
  }
  
  StateMachine::StateMachine()
  {
    constructingStateMachine = this;
    
    //setup state model object
    state = shared_ptr<BaseState>(new BaseState);
    blendingOn = false;
    
    //create system shader effects
    solidColorEffect = shared_ptr<ShaderEffect>(new SolidColorShaderEffect);
    gouraudEffect = shared_ptr<ShaderEffect>(new GouraudShadingShaderEffect);
    pushShaderEffect(defaultEffectForDevice());
    
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
    
    cout << "VGL Seems to be up and running..." << endl;
    
    //drop this when the system is finished grabbing it
    System::system().scheduleTask([] {
      constructingStateMachine = nullptr;
    });
  }
  
  StateMachine::~StateMachine()
  {
    cout << "State machine going down... " << endl;
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
    shaderEffect = effect;
  }
  
  void StateMachine::pushShaderEffect(shared_ptr<ShaderEffect> effect)
  {
    shaderEffectStack.push(effect);
    shaderEffect = effect;
  }
  
  void StateMachine::popShaderEffect()
  {
    shaderEffectStack.pop();
    shaderEffect = shaderEffectStack.top();
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
      throw runtime_error("modelview transform stack overflow");
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

  void StateMachine::updateTextureMatrixState()
  {
    mat4 &tm = transform.textureMatrix.top();
    state->setTextureTransformMatrix(tm);

    shaderEffect->setTextureTransformDirty();
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

  void StateMachine::modelViewLoadIdentity()
  {
    transform.modelviewMatrix.top() = mat4Identity;
    updateMatrixState();
  }
  
  void StateMachine::transformf(const mat4 &matrix)
  {
    transform.modelviewMatrix.top() = mat4Multiply(transform.modelviewMatrix.top(), matrix);
    updateMatrixState();
  }
  
  void StateMachine::transformf(const GLfloat *matrix)
  {
    mat4 mat = mat4MakeWithArray((float *)matrix);
    transformf(mat);
  }
  
  void StateMachine::translate(float3 trans)
  {
    transform.modelviewMatrix.top() = mat4Translate(transform.modelviewMatrix.top(), trans.x, trans.y, trans.z);
    updateMatrixState();
  }
  
  void StateMachine::scale(float3 scale)
  {
    transform.modelviewMatrix.top() = mat4Scale(transform.modelviewMatrix.top(), scale.x, scale.y, scale.z);
    updateMatrixState();
  }
  
  void StateMachine::rotate(float ang, float3 normalizedVector)
  {
    transform.modelviewMatrix.top() = mat4RotateWithVector3(transform.modelviewMatrix.top(), RAD(ang),
                                                            make_float3(normalizedVector.x, normalizedVector.y, normalizedVector.z));
    updateMatrixState();
  }

  void StateMachine::lookAt(float3 point, float3 viewer, float3 up)
  {
    transform.modelviewMatrix.top() = mat4MakeLookAt(viewer.x, viewer.y, viewer.z, point.x, point.y, point.z, up.x, up.y, up.z);
    updateMatrixState();
  }
  
  void StateMachine::pushTextureMatrix()
  {
    transform.textureMatrix.push(transform.textureMatrix.top());
#ifdef DEBUG
    if(transform.textureMatrix.size() > 10)
      throw runtime_error("texture transform stack overflow");
#endif
  }
  
  void StateMachine::popTextureMatrix()
  {
    transform.textureMatrix.pop();
    if(transform.textureMatrix.size() == 0)
    {
      cerr << "** Warning: underflow of texture matrix stack!!!!" << endl;
      return;
    }
    updateMatrixState();
  }
  
  void StateMachine::textureMatrixLoadIdentity()
  {
    transform.textureMatrix.top() = mat4Identity;
    updateMatrixState();
  }
  
  void StateMachine::textureTranslate(float2 trans)
  {
    transform.textureMatrix.top() = mat4Translate(transform.textureMatrix.top(), trans.x, trans.y, 0);
    updateMatrixState();
  }
  
  void StateMachine::textureScale(float2 scale)
  {
    transform.textureMatrix.top() = mat4Scale(transform.textureMatrix.top(), scale.x, scale.y, 1);
    updateMatrixState();
  }
  
  void StateMachine::textureRotate(float angle)
  {
    transform.textureMatrix.top() = mat4Rotate(transform.textureMatrix.top(), RAD(angle), 0, 0, 1);
    updateMatrixState();
  }
  
  void StateMachine::pushProjection()
  {
    transform.projectionMatrix.push(transform.projectionMatrix.top());
#ifdef DEBUG
    if(transform.projectionMatrix.size() > 10)
      throw runtime_error("projection transform stack overflow");
#endif
  }
  
  void StateMachine::popProjection()
  {
    transform.projectionMatrix.pop();
    updateMatrixState();
  }
  
  void StateMachine::ortho2D(float left, float right, float bottom, float top, float znear, float zfar)
  {
    transform.projectionMatrix.top() = mat4MakeOrtho(left, right, bottom, top, znear, zfar);
    updateMatrixState();
  }
  
  void StateMachine::perspective(float fovy, float aspect, float znear, float zfar)
  {
    transform.projectionMatrix.top() = mat4MakePerspective(RAD(fovy), aspect, znear, zfar);
    updateMatrixState();
  }
  
  void StateMachine::enableTexture0(bool b)
  {
    bool oldB = state->getTexture0()->isEnabled();

    state->getTexture0()->setEnabled(b ? GL_TRUE : GL_FALSE);
    if(b != oldB)
      shaderEffect->setTextureDirty(0);
  }
  
  void StateMachine::setPrimaryColor(float4 color)
  {
    state->setConstantColor(color);
    shaderEffect->setConstantColorDirty();
  }
  
  void StateMachine::setPrimaryColor(uint4 colorBytes)
  {
    state->setConstantColor(make_float4(colorBytes.x/(float)255, colorBytes.y/(float)255, colorBytes.z/(float)255, colorBytes.w/(float)255));
    shaderEffect->setConstantColorDirty();
  }
  
  bool StateMachine::enableBlending(bool b)
  {
    if(b != blendingOn)
    {
      if(b)
        glEnable(GL_BLEND);
      else
        glDisable(GL_BLEND);
      
      blendingOn = b;
      
      //state was changed
      return true;
    }
    
    return false;
  }
  
  void StateMachine::setBlendFuncSourceFactor(GLenum srcFactor, GLenum dstFactor)
  {
    glBlendFunc(srcFactor, dstFactor);
  }
  
  void StateMachine::enableNormalRescale(bool b)
  {
    //most of my shaders have optimized this out.. so nothing for now
  }
  
  void StateMachine::setVertexPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen)
  {
    glBindBuffer(GL_ARRAY_BUFFER, utilityVBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, bufferLen, ptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(VertexAttribPosition, size, type, GL_FALSE, stride, 0);
  }
  
  void StateMachine::setVertexVBO(int size, GLenum type, int stride, GLvoid *offset)
  {
    glVertexAttribPointer(VertexAttribPosition, size, type, GL_FALSE, stride, offset);
  }
  
  void StateMachine::enableVertexArray(bool b)
  {
    if(b)
      glEnableVertexAttribArray(VertexAttribPosition);
    else
      glDisableVertexAttribArray(VertexAttribPosition);
  }
  
  void StateMachine::setNormalPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen)
  {
    glBindBuffer(GL_ARRAY_BUFFER, utilityVBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, bufferLen, ptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(VertexAttribNormal, size, type, GL_FALSE, stride, 0);
  }
  
  void StateMachine::setNormalVBO(int size, GLenum type, int stride, GLvoid *offset)
  {
    glVertexAttribPointer(VertexAttribNormal, size, type, GL_FALSE, stride, offset);
  }
  
  void StateMachine::enableNormalArray(bool b)
  {
    if(b)
      glEnableVertexAttribArray(VertexAttribNormal);
    else
      glDisableVertexAttribArray(VertexAttribNormal);
  }
  
  void StateMachine::setTangentVBO(int size, GLenum type, int stride, GLvoid *offset)
  {
    glVertexAttribPointer(VertexAttribTangent, size, type, GL_FALSE, stride, offset);
  }
  
  void StateMachine::enableTangentArray(bool b)
  {
    if(b)
      glEnableVertexAttribArray(VertexAttribTangent);
    else
      glDisableVertexAttribArray(VertexAttribTangent);
  }

  void StateMachine::setTexcoordPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen)
  {
    glBindBuffer(GL_ARRAY_BUFFER, utilityVBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, bufferLen, ptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(VertexAttribTexCoord0, size, type, GL_FALSE, stride, 0);
  }
  
  void StateMachine::setTexcoordVBO(int size, GLenum type, int stride, GLvoid *offset)
  {
    glVertexAttribPointer(VertexAttribTexCoord0, size, type, GL_FALSE, stride, offset);
  }
  
  void StateMachine::enableTexcoordArray(bool b)
  {
    if(b)
      glEnableVertexAttribArray(VertexAttribTexCoord0);
    else
      glDisableVertexAttribArray(VertexAttribTexCoord0);
  }

  void StateMachine::setColorPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen)
  {
    glBindBuffer(GL_ARRAY_BUFFER, utilityVBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, bufferLen, ptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(VertexAttribColor, size, type, GL_FALSE, stride, 0);
  }
  
  void StateMachine::setColorVBO(int size, GLenum type, int stride, GLvoid *offset)
  {
    glVertexAttribPointer(VertexAttribColor, size, type, GL_FALSE, stride, offset);
  }
  
  void StateMachine::enableColorArray(bool b)
  {
    if(b)
      glEnableVertexAttribArray(VertexAttribColor);
    else
      glDisableVertexAttribArray(VertexAttribColor);
  }
  
  void StateMachine::setPointSize(float sz)
  {
    shaderEffect->setPointSize(sz);
  }

  void StateMachine::prepareToDraw()
  {
    shaderEffect->prepareToDrawFromState(this);
  }
  
  void StateMachine::invalidateFramebufferAttachments(GLenum *discards, int count)
  {
    //only has implementation on mobile
  }
  
  GLuint StateMachine::getUtilityBuffer()
  {
    return utilityVBOs[4];
  }
  
  void StateMachine::enableFlatSolidColorRendering(bool b)
  {
    if(shaderEffectStack.empty())
      return;
    
    
    //god I love c++11
    bool alreadySolid = (typeid(shaderEffectStack.top()) == typeid(SolidColorShaderEffect));
    
    if(b)
    {
      if(!alreadySolid)
        pushShaderEffect(solidColorEffect);
    }
    else
    {
      if(alreadySolid)
        popShaderEffect();
    }
  }
}

