#include "CgLeastSquares.h"
//#include "CgMath/CgEigenDecomposition3x3.h"
//#include "glm/gtx/string_cast.hpp"
//#include "glm/gtx/norm.hpp"
#include <CgMath/Eigen/SVD>
#include <CgMath/Eigen/Core>
#include <iostream>
#include <CgMath/Eigen/Dense>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <string>

using namespace Eigen;

CgLeastSquares::CgLeastSquares()
{

}




/**
 * @brief calculateSingularValueDecompositionAPlus copied and changed accordingly from CgSceneControl
 * Calculates the A+ Matrix for the least squares calculation
 * @param A
 * @return A+ Matrix
 */
Eigen::MatrixXd calculateSingularValueDecompositionAPlus(Eigen::MatrixXd A)
{
    //compute decomposition
    JacobiSVD<Eigen::MatrixXd> svd(A,ComputeThinU|ComputeThinV);
    Eigen::MatrixXd U=svd.matrixU();
    Eigen::MatrixXd V=svd.matrixV();
    Eigen::VectorXd SV=svd.singularValues();

    //build a diagonal matrix out of the singular values
    Eigen::MatrixXd S(SV.size(), SV.size());
    S.setZero();
    for(int i=0; i<SV.size(); i++)
        S(i,i) = SV(i);


    // compute Moore-Penrose inverse now
    Eigen::MatrixXd Smin1(S.rows(), S.rows());
    Smin1.setZero();
    for(int i=0; i<S.rows(); i++){
        if(S(i,i) > 0.00000000001) //else 0
            Smin1(i,i) = 1.0 / S(i,i); // reciprocal calculation
    }

    Eigen::MatrixXd Aplus = V*Smin1*U.transpose();
    return Aplus;
}



/**
 * @brief parametrizePoints calculates new coordinate system for points to project them on the plane
 * @param points to project onto plane
 * @param planeNormal
 * @param center point that lies on the plane, that will make new coordiante center
 * @param transform resulting transformation matrix to project points back to their intended position
 * @return projected and transformed points
 */
std::vector<glm::vec3> parametrizePoints(std::vector<glm::vec3> points, glm::vec3 planeNormal, glm::vec3 center, glm::mat3x3& transform)
{
    glm::vec3 uDir;
    glm::vec3 vDir;
    if(glm::dot(planeNormal, glm::vec3(0,0,1)) > 0.999){
        //do not do anything
        uDir = glm::vec3(1,0,0);
        vDir = glm::vec3(0,1,0);
    }
    else {
        uDir = glm::cross(planeNormal, glm::vec3(0,0,1));
        vDir = glm::cross(planeNormal, uDir);;
    }

    vDir = glm::normalize(vDir);
    uDir = glm::normalize(uDir);

    //u and v are perpendicular to planeNormal and lie on plane and are perpendiculat to eachother, thus building a base
    transform = glm::mat3x3(uDir.x, vDir.x, planeNormal.x, uDir.y, vDir.y, planeNormal.y, uDir.z , vDir.z, planeNormal.z);

    std::vector<glm::vec3> parametrized;
    for(int i=0; i<points.size(); i++){
        glm::vec3 centeredPt = points[i] - center;

        glm::vec3 projected;
        projected.x = glm::dot(centeredPt, uDir);
        projected.y = glm::dot(centeredPt, vDir);
        projected.z = glm::dot(centeredPt, planeNormal);
        parametrized.push_back(projected);
    }

    return parametrized;
}


