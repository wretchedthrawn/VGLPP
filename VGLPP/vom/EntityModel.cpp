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
#include <map>
#include <stdexcept>
#include "System.h"
#include "StateMachine.h"
#include "StringPathUtils.h"
#include "VecTypes.h"
#include "Mesh.h"
#include "EntityModel.h"
#include "Material.h"
#include "MaterialManager.h"
#include "TextureManager.h"

using namespace std;

namespace vom
{
  EntityModel::EntityModel()
  {
    num_verts = 0;
    vertex_vbo = normal_vbo = tex_vbo = tangent_vbo = vao = 0;
    
    glGenVertexArrays(1, &vao);
  }
  
  EntityModel::EntityModel(const string &filename, float scale, shared_ptr<MaterialManager> materialManager, shared_ptr<TextureManager> textureManager)
  {
    num_verts = 0;
    vertex_vbo = normal_vbo = tex_vbo = tangent_vbo = vao = 0;
    glGenVertexArrays(1, &vao);

    loadOBJ(filename, scale, materialManager, textureManager);
  }
  
  EntityModel::~EntityModel()
  {
    if(vao)
      glDeleteVertexArrays(1, &vao);
    if(vertex_vbo)
      glDeleteBuffers(1, &vertex_vbo);
    if(normal_vbo)
      glDeleteBuffers(1, &normal_vbo);
    if(tex_vbo)
      glDeleteBuffers(1, &tex_vbo);
    if(tangent_vbo)
      glDeleteBuffers(1, &tangent_vbo);
    if(bone_vbo)
      glDeleteBuffers(1, &bone_vbo);
  }
  
