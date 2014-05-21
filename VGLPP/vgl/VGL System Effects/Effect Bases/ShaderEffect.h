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

#ifndef __VGLPP_Demo__ShaderEffect__
#define __VGLPP_Demo__ShaderEffect__

#include <map>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "vgl.h"
#include "BaseState.h"
#include "EffectPropertyMaterial.h"
#include "EffectPropertyTransform.h"
#include "EffectPropertyLight.h"
#include "EffectPropertyTexture.h"
#include "ShaderUniformInfo.h"
#include "MatTypes.h"
#include "StateMachine.h"

//Dirty state flags
#define DS_TRANSFORM            1<<0
#define DS_MAT_DIFFUSE          1<<1
#define DS_MAT_SPECULAR         1<<2
#define DS_MAT_EMISSIVE         1<<3
#define DS_MAT_SHINY            1<<4
#define DS_MAT_AMBIENT          1<<5
#define DS_TEXTURE0             1<<6
#define DS_TEXTURE1             1<<7
#define DS_CONSTANT_COLOR       1<<8
#define DS_FOG                  1<<9
#define DS_TRANSFORM_TEXTURE    1<<10
#define DS_LIGHT0               1<<11   //grouping lights for now since they don't change often
#define DS_LIGHT1               1<<12
#define DS_LIGHT2               1<<14
#define DS_LIGHT3               1<<15
#define DS_LIGHT4               1<<16
#define DS_LIGHT5               1<<17
#define DS_LIGHT6               1<<18
#define DS_LIGHT7               1<<19
#define DS_RENDER_TARGET        1<<20
#define DS_TIME                 1<<21
#define DS_MISC_FLAGS           1<<31
#define DS_ALL                  0xffffffff

using std::string;
using std::vector;

namespace vgl
{
  class ShaderEffect
  {
  public:
    
    ShaderEffect();
    virtual ~ShaderEffect();
    
    virtual void prepareToDraw();
    virtual void prepareToDrawFromState(StateMachine *machine);
    
    virtual void setAllDirty();
    virtual void setTransformDirty();
    virtual void setTextureTransformDirty();
    virtual void setMaterialAmbientDirty();
    virtual void setMaterialDiffuseDirty();
    virtual void setMaterialSpecularDirty();
    virtual void setMaterialEmissiveDirty();
    virtual void setMaterialShininessDirty();
    virtual void setLightDirty(unsigned int light);
    virtual void setTextureDirty(unsigned int texture);
    virtual void setFogDirty();
    virtual void setConstantColorDirty();
    virtual void setRenderTargetDirty();
    virtual void setTimeDirty();
    virtual void setMiscDirty();
    
    bool compileShader(GLuint *shader, GLenum type, const string &src);
    bool recompileShader(GLuint *shader, GLenum type, string src);
    bool compileShaderFromFile(GLuint *shader, GLenum type, const string &file);
    bool linkProgram(GLuint prog);
    bool validateProgram(GLuint prog);
    
    void loadShaders(const vector<string> &vtxFiles, const vector<string> &frgFiles, GLuint *program, GLuint **keep);
    void loadShader(const string &vtxFile, const string &frgFile, GLuint *program);
    void loadShaderSrc(const string &vtxSrc, const string &frgSrc, GLuint *program);
    void loadGeometryShaderSrc(const string &geomSrc, const string &vtxSrc, const string &frgSrc, GLuint *program);

    GLint getUniformLocation(const string &str);
    
    ///Currently only supports 1,2,3,4-vectors of floats and single ints, use other methods for more control
    template <typename T>
    void setShaderUniformVal(const string &uniformName, const T &val, GLuint program);

    template <typename T>
    void setShaderUniformVal(const string &uniformName, const T &val);

    void setShaderUniform(const string &uniformName, const float4 &val, GLuint program);
    void setShaderUniform(const string &uniformName, const float3 &val, GLuint program);
    void setShaderUniform(const string &uniformName, const float2 &val, GLuint program);
    void setShaderUniform(const string &uniformName, const float &val, GLuint program);
    void setShaderUniform(const string &uniformName, const int4 &val, GLuint program);
    void setShaderUniform(const string &uniformName, const int3 &val, GLuint program);
    void setShaderUniform(const string &uniformName, const int2 &val, GLuint program);
    void setShaderUniform(const string &uniformName, const int &val, GLuint program);

    void setShaderUniformMat(const string &uniformName, const float4 &mat2, GLuint program);
    void setShaderUniformMat(const string &uniformName, const mat3 &val, GLuint program);
    void setShaderUniformMat(const string &uniformName, const mat4 &val, GLuint program);
    
    bool isUniformNameBuiltin(const string &name);
    
    std::map<string, std::shared_ptr<ShaderUniformInfo>> getUniformInfo();
    
    void clearShaders(GLuint *nShaders, int count);

    void setPreLinkStep(std::function<void ()> &pls);
    
    virtual void setPointSize(float ps);
    
  protected:
    std::shared_ptr<BaseState> state;
    
    GLuint shaderProgram;
    
    unsigned int dirtyStateFlags;
    std::map<string, int> uniformLookupTable;
    
    GLboolean useFullVertexAttribs;
    
    std::function<void()> preLinkStep;
  };
}

#endif /* defined(__VGLPP_Demo__ShaderEffect__) */
