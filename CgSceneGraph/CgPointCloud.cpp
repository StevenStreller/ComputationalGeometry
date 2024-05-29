#include "CgPointCloud.h"
#include "CgBase/CgEnums.h"
#include "CgUtils/ObjLoader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <functional>

CgPointCloud::Node::Node(const glm::vec3 &point)
        : location(point), leftChild(nullptr), rightChild(nullptr) {}

CgPointCloud::CgPointCloud() :
        CgPointCloud::CgPointCloud(51) {

}

CgPointCloud::CgPointCloud(int id) :
        m_type(Cg::PointCloud),
        m_id(id) {

    m_vertices.push_back(glm::vec3(0.0, 0.0, 0.0));
    m_vertex_normals.push_back(glm::vec3(0.0, 0.0, 1.0));
    m_vertex_colors.push_back(glm::vec3(0.0, 0.0, 1.0));

    calculateSplatOrientations();

}


CgPointCloud::~CgPointCloud() {
    m_vertices.clear();
    m_vertex_normals.clear();
    m_vertex_colors.clear();
    m_splat_indices.clear();
}


void CgPointCloud::calculateSplatOrientations() {
    // calculate local coordinate system for splats (arbitrary orientation of ellipse in plane)
    // replace this if you have the real coordinate system, use up vector = y-Axis of your local coordinate system instead of getPerpendicularVector(...)

    m_splat_orientations.clear();
    m_splat_scaling.clear();
    m_splat_indices.clear();

    for (unsigned int i = 0; i < m_vertices.size(); i++) {
        glm::mat4 lookAt_matrix(glm::lookAt(glm::vec3(m_vertices[i]), glm::vec3(m_vertices[i] - m_vertex_normals[i]),
                                            getPerpendicularVector(m_vertex_normals[i])));
        m_splat_orientations.push_back(lookAt_matrix);
        m_splat_scaling.push_back(glm::vec2(0.02, 0.005));

        // use all points for splatting by default
        m_splat_indices.push_back(i);
    }


}


void CgPointCloud::init(std::string filename, bool cheat_normals) {
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

    CgPointCloud::Node *root = kdtree(m_vertices);
    if (root) {
        std::cout << "Root node location: (" << root->location.x << ", " << root->location.y << ", " << root->location.z
                  << ")\n";

        Node *test = getNeighbor(glm::vec3(0, 1, 0), root);
        std::cout << "Nachbar node location: (" << test->location.x << ", " << test->location.y << ", "
                  << test->location.z
                  << ")\n";

    } else {
        std::cout << "The KD-tree is empty.\n";
    }


    // do this for cheating with the normals
    // you need to replace this by a normal estimation algorithm
    if (cheat_normals)
        loader.getNormalData(m_vertex_normals);


    // calculate local coordinate system for splats (arbitrary orientation of ellipse in plane)
    // replace this if you have the real coordinate system, use up vector = y-Axis of your local coordinate system instead of getPerpendicularVector(...)

    calculateSplatOrientations();

    //add a standard color for each point if lighting turned off
    for (unsigned int i = 0; i < m_vertices.size(); i++) {
        m_vertex_colors.push_back(glm::vec3(0.0, 1.0, 0.0));
    }


    //test of getNeartestNeighbors(..) method
    // generates blue dots on the tail of the bunny

    unsigned int k = 3;
    std::vector<int> neighbors = getNearestNeighbors(0, k);

    for (unsigned int i = 0; i < k; i++) {
        m_vertex_colors[neighbors[i]] = glm::vec3(0.0, 0.0, 1.0);
    }
}


std::vector<int> CgPointCloud::getNearestNeighbors(int current_point, unsigned int k) {

    glm::vec3 q = m_vertices[current_point];

    std::vector<std::pair<double, int>> distances;

    // very inefficient, just to show that it works for rendering colored neighborhood
    // use min heap for real purposes


    for (unsigned int i = 0; i < m_vertices.size(); i++) {
        double dist = glm::distance(m_vertices[i], q);

        distances.push_back(std::make_pair(dist, i));
    }

    std::sort(distances.begin(), distances.end());

    std::vector<int> erg;

    for (unsigned int i = 0; i < k; i++) {
        erg.push_back(distances[i].second);
    }

    return erg;
}


