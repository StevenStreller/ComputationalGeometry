#include "CgHalfEdgeTriangleMesh.h"

#include "CgBase/CgEnums.h"
#include "CgUtils/ObjLoader.h"
#include <iostream>
#include <cassert>


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

CgHeEdge *nonConstEdge(const CgBaseHeEdge *edge) {
    assert(edge != nullptr); // Überprüfung auf nullptr
    return const_cast<CgHeEdge *>(dynamic_cast<const CgHeEdge *>(edge));
}

void CgHalfEdgeTriangleMesh::subdivision() {
    for (auto &m_face: m_faces) {

        // TODO GEFÄHRLICH const_cast
        auto *e1 = nonConstEdge(m_face->edge());
        auto *e2 = nonConstEdge(m_face->edge()->next());
        auto *e3 = nonConstEdge(m_face->edge()->next()->next());

        // Neue Kante erzeugen und Startpunkt in der Mitte zum alten Punkt von alter Kante
        // Neue Kante in m_edges reinknüppeln :-)
        // half-edge struktur auffrischen
        // Faces generieren mithelfe von half-edge

        // Alte Vertices
        CgHeVert *ov1 = e1->m_vert;
        CgHeVert *ov2 = e2->m_vert;
        CgHeVert *ov3 = e3->m_vert;

        // Erstelle neue Vertices
        auto *nv1 = new CgHeVert();
        nv1->m_position = (e1->vert()->position() + e1->next()->next()->vert()->position()) / 2.0f;

        auto *nv2 = new CgHeVert();
        nv2->m_position = (e2->vert()->position() + e2->next()->next()->vert()->position()) / 2.0f;

        auto *nv3 = new CgHeVert();
        nv3->m_position = (e3->vert()->position() + e3->next()->next()->vert()->position()) / 2.0f;


        // INNERES DREIECK
        // Erstelle neue Edges
        auto *n1 = new CgHeEdge();
        auto *n2 = new CgHeEdge();
        auto *n3 = new CgHeEdge();

        // Setze die Vertices der neuen Kanten
        n1->m_vert = nv1;
        n1->m_next = n2;

        n2->m_vert = nv2;
        n2->m_next = n3;

        n3->m_vert = nv3;
        n3->m_next = n1;

        // Erstelle ein neues Face
        auto *newFace = new CgHeFace();

        // Setze die Kante des Faces auf eine der neu erstellten Kanten (z.B. n1)
        newFace->m_edge = n1;

        // Setze die Farbe für die Kanten
        nv1->m_color = glm::vec3(1.0f, 0.0f, 0.0f);
        nv2->m_color = glm::vec3(1.0f, 0.0f, 0.0f);
        nv3->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

        // Füge die neuen Vertices in die Liste der Vertices ein
        temp_m_verts.push_back(nv1);
        temp_m_verts.push_back(nv2);
        temp_m_verts.push_back(nv3);

        // Füge die neuen Kanten in die Liste der Kanten ein
        temp_m_edges.push_back(n1);
        temp_m_edges.push_back(n2);
        temp_m_edges.push_back(n3);

        // Füge das neue Face in die Liste der Faces ein
        temp_m_faces.push_back(newFace);

        // OBERES DREIECK
        // Erstelle neue Edges
        auto *e6 = new CgHeEdge();
        auto *e4 = new CgHeEdge();
        auto *e5 = new CgHeEdge();

        // Vertices setzen

        e6->m_vert = nv1;
        e4->m_vert = nv3;
        e5->m_vert = ov3;

        e6->m_next = e4;
        e4->m_next = e5;
        e5->m_next = e6;

        // Erstelle ein neues Face
        auto *nf2 = new CgHeFace();

        // Setze die Kante des Faces auf eine der neu erstellten Kanten
        nf2->m_edge = e6;

        temp_m_edges.push_back(e6);
        temp_m_edges.push_back(e4);
        temp_m_edges.push_back(e5);

        temp_m_faces.push_back(nf2);

        // LINKES DREIECK
        // Erstelle neue Edges
        auto *e7 = new CgHeEdge();
        auto *e8 = new CgHeEdge();
        auto *e9 = new CgHeEdge();

        // Vertices setzen
        e7->m_vert = ov1;
        e8->m_vert = nv2;
        e9->m_vert = nv1;

        e7->m_next = e8;
        e8->m_next = e9;
        e9->m_next = e7;

        // Erstelle ein neues Face
        auto *nf3 = new CgHeFace();

        // Setze die Kante des Faces auf eine der neu erstellten Kanten
        nf3->m_edge = e7;

        temp_m_edges.push_back(e7);
        temp_m_edges.push_back(e8);
        temp_m_edges.push_back(e9);

        temp_m_faces.push_back(nf3);

        // RECHTES DREIECK
        // Erstelle neue Edges
        auto *e10 = new CgHeEdge();
        auto *e11 = new CgHeEdge();
        auto *e12 = new CgHeEdge();

        // Vertices setzen
        e10->m_vert = ov2;
        e11->m_vert = nv3;
        e12->m_vert = nv2;

        e10->m_next = e11;
        e11->m_next = e12;
        e12->m_next = e10;

        // Erstelle ein neues Face
        auto *nf4 = new CgHeFace();

        // Setze die Kante des Faces auf eine der neu erstellten Kanten
        nf4->m_edge = e10;

        temp_m_edges.push_back(e10);
        temp_m_edges.push_back(e11);
        temp_m_edges.push_back(e12);

        temp_m_faces.push_back(nf4);


        // Für Kante e1
        std::cout << "Koordinaten von e1:" << std::endl;
        std::cout << "Punkt 1: (" << e1->vert()->position().x << ", " << e1->vert()->position().y << ", "
                  << e1->vert()->position().z << ")" << std::endl;
        if (e1->pair() != nullptr) {
            std::cout << "Punkt 2: (" << e1->pair()->vert()->position().x << ", "
                      << e1->pair()->vert()->position().y
                      << ", " << e1->pair()->vert()->position().z << ")" << std::endl;
        }
        // Für Kante e2
        std::cout << "Koordinaten von e2:" << std::endl;
        std::cout << "Punkt 1: (" << e2->vert()->position().x << ", " << e2->vert()->position().y << ", "
                  << e2->vert()->position().z << ")" << std::endl;
        if (e2->pair() != nullptr) {
            std::cout << "Punkt 2: (" << e2->pair()->vert()->position().x << ", "
                      << e2->pair()->vert()->position().y
                      << ", " << e2->pair()->vert()->position().z << ")" << std::endl;
        }
        // Für Kante e3
        std::cout << "Koordinaten von e3:" << std::endl;
        std::cout << "Punkt 1: (" << e3->vert()->position().x << ", " << e3->vert()->position().y << ", "
                  << e3->vert()->position().z << ")" << std::endl;
        if (e3->pair() != nullptr) {
            std::cout << "Punkt 2: (" << e3->pair()->vert()->position().x << ", "
                      << e3->pair()->vert()->position().y
                      << ", " << e3->pair()->vert()->position().z << ")" << std::endl;
        }
    }
    m_edges.clear();
    m_verts.clear();
    m_faces.clear();
    m_edges = temp_m_edges;
    m_verts = temp_m_verts;
    m_faces = temp_m_faces;
}

