#ifndef CGLEASTSQUARES_H
#define CGLEASTSQUARES_H
#include <glm/glm.hpp>
#include <vector>
#include "CgSceneGraph/CgTriangleMesh.h"

#define MESH_VISUALISIERUNG_RESOLUTION 20

class CgLeastSquares
{
public:
    enum BaseFunction { biquadratic = 2, bicubic = 3};

    CgLeastSquares();

    /**
     * @brief calculateLeastSquares calculates least squares positions for given neighbors
     * @param point start point
     * @param normal plane normal (normal of point)
     * @param neighbors neighbors of "point"
     * @param function base function, either bisquared or bicubic
     * @return list of corrected points
     */
    static std::vector<glm::vec3> calculateLeastSquares(glm::vec3 point, glm::vec3 normal,
                                                std::vector<glm::vec3> neighbors, BaseFunction function);

    /**
     * @brief visualizeLeastSquares sets visualization mesh to mesh of predicted function
     * @param point start point
     * @param normal plane normal (normal of point)
     * @param neighbors neighbors of "point"
     * @param function base function, either bisquared or bicubic
     * @param visualization mesh to set calculated mesh to
     */
    static void visualizeLeastSquares(glm::vec3 point, glm::vec3 normal, std::vector<glm::vec3> neighbors,
                                               BaseFunction function, CgTriangleMesh** visualization);

};

#endif // CGLEASTSQUARES_H
