#include "CgHalfEdgeTriangleMesh.h"

#include "CgBase/CgEnums.h"
#include "CgUtils/ObjLoader.h"
#include <iostream>


CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh() :
        m_type(Cg::HalfEdgeTriangleMesh),
        m_id(42) {

    CgHeFace *nf1 = new CgHeFace();
    CgHeFace *nf2 = new CgHeFace();

    CgHeVert *nv1 = new CgHeVert();
    CgHeVert *nv2 = new CgHeVert();
    CgHeVert *nv3 = new CgHeVert();
    CgHeVert *nv4 = new CgHeVert();

    CgHeEdge *n1 = new CgHeEdge();
    CgHeEdge *n2 = new CgHeEdge();
    CgHeEdge *n3 = new CgHeEdge();
    CgHeEdge *n4 = new CgHeEdge();
    CgHeEdge *n5 = new CgHeEdge();
    CgHeEdge *n6 = new CgHeEdge();


    n1->m_next = n2;
    n2->m_next = n3;
    n3->m_next = n1;
    nf1->m_edge = n1;

    n4->m_next = n5;
    n5->m_next = n6;
    n6->m_next = n4;
    nf2->m_edge = n4;

    nv1->m_position = glm::vec3(0.0, 0.0, 0.0);
    nv2->m_position = glm::vec3(0.0, 1.0, 0.0);
    nv3->m_position = glm::vec3(1.0, 1.0, 0.0);
    nv4->m_position = glm::vec3(1.0, 0.0, 0.0);

    nv1->m_edge = n1;
    nv2->m_edge = n2;
    nv3->m_edge = n6;
    nv4->m_edge = n3;

    n4->m_vert = nv4;
    n5->m_vert = nv2;
    n6->m_vert = nv3;

    n1->m_vert = nv1;
    n2->m_vert = nv2;
    n3->m_vert = nv4;

    n2->m_pair = n4;
    n4->m_pair = n2;

    // store into lists
    m_faces.push_back(nf1);
    m_faces.push_back(nf2);

    m_verts.push_back(nv1);
    m_verts.push_back(nv2);
    m_verts.push_back(nv3);
    m_verts.push_back(nv4);

    m_edges.push_back(n1);
    m_edges.push_back(n2);
    m_edges.push_back(n3);
    m_edges.push_back(n4);
    m_edges.push_back(n5);
    m_edges.push_back(n6);

    //attributes
    nv1->m_color = glm::vec3(1.0, 0.0, 0.0);
    nv2->m_color = glm::vec3(1.0, 0.0, 0.0);
    nv3->m_color = glm::vec3(1.0, 0.0, 0.0);
    nv4->m_color = glm::vec3(1.0, 0.0, 0.0);

    nf1->m_normal = glm::vec3(0.0, 0.0, 1.0);
    nf2->m_normal = glm::vec3(0.0, 0.0, 1.0);

}

CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh(int id) :
        m_type(Cg::HalfEdgeTriangleMesh),
        m_id(id) {

}


CgHalfEdgeTriangleMesh::~CgHalfEdgeTriangleMesh() {
    // thats not enough, have to kill Objects as well´
    m_faces.clear();
    m_edges.clear();
    m_verts.clear();
}

const std::vector<CgBaseHeFace *> &CgHalfEdgeTriangleMesh::getFaces() const {
    return m_faces;
}

