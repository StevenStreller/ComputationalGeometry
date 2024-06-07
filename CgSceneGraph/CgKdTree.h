#ifndef CGKDTREE_H
#define CGKDTREE_H

#include <glm/glm.hpp>
#include <vector>

/**
 * @brief The CgKdTree class is used to organize point cloud into effecientely searchable tree.
 * Always split in x,y and z directions depending on the depth. Every Node is a point that splits remaining points into two sets
 * unless no further nodes can be found
 */
class CgKdTree
{
    /**
     * @brief The Node class single element of kd-tree,
     */
    struct Node {
        // index of node in point cloud
        int index;
        // position in 3d space of this node
        glm::vec3 position;
        // element that is less than in given coordinate
        Node* left;
        // element that is more/equal than in given coordinate
        Node* right;
        // parent element in tree of this node
        Node* parent;

        /**
         * @brief Node creates a Node element
         * @param idx - index in the positions list of point cloud
         * @param pos - position of this node
         */
        Node(int idx, glm::vec3 pos) {
            index = idx;
            position = pos;
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
    };



public:

    /**
     * @brief CgKdTree creates kd-Tree object
     * @param points - list of points to be used in building of the tree, nodes of tree will have same indicies as points in this list
     */
    CgKdTree(const std::vector<glm::vec3>& points);

    /**
     * @brief destructor - deletes all nodes before deleting of kd-tree
     */
    ~CgKdTree();

    ///
    /// \brief getKNearestNeighbors - scans kdTree looking for k nearest neighbors to a given point
    /// \param point - point to which nearest neighbors must be found
    /// \param k - point count to be found
    /// \return list of node indicies from the array that was used in building of kd tree
    ///
    std::vector<int> getKNearestNeighbors(glm::vec3 point, int k);
    ///
    /// \brief findNearestNodeByPoint - tries fo find node that is on given position, or at least the closest one to
    /// \param pt - point to look for
    /// \return Node* that is nearest to given point
    ///
    Node* findNearestNodeByPoint(glm::vec3 pt);

    /**
     * @brief getSplitPlanes returns split planes to visualize the tree. Every plane is a node in the tree
     * and stretches only as far as its children are
     * @param depth limit up to which level planes should be generated
     * @return List of split planes with a tuple of <bb-min, bb-max, plane level> where plane level means 0->x 1->y 2->z...
     */
    std::vector<std::tuple<glm::vec3, glm::vec3, int>> getSplitPlanes(int depth);

private:

    Node* buildKdTree(std::vector<std::pair<int, glm::vec3>>::iterator begin, std::vector<std::pair<int, glm::vec3>>::iterator end, int depth, Node* parent);

    /**
     * @brief nodes list of nodes used in the tree, order is not the same as with point cloud,
     * should be used only to delete all nodes in destructor
     */
    std::vector<Node*> nodes;

    /**
     * @brief root start of the kd tree
     */
    Node* root;

    /**
     * @brief findNextKNearest recursively checks the tree for next possible node in search of the k nearest nodes
     * @param foundNodes currently found nodes, sorted by distance ascending
     * @param depth current depth
     * @param referencePoint point to which found items should be closest to
     * @param current currently checked node
     */
    void findNextKNearest(std::vector<std::pair<float,Node*>>& foundNodes, int depth, glm::vec3 referencePoint, Node* current);

    /**
     * @brief addSplitPlanes adds further planes that are used to visualize kd-tree
     * @param currDepth current tree level
     * @param currNode current Node
     * @param maxDepth maximum depth allowed
     * @param planes planes that were gathered until now as tuple <bbmin, bbmax, depth>
     */
    void addSplitPlanes(int currDepth, Node* currNode, int maxDepth, std::vector<std::tuple<glm::vec3, glm::vec3, int>>& planes);

    /**
     * @brief getSplitPlane gets the split plane for goven node using its coordiantes and the coordinates of nodes below
     * @param min vector to write bounding box minimum to as return value
     * @param max vector to write bounding box maximum to as return value
     * @param depth current depth
     * @param currentNode
     */
    void getSplitPlane(glm::vec3& min, glm::vec3& max, int depth, Node* currentNode);

    /**
     * @brief clampSplitPlane no longer used
     * @param min
     * @param max
     * @param currDepth
     * @param currentNode
     */
    void clampSplitPlane(glm::vec3& min, glm::vec3& max, int currDepth, Node* currentNode);


    /**
     * @brief addSplitPlanesClamped no longer used, similar to getSplitPlane() method but only clamps to bounding box of the whole mesh
     * @param currDepth
     * @param currNode
     * @param maxDepth
     * @param bbmin
     * @param bbmax
     * @param planes
     */
    void addSplitPlanesClamped(int currDepth, Node* currNode, int maxDepth, glm::vec3 bbmin,
                               glm::vec3 bbmax, std::vector<std::tuple<glm::vec3, glm::vec3, int>>& planes);



};

#endif // CGKDTREE_H