void CgHalfEdgeTriangleMesh::parseOBJ(const std::vector <glm::vec3> &vertices, const std::vector <glm::vec3> &normals,
                                      const std::vector<unsigned int> &indices) {

    m_faces.clear();
    m_edges.clear();
    m_verts.clear();

    std::vector < CgHeVert * > cgHeVerts(vertices.size());
    std::vector < CgHeEdge * > cgHeEdges;
    std::vector < CgHeFace * > cgHeFaces;

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
        cgHeFaces[i]->m_normal = glm::cross(cgHeFaces[i]->m_edge->m_vert->m_position,
                                            cgHeFaces[i]->m_edge->m_next->m_vert->m_position);
        m_faces.push_back(cgHeFaces[i]);
    }
    for (size_t i = 0; i < cgHeEdges.size(); i++) {
        m_edges.push_back(cgHeEdges[i]);
    }
    for (size_t i = 0; i < cgHeVerts.size(); i++) {
        cgHeVerts[i]->m_color = glm::vec3(0.0, 1.0, 0.0);
        m_verts.push_back(cgHeVerts[i]);
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
    std::vector <glm::vec3> temp_vertices;
    std::vector <glm::vec3> temp_vertnormals;
    std::vector<unsigned int> temp_indices;

    ObjLoader loader;
    loader.load(filename);

    loader.getPositionData(temp_vertices);
    loader.getNormalData(temp_vertnormals);
    loader.getFaceIndexData(temp_indices);

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
