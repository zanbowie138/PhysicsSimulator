#pragma once
#include <unordered_map>
#include "BoundingBox.h"

namespace Physics {
    template <typename T>
    class StaticTree
    {
    public:
        explicit StaticTree();
    private:
        // Queried to get object
		std::unordered_map<size_t, T> nodeToObjectMap;
		// Queried to get node
		std::unordered_map<T, size_t> objectToNodeMap;

		// Stack of free nodes
		std::stack<size_t> mFreeList;
    };
}