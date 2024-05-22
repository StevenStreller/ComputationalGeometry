#include "cgkdtree.h"
#include <algorithm>
#include <stack>
#include <iostream>
#include <limits>
#include <stack>

CgKdTree::CgKdTree(const std::vector<glm::vec3>& points)
{

    if(points.empty())
        return;

    //create tree

    //Nodes left have coord smaller than
    //right have greater equal

    for(int i = 0; i < points.size(); i++)
    {
        nodes.push_back(new Node(i, points[i]));
    }

    //The idea is to sort nodes vector (or parts of it by x then y then z then x... coordinates and always find mid element as next branch

    std::sort(nodes.begin(), nodes.end(), [](const Node* a, const Node* b) {
        return a->position.x < b->position.x;
    });

    // index of left of / right of and depth and parent
    std::stack<std::tuple<int, int, int, int>> midPts;

    //list start, list end, node num, node itself, depth

    //correct root - first of nodes with same coordinate must be taken as root to make
    //less than and more equal than correctly
    size_t rootIdx = nodes.size()/2;
    while(rootIdx > 0 && nodes[rootIdx]->position.x == nodes[rootIdx-1]->position.x ){
        rootIdx--;
    }

    root = nodes[rootIdx];
    root->parent = nullptr;

    //Stack is used to avoid recursion, nodes are reordered in place
    midPts.push({0, nodes.size(), 0, rootIdx});

    while(!midPts.empty())
    {
        auto top = midPts.top();
        midPts.pop();

        int listStart = std::get<0>(top);
        int listEnd = std::get<1>(top);
        int depth = std::get<2>(top);
        int nodeNumber = std::get<3>(top);
        Node* thisNode = nodes[nodeNumber];

        if(depth % 3 == 0)
        {
            //was divided by x, now must do by y
            std::sort(nodes.begin()+listStart, nodes.begin()+nodeNumber, [](const Node* a, const Node* b) {
                return a->position.y < b->position.y;
            });

            std::sort(nodes.begin()+nodeNumber+1, nodes.begin()+listEnd, [](const Node* a, const Node* b) {
                return a->position.y < b->position.y;
            });

            //if not leafNodes then continue on
            if(listStart != nodeNumber){
                int nodeIdxLeft = (listStart + nodeNumber)/2;
                while(nodeIdxLeft > listStart && nodes[nodeIdxLeft]->position.y == nodes[nodeIdxLeft-1]->position.y ){
                    nodeIdxLeft--;
                }
                thisNode->left = nodes[nodeIdxLeft];
                thisNode->left->parent = thisNode;
                midPts.push({listStart, nodeNumber, depth+1, nodeIdxLeft});
            }

            if(listEnd != nodeNumber+1){
                int nodeIdxRight = (listEnd + nodeNumber + 1)/2;
                while(nodeIdxRight > (nodeNumber+1) && nodes[nodeIdxRight]->position.y == nodes[nodeIdxRight-1]->position.y ){
                    nodeIdxRight--;
                }
                thisNode->right = nodes[nodeIdxRight];
                thisNode->right->parent = thisNode;
                midPts.push({nodeNumber+1, listEnd, depth+1, nodeIdxRight});
            }

        }
        else if(depth % 3 == 1)
        {
            //was divided by y, now must do by z
            std::sort(nodes.begin()+listStart, nodes.begin()+nodeNumber, [](const Node* a, const Node* b) {
                return a->position.z < b->position.z;
            });
            std::sort(nodes.begin()+nodeNumber+1, nodes.begin()+listEnd, [](const Node* a, const Node* b) {
                return a->position.z < b->position.z;
            });


            if(listStart != nodeNumber){
                int nodeIdxLeft = (listStart + nodeNumber)/2;
                while(nodeIdxLeft > listStart && nodes[nodeIdxLeft]->position.z == nodes[nodeIdxLeft-1]->position.z ){
                    nodeIdxLeft--;
                }
                thisNode->left = nodes[nodeIdxLeft];
                thisNode->left->parent = thisNode;
                midPts.push({listStart, nodeNumber, depth+1, nodeIdxLeft});
            }

            if(listEnd != nodeNumber+1){
                int nodeIdxRight = (listEnd + nodeNumber + 1)/2;
                while(nodeIdxRight > (nodeNumber+1) && nodes[nodeIdxRight]->position.z == nodes[nodeIdxRight-1]->position.z ){
                    nodeIdxRight--;
                }
                thisNode->right = nodes[nodeIdxRight];
                thisNode->right->parent = thisNode;
                midPts.push({nodeNumber+1, listEnd, depth+1, nodeIdxRight});
            }

        }
        else
        {
            //was divided by z, now must do by x
            std::sort(nodes.begin()+listStart, nodes.begin()+nodeNumber, [](const Node* a, const Node* b) {
                return a->position.x < b->position.x;
            });
            std::sort(nodes.begin()+nodeNumber+1, nodes.begin()+listEnd, [](const Node* a, const Node* b) {
                return a->position.x < b->position.x;
            });


            if(listStart != nodeNumber){
                int nodeIdxLeft = (listStart + nodeNumber)/2;
                while(nodeIdxLeft > listStart && nodes[nodeIdxLeft]->position.x == nodes[nodeIdxLeft-1]->position.x ){
                    nodeIdxLeft--;
                }
                thisNode->left = nodes[nodeIdxLeft];
                thisNode->left->parent = thisNode;
                midPts.push({listStart, nodeNumber, depth+1, nodeIdxLeft});
            }

            if(listEnd != nodeNumber+1){
                int nodeIdxRight = (listEnd + nodeNumber + 1)/2;
                while(nodeIdxRight > (nodeNumber+1) && nodes[nodeIdxRight]->position.x == nodes[nodeIdxRight-1]->position.x ){
                    nodeIdxRight--;
                }
                thisNode->right = nodes[nodeIdxRight];
                thisNode->right->parent = thisNode;
                midPts.push({nodeNumber+1, listEnd, depth+1, nodeIdxRight});
            }
        }
    }
}


