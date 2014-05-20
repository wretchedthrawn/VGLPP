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
#include <fstream>
#include <sstream>
#include <math.h>
#include "GouraudShadingShaderEffect.h"
#include "FastLightingShaderSource.h"

#if TARGET_OS_IPHONE
#define MobileTarget()   1
#else
#define MobileTarget()   0
#endif

using namespace std;

namespace vgl
{
  static inline float RAD(float n)  { return (n * (M_PI/180.0f)); }

  static string stringWithContentsOfFile(const string &file)
  {
    string ret;
    ifstream infile(file);
    
    if(infile.is_open())
    {
      string str((istreambuf_iterator<char>(infile)), istreambuf_iterator<char>());
      ret = str;
      infile.close();
    }
    
    return ret;
  }

  static string shaderSrc(const string &fileName)
  {
    return stringWithContentsOfFile("vgl/shaders/"+fileName);
  }

  GouraudShadingShaderEffect::GouraudShadingShaderEffect()
  {
    string vertex[] = { genVertexShaderForTextureMode(false) };
    string fragment[] = { genFragmentShaderForTextureMode(false) };
    string vertexTex[] = { genVertexShaderForTextureMode(true) };
    string fragmentTex[] = { genFragmentShaderForTextureMode(true) };

    auto vertexVec = vector<string>(vertex, vertex+1);
    auto fragmentVec = vector<string>(fragment, fragment+1);
    
    //compile non textured shader
    loadShaders(vertexVec, fragmentVec, &shaderProgram, &shaders);
    
    //compile textured shader
    vertexVec = vector<string>(vertexTex, vertexTex+1);
    fragmentVec = vector<string>(fragmentTex, fragmentTex+1);
    auto strongSelf = shared_ptr<GouraudShadingShaderEffect>(this);
    preLinkStep = function<void()>([=]{
      glBindAttribLocation(strongSelf->shaderProgramTex, VertexAttribNormal, "normal");
      glBindAttribLocation(strongSelf->shaderProgramTex, VertexAttribTexCoord0, "texcoord0");
    });
    loadShaders(vertexVec, fragmentVec, &shaderProgramTex, &shadersTex);
    preLinkStep = 0;
    
    pointSize = 1.0f;
    
    //Uniform initialization
    glUseProgram(shaderProgramTex);
    GLuint tmp = shaderProgram;
    shaderProgram = shaderProgramTex;
    setShaderUniform("texture0", 0, shaderProgramTex);
    shaderProgram = tmp;
    uniformLookupTable.clear();
  }
  
  GouraudShadingShaderEffect::~GouraudShadingShaderEffect()
  {
    if(shaders)
      clearShaders(shaders, 2);
    if(shadersTex)
      clearShaders(shadersTex, 2);
  }
  
  string GouraudShadingShaderEffect::genFragmentShaderForTextureMode(bool tex)
  {
    return (!tex) ? shaderSrc("gouraud.fs") : shaderSrc("gouraud_textured.fs");
  }
  
  static int indexOfLine(int line, const string &str)
  {
    int index = 0, currentLine = 0;
    char c = 0;
    bool found = false;
    
    while((c=str[index++]))
    {
      if(c=='\n')
      {
        currentLine++;
        if(currentLine == line)
        {
          found = true;
          break;
        }
      }
    }
    
    if(!found)
      index = -1;
    
    return index;
  }
  
  static void replace(string &str, const string &oldStr, const string &newStr)
  {
    size_t pos = 0;
    while((pos = str.find(oldStr, pos)) != string::npos)
    {
      str.replace(pos, oldStr.length(), newStr);
      pos += newStr.length();
    }
  }
  
