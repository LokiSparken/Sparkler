#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../includes/glm/glm.hpp"
#include "../includes/glm/gtc/matrix_transform.hpp"
#include "../includes/glm/gtc/type_ptr.hpp"

#include "../sources/shader.h"
#include "../sources/texture.h"

class Vertex
{
public:
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	Vertex() {}
	Vertex(glm::vec3 position_, glm::vec3 normal_, glm::vec2 texCoords_) : position(position_), normal(normal_), texCoords(texCoords_) {}
};

struct modelTexture
{
	unsigned int id;
	std::string type;
	aiString path;
};

class Texture
{
public:
	unsigned int id;
	std::string type;	// same to the uniform variable in fragment shader
	std::string path;

	Texture() {}
	Texture(unsigned int id_, std::string type_, std::string path_) : id(id_), type(type_), path(path_) {}
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<modelTexture> textures;

	Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<modelTexture> &textures, GLenum type);
	void draw(Shader shader);

private:
	unsigned int VAO, VBO, EBO;
	GLenum meshType;
	void setupMesh();
};

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<unsigned int> &indices, std::vector<modelTexture> &textures, GLenum type = GL_TRIANGLES)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	meshType= type;

	setupMesh();
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Mesh::draw(Shader shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string textureIndex;
		std::string textureType = textures[i].type;
		if (textureType == "textureDiffuse")
			textureIndex = std::to_string(diffuseNr++);
		else if (textureType == "textureSpecular")
			textureIndex = std::to_string(specularNr++);

		shader.setFloat(("material." + textureType + textureIndex).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(meshType, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

class Model
{
public:
	Model(const char* path);
	void setModelMatrix(glm::mat4 matrix);
	void draw(Shader shader);
	void draw(Shader shader, bool outlining, Shader shaderOutlining);
private:
	glm::mat4 modelMatrix;
	std::string directory;
	std::vector<Mesh> meshes;
	std::vector<modelTexture> texturesLoaded;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<modelTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

Model::Model(const char* path)
{
	loadModel(path);
}

void Model::setModelMatrix(glm::mat4 matrix)
{
	modelMatrix = matrix;
}

void Model::draw(Shader shader)
{
	shader.use();
	shader.setMat4("model", modelMatrix);
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
}

void Model::draw(Shader shader, bool outlining, Shader shaderOutlining)
{
	shader.use();
	shader.setMat4("model", modelMatrix);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilMask(0xFF);
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	shaderOutlining.use();
	shaderOutlining.setMat4("model", glm::scale(modelMatrix, glm::vec3(1.05f, 1.05f, 1.05f)));
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shaderOutlining);

	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<modelTexture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// get the position, normal, texCoords data
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	
	// get vertex indices on each face
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	
	// get textures
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<modelTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "textureDiffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<modelTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "textureSpecular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<modelTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "textureNormal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<modelTexture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "textureHeight");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<modelTexture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<modelTexture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// check whether the texture has been loaded
		bool skip = false;
		for (unsigned int j = 0; j < texturesLoaded.size(); j++)
		{
			if (std::strcmp(texturesLoaded[j].path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		// load new texture
		if (!skip)
		{
			modelTexture texture;
			texture.id = loadTexture(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}
	}
	return textures;
}

class Sphere
{
public:
	Sphere(float radius, unsigned int rings, unsigned int sectors);
	Sphere(float radius, unsigned int rings, unsigned int sectors, std::vector<std::string> texturesPath);
	void initSphere();
	void loadSphereMap(std::string path);
	void setModelMatrix(glm::mat4 matrix);
	void draw(Shader shader);
private:
	unsigned int VAO, VBO, EBO;
	unsigned int sphereMapTexture;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	std::vector<Texture> texturesLoaded;

	glm::mat4 modelMatrix;
	
	std::vector<Texture> loadSphereTextures(std::vector<std::string> texturesPath);
};

Sphere::Sphere(float radius, unsigned int rings, unsigned int sectors)
{
	float const R = 1.0 / (float)(rings - 1);
	float const S = 1.0 / (float)(sectors - 1);
	int r, s;

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			glm::vec3 position = glm::vec3(x * radius, y * radius, z * radius);
			glm::vec3 normal = glm::vec3(x, y, z);
			glm::vec2 texCoord = glm::vec2(s * S, r * R);
			vertices.push_back(Vertex(position, normal, texCoord));
		}
	}

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++)
		{
			unsigned int a = r * sectors + s;
			unsigned int b = r * sectors + (s + 1);
			unsigned int c = (r + 1) * sectors + (s + 1);
			unsigned int d = (r + 1) * sectors + s;
			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);
			indices.push_back(a);
			indices.push_back(c);
			indices.push_back(d);
		}
	}
}

Sphere::Sphere(float radius, unsigned int rings, unsigned int sectors, std::vector<std::string> texturesPath)
{
	float const R = 1.0 / (float)(rings - 1);
	float const S = 1.0 / (float)(sectors - 1);
	int r, s;

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++) 
		{
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			glm::vec3 position = glm::vec3(x * radius, y * radius, z * radius);
			glm::vec3 normal = glm::vec3(x, y, z);
			glm::vec2 texCoord = glm::vec2(s * S, r * R);
			vertices.push_back(Vertex(position, normal, texCoord));
		}
	}

	for (r = 0; r < rings; r++)
	{
		for (s = 0; s < sectors; s++) 
		{
			unsigned int a = r * sectors + s;
			unsigned int b = r * sectors + (s + 1);
			unsigned int c = (r + 1) * sectors + (s + 1);
			unsigned int d = (r + 1) * sectors + s;
			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);
			indices.push_back(a);
			indices.push_back(c);
			indices.push_back(d);
		}
	}

	sphereMapTexture = 0;
	textures = loadSphereTextures(texturesPath);
}

void Sphere::initSphere()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

void Sphere::loadSphereMap(std::string path)
{
	sphereMapTexture = loadTexture(path.c_str(), false);
}

void Sphere::setModelMatrix(glm::mat4 matrix)
{
	modelMatrix = matrix;
}

std::vector<Texture> Sphere::loadSphereTextures(std::vector<std::string> texturesPath)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < texturesPath.size(); i++)
	{
		std::string str = texturesPath[i];

		// check whether the texture has been loaded
		bool skip = false;
		for (unsigned int j = 0; j < texturesLoaded.size(); j++)
		{
			if (std::strcmp(texturesLoaded[j].path.c_str(), str.c_str()) == 0)
			{
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		// load new texture
		if (!skip)
		{
			Texture texture;
			texture.id = loadTexture(str.c_str());
			texture.type = "skybox_sphereMap";
			texture.path = str.c_str();
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}
	}
	return textures;
}

void Sphere::draw(Shader shader)
{
	shader.use();
	shader.setMat4("model", modelMatrix);

	if (sphereMapTexture != 0)
	{
		//std::cout << sphereMapTexture << std::endl;
		glActiveTexture(GL_TEXTURE0);
		shader.setInt("skybox_sphereMap", 0);
		glBindTexture(GL_TEXTURE_2D, sphereMapTexture);
	}

	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string textureIndex;
		std::string textureType = textures[i].type;
		if (textureType == "textureDiffuse")
			textureIndex = std::to_string(diffuseNr++);
		else if (textureType == "textureSpecular")
			textureIndex = std::to_string(specularNr++);

		shader.setFloat(("material." + textureType + textureIndex).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	//std::cout << "vertices size: " << vertices.size() << std::endl;
	//std::cout << "indices size: " << indices.size() << std::endl;

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

#endif
