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

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include "ShaderEffect.h"
#include "StateMachine.h"

using namespace std;

namespace vgl
{
  ShaderEffect::ShaderEffect() : shaderProgram(0), useFullVertexAttribs(GL_FALSE)
  {
    state = StateMachine::machine().getState();
    
    //initially all 1's
    dirtyStateFlags = DS_ALL;
    
    preLinkStep = 0;
  }
  
  ShaderEffect::~ShaderEffect()
  {
    if(shaderProgram)
      glDeleteProgram(shaderProgram);
  }
  
  void ShaderEffect::clearShaders(GLuint *nShaders, int count)
  {
    for(int i = 0; i < count; i++) if(nShaders[i])
    {
      glDetachShader(shaderProgram, nShaders[i]);
      glDeleteShader(nShaders[i]);
    }
    free(nShaders);
  }
  
  void ShaderEffect::setPreLinkStep(std::function<void ()> &pls)
  {
    preLinkStep = pls;
  }
  
#pragma mark - Shader Compilation
  bool ShaderEffect::compileShader(GLuint *shader, GLenum type, const string &src)
  {
    *shader = glCreateShader(type);
    bool ret = recompileShader(shader, type, src);
    
    return ret;
  }
  
  bool ShaderEffect::recompileShader(GLuint *shader, GLenum type, string src)
  {
    //prepend version string dynamically
#if !TARGET_OS_IPHONE
    src = (string)"#version 150\n" + src;
#endif
    
    GLint status;
    const GLchar *source = src.c_str();
    
    if(!source)
    {
      cerr << "Failed to load vertex shader" << endl;
      return false;
    }
    
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#ifdef DEBUG
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
      GLchar *log = (GLchar *)malloc(logLength);
      glGetShaderInfoLog(*shader, logLength, &logLength, log);
      cerr << "Shader compile log:\n" <<  log << endl;
      free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if(status == 0)
    {
      glDeleteShader(*shader);
      return false;
    }
    
    return true;
  }
  
