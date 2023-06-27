#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>
#include "../core/GlobalTypes.h"

namespace Utils
{
	static MeshData CubeData(const bool reverseNormals = false)
	{
		const std::vector<glm::vec3> boxVertices =
		{
			glm::vec3(-0.5f, -0.5f, 0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),
			glm::vec3(0.5f, -0.5f, 0.5f),
			glm::vec3(-0.5f, 0.5f, 0.5f),
			glm::vec3(-0.5f, 0.5f, -0.5f),
			glm::vec3(0.5f, 0.5f, -0.5f),
			glm::vec3(0.5f, 0.5f, 0.5f)
		};
		const std::vector<glm::vec3> boxNormals =
		{
			glm::vec3(0, -1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(1, 0, 0),
			glm::vec3(0, 0, -1),
			glm::vec3(-1, 0, 0),
			glm::vec3(0, 1, 0)
		};
		const std::vector<GLuint> boxIndices =
		{
			0, 1, 2, // Down
			0, 2, 3,
			0, 4, 7, // Front
			0, 7, 3,
			3, 7, 6, // Right
			3, 6, 2,
			2, 6, 5, // Back
			2, 5, 1,
			1, 5, 4, // Left
			1, 4, 0,
			4, 5, 6, // Top
			4, 6, 7
		};
		std::vector<MeshPt> points;
		std::vector<GLuint> indices;
		size_t count = 0;
		for (int i = 0; i < boxIndices.size(); i++)
		{
			points.push_back(MeshPt{boxVertices[boxIndices[i]], boxNormals[i / 6] * (reverseNormals ? 1.0f : -1.0f)});
			indices.push_back(i);
		}

		return MeshData{ points, indices };
	}

	static ModelData PlaneData()
	{
		std::vector<ModelPt> board_vertexes
		{
			ModelPt{glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 0.0)},
			ModelPt{glm::vec3(0.5, 0.0, -0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 0.0)},
			ModelPt{glm::vec3(0.5, 0.0, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(1.0, 1.0)},
			ModelPt{glm::vec3(-0.5, 0.0, 0.5), glm::vec3(0.0, 1.0, 0.0), glm::vec2(0.0, 1.0)}
		};
		std::vector<GLuint> board_indices
		{
			0, 1, 2,
			2, 3, 0
		};
		return ModelData{ board_vertexes, board_indices };
	}

	// Algorithm source: https://gist.github.com/Pikachuxxxx/5c4c490a7d7679824e0e18af42918efc
	static ModelData UVSphereData(uint8_t latitudes, uint8_t longitudes)
	{
#define RADIUS 1
#define PI 3.1415

        if (longitudes < 3)
            longitudes = 3;
        if (latitudes < 2)
            latitudes = 2;

        std::vector<ModelPt> vertexes;
        std::vector<unsigned int> indices;

		vertexes.reserve(static_cast<size_t>(latitudes + 1) * (longitudes + 1));
		// TODO: reserve indices

        const float lengthInv = 1.0f / RADIUS;    // normal

        const float deltaLatitude = PI / latitudes;
        const float deltaLongitude = 2 * PI / longitudes;

        // Compute all vertices first except normals
        for (int i = 0; i <= latitudes; ++i)
        {
            float latitudeAngle = PI / 2 - i * deltaLatitude; /* Starting -pi/2 to pi/2 */
            float xy = RADIUS * cosf(latitudeAngle);    /* r * cos(phi) */
            float z = RADIUS * sinf(latitudeAngle);     /* r * sin(phi )*/

            /*
             * We add (latitudes + 1) vertices per longitude because of equator,
             * the North pole and South pole are not counted here, as they overlap.
             * The first and last vertices have same position and normal, but
             * different tex coords.
             */
            for (int j = 0; j <= longitudes; ++j)
            {
                float longitudeAngle = j * deltaLongitude;

                ModelPt temp;

                temp.position.x = xy * cosf(longitudeAngle);       /* x = r * cos(phi) * cos(theta)  */
                temp.position.y = xy * sinf(longitudeAngle);       /* y = r * cos(phi) * sin(theta) */
                temp.position.z = z;                               /* z = r * sin(phi) */
                temp.uv.x = static_cast<float>(j / longitudes);    /* s */
                temp.uv.y = static_cast<float>(i / latitudes);     /* t */

                // normalized vertex normal
                temp.normal = glm::vec3(temp.position.x * lengthInv, temp.position.y * lengthInv, temp.position.z * lengthInv);

				vertexes.emplace_back(temp);
            }
        }

        /*
         *  Indices
         *  k1--k1+1
         *  |  / |
         *  | /  |
         *  k2--k2+1
         */
		for (int i = 0; i < latitudes; ++i)
		{
			unsigned int k1 = i * (longitudes + 1);
			unsigned int k2 = k1 + longitudes + 1;

			// 2 Triangles per latitude block excluding the first and last longitudes blocks
			for (int j = 0; j < longitudes; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != (latitudes - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}
		return ModelData{ vertexes, indices };
	}
}
