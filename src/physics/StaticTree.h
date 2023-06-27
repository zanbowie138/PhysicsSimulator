#pragma once
#define BINS_AMT 8
#define TRI_LIMIT 1
#define DEBUG
#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
#include <thread>
#include <queue>

#include <glm/gtx/string_cast.hpp>

#include "BoundingBox.h"
#include "../core/GlobalTypes.h"
#include "../utils/Timer.h"
#include "../utils/ThreadPool.h"


// Adapted from: https://github.com/jbikker/bvh_article/blob/main/quickbuild.cpp
// Full article explanation: https://jacco.ompf2.com/2022/04/21/how-to-build-a-bvh-part-3-quick-builds/
// TODO: Make all models load at the same time
namespace Physics
{
	class StaticTree
	{
	private:
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
			glm::vec3 v1;
			glm::vec3 v2;
			glm::vec3 v3;
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

	
	void StaticTree::CreateStaticTree(const std::vector<MeshPt>& vertices, const std::vector<unsigned>& indices)
	{
		ClearData();

		size_t leafNodeAmount = indices.size() / 3;

#ifdef DEBUG
		//std::cout << "Leaf node amount " << leafNodeAmount << std::endl;
		Utils::Timer t("StaticTree");
#endif

		// Includes leaf nodes and internal nodes
		mNodes.resize(leafNodeAmount * 2 + 1);

		// Transfer vertice and indice information into triangle vector
		mCentroids.resize(leafNodeAmount);
		mTriIdx.resize(leafNodeAmount);
		mTriangles.resize(leafNodeAmount);
		for (size_t i = 0; i < leafNodeAmount; i++)
		{
			mTriIdx[i] = i;

			Triangle& tri = mTriangles[i];
			tri.v1 = vertices[indices[i * 3]].position;
			tri.v2 = vertices[indices[i * 3 + 1]].position;
			tri.v3 = vertices[indices[i * 3 + 2]].position;

			mCentroids[i] = (tri.v1 + tri.v2 + tri.v3) * 0.33333f;
		}

		BVHNode& root = mNodes[0];
		root.first = 0;
		root.triCount = leafNodeAmount;
		mNodesUsed = 1;

		mThreadPool.Start();
		mThreadPool.QueueJob([this] { Subdivide(0); });

		while (mThreadPool.Busy()){}
		mThreadPool.Clear();

#ifdef DEBUG
		std::cout << t.ToString().c_str() << std::endl;
		std::cout << "Nodes used: " << mNodesUsed << std::endl;
#endif
	}


	inline std::vector<BoundingBox> StaticTree::QueryTree(const StaticTree& other)
	{
		std::vector<BoundingBox> output;
		std::stack<std::pair<size_t, size_t>> stack;

		stack.emplace(0, 0);

		while (!stack.empty())
		{
			auto mine = stack.top().first;
			auto theirs = stack.top().second;
			stack.pop();

			if (mNodes[mine].box.IsColliding(other.mNodes[theirs].box))
			{
				if (IsInternal(mine) && other.IsInternal(theirs))
				{
					stack.emplace(mNodes[mine].first, other.mNodes[theirs].first);
					stack.emplace(mNodes[mine].first + 1, other.mNodes[theirs].first);
					stack.emplace(mNodes[mine].first, other.mNodes[theirs].first + 1);
					stack.emplace(mNodes[mine].first + 1, other.mNodes[theirs].first + 1);
				}
				else if (IsLeaf(mine))
				{
					stack.emplace(mine, other.mNodes[theirs].first);
					stack.emplace(mine, other.mNodes[theirs].first + 1);
				}
				else if (other.IsLeaf(theirs))
				{
					stack.emplace(mNodes[mine].first, theirs);
					stack.emplace(mNodes[mine].first + 1, theirs);
				}
				else
				{
					output.emplace_back(mNodes[mine].box);
					output.emplace_back(other.mNodes[theirs].box);
				}
			}
		}
		return output;
	}


