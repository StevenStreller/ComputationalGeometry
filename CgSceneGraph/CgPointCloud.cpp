//Undefine to have all points loaded by loader (duplicates inside cause triangle structure)
#define ONLY_UNIQUE_POINTS

#include "CgPointCloud.h"
#include "CgBase/CgEnums.h"
#include "CgUtils/ObjLoader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>
#include <limits>
#include <glm/gtx/norm.hpp>
#ifdef ONLY_UNIQUE_POINTS
#include <unordered_set>
#include "glm/gtx/hash.hpp" //not used but needed!

#include "CgSceneGraph/CgLeastSquares.h"

#endif

#include <chrono>
#include <unordered_map>

CgPointCloud::CgPointCloud():
CgPointCloud::CgPointCloud(51)
{

}

CgPointCloud::CgPointCloud(int id):
m_type(Cg::PointCloud),
m_id(id)
{

    m_vertices.push_back(glm::vec3(0.0,0.0,0.0));
    m_vertex_normals.push_back(glm::vec3(0.0,0.0,1.0));
    m_vertex_colors.push_back(glm::vec3(0.0,0.0,1.0));

    calculateSplatOrientations();

}


CgPointCloud::~CgPointCloud()
{
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    m_splat_indices.clear();
    if(kdTree != nullptr)
        delete kdTree;
}


void CgPointCloud::calculateSplatOrientations()
{
  // calculate local coordinate system for splats (arbitrary orientation of ellipse in plane)
  // replace this if you have the real coordinate system, use up vector = y-Axis of your local coordinate system instead of getPerpendicularVector(...)

  m_splat_orientations.clear();
  m_splat_scaling.clear();
  m_splat_indices.clear();

  for(unsigned int i=0;i<m_vertices.size();i++)
    {
      glm::mat4 lookAt_matrix(glm::lookAt(glm::vec3(m_vertices[i]),glm::vec3(m_vertices[i]-m_vertex_normals[i]),getPerpendicularVector(m_vertex_normals[i])));
      m_splat_orientations.push_back(lookAt_matrix);
      m_splat_scaling.push_back(glm::vec2(0.02,0.005));

      // use all points for splatting by default
      m_splat_indices.push_back(i);
    }


}


void CgPointCloud::init( std::string filename, bool cheat_normals)
{
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    m_splat_orientations.clear();
    m_splat_scaling.clear();
    m_splat_indices.clear();

    // load obj File
    ObjLoader loader;
    loader.load(filename);
    loader.getPositionData(m_vertices);

    // do this for cheating with the normals
    // you need to replace this by a normal estimation algorithm
    if(cheat_normals)
      loader.getNormalData(m_vertex_normals);

    std::cout<<"normals: "<<m_vertex_normals.size()<<" verts "<< m_vertices.size()<<std::endl;


#ifdef ONLY_UNIQUE_POINTS
    //std::unordered_set<glm::vec3> uniquePts;

    std::unordered_map<glm::vec3, glm::vec3, std::hash<glm::vec3>> uniqueVertsNormals;

    //for(auto& p : m_vertices){
    for(int i=0; i<m_vertices.size(); i++){
        auto found = uniqueVertsNormals.find(m_vertices[i]);
        if(found == uniqueVertsNormals.end())
        {
            uniqueVertsNormals.emplace(m_vertices[i], m_vertex_normals[i]);
        }
        else
        {
            uniqueVertsNormals[m_vertices[i]] += m_vertex_normals[i];
        }

    }
    std::cout<<"m_verts has " << m_vertices.size() << " elements and unique Pts only "<<uniqueVertsNormals.size()<<std::endl;


    m_vertices.clear();
    m_vertex_normals.clear();
    for(auto& a : uniqueVertsNormals){
        m_vertices.push_back(a.first);
        m_vertex_normals.push_back(glm::normalize(a.second));
    }
#endif





    // calculate local coordinate system for splats (arbitrary orientation of ellipse in plane)
    // replace this if you have the real coordinate system, use up vector = y-Axis of your local coordinate system instead of getPerpendicularVector(...)

    calculateSplatOrientations();

    //add a standard color for each point if lighting turned off
    for(unsigned int i=0;i<m_vertices.size();i++)
    {
          m_vertex_colors.push_back(glm::vec3(0.0,1.0,0.0));
    }


    //test of getNeartestNeighbors(..) method
    // generates blue dots on the tail of the bunny



    kdTree = new CgKdTree(m_vertices);


    //for(int i=0; i<m_vertices.size(); i++){
    //    kdTree->findNearestNodeByPoint(m_vertices[i]);
    //}

    //markKNearest(0, 10);
}


 std::vector<int> CgPointCloud::getNearestNeighbors(int current_point,unsigned int k)
{

    glm::vec3 q= m_vertices[current_point];

    std::vector<std::pair<double,int>> distances;

    // very inefficient, just to show that it works for rendering colored neighborhood
    // use min heap for real purposes


    for(unsigned int i=0;i<m_vertices.size();i++)
    {
        double dist=glm::distance(m_vertices[i],q);

        distances.push_back(std::make_pair(dist,i));
    }

    std::sort(distances.begin(),distances.end());

    std::vector<int> erg;

    for(unsigned int i=0;i<k;i++)
    {
         erg.push_back(distances[i].second);
    }

    return erg;


}