  string GouraudShadingShaderEffect::genVertexShaderForTextureMode(bool tex)
  {
    string base = shaderSrc("gouraud.vs");
    string lighting = shaderSrc("gouraud_light.vs");
    string lightingCalls, lightingMethods;
    
    int prependIndex = 0;
    int lightingMethodsIndex = indexOfLine(75, base);
    int dynamicCallsIndex = indexOfLine(36, lighting);
    
    if(!MobileTarget())
    {
      prependIndex = indexOfLine(2, base);
    }
    
    int i = 0;
    builtLightFlags = 0;
    bool atten = false;
    for(auto light : state->getLights())
    {
      float4 pos = light->getPosition();

      if(light->isEnabled())
      {
        //best to make these decisions ahead of time..
        if(light->getSpotCutoff() == 180.0f)
        {
          if(pos.w != 0.0f)
          {
            ostringstream ostr;
            ostr << "[" << i << "]";
            
            //instantiate the method
            string lightingMethod = pointLight;
            replace(lightingMethod, "[0]", ostr.str());
            
            ostr.str("");
            ostr << i << "(";
            replace(lightingMethod, "0(", ostr.str());

            lightingMethods += lightingMethod;
            
            //insert the call
            ostr.str("");
            ostr << "  pointLight" << i << "(normal, eye, ecPosition3);\n";
            lightingCalls += ostr.str();
          }
          else
          {
            ostringstream ostr;
            ostr << "[" << i << "]";

            //instantiate the method
            string lightingMethod = directionalLight;

            replace(lightingMethod, "[0]", ostr.str());
            ostr.str("");
            ostr << i << "(";
            replace(lightingMethod, "0(", ostr.str());
            
            lightingMethods += lightingMethod;
            
            //insert the call
            ostr.str("");
            ostr << "  directionalLight" << i << "(normal, eye);\n";
            lightingCalls += ostr.str();
          }
        }
        else
        {
          //so seldom used, not worth it to inline
          ostringstream ostr;
          
          if(pos.w != 0.0f)
          {
            ostr << "  spotLight(" << i << ", normal, eye, ecPosition3);\n";
            lightingCalls += ostr.str();
          }
          else
          {
            ostr << "  infiniteSpotLight(" << i << ", normal, eye);\n";
            lightingCalls += ostr.str();
          }
        }
        builtLightFlags |= (DS_LIGHT0<<i);
        
        float3 attenv = light->getAttenuation();
        if(attenv.x != 1.0 || attenv.y != 0.0 || attenv.z != 0.0)
          atten = true;
      }
      i++;
    }
    
    base.insert(lightingMethodsIndex, lightingMethods);
    if(tex)
      base.insert(prependIndex, "#define TEXTURE 1\n");
    if(atten)
      base.insert(prependIndex, "#define ATTENUATION 1\n");

    lighting.insert(dynamicCallsIndex, lightingCalls);

    string merged = base+lighting;
    
    return merged;
  }
  
  
  bool GouraudShadingShaderEffect::updateVertexShaderForLightingAndTextureMode(bool tex)
  {
#ifdef DEBUG
    cout << "Recompiling gouraud lighting shader due to changed lighting state" << endl;
#endif
    
    string vertexShader = genFragmentShaderForTextureMode(tex);
    GLuint *shader = (tex) ? &shadersTex[0] : &shaders[0];
    GLuint program = (tex) ? shaderProgramTex : shaderProgram;
    auto &lookup = (tex) ? uniformLookupTableTex : uniformLookupTable;
    
    if(recompileShader(shader, GL_VERTEX_SHADER, vertexShader))
    {
      // Bind attribute locations.
      // This needs to be done prior to linking.
      glBindAttribLocation(program, VertexAttribPosition, "position");
      if(useFullVertexAttribs)
      {
        glBindAttribLocation(program, VertexAttribNormal, "normal");
        glBindAttribLocation(program, VertexAttribTexCoord0, "texcoord0");
      }
    }
    
    //re-link program
    if(!linkProgram(program))
    {
      cerr << "Failed to link program: " << program << endl;
    }
    
    lookup.clear();
    dirtyStateFlags = DS_ALL;
    return true;
  }
  
  bool GouraudShadingShaderEffect::updateVertexShaderForLighting()
  {
    updateVertexShaderForLightingAndTextureMode(false);
    updateVertexShaderForLightingAndTextureMode(true);
    
    return true;
  }

