#pragma once
#define BINS_AMT 1000
#include <unordered_map>
#include <vector>
#include <array>
#include <algorithm>
#include "BoundingBox.h"
#include "../core/GlobalTypes.h"
#include <glm/gtx/string_cast.hpp>

// https://iss.oden.utexas.edu/Publications/Papers/RT2008.pdf
// https://jacco.ompf2.com/2022/04/21/how-to-build-a-bvh-part-3-quick-builds/

namespace Physics
{
	template <typename T>
	class StaticTree
	{
	private:
		struct BVHNode
		{
			BoundingBox box;
			size_t first;
			size_t triCount;
		};

		struct Triangle
		{
			glm::vec3 v1;
			glm::vec3 v2;
			glm::vec3 v3;
			glm::vec3 centroid;
		};

		struct Bin
		{
			BoundingBox bounds;
			size_t triCount;
		};

	public:
		std::vector<BVHNode> mNodes;

		explicit StaticTree() {}

		void CreateStaticTree(const std::vector<ModelPt>& vertices, const std::vector<unsigned int>& indices);

		std::vector<BoundingBox> GetBoxes();
		std::vector<BoundingBox> GetBoxes(const glm::mat4& modelMat);

	private:
		void Subdivide(size_t nodeIndex);

		float FindBestSplitPlane(size_t nodeIndex, uint8_t& axis, float& splitPos);

		Triangle& GetTriangle(const size_t nodeIndex);

		void UpdateNodeBoundingBox(size_t nodeIndex, bool useCentroid);
		void ClearData();

		size_t leafNodeAmount;

		size_t rootNodeIdx = 0;
		std::vector<size_t> mTriIdx;
		std::vector<Triangle> mTriangles;

		size_t mNodesUsed = 0;
	};

	template <typename T>
	void StaticTree<T>::CreateStaticTree(const std::vector<ModelPt>& vertices, const std::vector<unsigned int>& indices)
	{
		ClearData();

		leafNodeAmount = indices.size() / 3;

		// Includes leaf nodes and internal nodes
		mNodes.resize(leafNodeAmount * 2 + 1);

		// Transfer vertice and indice information into triangle vector
		mTriangles.resize(leafNodeAmount);
		mTriIdx.resize(leafNodeAmount);
		for (size_t i = 0; i < leafNodeAmount; i++)
		{
			mTriIdx[i] = i;

			auto& tri = mTriangles[i];

			tri.v1 = vertices[indices[i*3]].position;
			tri.v2 = vertices[indices[i*3+1]].position;
			tri.v3 = vertices[indices[i*3+2]].position;

			tri.centroid = (tri.v1 + tri.v2 + tri.v3) * 0.3333f;
		}

		BVHNode& root = mNodes[rootNodeIdx];
		root.first = 0;
		root.triCount = leafNodeAmount;
		mNodesUsed = 1;

		Subdivide(rootNodeIdx);
	}

	template <typename T>
	std::vector<BoundingBox> StaticTree<T>::GetBoxes() 
	{
		std::vector<BoundingBox> output;
		output.resize(mNodesUsed);
		for (size_t i = 0; i < mNodesUsed; ++i)
		{
			output.emplace_back(mNodes[i].box);
		}
		return output;
	}

	template <typename T>
	std::vector<BoundingBox> StaticTree<T>::GetBoxes(const glm::mat4& modelMat)
	{
		std::vector<BoundingBox> output;
		output.resize(mNodesUsed);
		for (size_t i = 0; i < mNodesUsed; ++i)
		{
			BoundingBox temp;
			temp.min = modelMat * glm::vec4(mNodes[i].box.min, 1.0);
			temp.max = modelMat * glm::vec4(mNodes[i].box.max,1.0);
			output.emplace_back(temp);
		}
		return output;
	}

	template <typename T>
	void StaticTree<T>::Subdivide(size_t nodeIndex)
	{
		BVHNode& node = mNodes[nodeIndex];
		if (node.triCount <= 1) return;

		uint8_t axis;
		float splitPos;

		// Find axis, split position, and split cost
		float splitCost = FindBestSplitPlane(nodeIndex, axis, splitPos);
		node.box.UpdateSurfaceArea();
		if (splitCost >= node.box.surfaceArea * static_cast<float>(node.triCount)) return;

		size_t beginIter = node.first;
		size_t endIter = node.first + (node.triCount - 1);
		while (beginIter <= endIter)
		{
			if (GetTriangle(beginIter).centroid[axis] < splitPos)
				++beginIter;
			else
				std::swap(mTriIdx[beginIter], mTriIdx[endIter--]);
		}
		
		// Stop split if one side is empty
		size_t leftCount = beginIter - node.first;
		if (leftCount == 0 || leftCount == node.triCount) return;

		// create child nodes
		size_t leftChildIdx = mNodesUsed++;
		size_t rightChildIdx = mNodesUsed++;

		mNodes[leftChildIdx].first = node.first;
		mNodes[leftChildIdx].triCount = leftCount;

		mNodes[rightChildIdx].first = beginIter;
		mNodes[rightChildIdx].triCount = node.triCount - leftCount;

		node.first = leftChildIdx;
		node.triCount = 0;

		// recurse
		Subdivide(leftChildIdx);
		Subdivide(rightChildIdx);
	}

