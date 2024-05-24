#ifndef CGPOINTCLOUD_H
#define CGPOINTCLOUD_H

#include <vector>
#include <glm/glm.hpp>
#include <string>
#include "CgBase/CgBasePointCloud.h"

class CgPointCloud : public CgBasePointCloud {
public:
    CgPointCloud();

    CgPointCloud(int id);

    struct Node {
        glm::vec3 location;
        Node *leftChild;
        Node *rightChild;

        Node(const glm::vec3 &point);
    };

    Node* kdtree(std::vector<glm::vec3>& pointList);

    ~CgPointCloud();

    //inherited from CgBaseRenderableObject
    Cg::ObjectType getType() const;

    unsigned int getID() const;


    //inherited from CgBasePointCloud

    // vertex positions in local coordinates
    const std::vector<glm::vec3> &getVertices() const;

    // normal directions in local coordinates (normalized)
    const std::vector<glm::vec3> &getVertexNormals() const;

    // vertex colors
    const std::vector<glm::vec3> &getVertexColors() const;

    // a local "look at matrix" for normal direction and the spanning directions of the tangent plane
    const std::vector<glm::mat4> &getSplatOrientations() const;

    // for rendering ellipes use a different scaling in the two spanning directions of the tangent plane
    const std::vector<glm::vec2> &getSplatScalings() const;

    // get the indices of the splats which should be rendered
    const std::vector<unsigned int> &getSplatIndices() const;

    // read a dataset from file, can cheat the normals, i.e read the mormals from file
    void init(std::string filename, bool cheat_normals = false);

    // the center of gravity of the object, for rendering
    const glm::vec3 getCenter() const;

private:

    Node* buildKdTree(std::vector<glm::vec3>::iterator begin, std::vector<glm::vec3>::iterator end, int depth);

    // the following demonstration methods have to be replaced by your own calculations

    //for demonstration: find local coordinate system (normal plus arbitrary tangent spanning directions)
    void calculateSplatOrientations();

    // for demonstration: for a given normal direction find an arbitrary vector to span the tangent plane
    glm::vec3 getPerpendicularVector(glm::vec3 arg);

    // for demonstration purposes, very inefficient
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


    const Node *kdTree(std::vector<glm::vec3> pointList, int depth);

};

inline Cg::ObjectType CgPointCloud::getType() const { return m_type; }

inline unsigned int CgPointCloud::getID() const { return m_id; }

inline const std::vector<glm::vec3> &CgPointCloud::getVertices() const { return m_vertices; }

inline const std::vector<glm::vec3> &CgPointCloud::getVertexNormals() const { return m_vertex_normals; }

inline const std::vector<glm::vec3> &CgPointCloud::getVertexColors() const { return m_vertex_colors; }

inline const std::vector<glm::mat4> &CgPointCloud::getSplatOrientations() const { return m_splat_orientations; }

inline const std::vector<unsigned int> &CgPointCloud::getSplatIndices() const { return m_splat_indices; }


#endif // CGPOINTCLOUD_H