CgKdTree::~CgKdTree()
{
    for(int i=0; i<nodes.size(); i++)
        delete nodes[i];
}



CgKdTree::Node* CgKdTree::findNearestNodeByPoint(glm::vec3 pt){

    Node* current = root;
    Node* lastCurrent;
    int depth = 0;
    while(current != nullptr)
    {
        lastCurrent = current;
        if(current->position == pt){
            return current;
        }
        if(depth%3 == 0)
        {
            if(pt.x < current->position.x){
                current = current->left;
            }
            else {
                current = current->right;
            }
        } else if(depth%3 == 1)
        {
            if(pt.y < current->position.y){
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }
        else
        {
            if(pt.z < current->position.z){
                current = current->left;
            }
            else
            {
                current = current->right;
            }
        }

        depth++;
    }
    std::cout<<"depth reached " << depth << " and not found, return closest point"<<std::endl;
    return lastCurrent;
}


void CgKdTree::findNextKNearest(std::vector<std::pair<float,Node*>>& foundNodes, int depth, glm::vec3 refPt, Node* current){

    Node* firstTry = nullptr;
    Node* nextTry = nullptr;

    float distCurrRef = glm::distance(refPt, current->position);

    //this node is better than ones in list
    if(foundNodes.back().first > distCurrRef){

        foundNodes.push_back({ distCurrRef, current });
        std::sort(foundNodes.begin(), foundNodes.end());
        foundNodes.pop_back();

    }
    if(depth%3 == 0)
    {
        if(refPt.x < current->position.x){
            firstTry = current->left;
            nextTry = current->right;
        }
        else {

            firstTry = current->right;
            nextTry = current->left;
        }
    } else if(depth%3 == 1)
    {
        if(refPt.y < current->position.y){
            firstTry = current->left;
            nextTry = current->right;
        }
        else
        {
            firstTry = current->right;
            nextTry = current->left;
        }
    }
    else
    {
        if(refPt.z < current->position.z){
            firstTry = current->left;
            nextTry = current->right;
        }
        else
        {
            firstTry = current->right;
            nextTry = current->left;
        }
    }

    //this check always since position of x can be closer there (<)
    if(firstTry != nullptr)
        findNextKNearest(foundNodes, depth+1, refPt, firstTry);

    //check distance after this is back. since call with first try it could have changed
    float currentMaxDistance = foundNodes.back().first;

    //if right of this is possible to be better distance, then check it too, else leave nullptr to leave it out
    if((depth % 3 == 0 && current->position.x - refPt.x < currentMaxDistance) ||
       (depth % 3 == 1 && current->position.y - refPt.y < currentMaxDistance) ||
       (depth % 3 == 2 && current->position.z - refPt.z < currentMaxDistance) )

        if(nextTry != nullptr)
            findNextKNearest(foundNodes, depth+1, refPt, nextTry);
}


std::vector<int> CgKdTree::getKNearestNeighbors(glm::vec3 point, int k) {

    std::vector<std::pair<float,Node*>> foundNodes;

    for(int i=0; i<k; i++)
        foundNodes.push_back({std::numeric_limits<float>::max(),nullptr});

    findNextKNearest(foundNodes, 0, point, root);

    std::vector<int> returnIndicies;
    if(foundNodes[k-1].second==nullptr)
        return returnIndicies;

    for(int i=0; i<k; i++){
        returnIndicies.push_back(foundNodes[i].second->index);
    }
    return returnIndicies;
}


void CgKdTree::getSplitPlane(glm::vec3& min, glm::vec3& max, int depth, Node* currentNode) {
    min = currentNode->position;
    max = currentNode->position;

    std::stack<Node*> stack;
    stack.push(currentNode);

    while(!stack.empty()){
        Node* n = stack.top();
        stack.pop();
        min = glm::min(min, n->position);
        max = glm::max(max, n->position);
        if(n->left != nullptr)
            stack.push(n->left);
        if(n->right != nullptr)
            stack.push(n->right);
    }

    //project found cube into plane, if omitted, could change the drawing algorithmus to draw cubes
    if(depth % 3 == 0){
        min.x = currentNode->position.x;
        max.x = currentNode->position.x;
    }
    if(depth % 3 == 1){
        min.y = currentNode->position.y;
        max.y = currentNode->position.y;
    }
    if(depth % 3 == 2){
        min.z = currentNode->position.z;
        max.z = currentNode->position.z;
    }
    return;
}

void CgKdTree::addSplitPlanes(int currDepth, Node* currNode, int maxDepth,
                    std::vector<std::tuple<glm::vec3, glm::vec3, int>>& planes)
{
    //max depth = 1 so only 0th plane is made
    if(currDepth >= maxDepth || currNode == nullptr)
        return;

    glm::vec3 min, max;

    getSplitPlane(min, max, currDepth, currNode);
    planes.push_back({min, max, currDepth});

    addSplitPlanes(currDepth+1, currNode->left , maxDepth, planes);
    addSplitPlanes(currDepth+1, currNode->right, maxDepth, planes);

}



void CgKdTree::clampSplitPlane(glm::vec3& min, glm::vec3& max, int depth, Node* currentNode){


    int currDepth = depth;
    while(currDepth > 0){
        Node* currentParent = currentNode->parent;
        int parentDepth = currDepth-1;
        bool wasLeft = currentParent->left == currentNode;

        if(depth - parentDepth % 3 != 0)
        { //skip planes that are parallel to this one
            if(parentDepth % 3 == 0) //parent was on X (yz plane)
            {
                if(wasLeft) //was smaller than
                {
                    max.x = fmin(currentParent->position.x, max.x);
                }
                else //was bigger equal than
                {
                    min.x = fmax(currentParent->position.x, min.x);
                }
            }
            if(parentDepth % 3 == 1) //parent was on X (yz plane)
            {
                if(wasLeft) //was smaller than
                {
                    max.y = fmin(currentParent->position.y, max.y);
                }
                else //was bigger equal than
                {
                    min.y = fmax(currentParent->position.y, min.y);
                }
            }
            if(parentDepth % 3 == 2) //parent was on X (yz plane)
            {
                if(wasLeft) //was smaller than
                {
                    max.z = fmin(currentParent->position.z, max.z);
                }
                else //was bigger equal than
                {
                    min.z = fmax(currentParent->position.z, min.z);
                }
            }
        }
        currentNode = currentParent;
        currDepth--;
    }
}



void CgKdTree::addSplitPlanesClamped(int currDepth, Node* currNode, int maxDepth, glm::vec3 bbmin, glm::vec3 bbmax,
                    std::vector<std::tuple<glm::vec3, glm::vec3, int>>& planes)
{
    //max depth = 1 so only 0th plane is made
    if(currDepth >= maxDepth || currNode == nullptr)
        return;

    glm::vec3 min, max;
    if(currDepth % 3 == 0){ //xsplit -> yz plane
        float x = currNode->position.x;
        min = glm::vec3(x, bbmin.y, bbmin.z);
        max = glm::vec3(x, bbmax.y, bbmax.z);
    } else if(currDepth % 3 == 1){ //ysplit -> xz plane
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

    addSplitPlanesClamped(currDepth+1, currNode->left , maxDepth, bbmin, bbmax, planes);
    addSplitPlanesClamped(currDepth+1, currNode->right, maxDepth, bbmin, bbmax, planes);

}



std::vector<std::tuple<glm::vec3, glm::vec3, int>> CgKdTree::getSplitPlanes(int depth)
{
    std::vector<std::tuple<glm::vec3, glm::vec3, int>> planes;

    addSplitPlanes(0, root, depth, planes);
    return planes;
}
