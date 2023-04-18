#include "Model.h"

Model::Model(const char* filename, bool is_stl)
{
    const char* localDir = "/Resources/Models/";
    if (!is_stl) 
    {
        // Reading packed file
        unsigned int vertex_len;
        unsigned int ind_len;

        std::ifstream is((std::filesystem::current_path().string() + localDir + filename).c_str(), std::ios::out | std::ios::binary);
        is.seekg(0);
        is.read((char*)&vertex_len, sizeof(unsigned int));
        is.read((char*)&ind_len, sizeof(unsigned int));

        glm::vec3 temp;
        ModelPt temp_pt;
        for (int i = 0; i < (int)vertex_len; i++)
        {
            is.read((char*)&temp, sizeof(glm::vec3));
            temp_pt.position = temp;
            is.read((char*)&temp, sizeof(glm::vec3));
            temp_pt.normal = temp;
            vertices.push_back(temp_pt);
        }

        GLuint temp_i;
        for (int i = 0; i < (int)ind_len; i++)
        {
            is.read((char*)&temp_i, sizeof(unsigned int));
            indices.push_back(temp_i);
        }
        is.close();
    }
    else 
    {
        STLmesh output = Model::readSTL((std::filesystem::current_path().string() + localDir + filename).c_str());
        vertices = output.vertices;
        indices = output.indices;
    }
    

    VAO.Bind();

    VBO VBO(vertices);
    EBO EBO(indices);

    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(ModelPt), (void*)0);
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(ModelPt), (void*)(3 * sizeof(float)));

    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Model::Draw(Shader& shader, Camera& camera)
{
    // Bind shader to be able to access uniforms
    shader.Activate();
    VAO.Bind();

    // Take care of the camera Matrix
    glUniform3f(shader.GetUniformLocation("camPos"), camera.position.x, camera.position.y, camera.position.z);
    camera.Matrix(shader, "camMatrix");

    // Draw the actual mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

STLmesh Model::readSTL(const char* filepath)
{
    // STL File format: https://people.sc.fsu.edu/~jburkardt/data/stlb/stlb.html

    // Vertice will have position and normal information
    std::set<std::tuple<glm::vec3, glm::vec3, unsigned int>, compareVec3> vertexes;
    std::vector<unsigned int> indices;

    std::string localDir = "";
    std::ifstream is((localDir + filepath), std::ios::binary);

    // First 80 chars is the header (title)
    // Next is the number of triangles, which is an unsigned integer (4 bytes or 4 chars).  Buffer index is offset by 80 chars
    is.seekg(80);
    unsigned int numTriangles;
    is.read((char*)&numTriangles, sizeof(unsigned int));

    glm::vec3 temp;
    unsigned int indice = 0;
    std::pair<std::set<std::tuple<glm::vec3, glm::vec3, unsigned int>>::iterator, bool> temp_pair;
    glm::vec3 temp_normal;

    for (int t = 0; t < (int)numTriangles; t++)
    {
        is.seekg(84 + 50 * t);

        // Normal vector
        is.read((char*)&temp_normal, sizeof(glm::vec3));

        // 3 Vertexes
        for (int i = 0; i < 3; i++)
        {
            is.read((char*)&temp, sizeof(glm::vec3));
            temp_pair = vertexes.insert(std::tuple<glm::vec3, glm::vec3, unsigned int>(temp, temp_normal, indice));

            // If inserted successfully, increment indice count and push a new indice value.  Else, find the copied vertex and push it's indice value.
            if (temp_pair.second)
            {
                indices.push_back(indice);
                indice++;
            }
            else
            {
                std::tuple<glm::vec3, glm::vec3, unsigned int> temp_vertex = (*(temp_pair.first));
                vertexes.erase(temp_vertex);

                indices.push_back(std::get<2>(temp_vertex));

                temp_vertex = std::make_tuple(std::get<0>(temp_vertex), std::get<1>(temp_vertex) + temp_normal, std::get<2>(temp_vertex));
                vertexes.insert(temp_vertex);
            }
        }
    }

    is.close();

    std::vector<std::pair<glm::vec3, glm::vec3>> vertexes_sorted(vertexes.size());

    for (const std::tuple<glm::vec3, glm::vec3, unsigned int>& t : vertexes)
    {
        vertexes_sorted[std::get<2>(t)] = std::pair<glm::vec3, glm::vec3>{std::get<0>(t),std::get<1>(t)};
    }

    std::vector<ModelPt> vertices_vector;

    for (const std::pair<glm::vec3, glm::vec3>& t : vertexes_sorted)
    {
        vertices_vector.push_back(ModelPt{ t.first, t.second });
    }

    return STLmesh{ vertices_vector, indices };
}
