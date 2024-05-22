#ifndef CGHALFEDGETRIANGLEMESH_H
#define CGHALFEDGETRIANGLEMESH_H

#include "CgBase/CgBaseHalfEdgeTriangleMesh.h"
#include "CgHalfEdgePrimitives.h"
#include "CgTriangleMesh.h"
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <unordered_set>
#include <set>
#include "CgSceneGraph/CgLeastSquares.h"

class CgHalfEdgeTriangleMesh : public CgBaseHalfEdgeTriangleMesh
{
public:
    /**
     * @brief CgHalfEdgeTriangleMesh inits HE mesh with simple square
     */
    CgHalfEdgeTriangleMesh();
    CgHalfEdgeTriangleMesh(int id);

    ~CgHalfEdgeTriangleMesh();

    //inherited from CgBaseRenderableObject
    Cg::ObjectType getType() const;
    unsigned int getID() const;

    //inherited from CgBaseHalfEdgeTriangleMesh

    const std::vector<CgBaseHeFace*>& getFaces() const;

    //own stuff

    /**
     * @brief init initializes Half Edge data structure
     * @param filename obj file to be loaded
     */
    void init( std::string filename);
    const glm::vec3 getCenter() const;

    /**
     * @brief calculateVertexNormals calculates normals for vertices using face normals from HE data
     */
    void calculateVertexNormals();
    /**
     * @brief calculateFaceNormals calculates face normals using vertex positions in HE data
     */
    void calculateFaceNormals();

    /**
     * @brief loopSubdivide subdivide algorithm of loop, creates new points, reorders HE pointers and moves all points accordingly
     */
    void loopSubdivide();

    /**
     * @brief pickNearestPointAndVisualizeLeastSquares picking, used to select point on mesh and changes colours of its 2-ring-neighborhood
     * @param rayStart
     * @param rayEnd
     * @param showSmoothingMesh if true, mesh pointer will be set to a mesh visualizong least squares fitting plane
     * @param baseFunction of the least squares fitting
     * @param mesh to be set to visualization of fitting function
     */
    void pickNearestPointAndVisualizeLeastSquares(glm::vec3 rayStart, glm::vec3 rayEnd, bool showSmoothingMesh, CgLeastSquares::BaseFunction baseFunction, CgTriangleMesh** mesh);

    /**
     * @brief getTwoRingNeighborhood retrives two ring neighborhood of vertex
     * @param vert
     * @return unordered set of neoghboring vertices
     */
    std::unordered_set<CgHeVert*> getTwoRingNeighborhood(CgHeVert* vert);

    /**
     * @brief getOneRingNeighborhood retrives direct neighbors of a vertex
     * @param vert
     * @return list of neighboring vertices
     */
    std::vector<CgHeVert*> getOneRingNeighborhood(CgHeVert* vert);

    /**
     * @brief getTwoRingNeighborhoodOrdered retrives two ring neighborhood of vertex
     * @param vert
     * @return set of neoghboring vertices
     */
    std::set<CgHeVert*> getTwoRingNeighborhoodOrdered(CgHeVert* vert);

    /**
     * @brief smoothWithLeastSquares smooths whole mesh using least squates fitting
     * @param baseFunction biquadratic or bicubic
     */
    void smoothWithLeastSquares(CgLeastSquares::BaseFunction baseFunction);
private:

    std::vector<CgBaseHeFace*> m_faces;
    std::vector<CgBaseHeVert*> m_verts;
    std::vector<CgBaseHeEdge*> m_edges;

    const Cg::ObjectType m_type;
    const unsigned int m_id;

};


inline Cg::ObjectType  CgHalfEdgeTriangleMesh::getType() const {return m_type;}
inline unsigned int CgHalfEdgeTriangleMesh::getID() const {return m_id;}



#endif // CGHALFEDGETRIANGLEMESH_H