// calculates an arbitrary verctor perpendicular to the given one
glm::vec3 CgPointCloud::getPerpendicularVector(glm::vec3 arg) {
    if ((arg[0] == 0.0) && (arg[1] == 0.0)) {
        if (arg[2] == 0.0)
            return glm::vec3(0.);

        return glm::vec3(0.0, 1.0, 0.0);
    }
    return glm::normalize(glm::vec3(-arg[1], arg[0], 0.0));
}

// Implementierung der kdtree Methode
CgPointCloud::Node *CgPointCloud::kdtree(std::vector<glm::vec3> &pointList) {
    return buildKdTree(pointList.begin(), pointList.end(), 0);
}

// Implementierung der buildKdTree Methode
CgPointCloud::Node *
CgPointCloud::buildKdTree(std::vector<glm::vec3>::iterator begin, std::vector<glm::vec3>::iterator end, int depth) {
    if (begin == end) {
        return nullptr;
    }

    int k = 3;  // Anzahl der Dimensionen
    int axis = depth % k;

    // Verwenden Sie std::nth_element, um den Median in-place zu finden
    auto mid = begin + (end - begin) / 2;
    std::nth_element(begin, mid, end, [axis](const glm::vec3 &a, const glm::vec3 &b) {
        if (axis == 0) return a.x < b.x;
        else if (axis == 1) return a.y < b.y;
        else return a.z < b.z;
    });

    Node *node = new Node(*mid);

    // Erstellen Sie die linken und rechten Unterbäume rekursiv
    node->leftChild = buildKdTree(begin, mid, depth + 1);
    node->rightChild = buildKdTree(mid + 1, end, depth + 1);

    return node;
}


const glm::vec3 CgPointCloud::getCenter() const {
    glm::vec3 center(0.);
    for (unsigned int i = 0; i < m_vertices.size(); i++) {
        center += m_vertices[i];
    }
    center /= (double) m_vertices.size();
    return center;
}

const std::vector<glm::vec2> &CgPointCloud::getSplatScalings() const {
    return m_splat_scaling;
}

CgPointCloud::Node *CgPointCloud::getNeighbor(glm::vec3 vec3, Node *root) {
    // Initialisierung des nächsten Nachbarn und der minimalen Distanz
    Node *best = nullptr;
    double bestDist = std::numeric_limits<double>::max();

    // Hilfsfunktion zur rekursiven Suche im k-d-Baum
    std::function<void(Node *, int)> searchTree = [&](Node *node, int depth) {
        if (node == nullptr) {
            return;
        }

        // Berechne die Distanz vom aktuellen Knoten zum Zielpunkt
        double dist = calcDistance(&vec3, &node->location);
        if (dist < bestDist) {
            bestDist = dist;
            best = node;
        }

        // Bestimme die aktuelle Achse
        int axis = depth % 3;
        double diff = 0;

        if (axis == 0) {
            diff = vec3.x - node->location.x;
        } else if (axis == 1) {
            diff = vec3.y - node->location.y;
        } else if (axis == 2) {
            diff = vec3.z - node->location.z;
        }

        // Wähle den nächsten Teilbaum basierend auf dem Vergleich
        Node *nextNode = (diff < 0) ? node->leftChild : node->rightChild;
        Node *otherNode = (diff < 0) ? node->rightChild : node->leftChild;

        // Suche im nächsten Teilbaum
        searchTree(nextNode, depth + 1);

        // Falls der andere Teilbaum auch relevant sein könnte, suche auch dort
        if (fabs(diff) < bestDist) {
            searchTree(otherNode, depth + 1);
        }
    };

    // Starte die rekursive Suche vom Wurzelknoten
    searchTree(root, 0);

    return best;
}

double CgPointCloud::calcDistance(glm::vec3 *a, glm::vec3 *b) {
    return std::sqrt(
            std::pow(b->x - a->x, 2) +
            std::pow(b->y - a->y, 2) +
            std::pow(b->z - a->z, 2)
    );
}