	template <typename T>
	float StaticTree<T>::FindBestSplitPlane(size_t nodeIndex, uint8_t& axis, float& splitPos)
	{
		BVHNode& node = mNodes[nodeIndex];
		UpdateNodeBoundingBox(nodeIndex, true);

		float bestCost = FLT_MAX;
		for (uint8_t currentAxis = 0; currentAxis < 3; ++currentAxis)
		{
			if (node.box.max[currentAxis] - node.box.min[currentAxis] == 0.0f) continue;
			Bin bins[BINS_AMT] = {};

			float scale = static_cast<float>(BINS_AMT) / (node.box.max[currentAxis] - node.box.min[currentAxis]);

			// Update bins based on triangles in the node
			for (size_t i = node.first; i < node.first + node.triCount; ++i)
			{
				Triangle& triangle = GetTriangle(i);

				// Determine which bin based on triangle centroid position
				unsigned binIdx = std::min(static_cast<unsigned>(BINS_AMT - 1), static_cast<unsigned>((triangle.centroid[currentAxis] - node.box.min[currentAxis]) * scale));

				if (bins[binIdx].triCount == 0)
				{
					bins[binIdx].bounds.SetToLimit();
				}
				++bins[binIdx].triCount;

				// Expand bin bounding box based on triangle vertices
				bins[binIdx].bounds.IncludePoint(triangle.v1);
				bins[binIdx].bounds.IncludePoint(triangle.v2);
				bins[binIdx].bounds.IncludePoint(triangle.v3);
			}

			// Keeps track of each split plane candidate's bounding box area and triangle count
			float leftArea[BINS_AMT - 1], rightArea[BINS_AMT - 1];
			size_t leftCount[BINS_AMT - 1], rightCount[BINS_AMT - 1];
			size_t leftSum = 0, rightSum = 0;

			BoundingBox leftBox;
			BoundingBox rightBox;

			leftBox.SetToLimit();
			rightBox.SetToLimit();

			for (size_t i = 0; i < BINS_AMT - 1; ++i)
			{
				leftSum += bins[i].triCount;
				leftCount[i] = leftSum;
				leftBox.Merge(bins[i].bounds);
				leftArea[i] = leftBox.surfaceArea;

				rightSum += bins[BINS_AMT - i - 2].triCount;
				rightCount[BINS_AMT - i - 2] = rightSum;
				rightBox.Merge(bins[BINS_AMT - i - 2].bounds);
				rightArea[BINS_AMT - i - 2] = rightBox.surfaceArea;
			}

			// calculate SAH cost for each split plane candidate
			scale = (node.box.max[currentAxis] - node.box.min[currentAxis]) / BINS_AMT;
			for (int i = 0; i < BINS_AMT - 1; i++)
			{
				float planeCost = leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];

				if (planeCost < bestCost)
				{
					bestCost = planeCost;
					//std::cout << leftCount[i] << " " << rightCount[i] << std::endl;

					// Update parameters
					axis = currentAxis;
					splitPos = node.box.min[currentAxis] + scale * (static_cast<int>(i) + 1.0f);
				}
			}
		}
		return bestCost;
	}

	template <typename T>
	typename StaticTree<T>::Triangle& StaticTree<T>::GetTriangle(const size_t nodeIndex)
	{
		return mTriangles[mTriIdx[nodeIndex]];
	}

	template <typename T>
	void StaticTree<T>::UpdateNodeBoundingBox(size_t nodeIndex, const bool useCentroid)
	{
		BVHNode& node = mNodes[nodeIndex];
		node.box.SetToLimit();

		// Update bounds
		for (int t = node.first; t < node.first + node.triCount; ++t)
		{
			Triangle& tri = GetTriangle(t);
			if (useCentroid)
				node.box.IncludePoint(tri.centroid);
			else
			{
				node.box.IncludePoint(tri.v1);
				node.box.IncludePoint(tri.v2);
				node.box.IncludePoint(tri.v3);
			}
		}
	}

	template <typename T>
	void StaticTree<T>::ClearData()
	{
		mNodes.clear();
	}
}
