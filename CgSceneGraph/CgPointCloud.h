#ifndef CGPOINTCLOUD_H
#define CGPOINTCLOUD_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "CgBase/CgBasePointCloud.h"
#include "CgKdTree.h"
#include "CgSceneGraph/CgTriangleMesh.h"
#include "CgSceneGraph/CgLeastSquares.h"

class CgPointCloud : public CgBasePointCloud
{
public:
  CgPointCloud();
  CgPointCloud(int id);

  ~CgPointCloud();

  //inherited from CgBaseRenderableObject
  Cg::ObjectType getType() const;
  unsigned int getID() const;


  //inherited from CgBasePointCloud

  // vertex positions in local coordinates
  const std::vector<glm::vec3>& getVertices()     const;

  // normal directions in local coordinates (normalized)
  const std::vector<glm::vec3>& getVertexNormals() const;

  // vertex colors
  const std::vector<glm::vec3>& getVertexColors() const;

  // a local "look at matrix" for normal direction and the spanning directions of the tangent plane
  const std::vector<glm::mat4>& getSplatOrientations() const;

  // for rendering ellipes use a different scaling in the two spanning directions of the tangent plane
  const std::vector<glm::vec2>& getSplatScalings() const;

  // get the indices of the splats which should be rendered
   const std::vector<unsigned int>& getSplatIndices() const;

  // read a dataset from file, can cheat the normals, i.e read the mormals from file
  void init( std::string filename, bool cheat_normals=false);

  // the center of gravity of the object, for rendering
  const glm::vec3 getCenter() const;



  /**
   * @brief markKNearest marks k-Nearest nodes using kd-tree to find them and sets them to red color
   * @param kNearestIndex searches for nearest point to node with this index
   * @param kNearestValue neighbors count to find
   */
  void markKNearest(int kNearestIndex, int kNearestValue);

  /**
   * @brief pickNearestPoint ray picking
   * @param rayStart picking ray start
   * @param rayDir picking ray direction
   * @param kVal neighbors count to find
   * @param showSmoothMesh if true mesh pointer will be swapped for least squares approximation function
   * @param mesh pointer to pointer of mesh where the visualization should be set
   * @param baseFunction for least squares visualization
   * @return index of Node that was found
   */
  int pickNearestPoint(glm::vec3 rayStart, glm::vec3 rayDir, int kVal, bool showSmoothMesh, CgTriangleMesh** mesh, CgLeastSquares::BaseFunction baseFunction);

  /**
   * @brief drawKdTree draws kd tree splitting planes
   * @param depth depth limit
   * @return mesh with splitting planes
   */
  CgTriangleMesh* drawKdTree(int depth);

  /**
   * @brief smoothWithLeastSquares smooths whole mesh with least squares
   * @param kValue neighborhood size
   * @param baseFunction either biquadratic or bicubic
   */
  void smoothWithLeastSquares(int kValue, CgLeastSquares::BaseFunction baseFunction);

private:

    // the following demonstration methods have to be replaced by your own calculations

    //for demonstration: find local coordinate system (normal plus arbitrary tangent spanning directions)
    void calculateSplatOrientations();

    // for demonstration: for a given normal direction find an arbitrary vector to span the tangent plane
    glm::vec3 getPerpendicularVector(glm::vec3 arg);

    // for demonstration purposes, very inefficient
    // kdTree->getKNearest should be used
    std::vector<int> getNearestNeighbors(int current_point, unsigned int k);


    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_vertex_normals;
    std::vector<glm::vec3> m_vertex_colors;

    // indices of vertices for which a splat will be rendered
    std::vector<unsigned int> m_splat_indices;


    std::vector<glm::mat4> m_splat_orientations;
    std::vector<glm::vec2> m_splat_scaling;

    const Cg::ObjectType m_type;
    const unsigned int m_id;

    /**
     * @brief kdTree kdTree used for neighbor search
     */
    CgKdTree* kdTree;
};

inline Cg::ObjectType  CgPointCloud::getType() const {return m_type;}
inline unsigned int CgPointCloud::getID() const {return m_id;}
inline const std::vector<glm::vec3>& CgPointCloud::getVertices() const{return m_vertices;}
inline const std::vector<glm::vec3>& CgPointCloud::getVertexNormals() const{return m_vertex_normals;}
inline const std::vector<glm::vec3>& CgPointCloud::getVertexColors() const{return m_vertex_colors;}
inline const std::vector<glm::mat4>& CgPointCloud::getSplatOrientations() const{return m_splat_orientations;}
inline const std::vector<unsigned int>& CgPointCloud::getSplatIndices() const{return m_splat_indices;}





#endif // CGPOINTCLOUD_H