void CgHalfEdgeTriangleMesh::parseOBJ(const std::vector<glm::vec3> &vertices, const std::vector<glm::vec3> &normals,
                                      const std::vector<unsigned int> &indices) {

    m_faces.clear();
    m_edges.clear();
    m_verts.clear();

    std::vector<CgHeVert *> cgHeVerts(vertices.size());
    std::vector<CgHeEdge *> cgHeEdges;
    std::vector<CgHeFace *> cgHeFaces;

    for (unsigned int i = 0; i < vertices.size(); i++) {
        cgHeVerts[i] = new CgHeVert();
        cgHeVerts[i]->m_position = vertices[i];
    }

    std::cout << indices.size() << std::endl;
    // Create half-edges for each face
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int vIndex1 = indices[i];
        unsigned int vIndex2 = indices[i + 1];
        unsigned int vIndex3 = indices[i + 2];

        // Create edges
        CgHeEdge *edge1 = new CgHeEdge();
        CgHeEdge *edge2 = new CgHeEdge();
        CgHeEdge *edge3 = new CgHeEdge();

        cgHeEdges.push_back(edge1);
        cgHeEdges.push_back(edge2);
        cgHeEdges.push_back(edge3);

        // Assign edge to vertex
        cgHeVerts[vIndex1]->m_edge = edge3;
        cgHeVerts[vIndex2]->m_edge = edge1;
        cgHeVerts[vIndex3]->m_edge = edge2;

        // Assign vertices to edges
        cgHeEdges[cgHeEdges.size() - 3]->m_vert = cgHeVerts[vIndex1];
        cgHeEdges[cgHeEdges.size() - 2]->m_vert = cgHeVerts[vIndex2];
        cgHeEdges[cgHeEdges.size() - 1]->m_vert = cgHeVerts[vIndex3];


        // Assign next pointers
        cgHeEdges[cgHeEdges.size() - 3]->m_next = cgHeEdges[cgHeEdges.size() - 2];
        cgHeEdges[cgHeEdges.size() - 2]->m_next = cgHeEdges[cgHeEdges.size() - 1];
        cgHeEdges[cgHeEdges.size() - 1]->m_next = cgHeEdges[cgHeEdges.size() - 3];

        // Assign face to edge
        CgHeFace *face = new CgHeFace();
        cgHeEdges[cgHeEdges.size() - 3]->m_face = face;
        cgHeEdges[cgHeEdges.size() - 2]->m_face = face;
        cgHeEdges[cgHeEdges.size() - 1]->m_face = face;
        cgHeFaces.push_back(face);

        // Assign edge to face
        face->m_edge = cgHeEdges[cgHeEdges.size() - 3];

        // Assign pair to edge
        for (size_t j = 0; j < cgHeEdges.size(); j++) {
            for (size_t m = 0; m < cgHeEdges.size(); m++) {

                // TODO only works with triangles
                if (cgHeEdges[j]->m_next->m_next->m_vert == cgHeEdges[m]->m_vert &&
                    cgHeEdges[m]->m_next->m_next->m_vert == cgHeEdges[j]->m_vert) {
                    cgHeEdges[j]->m_pair = cgHeEdges[m];
                    cgHeEdges[m]->m_pair = cgHeEdges[j];
                }
            }
        }
    }

    for (size_t i = 0; i < cgHeFaces.size(); i++) {
        cgHeFaces[i]->m_normal = glm::vec3(0.0, 0.0, 1.0);
        m_faces.push_back(cgHeFaces[i]);
    }
    for (size_t i = 0; i < cgHeEdges.size(); i++) {
        m_edges.push_back(cgHeEdges[i]);
    }
    for (size_t i = 0; i < cgHeVerts.size(); i++) {
        cgHeVerts[i]->m_color = glm::vec3(0.0, 1.0, 0.0);
        m_verts.push_back(cgHeVerts[i]);
    }

    // calculate normals
    for (size_t i = 0; i < cgHeFaces.size(); i++) {
        CgHeEdge *edge = cgHeFaces[i]->m_edge;

        CgHeVert *v1 = edge->m_vert;
        CgHeVert *v2 = edge->m_next->m_vert;
        CgHeVert *v3 = edge->m_next->m_next->m_vert;

        std::cout << "v1: " << v1->m_position.x << v1->m_position.y << v1->m_position.z << std::endl;
        std::cout << "v2: " << v2->m_position.x << v2->m_position.y << v2->m_position.z << std::endl;
        std::cout << "v3: " << v3->m_position.x << v3->m_position.y << v3->m_position.z << std::endl;

        // TODO: normalen berechnen (Kreuzprodukt)
    }


    // Alle Halb-Kanten eines Faces durchlaufen
    int index = 0;
    CgHeEdge *edge = cgHeFaces[index]->m_edge;

    do {
        edge = edge->m_next;
        // std::cout << "index: " << index << ", " << "x: " << edge->m_vert->m_position.x << std::endl;
    } while (edge != cgHeFaces[index]->m_edge);


    // Lauf über Kanten, die zu gegebenem Vertex gehören
    edge = cgHeVerts[index]->m_edge;
    while (edge != cgHeVerts[index]->m_edge && edge->m_pair != nullptr) {
        edge = edge->m_pair->m_next;
        std::cout << "x: " << edge->m_next->m_vert->m_position.x << " y: " << edge->m_next->m_vert->m_position.y
                  << " z: " << edge->m_next->m_vert->m_position.z << std::endl;
    }
}

void CgHalfEdgeTriangleMesh::init(std::string filename) {
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_vertnormals;
    std::vector<unsigned int> temp_indices;


    ObjLoader loader;
    loader.load(filename);

    std::cout << "Size:" << temp_vertnormals.size();

    loader.getPositionData(temp_vertices);
    loader.getNormalData(temp_vertnormals);
    loader.getFaceIndexData(temp_indices);

    std::cout << " Size:" << temp_vertnormals.size() << std::endl;

    /*for(size_t i=0; i < temp_vertnormals.size(); i++) {
        std::cout << "test: " << temp_vertnormals[i].x << " " << temp_vertnormals[i].y << " " << temp_vertnormals[i].z << std::endl;
    }*/


    parseOBJ(temp_vertices, temp_vertnormals, temp_indices);

    // now need to convert into HalfEdge Datastructure :-) ...
}

const glm::vec3 CgHalfEdgeTriangleMesh::getCenter() const {
    glm::vec3 center(0.);
    for (unsigned int i = 0; i < m_verts.size(); i++) {
        center += m_verts[i]->position();
    }
    center /= (double) m_verts.size();
    return center;
}