	inline std::vector<BoundingBox> StaticTree::QueryTree(const BoundingBox& box)
	{
		std::vector<BoundingBox> output;
		std::stack<size_t> stack;

		stack.emplace(0);

		while (!stack.empty())
		{
			size_t nodeID = stack.top();
			stack.pop();

			if (box.IsColliding(mNodes[nodeID].box))
			{
				if (IsLeaf(nodeID))
				{
					output.emplace_back(mNodes[nodeID].box);
				}
				else
				{
					stack.emplace(mNodes[nodeID].first);
					stack.emplace(mNodes[nodeID].first + 1);
				}
			}
		}
		return output;
	}


	inline std::vector<BoundingBox> StaticTree::GetBoxes(const bool onlyLeaf) const
	{
		std::vector<BoundingBox> output;
		for (size_t i = 0; i < mNodesUsed - 1; ++i)
		{
			if (IsLeaf(i) || !onlyLeaf)
				output.emplace_back(mNodes[i].box);
		}
		return output;
	}


	inline std::vector<BoundingBox> StaticTree::GetBoxes(const glm::mat4& modelMat, const bool onlyLeaf) const
	{
		std::vector<BoundingBox> output;
		for (size_t i = 0; i < mNodesUsed; ++i)
		{
			if (IsLeaf(i) || !onlyLeaf)
				output.emplace_back(modelMat * glm::vec4(mNodes[i].box.min, 1.0), modelMat * glm::vec4(mNodes[i].box.max, 1.0));
		}
		std::cout << output.size() << std::endl;
		return output;
	}


	inline void StaticTree::Subdivide(size_t nodeIndex)
	{
		BVHNode& node = mNodes[nodeIndex];
		UpdateNodeBoundingBox(nodeIndex);

		if (node.triCount <= TRI_LIMIT) return;

		uint8_t axis = 4;
		float splitPos;

		// Find axis, split position, and split cost
		float splitCost = FindBestSplitPlane(nodeIndex, axis, splitPos);
		node.box.UpdateSurfaceArea();
		if (splitCost >= node.box.surfaceArea * static_cast<float>(node.triCount)) return;

		size_t beginIter = node.first;
		size_t endIter = node.first + (node.triCount - 1);
		while (beginIter <= endIter)
		{
			if (GetCentroid(beginIter)[axis] <= splitPos)
				++beginIter;
			else
				std::swap(mTriIdx[beginIter], mTriIdx[endIter--]);
		}

		// Stop split if one side is empty
		size_t leftCount = beginIter - node.first;
#ifdef DEBUG
		assert(leftCount != 0 && leftCount != node.triCount);
#endif
		// if (leftCount == 0 || leftCount == node.triCount) return;

		// create child nodes
		size_t leftChildIdx;
		size_t rightChildIdx;
		{
			// Ensures left child index and right child index are 1 apart
			// Also prevents data racing
			std::unique_lock<std::mutex> lock(nodesUsedMutex);
			leftChildIdx = mNodesUsed++;
			rightChildIdx = mNodesUsed++;
		}

		mNodes[leftChildIdx].first = node.first;
		mNodes[leftChildIdx].triCount = leftCount;

		mNodes[rightChildIdx].first = beginIter;
		mNodes[rightChildIdx].triCount = node.triCount - leftCount;

		node.first = leftChildIdx;
		node.triCount = 0;

		// recurse
		mThreadPool.QueueJob([this, leftChildIdx] { Subdivide(leftChildIdx); });
		mThreadPool.QueueJob([this, rightChildIdx] { Subdivide(rightChildIdx); });
	}