  void EntityModel::render()
  {
    bool textureState = true;
    shared_ptr<vgl::StateMachine> vgl = vgl::System::system().currentStateMachine();

    glBindVertexArray(vao);
    vgl->enableTexcoordArray(true);
    vgl->enableTexture0(true);
    
    for(int submeshIndex = 0; submeshIndex < meshes.size(); submeshIndex++)
    {
      auto s = meshes[submeshIndex];
      bool usesTextureCoordinates = (bool)s->getMaterial()->getTexture();
      
      if(!s->getMaterial()->getTexture() && textureState)
      {
        vgl->enableTexture0(false);
        vgl->enableTexcoordArray(false);
        textureState = false;
      }
      else if(s->getMaterial()->getTexture() && !textureState)
      {
        vgl->enableTexture0(true);
        vgl->enableTexcoordArray(true);
        textureState = true;
      }
      
      if(usesTextureCoordinates && !textureState)
      {
        vgl->enableTexcoordArray(true);
      }
      
      if(s->getMaterial()->requiresBlending())
      {
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
      }
      
      //apply material
      s->getMaterial()->apply();
      
      //and draw the thing
      vgl->prepareToDraw();
      
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->ind_vbo);
      glDrawElements(GL_TRIANGLES, s->num_inds, GL_UNSIGNED_INT, 0);
      
      if(s->num_quad_inds)
      {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->ind_vbo_quad);
        for(int q = 0; q < s->num_quad_inds; q+=4)
        {
          //emulate the quad as two triangles in a fan
          glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, (void *)(q*sizeof(uint)));
        }
      }
      
      //restore default material
      const float4 colvec = make_float4(0.8, 0.8, 0.8, 1.0f), amb = make_float4(0.2, 0.2, 0.2, 1.0),
      zero = make_float4(0, 0, 0, 0);
      
      vgl->setDiffuseMaterialColor(colvec);
      vgl->setAmbientMaterialColor(amb);
      vgl->setSpecularMaterialColor(zero);
      vgl->setEmissiveMaterialColor(zero);
      vgl->setShininessMaterialValue(0);
    
      if(s->getMaterial()->requiresBlending())
      {
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
      }
    }
  
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    vgl->bindDefaultVAO();
  }
  
  void EntityModel::finishSubmesh(shared_ptr<Mesh> submesh, uint3 *h_ind, uint4 *h_ind_quad, int istart, int istart_quad, int num_inds)
  {
    GLuint ind_vbo = 0;
    glGenBuffers(1, &ind_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, ind_vbo);
    glBufferData(GL_ARRAY_BUFFER, submesh->getNumInds()*sizeof(uint), h_ind+istart, GL_STATIC_DRAW);
    submesh->setIndVbo(ind_vbo);
    
    if(h_ind_quad)
    {
      glGenBuffers(1, &ind_vbo);
      glBindBuffer(GL_ARRAY_BUFFER, ind_vbo);
      glBufferData(GL_ARRAY_BUFFER, submesh->getNumQuadInds()*sizeof(uint), h_ind_quad+istart_quad, GL_STATIC_DRAW);
      submesh->setQuadIndVbo(ind_vbo);
    }
    
    meshes.push_back(submesh);
  }

  
  //This is the OBJ parser, some of this is older and uglier code of mine
  void EntityModel::loadOBJ(const string &filename, float scale, shared_ptr<MaterialManager> materialManager, shared_ptr<TextureManager> textureManager)
  {
    const char *fpath = filename.c_str();
    bool no_norms = false;
    float3 bbmin, bbmax;
    
    //ifstream in((filename).c_str());
    ifstream in(fpath, ifstream::in);
    if(!in.is_open())
    {
      perror("OBJ File open");
      throw 1;
    }
    
    int istart = 0, i, j = 0, k = 0, ii = 0, num_norms = 0, num_tc = 0;
    int istart_q = 0, iiq = 0;
    float x, y, z;
    string token;
    
    bbmin = make_float3(1.0e10f,  1.0e10f,  1.0e10f);
    bbmax = make_float3(-1.0e10f,  -1.0e10f,  -1.0e10f);
    
    shared_ptr<Mesh> submesh;
    
    int n_verts = 0, n_inds = 0, n_inds_q = 0;
    if(in.is_open())
    {
      bool tokenAlreadyRead = false;
      
      //perform a quick scan of the file to determine storage requirements
      while(in.good())
      {
        if(!tokenAlreadyRead)
          in >> token;
        
        if(token == "v")
          n_verts++;
        if(token == "f")
        {
          int count = 0;
          
          do
          {
            in >> token;
            count++;
          } while(in.good() && !token.empty() && isdigit(token[0]));
          count--;
          
          tokenAlreadyRead = true;
          
          if(count == 3)
            n_inds += 3;
          else if(count == 4)
            n_inds_q += 4;
          else if(count == 2)
            throw 1;   //edges not yet supported!
        }
        else
          tokenAlreadyRead = false;
        
        if(token == "vn")
          num_norms++;
        if(token == "vt")
          num_tc++;
      }
      
      //rewind file pointer
      in.clear();
      in.seekg(0, ios::beg);
      
      if(num_norms == 0)
      {
        no_norms = true;
        num_norms = n_verts;
      }
      i = 0;
      
      float3 *h_pos, *tmp_pos, *h_norm, *tmp_norm;
      float2 *h_tc, *tmp_tc;
      uint3 *h_ind;
      uint4 *h_ind_quad;
      
      const uint maxVal = UINT_MAX;
      uint verts_alloc_sz = min<uint>(maxVal, n_inds+n_inds_q+1);
      int num_inds = n_inds;
      
      num_verts = n_verts;
      
      h_pos = new float3[verts_alloc_sz];
      h_norm = new float3[verts_alloc_sz];
      tmp_pos = new float3[n_verts];
      tmp_norm = new float3[num_norms];
      h_ind = new uint3[n_inds/3];
      h_ind_quad = new uint4[n_inds_q/4];
      h_tc = new float2[verts_alloc_sz];
      //assigned = new unsigned short[max(num_verts, num_tc)];
      tmp_tc = new float2[num_tc];
      
#ifdef DEBUG
      cout << "nv: " << num_verts << ", nt: " << num_tc << ", " << "alloc: " << verts_alloc_sz << endl;
#endif
      n_verts = 0;
      
      //ridiculous construct to ensure efficient duplication of vertices
      //for normal or texture coordinate reasons
      map<int, map<int, map<int, int>>> used;
      map<int, list<int>> dup_verts;
      
      while(in.good())
      {
        token = "";
        in >> token;
        
        if(token == "mtllib")
        {
          string name;
          in >> name;
          
          string baseName = removeLastPathComponent(filename);
          materialManager->readMtl(baseName + "/" + name, textureManager);
        }
        else if(token == "usemtl")
        {
          string name;
          in >> name;
          
          if(!submesh)
          {
            //start a default submesh if no groups exist
            submesh = shared_ptr<Mesh>(new Mesh);
            istart = ii;
            istart_q = iiq;
          }
          
          if(submesh && submesh->getMaterial())
          {
            finishSubmesh(submesh, h_ind, h_ind_quad, istart, istart_q, num_inds);

            submesh = shared_ptr<Mesh>(new Mesh);
            submesh->setName(name);
            istart = ii;
            istart_q = iiq;
          }
          
          submesh->setMaterial(materialManager->getMaterial(name));
          if(!submesh->getMaterial())
            cerr << "Cannot find material " << name << endl;
        }
        else if(token == "g")
        {
          //read mesh group name
          string name;
          in >> name;
          
          if(submesh)
          {
            finishSubmesh(submesh, h_ind, h_ind_quad, istart, istart_q, num_inds);
          }
          
          submesh = shared_ptr<Mesh>(new Mesh);
          submesh->setName(name);
          istart = ii;
          istart_q = iiq;
        }
        else if(token == "v")
        {
          in >> x >> y >> z;
          if(scale != 1.0f)
          {
            x *= scale; y *= scale; z *= scale;
          }
          tmp_pos[i] = make_float3(x, y, z);
          
          // adjust to a better bound for x
          float val = tmp_pos[i].x;
          bbmin.x = (val < bbmin.x) ? val : bbmin.x;
          bbmax.x = (val > bbmax.x) ? val : bbmax.x;
          
          // adjust to a better bound for y
          val = tmp_pos[i].y;
          bbmin.y = (val < bbmin.y) ? val : bbmin.y;
          bbmax.y = (val > bbmax.y) ? val : bbmax.y;
          
          // adjust to a better bound for z
          val = tmp_pos[i].z;
          bbmin.z = (val < bbmin.z) ? val : bbmin.z;
          bbmax.z = (val > bbmax.z) ? val : bbmax.z;
          
          i++;
        }
        else if(token == "vn")
        {
          in >> x >> y >> z;
          
          tmp_norm[j] = make_float3(x, y, z);
          j++;
        }
        else if(token == "vt")
        {
          in >> x >> y;
          
          tmp_tc[k].x = x;
          tmp_tc[k].y = -y;
          k++;
        }
        else if(token == "f")
        {
          if(!submesh)
          {
            //start a default submesh if no groups exist
            submesh = shared_ptr<Mesh>(new Mesh);
            istart = ii;
          }
          
          int vi[4], ti[4] = { -1, -1, -1, -1 }, ni[4] = { -1, -1, -1, -1 };
          int count = 0;
          string s;
          char c;
          
          for(int l = 0; l < 4; l++)
          {
            in >> vi[l];
            c = in.peek();
            if(!isspace(c))
            {
              in >> c;
              if(isdigit(in.peek()))
                in >> ti[l];
              c = in.peek();
              if(!isspace(c))
              {
                in >> c;
                in >> ni[l];
              }
            }
            
            int &ind = used[vi[l]][ni[l]][ti[l]];
            if(!ind)
            {
              h_pos[n_verts] = tmp_pos[vi[l]-1];
              if(ni[l] >= 0)
                h_norm[n_verts] = tmp_norm[ni[l]-1];
              else
                h_norm[n_verts] = make_float3(0, 0, 0);
              if(ti[l] >= 0)
                h_tc[n_verts] = tmp_tc[ti[l]-1];
              
              dup_verts[vi[l]].push_back(n_verts);
              
              vi[l] = ind = n_verts+1;
              n_verts++;
              
              if(n_verts >= verts_alloc_sz)
              {
                cerr << "obj load overflow " << n_verts << "!!!" << endl;
                vi[l] = 0;
                throw std::runtime_error("OBJ file too large");
              }
            }
            else
              vi[l] = ind;
            
            if(l == 2)
            {
              while(in.peek() == ' ')
                in.get();
              
              //check to see if this thing is a quad or not
              //if(in.peek() == '\r' || in.peek() == '\n')
              if(isdigit(in.peek()))
              {
                count = 4;
              }
              else
              {
                count = 3;
                break;
              }
            }
          }
          
          if(!in.good())
            throw std::runtime_error("Bad face read");
          
          //if(verts_alloc_sz < 1000)
          //  NSLog(@"%d %d %d, %d", vi[0]-1, vi[1]-1, vi[2]-1, num_verts);
          
          if(count == 3)
          {
            h_ind[ii] = make_uint3(vi[0]-1, vi[1]-1, vi[2]-1);
            ii++;
            submesh->num_inds += 3;
          }
          else if(count == 4)
          {
            h_ind_quad[iiq] = make_uint4(vi[0]-1, vi[1]-1, vi[2]-1, vi[3]-1);
            iiq++;
            submesh->num_quad_inds += 4;
          }
        }
      }
      
      //NSLog(@"new num verts: %d", n_verts);
      in.close();
      num_verts = n_verts;
      
      //center bbox
      float3 size = make_float3(bbmax.x-bbmin.x, bbmax.y-bbmin.y, bbmax.z-bbmin.z);
      bbsize = size;
      
      float maxDim = max(bbsize.x, bbsize.y);
      maxDim = max(bbsize.z, maxDim);
      
      //scale to fit
      //NSLog(@"Obj import max dim: %g", maxDim);
      if(maxDim > 50)
      {
        float fitScale = 50.0f/maxDim;
        for(int i = 0; i < num_verts; i++)
        {
          h_pos[i].x *= fitScale;
          h_pos[i].y *= fitScale;
          h_pos[i].z *= fitScale;
        }
        size.x *= fitScale;
        size.y *= fitScale;
        size.z *= fitScale;
      }
      
      if(submesh)
      {
        finishSubmesh(submesh, h_ind, h_ind_quad, istart, istart_q, num_inds);
      }
      
      glGenBuffers(1, &vertex_vbo);
      glGenBuffers(1, &normal_vbo);
      glGenBuffers(1, &tex_vbo);

      glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
      glBufferData(GL_ARRAY_BUFFER, num_verts*sizeof(float)*3, h_pos, GL_STATIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
      glBufferData(GL_ARRAY_BUFFER, num_verts*sizeof(float)*3, h_norm, GL_STATIC_DRAW);
      
      glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
      glBufferData(GL_ARRAY_BUFFER, num_verts*sizeof(float)*2, h_tc, GL_STATIC_DRAW);
      
      delete []tmp_pos;
      delete []h_pos;
      delete []h_ind;     
      delete []h_ind_quad;
      delete []h_norm;
      delete []h_tc;
      delete []tmp_norm;
      delete []tmp_tc;    
      
      if(no_norms)
      {
        //todo: this
        //[self recalcNorms];
      }
     
      setupVAO();
    }
  }
  
  void EntityModel::setupVAO()
  {
    shared_ptr<vgl::StateMachine> vgl = vgl::System::system().currentStateMachine();

    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    vgl->setVertexVBO(3, GL_FLOAT, 0, 0);
    vgl->enableVertexArray(true);
    
    glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
    vgl->setNormalVBO(3, GL_FLOAT, 0, 0);
    vgl->enableNormalArray(true);
    
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
    vgl->setTexcoordVBO(2, GL_FLOAT, 0, 0);
    vgl->enableTexcoordArray(true);
    
    if(tangent_vbo)
    {
      glBindBuffer(GL_ARRAY_BUFFER, tangent_vbo);
      vgl->setTangentVBO(3, GL_FLOAT, 0, 0);
      vgl->enableTangentArray(true);
    }
    else
    {
      vgl->enableTangentArray(false);
    }
    
    vgl->bindDefaultVAO();
  }
}

