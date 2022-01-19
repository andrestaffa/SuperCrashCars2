#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include "Mesh.h"

class Model {

public:
	Model(ShaderProgram& shader, const char* path, bool flipTexture = false, int renderMode = GL_FILL);

	void translate(const glm::vec3& offset);
	void setPosition(const glm::vec3& position);
	void scale(const glm::vec3& scale);
	void rotate(float angleRadian, const glm::vec3& axis);
	void rotateAround(const glm::vec3& position, float theta, float phi, float radius);
	void reset();

	void draw(glm::mat4& TM);
	void draw();

	// static methods

	static std::pair<Model, Model> createJeepModel(ShaderProgram& shader);
	static Model createGroundModel(ShaderProgram& shader);

private:
	ShaderProgram& m_shader;

	std::vector<TexMesh> m_textures_loaded;
	std::vector<Mesh> m_meshes;
	std::string m_directory;

	bool m_flipTexture;
	int m_renderMode;

	glm::mat4 m_TM = glm::mat4(1.0f);
	glm::vec3 m_position = glm::vec3(0.0f);
	glm::vec3 m_scale = glm::vec3(1.0f);
	float m_angle = 0.0f;
	float m_theta = 0.0f;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<TexMesh> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

};