	inline float StaticTree::FindBestSplitPlane(size_t nodeIndex, uint8_t& axis, float& splitPos)
	{
		BVHNode& node = mNodes[nodeIndex];

		BoundingBox centroidBox;
		centroidBox.SetToLimit();

		// Have box include all centroids
		for (size_t t = node.first; t < node.first + node.triCount; ++t)
		{
			centroidBox.IncludePoint(GetCentroid(t));
		}

		float bestCost = FLT_MAX;
		for (uint8_t currentAxis = 0; currentAxis < 3; ++currentAxis)
		{
			Bin bins[BINS_AMT] = {};

			float scale = static_cast<float>(BINS_AMT) / (centroidBox.max[currentAxis] - centroidBox.min[currentAxis]);

			// Update bins based on triangles in the node
			for (size_t i = node.first; i < node.first + node.triCount; ++i)
			{
				// Determine which bin based on triangle centroid position
				unsigned binIdx = std::min(static_cast<unsigned>(BINS_AMT - 1),
				                           static_cast<unsigned>((GetCentroid(i)[currentAxis] - centroidBox.min[
					                           currentAxis]) * scale));

				++bins[binIdx].triCount;

				// Expand bin bounding box based on triangle vertices
				const Triangle& tri = GetTriangle(i);
				bins[binIdx].bounds.IncludePoint(tri.v1);
				bins[binIdx].bounds.IncludePoint(tri.v2);
				bins[binIdx].bounds.IncludePoint(tri.v3);
			}

			// Keeps track of each split plane candidate's bounding box area and triangle count
			float leftArea[BINS_AMT - 1], rightArea[BINS_AMT - 1];
			size_t leftCount[BINS_AMT - 1], rightCount[BINS_AMT - 1];
			size_t leftSum = 0, rightSum = 0;

			BoundingBox leftBox;
			BoundingBox rightBox;

			for (size_t i = 0; i < BINS_AMT - 1; ++i)
			{
				leftSum += bins[i].triCount;
				leftCount[i] = leftSum;
				if (bins[i].triCount > 0)
					leftBox.Merge(bins[i].bounds);
				leftArea[i] = leftBox.surfaceArea;


				rightSum += bins[BINS_AMT - i - 2].triCount;
				rightCount[BINS_AMT - i - 2] = rightSum;
				if (bins[BINS_AMT - i - 2].triCount > 0)
					rightBox.Merge(bins[BINS_AMT - i - 2].bounds);
				rightArea[BINS_AMT - i - 2] = rightBox.surfaceArea;
			}

			// calculate SAH cost for each split plane candidate
			scale = (centroidBox.max[currentAxis] - centroidBox.min[currentAxis]) / BINS_AMT;
			for (int i = 0; i < BINS_AMT - 1; i++)
			{
				const float planeCost = leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];

				if (planeCost < bestCost && leftCount[i] != 0 && rightCount[i] != 0)
				{
					bestCost = planeCost;

					// Update parameters
					axis = currentAxis;
					splitPos = centroidBox.min[currentAxis] + scale * (static_cast<float>(i) + 1.0f);
				}
			}
		}
		return bestCost;
	}


	inline glm::vec3 StaticTree::GetCentroid(const size_t index) const
	{
		return mCentroids[mTriIdx[index]];
	}

	inline const typename StaticTree::Triangle& StaticTree::GetTriangle(const size_t index) const
	{
		return mTriangles[mTriIdx[index]];
	}


	inline void StaticTree::UpdateNodeBoundingBox(size_t nodeIndex)
	{
		BVHNode& node = mNodes[nodeIndex];
		node.box.SetToLimit();

		// Update bounds
		for (int t = node.first; t < node.first + node.triCount; ++t)
		{
			const Triangle& tri = GetTriangle(t);
			node.box.IncludePoint(tri.v1);
			node.box.IncludePoint(tri.v2);
			node.box.IncludePoint(tri.v3);
		}
	}


	inline void StaticTree::ClearData()
	{
		mNodes.clear();
	}


	inline bool StaticTree::IsLeaf(size_t nodeIndex) const
	{
		return mNodes[nodeIndex].triCount > 0;
	}

	inline bool StaticTree::IsInternal(size_t nodeIndex) const
	{
		return !IsLeaf(nodeIndex);
	}
}
