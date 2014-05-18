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
    state = shared_ptr<BaseState>(new BaseState);
    
    transform.modelviewMatrix.push(mat4Identity);
    transform.projectionMatrix.push(mat4Identity);
    transform.textureMatrix.push(mat4Identity);
    
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
    //[self updateMatrixState];
  }
  
}

