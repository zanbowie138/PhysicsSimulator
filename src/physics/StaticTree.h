#pragma once
#include <unordered_map>
#include <vector>
#include <array>
#include "BoundingBox.h"
#include "../core/GlobalTypes.h"

// https://iss.oden.utexas.edu/Publications/Papers/RT2008.pdf

namespace Physics {

    constexpr size_t NULL_NODE = 0xffffffff;

	struct Node
	{
		BoundingBox box;

		size_t height;

		size_t parent;

		size_t left;
		size_t right;
	};

    struct TempNode
	{
		BoundingBox box;

		size_t height;

		Node* parent;

		Node* left;
		Node* right;
	};

    template <typename T>
    class StaticTree
    {
    public:
        std::vector<Node> mNodes;

        explicit StaticTree(){}

        size_t GetLeafNodeAmount();
        
        void CreateStaticTree(const std::vector<ModelPt>& vertices, const std::vector<size_t>& indices);
    private:
        // Queried to get object
		std::unordered_map<size_t, T> nodeToObjectMap;
		// Queried to get node
		std::unordered_map<T, size_t> objectToNodeMap;

        void ClearData();

        size_t leafNodeAmount;
    };

    template <typename T>
    inline void StaticTree<T>::CreateStaticTree(const std::vector<ModelPt> &vertices, const std::vector<size_t> &indices)
    {
        ClearData();
        
        leafNodeAmount = indices/3;

        // Includes leaf nodes and internal nodes
        mNodes.resize(leafNodeAmount * 2 + 1);

        
    }
    template <typename T>
    inline void StaticTree<T>::ClearData()
    {
        mNodes.clear();
        nodeToObjectMap.clear();
        objectToNodeMap.clear();
    }
}