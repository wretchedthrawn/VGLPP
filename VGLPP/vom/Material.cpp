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

#include "System.h"
#include "StateMachine.h"
#include "Material.h"
#include "Texture2D.h"

using namespace std;

namespace vom
{
  Material::Material()
  {
    amb = make_float4(0.2f, 0.2f, 0.2f, 1.0f);
    diff = make_float4(0.8f, 0.8f, 0.8f, 1.0f);
    spec = make_float4(0, 0, 0, 1);
    emit = make_float4(0, 0, 0, 1);
    shininess = 64;
    tileX = tileY = 1.0f;
  }
  
  Material::Material(std::shared_ptr<Texture2D> texture, float4 diff, float4 amb, float4 spec, float4 emit)
  : diff(diff), amb(amb), spec(spec), emit(emit), shininess(64), tileX(1.0f), tileY(1.0f), texture(texture)
  {
    
  }
  
  Material::~Material()
  {
    
  }
  
  void Material::apply()
  {
    if(texture)
      glBindTexture(GL_TEXTURE_2D, texture->getName());

    shared_ptr<vgl::StateMachine> vgl = vgl::System::system().currentStateMachine();
    vgl->setDiffuseMaterialColor(diff);
    vgl->setAmbientMaterialColor(amb);
    vgl->setSpecularMaterialColor(spec);
    vgl->setEmissiveMaterialColor(emit);
    vgl->setShininessMaterialValue(shininess);
    
    vgl->textureMatrixLoadIdentity();
    if(tileX > 1 || tileY > 1)
    {
      vgl->textureScale(make_float2(tileX, tileY));
    }
  }
  
  void Material::setWrap(GLenum w)
  {
    wrap = w;
    if(texture)
    {
      //replace legacy GL_CLAMP
      glBindTexture(GL_TEXTURE_2D, texture->getName());
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, w);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, w);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
  
  bool Material::requiresBlending()
  {
    return (diff.w < 1.0f);
  }
}