void CgLeastSquares::visualizeLeastSquares(glm::vec3 point, glm::vec3 normal, std::vector<glm::vec3> neighbors,
                                           BaseFunction function, CgTriangleMesh** visualization){

    glm::mat3x3 transform;
    std::vector<glm::vec3> projectedPoints = parametrizePoints(neighbors, normal, point, transform);

    int matrixSize = 9;
    int n = 2;
    if(function == BaseFunction::bicubic){
        matrixSize = 16;
        n = 3;
    }
    Eigen::MatrixXd A(projectedPoints.size(), matrixSize);
    //Eigen::MatrixXd X(projectedPoints.size(),1);
    //Eigen::MatrixXd Y(projectedPoints.size(),1);
    Eigen::MatrixXd Z(projectedPoints.size(),1);

    for(int i=0; i<projectedPoints.size(); i++){
        //X(i,0) = projectedPoints[i].x;
        //Y(i,0) = projectedPoints[i].y;
        Z(i,0) = projectedPoints[i].z;

        for(int x=0; x<=n; x++){
            for(int y=0; y<=n;y++){
                A(i,y + x * n) = pow(projectedPoints[i].x, x) * pow(projectedPoints[i].y, y);
            }
        }
    }

    Eigen::MatrixXd Aplus = calculateSingularValueDecompositionAPlus(A);

    //Eigen::MatrixXd Cx = Aplus * X;
    //Eigen::MatrixXd Cy = Aplus * Y;
    Eigen::MatrixXd Cz = Aplus * Z;

    auto transformBack = glm::transpose(transform);


    float xmin=0, xmax=0, ymin=0, ymax=0;

    //BB
    for(int i=0; i<projectedPoints.size(); i++){
        xmin = fmin(xmin, projectedPoints[i].x);
        xmax = fmax(xmax, projectedPoints[i].x);
        ymin = fmin(ymin, projectedPoints[i].y);
        ymax = fmax(ymax, projectedPoints[i].y);
    }

    glm::vec3 positions[MESH_VISUALISIERUNG_RESOLUTION][MESH_VISUALISIERUNG_RESOLUTION];
    float uStep = (xmax - xmin) / MESH_VISUALISIERUNG_RESOLUTION;
    float vStep = (ymax - ymin) / MESH_VISUALISIERUNG_RESOLUTION;
    //for(float u = xmin; u<= xmax; u+=0.005f){
    float uEval = xmin;
    for(int u=0; u<MESH_VISUALISIERUNG_RESOLUTION; u++) {
        float vEval = ymin;
        //for(float v = ymin; v<= ymax; v+=0.005f){
        for(int v=0; v<MESH_VISUALISIERUNG_RESOLUTION; v++) {
            glm::vec3 ptToTransform(0,0,0);

            for(int x=0; x<=n; x++){
                for(int y=0; y<=n;y++){
                    ptToTransform += powf(uEval, (float)x) * powf(vEval, (float)y) * Cz(y + x * n); //Cz(y+x*n)
                    ptToTransform.x = uEval;
                    ptToTransform.y = vEval;
                }
            }
            vEval += vStep;
            auto transformed = (transformBack * ptToTransform) + point; //pttotransform
            positions[u][v] = transformed;
        }
        uEval += uStep;
    }

    std::vector<glm::vec3> vertexPositions;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec3> vertexColors;

    for(int x=0; x<MESH_VISUALISIERUNG_RESOLUTION-1; x++){
        for(int y=0; y<MESH_VISUALISIERUNG_RESOLUTION-1; y++){
            vertexPositions.push_back(positions[x][y]);
            vertexPositions.push_back(positions[x+1][y]);
            vertexPositions.push_back(positions[x][y+1]);

            vertexPositions.push_back(positions[x+1][y]);
            vertexPositions.push_back(positions[x+1][y+1]);
            vertexPositions.push_back(positions[x][y+1]);
            for(int i=0; i<6; i++){
                vertexColors.push_back(glm::vec3(0.7,0.7,0.7));
                vertexNormals.push_back(normal); //reuse same normal, do not calculate new ones
            }

        }
    }
    if(*visualization != nullptr)
        delete *visualization;
    *visualization =  new CgTriangleMesh(vertexPositions, vertexNormals, vertexColors);

}


std::vector<glm::vec3> CgLeastSquares::calculateLeastSquares(glm::vec3 point, glm::vec3 normal,
                                         std::vector<glm::vec3> neighbors, BaseFunction function)
{
    glm::mat3x3 transform;
    std::vector<glm::vec3> projectedPoints = parametrizePoints(neighbors, normal, point, transform);

    int matrixSize = 9;
    int n = 2;
    if(function == BaseFunction::bicubic){
        matrixSize = 16;
        n = 3;
    }
    Eigen::MatrixXd A(projectedPoints.size(), matrixSize);
    //Eigen::MatrixXd X(projectedPoints.size(),1);
    //Eigen::MatrixXd Y(projectedPoints.size(),1);
    Eigen::MatrixXd Z(projectedPoints.size(),1);

    for(int i=0; i<projectedPoints.size(); i++){
        //X(i,0) = projectedPoints[i].x;
        //Y(i,0) = projectedPoints[i].y;
        Z(i,0) = projectedPoints[i].z;

        for(int x=0; x<=n; x++){
            for(int y=0; y<=n;y++){
                A(i,y + x * n) = pow(projectedPoints[i].x, x) * pow(projectedPoints[i].y, y);
            }
        }
    }

    Eigen::MatrixXd Aplus = calculateSingularValueDecompositionAPlus(A);

    //Eigen::MatrixXd Cx = Aplus * X;
    //Eigen::MatrixXd Cy = Aplus * Y;
    Eigen::MatrixXd Cz = Aplus * Z;

    auto transformBack = glm::transpose(transform);

    std::vector<glm::vec3> smoothedList;
    for(int i=0;i<neighbors.size(); i++){
        glm::vec3 smoothed(projectedPoints[i].x, projectedPoints[i].y, 0);
        for(int x=0; x<=n; x++){
            for(int y=0; y<=n;y++){
                smoothed.z += (float)(powf(projectedPoints[i].x, (float)x) * powf(projectedPoints[i].y, (float)y) * Cz(y + x * n));
            }
        }
        smoothedList.push_back((transformBack * smoothed) + point);
    }
    return smoothedList;
}
