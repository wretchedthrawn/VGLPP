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

#include "SolidColorShaderEffect.h"

namespace vgl
{
  SolidColorShaderEffect::SolidColorShaderEffect()
  {
    useFullVertexAttribs = GL_FALSE;
    loadShader("solidColor.vs", "solidColor.fs", &shaderProgram);
    
    //Uniform initialization
    glUseProgram(shaderProgram);
    setShaderUniform("pointSize", 1.0f, shaderProgram);
    setShaderUniform("solidColor", make_float4(1, 1, 1, 1), shaderProgram);
    uniformLookupTable.clear();
    
    pointSize = 1.0f;
  }
  
  SolidColorShaderEffect::~SolidColorShaderEffect()
  {
    
  }
  
  void SolidColorShaderEffect::setPointSize(float ps)
  {
    pointSize = ps;
    dirtyStateFlags |= DS_MISC_FLAGS;
  }
  
  void SolidColorShaderEffect::prepareToDrawFromState(StateMachine *machine)
  {
    ShaderEffect::prepareToDrawFromState(machine);
    
    GLint loc = 0;
    
    if(machine)
      machine->useShaderProgram(shaderProgram);
    else
      glUseProgram(shaderProgram);
    
    //Set transform
    if(dirtyStateFlags & DS_TRANSFORM)
    {
      mat4 modelViewProjectionMatrix = mat4Multiply(state->getTransform()->getProjectionMatrix(), state->getTransform()->getModelviewMatrix());
      
      loc = getUniformLocation("modelViewProjectionMatrix");
      if(loc >= 0)
        glUniformMatrix4fv(loc, 1, 0, modelViewProjectionMatrix.m);
    }
    
    if(dirtyStateFlags & DS_CONSTANT_COLOR)
    {
      setShaderUniform("solidColor", state->getConstantColor(), shaderProgram);
    }
    
    if(dirtyStateFlags & DS_MISC_FLAGS)
    {
      setShaderUniform("pointSize", pointSize, shaderProgram);
    }
    
    dirtyStateFlags = 0;
  }
}
