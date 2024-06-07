#include "cgkdtree.h"
#include <algorithm>
#include <stack>
#include <iostream>
#include <limits>
#include <stack>
#include <tuple>


CgKdTree::CgKdTree(const std::vector<glm::vec3> &points) {
    // Vektor von Paaren (Index, Punkt) erstellen, um die Indizes zu verfolgen
    std::vector<std::pair<int, glm::vec3>> indexedPoints;
    for (int i = 0; i < points.size(); ++i) {
        // Neues Paar (Index, Punkt) am Ende des Vektors erzeugen
        indexedPoints.emplace_back(i, points[i]);
    }

    // KD-Baum aus den indizierten Punkten erstellen und root setzen
    root = buildKdTree(indexedPoints.begin(), indexedPoints.end(), 0, nullptr);
}


CgKdTree::Node *CgKdTree::buildKdTree(std::vector<std::pair<int, glm::vec3>>::iterator begin,
                                      std::vector<std::pair<int, glm::vec3>>::iterator end, int depth,
                                      CgKdTree::Node *parent) {
    // Wenn der Bereich leer ist, gibt es keinen Knoten zu erstellen, also null zurückgeben
    if (begin == end) {
        return nullptr;
    }

    int k = 3;  // Anzahl der Dimensionen (x, y, z)
    int axis = depth % k;  // Bestimmen der Achse für die aktuelle Tiefe

    // std::nth_element verwenden, um das Median-Element im aktuellen Bereich in-place zu finden
    auto mid = begin + (end - begin) / 2;
    std::nth_element(begin, mid, end, [axis](const std::pair<int, glm::vec3> &a, const std::pair<int, glm::vec3> &b) {
        if (axis == 0) return a.second.x < b.second.x;  // Vergleichen nach der x-Achse
        else if (axis == 1) return a.second.y < b.second.y;  // Vergleichen nach der y-Achse
        else return a.second.z < b.second.z;  // Vergleichen nach der z-Achse
    });

    // Neuen Knoten mit dem Index und der Position des Median-Elements erstellen
    Node *node = new Node(mid->first, mid->second);
    node->parent = parent;  // Den Elternelement setzen

    // Rekursiv die linken und rechten Unterbäume erstellen
    node->left = buildKdTree(begin, mid, depth + 1, node);
    node->right = buildKdTree(mid + 1, end, depth + 1, node);

    return node;  // Den aktuellen Knoten zurückgeben
}


CgKdTree::~CgKdTree() {
    for (int i = 0; i < nodes.size(); i++)
        delete nodes[i];
}


CgKdTree::Node *CgKdTree::findNearestNodeByPoint(glm::vec3 pt) {
    // Start bei der Wurzel des kd-tree
    Node *current = root;
    Node *lastCurrent;
    int depth = 0;

    // Durch den Baum traversieren, bis ein Blatt erreicht wird
    while (current != nullptr) {
        lastCurrent = current; // Speichert den letzten besuchten Knoten
        if (current->position == pt) {
            // Wenn der aktuelle Knoten die gleiche Position wie der gesuchte Punkt hat, wird er zurückgegeben
            return current;
        }

        // Je nach Tiefe abwechselnd nach x, y oder z sortiert weitergehen
        if (depth % 3 == 0) {
            // Vergleich anhand der x-Koordinate
            if (pt.x < current->position.x) {
                current = current->left;
            } else {
                current = current->right;
            }
        } else if (depth % 3 == 1) {
            // Vergleich anhand der y-Koordinate
            if (pt.y < current->position.y) {
                current = current->left;
            } else {
                current = current->right;
            }
        } else {
            // Vergleich anhand der z-Koordinate
            if (pt.z < current->position.z) {
                current = current->left;
            } else {
                current = current->right;
            }
        }

        // Tiefe erhöhen, um zur nächsten Dimension überzugehen
        depth++;
    }

    // Wenn der genaue Punkt nicht gefunden wird, wird der zuletzt besuchte Knoten zurückgegeben
    std::cout << "Tiefe erreicht: " << depth << " und nicht gefunden, nächster Punkt wird zurückgegeben" << std::endl;
    return lastCurrent;
}


void CgKdTree::findNextKNearest(std::vector<std::pair<float, Node *>> &foundNodes, int depth, glm::vec3 refPt,
                                Node *current) {

    Node *firstTry = nullptr;
    Node *nextTry = nullptr;

    float distCurrRef = glm::distance(refPt, current->position);

    //this node is better than ones in list
    if (foundNodes.back().first > distCurrRef) {

        foundNodes.push_back({distCurrRef, current});
        std::sort(foundNodes.begin(), foundNodes.end());
        foundNodes.pop_back();

    }
    if (depth % 3 == 0) {
        if (refPt.x < current->position.x) {
            firstTry = current->left;
            nextTry = current->right;
        } else {

            firstTry = current->right;
            nextTry = current->left;
        }
    } else if (depth % 3 == 1) {
        if (refPt.y < current->position.y) {
            firstTry = current->left;
            nextTry = current->right;
        } else {
            firstTry = current->right;
            nextTry = current->left;
        }
    } else {
        if (refPt.z < current->position.z) {
            firstTry = current->left;
            nextTry = current->right;
        } else {
            firstTry = current->right;
            nextTry = current->left;
        }
    }

    //this check always since position of x can be closer there (<)
    if (firstTry != nullptr)
        findNextKNearest(foundNodes, depth + 1, refPt, firstTry);

    //check distance after this is back. since call with first try it could have changed
    float currentMaxDistance = foundNodes.back().first;

    //if right of this is possible to be better distance, then check it too, else leave nullptr to leave it out
    if ((depth % 3 == 0 && current->position.x - refPt.x < currentMaxDistance) ||
        (depth % 3 == 1 && current->position.y - refPt.y < currentMaxDistance) ||
        (depth % 3 == 2 && current->position.z - refPt.z < currentMaxDistance))

        if (nextTry != nullptr)
            findNextKNearest(foundNodes, depth + 1, refPt, nextTry);
}


