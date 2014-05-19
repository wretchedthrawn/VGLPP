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

#include <fstream>
#include <sstream>
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
  
  
  

  
}