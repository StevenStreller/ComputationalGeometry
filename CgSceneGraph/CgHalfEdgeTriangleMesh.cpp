#include "CgHalfEdgeTriangleMesh.h"

#include "CgBase/CgEnums.h"
#include "CgUtils/ObjLoader.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "glm/gtx/hash.hpp" //not used but needed!
#include <glm/gtx/norm.hpp>
#include <thread>


//Parallel execution
//#if __cplusplus >= 201703L
//#include <algorithm>
//#include <execution>
//#endif






void CgHalfEdgeTriangleMesh::pickNearestPointAndVisualizeLeastSquares(glm::vec3 rayStart, glm::vec3 rayEnd, bool showSmoothingMesh,
                                                                      CgLeastSquares::BaseFunction baseFunction, CgTriangleMesh** mesh)
{
    //Line(t) = rayStart + t * rayEnd-rayStart

    for(auto& v : m_verts)
        ((CgHeVert*)v)->m_color = glm::vec3(0.0,1.0,0.0);

    glm::vec3 rayDir = glm::normalize(rayEnd - rayStart);
    float currentMinDistance = std::numeric_limits<float>::max();
    int currentBest = 0;
    for(int i=0; i<m_verts.size(); i++)
    {
        float t = glm::dot(rayDir, m_verts[i]->position()-rayStart);
        float distance = glm::length2(m_verts[i]->position() - (rayStart + t * rayDir));
        if(distance < currentMinDistance){
            currentMinDistance = distance;
            currentBest = i;
        }
    }

    CgBaseHeVert* bestVert = m_verts[currentBest];
    std::unordered_set<CgHeVert*> neighbors = getTwoRingNeighborhood((CgHeVert*)bestVert);

    std::vector<glm::vec3> positions;
    for(auto& v : neighbors){
        positions.push_back(v->position());
        v->m_color = glm::vec3(1.0,0.0,0.0);
    }

    if(showSmoothingMesh)
        CgLeastSquares::visualizeLeastSquares(m_verts[currentBest]->position(), ((CgHeVert*)m_verts[currentBest])->m_normal,
                                     positions, baseFunction, mesh);
}


void CgHalfEdgeTriangleMesh::smoothWithLeastSquares(CgLeastSquares::BaseFunction baseFunction) {
    std::unordered_map<CgHeVert*, glm::vec4> positionSumsCounts;
    int nans = 0;
    for(int v=0; v<m_verts.size(); v++){

        auto neighbors = getTwoRingNeighborhoodOrdered((CgHeVert*)m_verts[v]);

        std::vector<glm::vec3> positions;
        for(auto& v : neighbors)
            positions.push_back(v->position());

        auto smoothVerts = CgLeastSquares::calculateLeastSquares(m_verts[v]->position(), ((CgHeVert*)m_verts[v])->m_normal,
                                             positions, baseFunction);

        int iter = 0;
        for(auto& v : neighbors)
        {
            if(!(std::isnan(smoothVerts[iter].x) || std::isnan(smoothVerts[iter].y) || std::isnan(smoothVerts[iter].z))){

                auto find = positionSumsCounts.find(v);
                if(find == positionSumsCounts.end()){
                    positionSumsCounts.insert(std::make_pair(v, glm::vec4(smoothVerts[iter],1)));
                }
                else
                    positionSumsCounts[v] += glm::vec4(smoothVerts[iter],1);
            } else {
                nans++;
            }
            iter++;

        }
    }
    std::cout<<nans << " NaN values detected and skipped"<<std::endl;
    for(auto& v : positionSumsCounts){
        v.first->m_position.x = v.second.x / v.second.w;
        v.first->m_position.y = v.second.y / v.second.w;
        v.first->m_position.z = v.second.z / v.second.w;

    }
}



CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh():
m_type(Cg::HalfEdgeTriangleMesh),
m_id(42)
{

   CgHeFace* nf1 = new CgHeFace();
   CgHeFace* nf2 = new CgHeFace();

   CgHeVert* nv1 = new CgHeVert();
   CgHeVert* nv2 = new CgHeVert();
   CgHeVert* nv3 = new CgHeVert();
   CgHeVert* nv4 = new CgHeVert();

   CgHeEdge* n1 = new CgHeEdge();
   CgHeEdge* n2 = new CgHeEdge();
   CgHeEdge* n3 = new CgHeEdge();
   CgHeEdge* n4 = new CgHeEdge();
   CgHeEdge* n5 = new CgHeEdge();
   CgHeEdge* n6 = new CgHeEdge();


    n1->m_next=n2;
    n2->m_next=n3;
    n3->m_next=n1;
    nf1->m_edge=n1;

    n4->m_next=n5;
    n5->m_next=n6;
    n6->m_next=n4;
    nf2->m_edge=n4;

    n1->m_face = nf1;
    n2->m_face = nf1;
    n3->m_face = nf1;
    n4->m_face = nf2;
    n5->m_face = nf2;
    n6->m_face = nf2;

    nv1->m_position=glm::vec3(0.0,0.0,0.0);
    nv2->m_position=glm::vec3(0.0,1.0,0.0);
    nv3->m_position=glm::vec3(1.0,1.0,0.0);
    nv4->m_position=glm::vec3(1.0,0.0,0.0);

    nv1->m_edge=n2;
    nv2->m_edge=n1;
    nv3->m_edge=n6;
    nv4->m_edge=n3;

    n4->m_vert=nv4;
    n5->m_vert=nv3;
    n6->m_vert=nv2;

    n1->m_vert=nv1;
    n2->m_vert=nv4;
    n3->m_vert=nv2;

    n3->m_pair=n4;
    n4->m_pair=n3;

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
    nv1->m_color=glm::vec3(0.0,1.0,0.0);
    nv2->m_color=glm::vec3(0.0,1.0,0.0);
    nv3->m_color=glm::vec3(0.0,1.0,0.0);
    nv4->m_color=glm::vec3(0.0,1.0,0.0);

    nf1->m_normal=glm::vec3(0.0,0.0,1.0);
    nf2->m_normal=glm::vec3(0.0,0.0,1.0);

}

CgHalfEdgeTriangleMesh::CgHalfEdgeTriangleMesh(int id):
m_type(Cg::HalfEdgeTriangleMesh),
m_id(id)
{

}


CgHalfEdgeTriangleMesh::~CgHalfEdgeTriangleMesh()
{
    // thats not enough, have to kill Objects as well´
    m_faces.clear();
    m_edges.clear();
    m_verts.clear();
}

const std::vector<CgBaseHeFace*>& CgHalfEdgeTriangleMesh::getFaces() const
{
    return m_faces;
}

//calculates hash of edge by using its two vertices indicies (2xint32 -> int64)
glm::uint64 getHashForEdge(glm::uint32 start, glm::uint32 end){
    glm::uint64 val = start;
    val = val << 32; //.........num1|..........num2 (2*32bit)
    val += end;
    return val;
}


