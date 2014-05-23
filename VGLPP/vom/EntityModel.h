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

#ifndef __VGLPP_Demo__EntityModel__
#define __VGLPP_Demo__EntityModel__

#include <list>
#include <memory>
#include "vgl.h"
#include "Entity.h"
#include "VecTypes.h"

namespace vom
{
  class Mesh;
  class MaterialManager;
  class TextureManager;
  
  class EntityModel : public Entity
  {
  public:
    EntityModel();
    
    ///Loads an entity from a file
    EntityModel(const std::string &filename, float scale, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<TextureManager> textureManager);
    
    virtual ~EntityModel();
    
    virtual void render();
    
  private:
    void loadOBJ(const std::string &filename, float scale, std::shared_ptr<MaterialManager> materialManager, std::shared_ptr<TextureManager> textureManager);
    void finishSubmesh(std::shared_ptr<Mesh> submesh, uint3 *h_ind, uint4 *h_ind_quad, int istart, int istart_quad, int num_inds);
    
    void setupVAO();
    
    GLuint vao;
    GLuint tex_vbo, vertex_vbo, normal_vbo, tangent_vbo, bone_vbo;
    int num_verts;
    std::list<std::shared_ptr<Mesh>> meshes;
    
    float3 bbsize;
  };
}

#endif /* defined(__VGLPP_Demo__EntityModel__) */