// calculates an arbitrary verctor perpendicular to the given one
glm::vec3 CgPointCloud::getPerpendicularVector(glm::vec3 arg)
{
  if((arg[0]==0.0)&&(arg[1]==0.0))
    {
    if(arg[2]==0.0)
      return glm::vec3(0.);

    return glm::vec3(0.0,1.0,0.0);
    }
  return glm::normalize(glm::vec3(-arg[1],arg[0],0.0));
}




const glm::vec3 CgPointCloud::getCenter() const
{
  glm::vec3 center(0.);
  for(unsigned int i=0;i<m_vertices.size();i++)
    {
      center+=m_vertices[i];
    }
  center/=(double)m_vertices.size();
  return center;
}

const std::vector<glm::vec2>& CgPointCloud::getSplatScalings() const
{
  return m_splat_scaling;
}


void CgPointCloud::markKNearest(int kNearestIndex, int kNearestValue)
{
    std::cout<<"index "<< kNearestIndex << " k="<<kNearestValue<<std::endl;

    if(kNearestIndex >= m_vertices.size()) //index oob
        return;
    if(kNearestValue >= m_vertices.size())
        return;


    std::cout<<"Mark nearest "<< kNearestIndex<<std::endl;
    //reset colors
    for(unsigned int i=0;i<m_vertices.size();i++)
    {
        m_vertex_colors[i]=glm::vec3(0.0,1.0,0.0);
    }


    std::vector<int> neighborsKd = kdTree->getKNearestNeighbors(m_vertices[kNearestIndex], kNearestValue);

    for(int i=0;i<kNearestValue;i++)
    {
        m_vertex_colors[neighborsKd[i]]=glm::vec3(1.0,0.0,0.0);
    }
}

void CgPointCloud::smoothWithLeastSquares(int kValue, CgLeastSquares::BaseFunction baseFunction) {

    glm::vec3* positionsSums = new glm::vec3[m_vertices.size()];
    int* positionsCounts = new int[m_vertices.size()];
    for(int i=0;i<m_vertices.size(); i++)
    {
        positionsCounts[i] = 0;
        positionsSums[i] = glm::vec3(0,0,0);
    }
    int infiniteNodes = 0;
    for(int v=0; v<m_vertices.size(); v++){

        auto neighbours = kdTree->getKNearestNeighbors(m_vertices[v], kValue);
        if(neighbours.empty()){
            infiniteNodes++;
            continue; //infinite distances found, can not find nearest nodes
        }
        std::vector<glm::vec3> positions;
        for(auto& v : neighbours)
            positions.push_back(m_vertices[v]);

        auto smoothVerts = CgLeastSquares::calculateLeastSquares(m_vertices[v], m_vertex_normals[v],
                                             positions, baseFunction);

        for(int i=0; i<neighbours.size(); i++)
        {
            positionsSums[neighbours[i]] += smoothVerts[i];
            positionsCounts[neighbours[i]]++;
        }
    }
    for(int i=0; i<m_vertices.size(); i++)
        m_vertices[i] = positionsSums[i] / (float)positionsCounts[i];

    if(infiniteNodes)
        std::cout<<"Infinite nodes detected, probably tree is corrupt"<<std::endl;

    delete[] positionsSums;
    delete[] positionsCounts;

    delete kdTree;
    kdTree = new CgKdTree(m_vertices);
}