void CgHalfEdgeTriangleMesh::init( std::string filename)
{
    //verts -> HE_Verts
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_vertnormals;
    //3-er tupel per face?
    std::vector<unsigned int> temp_indices;


    ObjLoader loader;
    loader.load(filename);
    std::cout<<"HE Mesh loaded: " << filename << std::endl;
    std::cout<<"Loading index count of " << loader.getIndexCount()<< std::endl;
    std::cout<<"Vertex count = " << loader.getVertCount() << std::endl;

    loader.getPositionData(temp_vertices);
    loader.getNormalData(temp_vertnormals);
    loader.getFaceIndexData(temp_indices);

    //clean old data
    for(auto& f : m_faces)
        delete f;
    for(auto& v : m_verts)
        delete v;
    for(auto& e : m_edges)
        delete e;

    m_verts.clear();
    m_faces.clear();
    m_edges.clear();


    //save vertices by their positions, multiple vertices laying on same position will be merged, int value is the new index of those vertices
    std::unordered_map<glm::vec3, int, std::hash<glm::vec3>> uniqueVerts;
    int currentUniqueVertexCount = 0;
    for(int i=0; i<temp_vertices.size(); i++){
        //identify unique vertices
        auto found = uniqueVerts.find(temp_vertices[i]);
        //Create new vertices if no preset at position, else reuse already made ones
        if(found == uniqueVerts.end()) {
            uniqueVerts.emplace(std::make_pair(temp_vertices[i], currentUniqueVertexCount++));
            //create Vertex if needed
            CgHeVert* v = new CgHeVert();
            v->m_color = glm::vec3(0,1,0);
            v->m_position = temp_vertices[i];
            m_verts.push_back(v);
        }
    }


    //indicies should be already counterclockwise! (as per wikipedia)

    //As faces are added by using 3 vertices indicies,
    //the indicies can show us the unique vertex by using positions hash function in unique verts map declared before
    //to get vertex numbers that need to be connected

    //by creating an edge, its pair must be found, so put first edge A->B into the map,
    //if its pair will look in this map by its own AB referred as "B->A" it will find pair that is already created
    std::unordered_map<glm::uint64, CgHeEdge*> edgesPerIndicies;

    for(int i=0, f=0; i<temp_indices.size();i+=3, f++){

        //all face edges point to the same face
        CgHeEdge* edge1 = new CgHeEdge();
        CgHeEdge* edge2 = new CgHeEdge();
        CgHeEdge* edge3 = new CgHeEdge();
        CgHeFace* face = new CgHeFace();

        //edges point to their face
        edge1->m_face = face;
        edge2->m_face = face;
        edge3->m_face = face;

        //face points to its "first" edge
        face->m_edge = edge1;

        //edges point to their next edge in face
        edge1->m_next = edge2;
        edge2->m_next = edge3;
        edge3->m_next = edge1;

        //edges point to their vertices
        //find what vertex lies on this postion and use it
        int vert1Idx = uniqueVerts.find(temp_vertices[temp_indices[i]])->second;
        edge1->m_vert = (CgHeVert*)m_verts[vert1Idx];
        int vert2Idx = uniqueVerts.find(temp_vertices[temp_indices[i+1]])->second;
        edge2->m_vert = (CgHeVert*)m_verts[vert2Idx];
        int vert3Idx = uniqueVerts.find(temp_vertices[temp_indices[i+2]])->second;
        edge3->m_vert = (CgHeVert*)m_verts[vert3Idx];

        //if opening then will stay nullptr
        edge1->m_pair = nullptr;
        edge2->m_pair = nullptr;
        edge3->m_pair = nullptr;

        // map "edgesPerIndicies" for HE A->B so HE B->A can easily be found
        //all is CCW so if no B->A found, add A->B else take B->A as Pair and set pair to other side
        //if edge found, no "third" HE will be needed so can remove from map
        auto findE1Pair = edgesPerIndicies.find(getHashForEdge(vert3Idx, vert1Idx));
        auto findE2Pair = edgesPerIndicies.find(getHashForEdge(vert1Idx, vert2Idx));
        auto findE3Pair = edgesPerIndicies.find(getHashForEdge(vert2Idx, vert3Idx));

        if(findE1Pair == edgesPerIndicies.end()){
            //if not found pair edge, then add this and it will be found later when its pair gets created
            edgesPerIndicies.emplace(getHashForEdge(vert1Idx, vert3Idx), edge1);
        } else {
            //pair edge was already inserted, find it and set both pair pointers
            CgHeEdge* e1Pair = findE1Pair->second;
            edge1->m_pair = e1Pair;
            e1Pair->m_pair = edge1;
            edgesPerIndicies.erase(findE1Pair); //not needed but probably better to not grow too much as this element will not be used more than once?
        }
        if(findE2Pair == edgesPerIndicies.end()){
            edgesPerIndicies.emplace(getHashForEdge(vert2Idx, vert1Idx), edge2);
        } else {
            CgHeEdge* e2Pair = findE2Pair->second;
            edge2->m_pair = e2Pair;
            e2Pair->m_pair = edge2;
            edgesPerIndicies.erase(findE2Pair);
        }
        if(findE3Pair == edgesPerIndicies.end()){
            edgesPerIndicies.emplace(getHashForEdge(vert3Idx, vert2Idx), edge3);
        } else {
            CgHeEdge* e3Pair = findE3Pair->second;
            edge3->m_pair = e3Pair;
            e3Pair->m_pair = edge3;
            edgesPerIndicies.erase(findE3Pair);
        }


        //calc normal
        face->m_normal = glm::normalize(
                         glm::cross(edge2->vert()->position() - edge1->vert()->position(),
                                   edge3->vert()->position() - edge1->vert()->position()));

        //put new edges into the list
        m_edges.push_back(edge1);
        m_edges.push_back(edge2);
        m_edges.push_back(edge3);
        m_faces.push_back(face);
    }

    //Set any valid edge to point from vertex
    for(int i=0; i<m_edges.size(); i++){
        CgHeEdge* e = (CgHeEdge*)m_edges[i];
        if(e->vert()->edge() == NULL)
            e->m_vert->m_edge = (CgHeEdge*)e->next();

    }

    std::cout<<"Created HE Data"<<std::endl;
    std::cout<<"Unique Vertices count: "<<currentUniqueVertexCount<<std::endl;

    calculateVertexNormals();
}

