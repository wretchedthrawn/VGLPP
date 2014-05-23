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

#ifndef __VGLPP_Demo__Material__
#define __VGLPP_Demo__Material__

#include <memory>
#include <string>
#include "vgl.h"
#include "VecTypes.h"

namespace vom
{
  class Texture2D;
  
  class Material
  {
  public:
    Material();
    Material(std::shared_ptr<Texture2D> texture, float4 diff, float4 amb, float4 spec, float4 emit);
    ~Material();
    
    void apply();
    bool requiresBlending();
    
    inline void setName(const std::string &aName) { name = aName; }
    inline const std::string &getName() { return name; }
    
    inline void setDiffuse(float4 diffuse) { diff = diffuse; }
    inline float4 getDiffuse() { return diff; }

    inline void setAmbient(float4 ambient) { amb = ambient; }
    inline float4 getAmbient() { return amb; }

    inline void setSpecular(float4 specular) { spec = specular; }
    inline float4 getSpecular() { return spec; }

    inline void setEmissive(float4 emissive) { emit = emissive; }
    inline float4 getEmissive() { return emit; }
    
    inline void setShininess(float shin) { shininess = shin; }
    inline float getShininess() { return shininess; }
    
    inline void setTexture(std::shared_ptr<Texture2D> tex) { texture = tex; };
    inline std::shared_ptr<Texture2D> getTexture() { return texture; };
    
    inline void setTextureTile(float tx, float ty) { tileX = tx; tileY = ty; }
    inline float2 getTextureTile() { return make_float2(tileX, tileY); }
    
    void setWrap(GLenum w);
    inline GLenum getWrap() { return wrap; }

  private:
    std::string name;
    float4 diff, amb, spec, emit;
    float shininess;
    
    float tileX, tileY;
    GLenum wrap;
    std::shared_ptr<Texture2D> texture;
  };
  
}
#endif /* defined(__VGLPP_Demo__Material__) */
