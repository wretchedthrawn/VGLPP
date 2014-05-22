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

#ifndef __VGLPP_Demo__Texture2D__
#define __VGLPP_Demo__Texture2D__

#include "vgl.h"
#include <string>
#include <memory>
#include "VecTypes.h"

namespace vom
{
  class TextureLoader;
  
  class Texture2D
  {
  public:
    Texture2D(const std::string &filename);
    virtual ~Texture2D();

    ///Sets the texture loader class to be used for texture loading operations
    static void setTextureLoader(std::shared_ptr<TextureLoader> textureLoader);
    
    ///Returns the size of the image that was used to create this texture
    inline uint2 getImageSize() { return imageSize; }
    
    ///Returns the size of the texture
    inline uint2 getSize() { return size; }
    
    inline void setSize(uint2 sz) { size = sz; }
    inline void setImageSize(uint2 sz) { imageSize = sz; }
    
    inline GLuint getName() { return name; }
    
    inline const std::string &getFilename() { return filename; }
    
  private:
    std::string filename;
    uint2 imageSize;
    uint2 size;
    
    GLuint name = 0;
    
    static std::shared_ptr<TextureLoader> textureLoader;
  };
  
}

#endif /* defined(__VGLPP_Demo__Texture2D__) */
