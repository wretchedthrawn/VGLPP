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
    
    //Shaders and effects////////////////////////////////////////////////
    
    ///Used to efficiently determine the last bound GL shader program
    GLuint getCurrentShaderProgram();
    void useShaderProgram(GLuint shaderProgram);

    inline shared_ptr<ShaderEffect> getShaderEffect() { return shaderEffect; }
    void setShaderEffect(shared_ptr<ShaderEffect> effect);
    void pushShaderEffect(shared_ptr<ShaderEffect> effect);
    void popShaderEffect();
    shared_ptr<ShaderEffect> defaultEffectForDevice();

    //transforms/////////////////////////////////////////////////////////
    void pushModelView();
    void popModelView();
    
    void modelViewLoadIdentity();
    void transformf(const mat4 &matrix);
    void transformf(const GLfloat *matrix);
    void translate(float3 trans);
    void scale(float3 scale);
    void rotate(float ang, float3 normalizedVector);
    void lookAt(float3 point, float3 viewer, float3 up);
    
    void pushProjection();
    void popProjection();
    void ortho2D(float left, float right, float bottom, float top, float znear=-1.0f, float zfar=1.0f);
    void perspective(float fovy, float aspect, float znear, float zfar);
    
    void pushTextureMatrix();
    void popTextureMatrix();
    void textureMatrixLoadIdentity();
    void textureTranslate(float2 trans);
    void textureScale(float2 scale);
    void textureRotate(float angle);
    
    //texturing//////////////////////////////////////////////////////////
    void enableTexture0(bool b);
    
    //primary color/////////////////////////////////////////////////////
    void setPrimaryColor(float4 color);
    void setPrimaryColor(uint4 colorBytes);
    
    //blending/////////////////////////////////////////////////////////
    bool enableBlending(bool b);
    void setBlendFuncSourceFactor(GLenum srcFactor, GLenum dstFactor);

    
    //materials/////////////////////////////////
    void setDiffuseMaterialColor(float4 diff);
    void setAmbientMaterialColor(float4 amb);
    void setEmissiveMaterialColor(float4 em);
    void setSpecularMaterialColor(float4 spec);
    void setShininessMaterialValue(float shininess);
    
    //lights////////////////////////////////////////////////////////////
    void setLightPosition(float4 v, int lightIndex);
    void setLightAmbientColor(float4 v, int lightIndex);
    void setLightDiffuseColor(float4 v, int lightIndex);
    void setLightSpecularColor(float4 v, int lightIndex);
    void setLightSpotDirection(float3 v, int lightIndex);
    void setLightSpotExponent(float exp, int lightIndex);
    void setLightSpotCutoff(float cutoff, int lightIndex);
    void setLightAttenuationFactors(float3 v, int lightIndex);
    void enableLight(bool b, int lightIndex);
    
    //vertex array specification//////////////////////////////////////////

    ///This method prepopulates a VBO every time it is called and should only be used for legacy code that uses small vbos
    void setVertexPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen);
    
    ///The preferred method for specifying a vertex array, will use the currently bound GL_ARRAY_BUFFER
    void setVertexVBO(int size, GLenum type, int stride, GLvoid *offset);
    void enableVertexArray(bool b);

    ///This method prepopulates a VBO every time it is called and should only be used for legacy code that uses small vbos
    void setNormalPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen);
    
    ///The preferred method for specifying a vertex array, will use the currently bound GL_ARRAY_BUFFER
    void setNormalVBO(int size, GLenum type, int stride, GLvoid *offset);
    void enableNormalArray(bool b);
    
    ///The preferred method for specifying a vertex array, will use the currently bound GL_ARRAY_BUFFER
    void setTangentVBO(int size, GLenum type, int stride, GLvoid *offset);
    void enableTangentArray(bool b);
    
    ///This method prepopulates a VBO every time it is called and should only be used for legacy code that uses small vbos
    void setTexcoordPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen);
    
    ///The preferred method for specifying a vertex array, will use the currently bound GL_ARRAY_BUFFER
    void setTexcoordVBO(int size, GLenum type, int stride, GLvoid *offset);
    void enableTexcoordArray(bool b);

    ///This method prepopulates a VBO every time it is called and should only be used for legacy code that uses small vbos
    void setColorPointer(int size, GLenum type, int stride, GLvoid *ptr, GLsizeiptr bufferLen);
    
    ///The preferred method for specifying a vertex array, will use the currently bound GL_ARRAY_BUFFER
    void setColorVBO(int size, GLenum type, int stride, GLvoid *offset);
    void enableColorArray(bool b);

    /** currently not implemented **/
    void enableNormalRescale(bool b);
    
    /** Guaranteed to be available and not used by any other system routines */
    GLuint getUtilityBuffer();
    
    //Points///////////////////////////////////////////////////////////////
    
    void setPointSize(float sz);
    
    //Rendering modes (built-in shaders)///////////////////////////////////
    void enableFlatSolidColorRendering(bool b);
    
    //drawing//////////////////////////////////////////////////////////////
    
    //Note that I do not abstract glDraw* calls here, just go ahead and use those directly
    
    ///Should be called before any gl draw calls to sync state to the current shader effect
    void prepareToDraw();
    
    void invalidateFramebufferAttachments(GLenum *discards, int count);

  private:
    
    void updateMatrixState();
    void updateTextureMatrixState();
    
    //core state
    shared_ptr<BaseState> state;
    Transform transform;
    bool blendingOn;
    
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
