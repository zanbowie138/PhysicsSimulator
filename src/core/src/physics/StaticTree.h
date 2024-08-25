#pragma once
#define BINS_AMT 8
#define TRI_LIMIT 1
#define DEBUG 1

#include "BoundingBox.h"
#include "core/GlobalTypes.h"
#include "../utils/ThreadPool.h"

// Adapted from: https://github.com/jbikker/bvh_article/blob/main/quickbuild.cpp
// Full article explanation: https://jacco.ompf2.com/2022/04/21/how-to-build-a-bvh-part-3-quick-builds/
// TODO: Make all models load at the same time
namespace Physics
{
	class StaticTree
	{
		struct BVHNode
		{
			BoundingBox box;
			// Either means first triangle or left child, depending if it is a leaf or not
			size_t first{};
			// If higher than 0, node is a leaf node
			size_t triCount{};
		};

		struct Bin
		{
			BoundingBox bounds;
			size_t triCount{};
		};

		struct Triangle
		{
			glm::vec3 v1, v2, v3;
		};

		// Index positions of triangles, eventually sorted by centroids depending on node
		std::vector<size_t> mTriIdx;
		// Vector of all triangle centroids
		std::vector<glm::vec3> mCentroids;

		// Keeps index position of next free node
		size_t mNodesUsed = 0;
		// Prevents data racing
		// Using this instead of atomic var because left child idx and right child idx should be next to each other
		std::mutex nodesUsedMutex;

		// Triangle data
		std::vector<Triangle> mTriangles;

		// Thread pool for quick computation
		Utils::ThreadPool mThreadPool;
	public:
		std::vector<BVHNode> mNodes;

		StaticTree() = default;

		void CreateStaticTree(const std::vector<MeshPt>& vertices, const std::vector<unsigned>& indices);

		std::vector<BoundingBox> QueryTree(const StaticTree& other);
		std::vector<BoundingBox> QueryTree(const BoundingBox& box);

		std::vector<BoundingBox> GetBoxes(bool onlyLeaf = true) const;
		std::vector<BoundingBox> GetBoxes(const glm::mat4& modelMat, bool onlyLeaf = true) const;

	private:
		
		void Subdivide(size_t nodeIndex);

		float FindBestSplitPlane(size_t nodeIndex, uint8_t& axis, float& splitPos);

		glm::vec3 GetCentroid(size_t index) const;
		const Triangle& GetTriangle(size_t index) const;

		void UpdateNodeBoundingBox(size_t nodeIndex);
		void ClearData();

		bool IsLeaf(size_t nodeIndex) const;
		bool IsInternal(size_t nodeIndex) const;
	};
}