  void GouraudShadingShaderEffect::prepareToDrawFromState(StateMachine *machine)
  {
    ShaderEffect::prepareToDrawFromState(machine);
    
    if(lightingRecompilationDirtyFlags)
    {
      if(updateVertexShaderForLighting())
        lightingRecompilationDirtyFlags = 0;
    }
    
    GLint loc = 0;
    GLuint shaderProgTmp = shaderProgram;
    auto &uniformLookupTableTmp = uniformLookupTable;
    
    if(state->getTexture0()->isEnabled())
    {
      shaderProgram = shaderProgramTex;
      uniformLookupTable = uniformLookupTableTex;
    }
    
    if(machine)
      machine->useShaderProgram(shaderProgram);
    else
      glUseProgram(shaderProgram);
    
    if(dirtyStateFlags & DS_MISC_FLAGS)
    {
    }
    
    //Set transform
    if(dirtyStateFlags & DS_TRANSFORM)
    {
      mat3 normalMatrix = mat3InvertAndTranspose(mat4GetMatrix3(state->getTransform()->getModelviewMatrix()), NULL);
      mat4 modelViewProjectionMatrix = mat4Multiply(state->getTransform()->getProjectionMatrix(), state->getTransform()->getModelviewMatrix());
      
      loc = getUniformLocation("modelViewProjectionMatrix");
      if(loc >= 0)
        glUniformMatrix4fv(loc, 1, 0, modelViewProjectionMatrix.m);
      
      loc = getUniformLocation("modelViewMatrix");
      if(loc >= 0)
        glUniformMatrix4fv(loc, 1, 0, state->getTransform()->getModelviewMatrix().m);
      
      loc = getUniformLocation("normalMatrix");
      if(loc >= 0)
        glUniformMatrix3fv(loc, 1, 0, normalMatrix.m);
    }
    if(dirtyStateFlags & DS_TRANSFORM_TEXTURE && state->getTexture0()->isEnabled())
    {
      mat4 tm = state->getTextureTransformMatrix();
      float2 scale = make_float2(tm.m[0], tm.m[5]);
      
      setShaderUniform("textureScale", scale, shaderProgram);
    }
    
    //materials are pre-multiplied with light state.
    if(dirtyStateFlags & DS_MAT_DIFFUSE)
    {
      //[self setShaderUniform:@"material.diffuse" toVector4:material.diffuseColor forProgram:shaderProgram];
      setShaderUniform("material.alpha", state->getMaterial()->getDiffuse().a, shaderProgram);
    }
    if(dirtyStateFlags & DS_MAT_SHINY)
    {
      GLfloat shin = state->getMaterial()->getShininess();
      
      //hack to get rid of ugly artifacts in fragment shader since removing specular if-branch for performance
      if(shin == 0.0f)
        shin = 0.1f;
      setShaderUniform("material.shininess", shin, shaderProgram);
    }
    
    //Set light
    int i = 0;
    for(auto light : state->getLights())
    {
      ostringstream ostr;
      ostr << "lights[" << i << "].";
      string prefix = ostr.str();

      if(builtLightFlags & (DS_LIGHT0<<i))
      {
        if(dirtyStateFlags & (DS_LIGHT0<<i))
        {
          float3 attenv = light->getAttenuation();
          
          setShaderUniform(prefix+"position", light->getPosition(), shaderProgram);
          setShaderUniform(prefix+"spotDirection", light->getSpotDirection(), shaderProgram);
          setShaderUniform(prefix+"spotExponent", light->getSpotExponent(), shaderProgram);
          setShaderUniform(prefix+"spotCosCutoff", cosf(RAD(light->getSpotCutoff())), shaderProgram);

          setShaderUniform(prefix+"constantAttenuation", attenv.x, shaderProgram);
          setShaderUniform(prefix+"linearAttenuation", attenv.y, shaderProgram);
          setShaderUniform(prefix+"quadraticAttenuation", attenv.z, shaderProgram);
        }
        
        if(dirtyStateFlags & (DS_LIGHT0<<i) || (dirtyStateFlags & DS_MAT_AMBIENT))
        {
          setShaderUniform(prefix+"ambient", float4Multiply(light->getAmbient(), state->getMaterial()->getAmbient()), shaderProgram);
        }
        
        if(dirtyStateFlags & (DS_LIGHT0<<i) || (dirtyStateFlags & DS_MAT_DIFFUSE))
        {
          setShaderUniform(prefix+"diffuse", float4Multiply(light->getDiffuse(), state->getMaterial()->getDiffuse()), shaderProgram);
        }
        
        if(dirtyStateFlags & (DS_LIGHT0<<i) || (dirtyStateFlags & DS_MAT_SPECULAR))
        {
          float4 mult = float4Multiply(light->getSpecular(), state->getMaterial()->getSpecular());
          setShaderUniform(prefix+"specular", mult, shaderProgram);
        }
      }
      
      
      i++;
    }
    
    if(dirtyStateFlags & DS_MAT_EMISSIVE || dirtyStateFlags & DS_MAT_AMBIENT)
    {
      float4 ambientProduct = float4Multiply(state->getLightModelAmbientColor(), state->getMaterial()->getAmbient());
      float4 lightModelProductSceneColor = float4Add(state->getMaterial()->getEmissive(), ambientProduct);
      setShaderUniform("lightModelProductSceneColor", lightModelProductSceneColor, shaderProgram);
    }
    
    uniformLookupTable = uniformLookupTableTmp;
    shaderProgram = shaderProgTmp;
    dirtyStateFlags = 0;
  }

  void GouraudShadingShaderEffect::setLightingVeryDirty()
  {
    lightingRecompilationDirtyFlags = 0xffffffff;
  }
  
  void GouraudShadingShaderEffect::setPointSize(float p)
  {
    pointSize = p;
    dirtyStateFlags |= DS_MISC_FLAGS;
  }
  
  //more of a big deal for this shader
  void GouraudShadingShaderEffect::setTextureDirty(unsigned int texture)
  {
    if(texture == 0)
    {
      dirtyStateFlags = DS_ALL;
    }
  }
  
  
}