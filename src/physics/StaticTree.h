#pragma once
#include <unordered_map>
#include <vector>
#include <array>
#include "BoundingBox.h"

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

    static inline void ProcessStaticTree()
    {
        
    }

    template <typename T>
    class StaticTree
    {
    public:
        std::array<Node> mNodes;
        explicit StaticTree(size_t size): mNodes(std::array<Node, size>()){}
    private:
        // Queried to get object
		std::unordered_map<size_t, T> nodeToObjectMap;
		// Queried to get node
		std::unordered_map<T, size_t> objectToNodeMap;
    };
}