const glm::vec3 CgHalfEdgeTriangleMesh::getCenter() const
{
  glm::vec3 center(0.);
  for(unsigned int i=0;i<m_verts.size();i++)
    {
      center+=m_verts[i]->position();
    }
  center/=(double)m_verts.size();
  return center;
}

std::vector<CgHeVert*> CgHalfEdgeTriangleMesh::getOneRingNeighborhood(CgHeVert* vert){

    std::vector<CgHeVert*> neigh;

    CgHeEdge* initialEdge = (CgHeEdge*)vert->edge();
    CgHeEdge* currentEdge = initialEdge;

    neigh.push_back(currentEdge->m_vert);
    //add start vert and then after "L" the next neighbor, since after going over pair, it would not be added if pair is null
    do {
         neigh.push_back((CgHeVert*)currentEdge->next()->vert());
        currentEdge = (CgHeEdge*)currentEdge->next()->next()->pair();
    }
    while(currentEdge != nullptr && currentEdge != initialEdge);
    //opening found, go other way around
    if(currentEdge == nullptr){
        neigh.push_back((CgHeVert*)vert->edge()->next()->vert());
        currentEdge = (CgHeEdge*)initialEdge->pair();
        while(currentEdge != nullptr){
            currentEdge = (CgHeEdge*)currentEdge->next();
            neigh.push_back(currentEdge->m_vert);
            currentEdge = (CgHeEdge*)currentEdge->pair(); //pair can be null, so go pair, check if null ant then next
        }
    }


    return neigh;
}

std::unordered_set<CgHeVert*> CgHalfEdgeTriangleMesh::getTwoRingNeighborhood(CgHeVert* vert){

    std::vector<CgHeVert*> oneRing = getOneRingNeighborhood(vert);
    std::unordered_set<CgHeVert*> twoRingNeigh;

    for(auto& n : oneRing){
        auto onering = getOneRingNeighborhood(n);
        for(auto& o : onering)
            twoRingNeigh.insert(o);
    }
    return twoRingNeigh;

}

std::set<CgHeVert*> CgHalfEdgeTriangleMesh::getTwoRingNeighborhoodOrdered(CgHeVert* vert){

    std::vector<CgHeVert*> oneRing = getOneRingNeighborhood(vert);
    std::set<CgHeVert*> twoRingNeigh;

    for(auto& n : oneRing){
        auto onering = getOneRingNeighborhood(n);
        for(auto& o : onering)
            twoRingNeigh.insert(o);
    }
    return twoRingNeigh;

}

//update normals for vertices
void CgHalfEdgeTriangleMesh::calculateVertexNormals()
{

    for(auto& v : m_verts) {
        CgHeVert* vert = (CgHeVert*)v;
        glm::vec3 normalSum = {0,0,0};
        CgHeEdge* initialEdge = (CgHeEdge*)vert->edge();
        CgHeEdge* currentEdge = initialEdge;

        do {
            normalSum += currentEdge->face()->normal();
            currentEdge = (CgHeEdge*)currentEdge->next()->next()->pair();
            // nachbarn++;
        }
        while(currentEdge != nullptr && currentEdge != initialEdge);
        //opening found, go other way around
        if(currentEdge == nullptr){
            currentEdge = (CgHeEdge*)initialEdge->pair();
            while(currentEdge != nullptr){
                normalSum += currentEdge->face()->normal();
                currentEdge = (CgHeEdge*)currentEdge->next()->pair();
                //nachbarn++;
            }
        }
        vert->m_normal = glm::normalize(normalSum);
        //std::cout<<"vertex " << i << " hat " << nachbarn<<std::endl;
    }
}