std::vector<int> CgKdTree::getKNearestNeighbors(glm::vec3 point, int k) {

    std::vector<std::pair<float, Node *>> foundNodes;

    for (int i = 0; i < k; i++)
        foundNodes.push_back({std::numeric_limits<float>::max(), nullptr});

    findNextKNearest(foundNodes, 0, point, root);

    std::vector<int> returnIndicies;
    if (foundNodes[k - 1].second == nullptr)
        return returnIndicies;

    for (int i = 0; i < k; i++) {
        returnIndicies.push_back(foundNodes[i].second->index);
    }
    return returnIndicies;
}


void CgKdTree::getSplitPlane(glm::vec3 &min, glm::vec3 &max, int depth, Node *currentNode) {
    min = currentNode->position;
    max = currentNode->position;

    std::stack<Node *> stack;
    stack.push(currentNode);

    while (!stack.empty()) {
        Node *n = stack.top();
        stack.pop();
        min = glm::min(min, n->position);
        max = glm::max(max, n->position);
        if (n->left != nullptr)
            stack.push(n->left);
        if (n->right != nullptr)
            stack.push(n->right);
    }

    //project found cube into plane, if omitted, could change the drawing algorithmus to draw cubes
    if (depth % 3 == 0) {
        min.x = currentNode->position.x;
        max.x = currentNode->position.x;
    }
    if (depth % 3 == 1) {
        min.y = currentNode->position.y;
        max.y = currentNode->position.y;
    }
    if (depth % 3 == 2) {
        min.z = currentNode->position.z;
        max.z = currentNode->position.z;
    }
    return;
}

void CgKdTree::addSplitPlanes(int currDepth, Node *currNode, int maxDepth,
                              std::vector<std::tuple<glm::vec3, glm::vec3, int>> &planes) {
    //max depth = 1 so only 0th plane is made
    if (currDepth >= maxDepth || currNode == nullptr)
        return;

    glm::vec3 min, max;

    getSplitPlane(min, max, currDepth, currNode);
    planes.push_back({min, max, currDepth});

    addSplitPlanes(currDepth + 1, currNode->left, maxDepth, planes);
    addSplitPlanes(currDepth + 1, currNode->right, maxDepth, planes);

}


void CgKdTree::clampSplitPlane(glm::vec3 &min, glm::vec3 &max, int depth, Node *currentNode) {


    int currDepth = depth;
    while (currDepth > 0) {
        Node *currentParent = currentNode->parent;
        int parentDepth = currDepth - 1;
        bool wasLeft = currentParent->left == currentNode;

        if (depth - parentDepth % 3 != 0) { //skip planes that are parallel to this one
            if (parentDepth % 3 == 0) //parent was on X (yz plane)
            {
                if (wasLeft) //was smaller than
                {
                    max.x = fmin(currentParent->position.x, max.x);
                } else //was bigger equal than
                {
                    min.x = fmax(currentParent->position.x, min.x);
                }
            }
            if (parentDepth % 3 == 1) //parent was on X (yz plane)
            {
                if (wasLeft) //was smaller than
                {
                    max.y = fmin(currentParent->position.y, max.y);
                } else //was bigger equal than
                {
                    min.y = fmax(currentParent->position.y, min.y);
                }
            }
            if (parentDepth % 3 == 2) //parent was on X (yz plane)
            {
                if (wasLeft) //was smaller than
                {
                    max.z = fmin(currentParent->position.z, max.z);
                } else //was bigger equal than
                {
                    min.z = fmax(currentParent->position.z, min.z);
                }
            }
        }
        currentNode = currentParent;
        currDepth--;
    }
}


void CgKdTree::addSplitPlanesClamped(int currDepth, Node *currNode, int maxDepth, glm::vec3 bbmin, glm::vec3 bbmax,
                                     std::vector<std::tuple<glm::vec3, glm::vec3, int>> &planes) {
    //max depth = 1 so only 0th plane is made
    if (currDepth >= maxDepth || currNode == nullptr)
        return;

    glm::vec3 min, max;
    if (currDepth % 3 == 0) { //xsplit -> yz plane
        float x = currNode->position.x;
        min = glm::vec3(x, bbmin.y, bbmin.z);
        max = glm::vec3(x, bbmax.y, bbmax.z);
    } else if (currDepth % 3 == 1) { //ysplit -> xz plane
        float y = currNode->position.y;
        min = glm::vec3(bbmin.x, y, bbmin.z);
        max = glm::vec3(bbmax.x, y, bbmax.z);
    } else {
        float z = currNode->position.z;
        min = glm::vec3(bbmin.x, bbmin.y, z);
        max = glm::vec3(bbmax.x, bbmax.y, z);
    }

    clampSplitPlane(min, max, currDepth, currNode);
    planes.push_back({min, max, currDepth});

    addSplitPlanesClamped(currDepth + 1, currNode->left, maxDepth, bbmin, bbmax, planes);
    addSplitPlanesClamped(currDepth + 1, currNode->right, maxDepth, bbmin, bbmax, planes);

}


std::vector<std::tuple<glm::vec3, glm::vec3, int>> CgKdTree::getSplitPlanes(int depth) {
    std::vector<std::tuple<glm::vec3, glm::vec3, int>> planes;

    addSplitPlanes(0, root, depth, planes);
    return planes;
}