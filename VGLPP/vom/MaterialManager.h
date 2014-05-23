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

#ifndef __VGLPP_Demo__MaterialManager__
#define __VGLPP_Demo__MaterialManager__

#include <string>
#include <map>

namespace vom
{
  class Material;
  class TextureManager;
  
  class MaterialManager
  {
  public:
    MaterialManager();
    ~MaterialManager();
    
    ///Manages a new Material
    void setMaterial(const std::string &matName, std::shared_ptr<Material> mat);
    
    ///Retrieves a previously managed Material by its name
    std::shared_ptr<Material> getMaterial(const std::string &matName);

    ///Removes a previously managed Material by its name
    void removeMaterial(const std::string &matName);
    
    ///Parses materials from OBJ file's MTL-file counterpart
    void readMtl(const std::string &mtlFilename, std::shared_ptr<TextureManager> textureManager);

  private:
    std::map<std::string, std::shared_ptr<Material>> materials;
  };
}

#endif /* defined(__VGLPP_Demo__MaterialManager__) */
