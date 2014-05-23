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

#ifndef __VGLPP_Demo__Mesh__
#define __VGLPP_Demo__Mesh__

#include <memory>
#include <string>
#include "vgl.h"

namespace vom
{
  class Material;
  
  /** 
   One or many Meshes live inside EntityModels.  They contain the triangular or quadrilateral indices
   into the vertex arrays of a Model to define a polygonal mesh object.  They are externally referred
   to as "Material Groups"
   */
   
  class Mesh
  {
    friend class EntityModel;
    
  public:
    Mesh();
    ~Mesh();
    
    inline int getNumInds() { return num_inds; }
    inline int getNumQuadInds() { return num_quad_inds; }
    
    inline GLuint getIndVbo() { return ind_vbo; }
    inline GLuint getQuadIndVbo() { return ind_vbo_quad; }
    
    inline void setIndVbo(GLuint vbo) { ind_vbo = vbo; }
    inline void setQuadIndVbo(GLuint vbo) { ind_vbo_quad = vbo; }
    
    inline std::shared_ptr<Material> getMaterial() { return material; }
    void setMaterial(std::shared_ptr<Material> mat);
    
    inline const std::string &getName() { return name; }
    inline void setName(const std::string &nm) { name = nm; }

  private:
    int num_inds = 0, num_quad_inds = 0;
    
    GLuint ind_vbo = 0, ind_vbo_quad = 0;
    
    std::shared_ptr<Material> material;
    
    std::string name;
  };
}

#endif /* defined(__VGLPP_Demo__Mesh__) */