void CgHalfEdgeTriangleMesh::calculateFaceNormals(){

    for(auto& face : m_faces){
        ((CgHeFace*)face)->m_normal = glm::normalize(
                    glm::cross(face->edge()->next()->next()->vert()->position() - face->edge()->next()->vert()->position(),
                               face->edge()->vert()->position() - face->edge()->next()->vert()->position()));
    }
}


void CgHalfEdgeTriangleMesh::loopSubdivide()
{

    std::cout<<"loop Subdivide called!"<<std::endl;

    std::cout<<"Currently: "<<std::endl;
    std::cout<<m_faces.size() << " Faces"<<std::endl;
    std::cout<<m_verts.size() << " Verts"<<std::endl;
    std::cout<<m_edges.size() << " Edges"<<std::endl;

    //To Know what have been split
    std::unordered_set<CgHeEdge*> edgesSplit;

    std::vector<CgHeFace*> newFaces;
    std::vector<CgHeEdge*> newEdges;
    std::vector<CgHeVert*> newVerts;

    //Calculated from old positons! (of old vertices)
    glm::vec3* newPositions = new glm::vec3[m_verts.size()];

    //cache Betas?
    for(int i=0; i<m_verts.size(); i++){

        CgHeEdge* start = (CgHeEdge*)m_verts[i]->edge();
        CgHeEdge* current = start;

        int n = 0;
        glm::vec3 posSum = {0,0,0};
        do {
            n++;
            posSum += current->vert()->position();
            current = (CgHeEdge*)current->next()->next()->pair();

        } while(current != start && current != nullptr);

        if(current == nullptr){ //Boundaries calculate differentely
            //Prev and next * 1/8 + itself * 3/4

            CgHeEdge* first = start;
            CgHeEdge* last = start;

            while(first->next()->next()->pair() != nullptr)
            {
                first = (CgHeEdge*)first->next()->next()->pair();
            }

            while(last->pair() != nullptr)
            {
                last = (CgHeEdge*)last->pair()->next();
            }

            newPositions[i] = m_verts[i]->position() * 0.75f +
                    (first->next()->vert()->position() +
                     last->vert()->position()) * 0.125f;

        } else {
            float beta = (1.0/n)*(0.625-pow(0.375+0.25*cos(2*M_PI/n), 2));

            newPositions[i] = m_verts[i]->position() * (1.0f-n*beta) + (posSum * beta);
        }
    }

    for(int i=0; i<m_edges.size(); i++){
        //split single edge
        CgHeEdge* newEdge = new CgHeEdge();
        newEdges.push_back(newEdge); //add to temp list
        newEdge->m_next = (CgHeEdge*)m_edges[i]->next();
        newEdge->m_face = (CgHeFace*)m_edges[i]->face();
        newEdge->m_pair = (CgHeEdge*)m_edges[i]->pair();
        newEdge->m_vert = (CgHeVert*)m_edges[i]->vert();
        ((CgHeEdge*)m_edges[i])->m_next = newEdge;

        //if pair of this edge was already split, then pair of this edge will be found in set and its vert can be used as vert of this
        //else new vert must be created and add old edge to list (for next one to look for it)
        auto splitted = edgesSplit.find((CgHeEdge*)m_edges[i]->pair());

        //no vertex created for this edge yet
        if(splitted == edgesSplit.end()){
            CgHeVert* v = new CgHeVert();
            newVerts.push_back(v);//Add new Vertex to list
            v->m_color = m_edges[i]->vert()->color();
            v->m_edge = newEdge;
            ((CgHeEdge*)m_edges[i])->m_vert = v;
            edgesSplit.emplace((CgHeEdge*)m_edges[i]);

        } else {
            //reuse already added vertex (by pair of this)
            ((CgHeEdge*)m_edges[i])->m_vert = ((CgHeEdge*)*splitted)->m_vert;
            //and set pairs correctly
            CgHeEdge* oldPair = (CgHeEdge*)m_edges[i]->pair();
            oldPair->m_pair = newEdge;
            ((CgHeEdge*)m_edges[i])->m_pair = (CgHeEdge*)oldPair->next();
            edgesSplit.erase(splitted); //erase so set do not grow too much //both edges have been split
        }
    }



    //SET POSITION FOR NEW VERTICES
    for(auto& v : newVerts){
        if(v->edge()->pair() != nullptr){
            //Valued 3/8
            CgHeVert* v1 = (CgHeVert*)v->edge()->vert();
            CgHeVert* v2 = (CgHeVert*)v->edge()->pair()->next()->vert() ;
            //valued 1/8
            CgHeVert* v3 = (CgHeVert*)v->edge()->next()->next()->vert();
            CgHeVert* v4 = (CgHeVert*)v->edge()->pair()->next()->next()->next()->vert();

            v->m_position = 0.375f * (v1->position() + v2->position())
                          + 0.125f * (v3->position() + v4->position());
        }
        else {
            CgHeVert* v1 = (CgHeVert*)v->edge()->vert();
            CgHeVert* v2 = (CgHeVert*)v->edge()->next()->next()->next()->next()->vert() ;
            v->m_position = 0.5f * (v1->position() + v2->position());
        }

    }

    //Parallel possible. Must create elements and use indicies for temp array in parallel loop. After that push all elements to old array

    //add 6 new hedges to the face
    //face has a ptr to edge, the old - now halved edge
    for(auto& f : m_faces){

        //current->next and current->next->next and new halfedge are edges edge where new triangle will be formed
        CgHeEdge* current = (CgHeEdge*)f->edge();


        for(int i=0; i<3; i++) {

            CgHeEdge* newInside = new CgHeEdge();
            CgHeEdge* newOutside = new CgHeEdge();
            newEdges.push_back(newInside); //add to temp list
            newEdges.push_back(newOutside); //add to temp list

            newInside->m_next = (CgHeEdge*)current->next();
            newOutside->m_next = (CgHeEdge*)current->next()->next()->next();
            newInside->m_vert = (CgHeVert*)current->vert();
            newOutside->m_vert = (CgHeVert*)current->next()->next()->vert();
            newOutside->m_face = (CgHeFace*)current->face();
            newInside->m_pair = newOutside;
            newOutside->m_pair = newInside;
            ((CgHeEdge*)current->next()->next())->m_next = newInside;
            current->m_next = newOutside;

            //set new Face inside new Triangle
            newInside->m_face = new CgHeFace();
            newFaces.push_back(newInside->m_face);
            ((CgHeEdge*)newInside->next())->m_face = newInside->m_face;
            ((CgHeEdge*)newInside->next()->next())->m_face = newInside->m_face;
            newInside->m_face->m_edge = newInside;

            //set new start point to create second and then third face
            current = (CgHeEdge*)current->next();
            //with that, "old" face should be in the middle, surrounded by 3 new faces in the "old" triangle
        } //3 new triangles
        ((CgHeFace*)f)->m_edge = current;
    } //f in m_faces


    for(int i=0; i<m_verts.size(); i++){
        ((CgHeVert*)m_verts[i])->m_position = newPositions[i];
    }

    delete[] newPositions;
    //Add new Data to old containers
    m_faces.insert(m_faces.end(), newFaces.begin(), newFaces.end());
    m_edges.insert(m_edges.end(), newEdges.begin(), newEdges.end());
    m_verts.insert(m_verts.end(), newVerts.begin(), newVerts.end());


    calculateVertexNormals();
    calculateFaceNormals();
    std::cout<<"Split Done"<<std::endl;
    std::cout<<m_faces.size() << " Faces"<<std::endl;
    std::cout<<m_verts.size() << " Verts"<<std::endl;
    std::cout<<m_edges.size() << " Edges"<<std::endl;
}