  static string stringWithContentsOfFile(const string &file)
  {
    string ret;
    ifstream infile(file);
    
    if(infile.is_open())
    {
      string str((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
      ret = str;
      infile.close();
    }
    
    return ret;
  }
  
  bool ShaderEffect::compileShaderFromFile(GLuint *shader, GLenum type, const string &file)
  {
    GLint status;
    const GLchar *source;
    
    string str = stringWithContentsOfFile(file);
#if !TARGET_OS_IPHONE
    str = (string)"#version 150\n" + str;
#endif
    
    source = (GLchar *)str.c_str();
    if(!source)
    {
      cerr << "Failed to load vertex shader" << endl;
      return false;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
    
#ifdef DEBUG
    GLint logLength;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
      GLchar *log = (GLchar *)malloc(logLength);
      glGetShaderInfoLog(*shader, logLength, &logLength, log);
      cerr << "Shader compile log:\n" <<  log << endl;
      free(log);
    }
#endif
    
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if(status == 0)
    {
      glDeleteShader(*shader);
      return false;
    }
    
    return true;
  }
  
  bool ShaderEffect::linkProgram(GLuint prog)
  {
    GLint status;
    glLinkProgram(prog);
    
#ifdef DEBUG
    GLint logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if(logLength > 0)
    {
      GLchar *log = (GLchar *)malloc(logLength);
      glGetProgramInfoLog(prog, logLength, &logLength, log);
      cerr << "Program link log:  " << log << endl;
      free(log);
    }
#endif
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if(status == 0)
    {
      return false;
    }
    
    return true;
  }
  
  bool ShaderEffect::validateProgram(GLuint prog)
  {
    GLint logLength, status;
    
    glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
      GLchar *log = (GLchar *)malloc(logLength);
      glGetProgramInfoLog(prog, logLength, &logLength, log);
      cerr << "Program validate log:  " << log << endl;
      free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if(status == 0)
    {
      return false;
    }
    
    return true;
  }

  void ShaderEffect::loadShaders(const vector<string> &vtxFiles, const vector<string> &frgFiles, GLuint *program, GLuint **keep)
  {
    //GLES 2 can only do one of these at a time.. so
    GLuint *shaders = (GLuint *)malloc(sizeof(GLuint)*2);
    if(keep)
      *keep = shaders;
    
    int numVertexShaders = 1, numFragmentShaders = 1;
    
    if(*program)
      glDeleteProgram(*program);
    *program = glCreateProgram();
    
    // Create and compile vertex shader.
    for(int i = 0; i < numVertexShaders; i++)
    {
      if(!compileShader(shaders+i, GL_VERTEX_SHADER, vtxFiles[i]))
      {
        cerr << "Failed to compile vertex shader" << endl;
      }
      
      // Attach vertex shader to program.
      glAttachShader(*program, shaders[i]);
    }
    
    // Create and compile fragment shader.
    for(int i = 0; i < numFragmentShaders; i++)
    {
      if(!compileShader(*keep+numVertexShaders+i, GL_FRAGMENT_SHADER, frgFiles[i]))
      {
        cerr << "Failed to compile fragment shader" << endl;
      }
      
      // Attach fragment shader to program.
      glAttachShader(*program, shaders[numVertexShaders+i]);
    }
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(*program, VertexAttribPosition, "position");
    if(program == &shaderProgram && useFullVertexAttribs)
    {
      glBindAttribLocation(*program, VertexAttribNormal, "normal");
      glBindAttribLocation(*program, VertexAttribTexCoord0, "texcoord0");
    }
    if(preLinkStep)
      preLinkStep();
    
    // Link program.
    if(!linkProgram(*program))
    {
      cerr << "Failed to link program: " << *program << endl;
      
      clearShaders(shaders, 2);
      if(keep)
        *keep = NULL;
      if(*program)
      {
        glDeleteProgram(*program);
        *program = 0;
      }
    }
    
    if(!keep)
      clearShaders(shaders, 2);
  }

  void ShaderEffect::loadShader(const string &vtxFile, const string &frgFile, GLuint *program)
  {
    GLuint vertShader = 0, fragShader = 0;
    
    if(*program)
      glDeleteProgram(*program);
    *program = glCreateProgram();
    
    //!! vtxFile = [[NSBundle mainBundle] pathForResource:vtxFile ofType:nil];
    if(!compileShader(&vertShader, GL_VERTEX_SHADER, vtxFile))
    {
      cerr << "Failed to compile vertex shader" << endl;
    }
    
    // Create and compile fragment shader.
    //!! frgFile = [[NSBundle mainBundle] pathForResource:frgFile ofType:nil];
    if(!compileShader(&fragShader, GL_FRAGMENT_SHADER, frgFile))
    {
      cerr << "Failed to compile fragment shader" << endl;
    }
    
    // Attach vertex shader to program.
    glAttachShader(*program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(*program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(*program, VertexAttribPosition, "position");
    if(program == &shaderProgram && useFullVertexAttribs)
    {
      glBindAttribLocation(*program, VertexAttribNormal, "normal");
      glBindAttribLocation(*program, VertexAttribTexCoord0, "texcoord0");
    }
    if(preLinkStep)
      preLinkStep();
    
    // Link program.
    if(!linkProgram(*program))
    {
      cerr << "Failed to link program: " << *program << endl;
      
      if(vertShader)
      {
        glDeleteShader(vertShader);
        vertShader = 0;
      }
      if(fragShader)
      {
        glDeleteShader(fragShader);
        fragShader = 0;
      }
      if(*program)
      {
        glDeleteProgram(*program);
        *program = 0;
      }
    }
    
    // Release vertex and fragment shaders.
    if(vertShader)
    {
      glDetachShader(*program, vertShader);
      glDeleteShader(vertShader);
    }
    if(fragShader)
    {
      glDetachShader(*program, fragShader);
      glDeleteShader(fragShader);
    }
  }
  
  void ShaderEffect::loadShaderSrc(const string &vtxSrc, const string &frgSrc, GLuint *program)
  {
    GLuint vertShader = 0, fragShader = 0;
    
    if(*program)
      glDeleteProgram(*program);
    
    *program = glCreateProgram();
    
    if(!compileShader(&vertShader, GL_VERTEX_SHADER, vtxSrc))
    {
      cerr << "Failed to compile vertex shader" << endl;
    }
    
    // Create and compile fragment shader.
    if(!compileShader(&fragShader, GL_FRAGMENT_SHADER, frgSrc))
    {
      cerr << "Failed to compile fragment shader" << endl;
    }
    
    // Attach vertex shader to program.
    glAttachShader(*program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(*program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(*program, VertexAttribPosition, "position");
    if(program == &shaderProgram)
      glBindAttribLocation(*program, VertexAttribNormal, "normal");
    glBindAttribLocation(*program, VertexAttribTexCoord0, "texcoord0");
    
    // Link program.
    if(!linkProgram(*program))
    {
      cerr << "Failed to link program: " << *program << endl;
      
      if(vertShader)
      {
        glDeleteShader(vertShader);
        vertShader = 0;
      }
      if(fragShader)
      {
        glDeleteShader(fragShader);
        fragShader = 0;
      }
      if(*program)
      {
        glDeleteProgram(*program);
        *program = 0;
      }
    }
    
    // Release vertex and fragment shaders.
    if(vertShader)
    {
      glDetachShader(*program, vertShader);
      glDeleteShader(vertShader);
    }
    if(fragShader)
    {
      glDetachShader(*program, fragShader);
      glDeleteShader(fragShader);
    }
  }
  
  void ShaderEffect::loadGeometryShaderSrc(const string &geomSrc, const string &vtxSrc, const string &frgSrc, GLuint *program)
  {
    GLuint geomShader = 0, vertShader = 0, fragShader = 0;
    
    if(*program)
      glDeleteProgram(*program);
    
    *program = glCreateProgram();
    
    if(!compileShader(&geomShader, GL_GEOMETRY_SHADER, geomSrc))
    {
      cerr << "Failed to compile geometry shader" << endl;
    }
    
    if(!compileShader(&vertShader, GL_VERTEX_SHADER, vtxSrc))
    {
      cerr << "Failed to compile vertex shader" << endl;
    }
    
    // Create and compile fragment shader.
    if(!compileShader(&fragShader, GL_FRAGMENT_SHADER, frgSrc))
    {
      cerr << "Failed to compile fragment shader" << endl;
    }
    
    // Attach geometry shader to program
    glAttachShader(*program, geomShader);
    
    // Attach vertex shader to program.
    glAttachShader(*program, vertShader);
    
    // Attach fragment shader to program.
    glAttachShader(*program, fragShader);
    
    // Bind attribute locations.
    // This needs to be done prior to linking.
    glBindAttribLocation(*program, VertexAttribPosition, "position");
    if(program == &shaderProgram)
      glBindAttribLocation(*program, VertexAttribNormal, "normal");
    glBindAttribLocation(*program, VertexAttribTexCoord0, "texcoord0");
    
    // Link program.
    if(!linkProgram(*program))
    {
      cerr << "Failed to link program: " << *program << endl;
      
      if(geomShader)
      {
        glDeleteShader(geomShader);
        geomShader = 0;
      }
      if(vertShader)
      {
        glDeleteShader(vertShader);
        vertShader = 0;
      }
      if(fragShader)
      {
        glDeleteShader(fragShader);
        fragShader = 0;
      }
      if(*program)
      {
        glDeleteProgram(*program);
        *program = 0;
      }
    }
    
    // Release shaders.
    if(geomShader)
    {
      glDetachShader(*program, geomShader);
      glDeleteShader(geomShader);
    }
    if(vertShader)
    {
      glDetachShader(*program, vertShader);
      glDeleteShader(vertShader);
    }
    if(fragShader)
    {
      glDetachShader(*program, fragShader);
      glDeleteShader(fragShader);
    }
  }
 
#pragma mark - Uniforms and Shader State
  
  GLint ShaderEffect::getUniformLocation(const string &str)
  {
    auto iter = uniformLookupTable.find(str);
    int num = uniformLookupTable[str];
    if(iter != uniformLookupTable.end())
    {
      GLint loc = 0;
      
      loc = glGetUniformLocation(shaderProgram, str.c_str());
      if(loc >= 0)
        num = loc;
      else
        cerr << "Warning:  uniform " << str << " not found" << endl;
      
      if(num)
        uniformLookupTable[str] = num;
    }
    
    return (iter != uniformLookupTable.end()) ? num : -1;
  }
  
  template <typename T>
  void ShaderEffect::setShaderUniformVal(const string &uniformName, const T &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
    {
      if(is_same<T, float>())
      {
        glUniform1f(loc, val);
      }
      else if(is_same<T, float2>())
      {
        glUniform2f(loc, val.x, val.y);
      }
      else if(is_same<T, float3>())
      {
        glUniform3f(loc, val.x, val.y, val.z);
      }
      else if(is_same<T, float4>())
      {
        glUniform4fv(loc, val.x, val.y, val.z, val.w);
      }
      else if(is_same<T, int>() || is_same<T, unsigned int>() || is_same<T, bool>())
      {
        glUniform1i(loc, val);
      }
    }
  }
  
  template <typename T>
  void ShaderEffect::setShaderUniformVal(const string &uniformName, const T &val)
  {
    setShaderUniformVal<T>(uniformName, val, shaderProgram);
  }

  
  void ShaderEffect::setShaderUniform(const string &uniformName, const float4 &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform4f(loc, val.x, val.y, val.z, val.w);
  }
  
  void ShaderEffect::setShaderUniform(const string &uniformName, const float3 &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform3f(loc, val.x, val.y, val.z);
  }

  void ShaderEffect::setShaderUniform(const string &uniformName, const float2 &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform2f(loc, val.x, val.y);
  }

  void ShaderEffect::setShaderUniform(const string &uniformName, const float &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform1f(loc, val);
  }
  
  void ShaderEffect::setShaderUniform(const string &uniformName, const int4 &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform4i(loc, val.x, val.y, val.z, val.w);
  }
  
  void ShaderEffect::setShaderUniform(const string &uniformName, const int3 &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform3i(loc, val.x, val.y, val.z);
  }
  
  void ShaderEffect::setShaderUniform(const string &uniformName, const int2 &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform2i(loc, val.x, val.y);
  }
  
  void ShaderEffect::setShaderUniform(const string &uniformName, const int &val, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniform1i(loc, val);
  }
  
  void ShaderEffect::setShaderUniformMat(const string &uniformName, const float4 &mat2, GLuint program)
  {
    //sanity check
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniformMatrix2fv(loc, 1, 0, (float *)&mat2);
  }
  
  void ShaderEffect::setShaderUniformMat(const string &uniformName, const mat3 &val, GLuint program)
  {
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniformMatrix3fv(loc, 1, 0, val.m);
  }
  
  void ShaderEffect::setShaderUniformMat(const string &uniformName, const mat4 &val, GLuint program)
  {
    if(uniformName.empty())
      return;
    
    GLint loc = getUniformLocation(uniformName);
    if(loc >= 0)
      glUniformMatrix4fv(loc, 1, 0, val.m);
  }
  
  static bool prefix(const string &test, const string &prefix)
  {
    auto res = std::mismatch(prefix.begin(), prefix.end(), test.begin());

    if(res.first == prefix.end())
    {
      return true;
    }
    
    return false;
  }
  
  bool ShaderEffect::isUniformNameBuiltin(const string &name)
  {
    string builtins[] =
    {
      "modelViewProjectionMatrix", "modelViewMatrix", "normalMatrix", "lightModelProductSceneColor",
      "textureEnabled", "texture0", "time", "cameraPosition", "modelMatrix", "modelMatrixInverse",
      "textureScale", "renderTargetSize", "renderPassIndex"
    };
    auto builtinsSet = set<string>(builtins, builtins+sizeof(builtins)/sizeof(string));
    bool ret = false;
    
    if(prefix(name, "gl_") || prefix(name, "_"))
      ret = true;

    if(prefix(name, "material.") || prefix(name, "lights["))
      ret = true;
    
    if(builtinsSet.find(name) != builtinsSet.end())
      ret = true;
    
    return ret;
  }
  
  map<string, shared_ptr<ShaderUniformInfo> > ShaderEffect::getUniformInfo()
  {
    GLint numUniforms = 0, maxUniformNameLen = 0, size = 0, count = 0;
    GLsizei len = 0;
    map<string, shared_ptr<ShaderUniformInfo> > info;
    GLenum type;
    GLenum iformat = GL_INT;
    enum TYPE { ENTRY, CHECK, NONE } uiType = ENTRY;
    GLchar *buf = NULL;
    GLboolean isMatrix = GL_FALSE;
    
    //find out how many uniforms there are, and query them all
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &numUniforms);
    glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLen);
    buf = (GLchar *)malloc(maxUniformNameLen);
    
    for(int i = 0; i < numUniforms; i++)
    {
      glGetActiveUniform(shaderProgram, i, maxUniformNameLen, &len, &size, &type, buf);
      
      //don't care about builtins here
      if(isUniformNameBuiltin(buf))
        continue;
      
      iformat = GL_INT;
      switch(type)
      {
        case GL_FLOAT:
          iformat = GL_FLOAT;
        case GL_INT:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_2D_SHADOW:
          uiType = ENTRY;
          count = 1;
        break;
        case GL_FLOAT_VEC2:
          iformat = GL_FLOAT;
        case GL_INT_VEC2:
          uiType = ENTRY;
          count = 2;
        break;
        case GL_FLOAT_VEC3:
          iformat = GL_FLOAT;
        case GL_INT_VEC3:
          uiType = ENTRY;
          count = 3;
        break;
        case GL_FLOAT_VEC4:
          iformat = GL_FLOAT;
        case GL_INT_VEC4:
          uiType = ENTRY;
          count = 4;
        break;
        case GL_BOOL:
          uiType = CHECK;
          count = 1;
        break;
        case GL_BOOL_VEC2:
          uiType = CHECK;
          count = 2;
        break;
        case GL_BOOL_VEC3:
          uiType = CHECK;
          count = 3;
        break;
        case GL_BOOL_VEC4:
          uiType = CHECK;
          count = 4;
        break;
          
        case GL_FLOAT_MAT2:
          count = 4;
          uiType = NONE;
          iformat = GL_FLOAT;
          isMatrix = GL_TRUE;
        break;
        case GL_FLOAT_MAT3:
          count = 9;
          uiType = NONE;
          iformat = GL_FLOAT;
          isMatrix = GL_TRUE;
        break;
        case GL_FLOAT_MAT4:
          count = 16;
          uiType = NONE;
          iformat = GL_FLOAT;
          isMatrix = GL_TRUE;
        break;
        default:
          uiType = NONE;
          count = 1;
        break;
      }
      
      
      shared_ptr<ShaderUniformInfo> uniformInfo = shared_ptr<ShaderUniformInfo>(new ShaderUniformInfo);
      uniformInfo->set(type, iformat, count, isMatrix);
      uniformInfo->setName(buf);
      
      info[uniformInfo->getName()] = uniformInfo;
    }
    
    free(buf);
    
    return info;
  }

  void ShaderEffect::prepareToDraw()
  {
    prepareToDrawFromState(nullptr);
  }
  
  void ShaderEffect::prepareToDrawFromState(StateMachine *machine)
  {
  }
  
  void ShaderEffect::setAllDirty()
  {
    dirtyStateFlags = DS_ALL;
  }
  
  void ShaderEffect::setTransformDirty()
  {
    dirtyStateFlags |= DS_TRANSFORM;
  }
  
  void ShaderEffect::setTextureTransformDirty()
  {
    dirtyStateFlags |= DS_TRANSFORM_TEXTURE;
  }
  
  void ShaderEffect::setMaterialAmbientDirty()
  {
    dirtyStateFlags |= DS_MAT_AMBIENT;
  }
  
  void ShaderEffect::setMaterialDiffuseDirty()
  {
    dirtyStateFlags |= DS_MAT_DIFFUSE;
  }
  
  void ShaderEffect::setMaterialSpecularDirty()
  {
    dirtyStateFlags |= DS_MAT_SPECULAR;
  }
  
  void ShaderEffect::setMaterialEmissiveDirty()
  {
    dirtyStateFlags |= DS_MAT_EMISSIVE;
  }
  
  void ShaderEffect::setMaterialShininessDirty()
  {
    dirtyStateFlags |= DS_MAT_SHINY;
  }
  
  void ShaderEffect::setLightDirty(unsigned int light)
  {
    dirtyStateFlags |= (DS_LIGHT0<<light);
  }
  
  void ShaderEffect::setTextureDirty(unsigned int texture)
  {
    dirtyStateFlags |= (DS_TEXTURE0<<texture);
  }
  
  void ShaderEffect::setConstantColorDirty()
  {
    dirtyStateFlags |= DS_CONSTANT_COLOR;
  }
  
  void ShaderEffect::setRenderTargetDirty()
  {
    dirtyStateFlags |= DS_RENDER_TARGET;
  }
  
  void ShaderEffect::setFogDirty()
  {
    dirtyStateFlags |= DS_FOG;
  }
  
  void ShaderEffect::setTimeDirty()
  {
    dirtyStateFlags |= DS_TIME;
  }
  
  void ShaderEffect::setMiscDirty()
  {
    dirtyStateFlags |= DS_MISC_FLAGS;
  }

  
  
}