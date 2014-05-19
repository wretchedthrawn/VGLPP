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


    //transforms////////////////////////////////
    void pushModelView();
    void popModelView();
    
  private:
    //core state
    shared_ptr<BaseState> state;
    Transform transform;
    
    GLuint currentShaderProgram;

    GLuint defaultVAO, texRectVAO;
    GLuint utilityVBO, utilityVBOs[5], texRectVBOs[2];
  };
}

#endif /* defined(__VGLPP_Demo__StateMachine__) */