int CgPointCloud::pickNearestPoint(glm::vec3 rayStart, glm::vec3 rayEnd, int kVal, bool showSmoothMesh, CgTriangleMesh** mesh, CgLeastSquares::BaseFunction baseFunction)
{
    //Line(t) = rayStart + t * rayEnd-rayStart

    glm::vec3 rayDir = glm::normalize(rayEnd - rayStart);
    float currentMinDistance = std::numeric_limits<float>::max();
    int currentBest = 0;
    for(int i=0; i<m_vertices.size(); i++)
    {
        float t = glm::dot(rayDir, m_vertices[i]-rayStart);
        float distance = glm::length2(m_vertices[i] - (rayStart + t * rayDir));
        if(distance < currentMinDistance){
            currentMinDistance = distance;
            currentBest = i;
        }
    }

    //TODO Use KD to ray cast

    if(showSmoothMesh){

        std::vector<glm::vec3> neighbours;
        auto temp = kdTree->getKNearestNeighbors(m_vertices[currentBest], kVal);

        for(auto& nn :  temp){
            neighbours.push_back(m_vertices[nn]);
        }
        CgLeastSquares::visualizeLeastSquares(m_vertices[currentBest], m_vertex_normals[currentBest],
                                         neighbours, baseFunction, mesh);
    }

    return currentBest;
}

void addPlaneVertices(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& colors,
                      std::tuple<glm::vec3, glm::vec3, int> plane)
{
    glm::vec3 min = std::get<0>(plane);
    glm::vec3 max = std::get<1>(plane);
    int depth = std::get<2>(plane);

    //color intensity, drops with deeper layers
    float c = 1.0f - (depth/3) * 0.2f;

    if(depth % 3 == 0){ //x -> yz plane
        glm::vec3 ymaxzmin(max.x, max.y,min.z);
        glm::vec3 yminzmax(max.x, min.y,max.z);

        vertices.push_back(min);
        vertices.push_back(yminzmax);
        vertices.push_back(max);

        vertices.push_back(min);
        vertices.push_back(ymaxzmin);
        vertices.push_back(max);

        for(int i=0; i<6; i++){
            normals.push_back(glm::vec3(1,0,0));
            colors.push_back(glm::vec3(c,c,0.0));
        }
    }
    if(depth % 3 == 1){ //y -> xz plane
        glm::vec3 xmaxzmin(max.x, max.y,min.z);
        glm::vec3 xminzmax(min.x, max.y,max.z);

        vertices.push_back(min);
        vertices.push_back(xminzmax);
        vertices.push_back(max);

        vertices.push_back(min);
        vertices.push_back(xmaxzmin);
        vertices.push_back(max);

        for(int i=0; i<6; i++){
            normals.push_back(glm::vec3(0,1,0));
            colors.push_back(glm::vec3(0.0,c,c));
        }
    }
    if(depth % 3 == 2) {//z -> yx plane
        glm::vec3 xmaxymin(max.x, min.y,max.z);
        glm::vec3 xminymax(min.x, max.y,max.z);

        vertices.push_back(min);
        vertices.push_back(xminymax);
        vertices.push_back(max);

        vertices.push_back(min);
        vertices.push_back(xmaxymin);
        vertices.push_back(max);

        for(int i=0; i<6; i++){
            normals.push_back(glm::vec3(0,0,1));
            colors.push_back(glm::vec3(c,0.0,c));
        }
    }

}

CgTriangleMesh* CgPointCloud::drawKdTree(int depth)
{
    std::vector<std::tuple<glm::vec3, glm::vec3, int>> planes = kdTree->getSplitPlanes(depth);

    std::vector<glm::vec3> vertices, normals, colors;
    for(auto& plane : planes){
        addPlaneVertices(vertices, normals, colors, plane);
    }
    return new CgTriangleMesh(vertices, normals, colors);;
}

