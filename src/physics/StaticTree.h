#pragma once
#define BINS_AMT 20
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

		std::vector<BoundingBox> GetBoxes() const;
		std::vector<BoundingBox> GetBoxes(glm::mat4 modelMat) const;

	private:
		void Subdivide(size_t nodeIndex);

		float FindBestSplitPlane(size_t nodeIndex, uint8_t& axis, float& splitPos);

		Triangle& GetTriangle(const size_t nodeIndex);

		void UpdateNodeBoundingBox(size_t nodeIndex);
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

			tri.v1 = vertices[indices[i]].position;
			tri.v2 = vertices[indices[i+1]].position;
			tri.v3 = vertices[indices[i+2]].position;

			tri.centroid = (tri.v1 + tri.v2 + tri.v3) * 0.3333f;
		}

		BVHNode& root = mNodes[rootNodeIdx];
		root.first = 0;
		root.triCount = leafNodeAmount;

		Subdivide(rootNodeIdx);
	}

	template <typename T>
	std::vector<BoundingBox> StaticTree<T>::GetBoxes() const
	{
		std::vector<BoundingBox> output;
		output.resize(mNodes.size());
		for (const auto& node : mNodes)
		{
			output.emplace_back(node.box);
		}
		return output;
	}

	template <typename T>
	std::vector<BoundingBox> StaticTree<T>::GetBoxes(glm::mat4 modelMat) const
	{
		std::vector<BoundingBox> output;
		output.resize(mNodes.size());
		for (const auto& node : mNodes)
		{
			BoundingBox temp;
			temp.min = modelMat * node.box.min;
			temp.max = modelMat * node.box.max;
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
		if (splitCost >= node.box.surfaceArea) return;

		size_t beginIter = node.first;
		size_t endIter = node.first + (node.triCount - 1);
		while (beginIter < endIter)
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

		UpdateNodeBoundingBox(leftChildIdx);
		UpdateNodeBoundingBox(rightChildIdx);

		// recurse
		Subdivide(leftChildIdx);
		Subdivide(rightChildIdx);
	}

	template <typename T>
	float StaticTree<T>::FindBestSplitPlane(size_t nodeIndex, uint8_t& axis, float& splitPos)
	{
		BVHNode& node = mNodes[nodeIndex];
		UpdateNodeBoundingBox(nodeIndex);

		float bestCost = 0;
		for (uint8_t currentAxis = 0; currentAxis < 3; ++currentAxis)
		{
			Bin bins[BINS_AMT];

			float scale = static_cast<float>(BINS_AMT) / (node.box.max[currentAxis] - node.box.min[currentAxis]);

			// Update bins based on triangles in the node
			for (size_t i = node.first; i < node.first + node.triCount; ++i)
			{
				Triangle& triangle = GetTriangle(i);

				// Determine which bin based on triangle centroid position
				unsigned binIdx = std::min(static_cast<unsigned>(BINS_AMT - 1), static_cast<unsigned>((triangle.centroid[currentAxis] - node.box.min[currentAxis]) * scale));

				++bins[binIdx].triCount;

				// Expand bin bounding box based on triangle vertices
				bins[binIdx].bounds.IncludePoint(triangle.v1);
				bins[binIdx].bounds.IncludePoint(triangle.v2);
				bins[binIdx].bounds.IncludePoint(triangle.v3);
			}

			// Keeps track of each bin's 
			float leftArea[BINS_AMT], rightArea[BINS_AMT];
			size_t leftCount[BINS_AMT], rightCount[BINS_AMT];
			size_t leftSum = 0, rightSum = 0;
			BoundingBox leftBox;
			BoundingBox rightBox;

			for (size_t i = 0; i < BINS_AMT; ++i)
			{
				leftSum += bins[i].triCount;
				leftCount[i] = leftSum;
				if (bins[i].triCount > 0)
				{
					leftBox.Merge(bins[i].bounds);
					leftArea[i] = leftBox.surfaceArea;
				}
				leftArea[i] = leftBox.surfaceArea;

				rightSum += bins[BINS_AMT - i - 1].triCount;
				rightCount[BINS_AMT - i - 1] = rightSum;
				if (bins[BINS_AMT - i - 1].triCount > 0)
				{
					rightBox.Merge(bins[BINS_AMT - i - 1].bounds);
				}
				rightArea[BINS_AMT - i - 1] = rightBox.surfaceArea;
			}

			// calculate SAH cost for the 7 planes
			scale = (node.box.max[currentAxis] - node.box.min[currentAxis]) / BINS_AMT;
			for (int i = 0; i < BINS_AMT; i++)
			{
				float planeCost = leftCount[i] * leftArea[i] + rightCount[i] * rightArea[i];

				if (planeCost < bestCost || (i == 0 && currentAxis == 0))
				{
					bestCost = planeCost;

					// Update parameters
					axis = currentAxis;
					splitPos = node.box.min[currentAxis] + scale * (i + 1);
				}
			}
		}
		return bestCost;
	}

	template <typename T>
	StaticTree<T>::Triangle& StaticTree<T>::GetTriangle(const size_t nodeIndex)
	{
		return mTriangles[mTriIdx[nodeIndex]];
	}

	template <typename T>
	void StaticTree<T>::UpdateNodeBoundingBox(size_t nodeIndex)
	{
		BVHNode& node = mNodes[nodeIndex];
		node.box.max = GetTriangle(node.first).centroid;
		node.box.min = node.box.max;

		// Update bounds
		for (int t = node.first; t < node.first + node.triCount; ++t)
			node.box.IncludePoint(GetTriangle(t).centroid);
	}

	template <typename T>
	void StaticTree<T>::ClearData()
	{
		mNodes.clear();
	}
}
