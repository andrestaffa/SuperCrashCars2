#include "Model.h"

Model::Model() :
	m_flipTexture(false),
	m_renderMode(GL_FILL),
	m_TM(1.0f),
	m_position(0.0f),
	m_scale(1.0f),
	m_angle(0.0f),
	m_theta(0.0f)
{}

Model::Model(const char* path, bool flipTexture, int renderMode) : 
	m_flipTexture(flipTexture),
	m_renderMode(renderMode),
	m_TM(1.0f),
	m_position(0.0f),
	m_scale(1.0f),
	m_angle(0.0f),
	m_theta(0.0f)
{
	this->loadModel(path);
}

Model::Model(const Model& model) :
	m_flipTexture(model.m_flipTexture),
	m_renderMode(model.m_renderMode),
	m_textures_loaded(model.m_textures_loaded),
	m_meshes(model.m_meshes),
	m_directory(model.m_directory),
	m_TM(model.m_TM),
	m_position(model.m_position),
	m_scale(model.m_scale),
	m_angle(model.m_angle),
	m_theta(model.m_theta)
{}

Model& Model::operator=(const Model& model) {
	this->m_textures_loaded = model.m_textures_loaded;
	this->m_meshes = model.m_meshes;
	this->m_directory = model.m_directory;
	this->m_flipTexture = model.m_flipTexture;
	this->m_renderMode = model.m_renderMode;
	return *this;
}

void Model::translate(const glm::vec3& offset) {
	glm::mat4 T = glm::translate(glm::mat4(1.0f), offset);
	this->m_TM = T * this->m_TM;
	this->m_position = this->m_position + offset;
}

void Model::setPosition(const glm::vec3& position) {
	this->translate(-this->m_position);
	this->translate(position);
}

void Model::scale(const glm::vec3& scale) {
	glm::mat4 T_P = glm::translate(glm::mat4(1.0f), this->m_position);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 T_N = glm::translate(glm::mat4(1.0f), -this->m_position);
	this->m_TM = T_P * S * T_N * this->m_TM;
	this->m_scale *= scale;
}

void Model::rotate(float angleRadian, const glm::vec3& axis) {
	glm::mat4 T_P = glm::translate(glm::mat4(1.0f), this->m_position);
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), angleRadian, axis);
	glm::mat4 T_N = glm::translate(glm::mat4(1.0f), -this->m_position);
	this->m_TM = T_P * R * T_N * this->m_TM;
	this->m_angle += angleRadian;
	float degree = this->m_angle * 180.0f / glm::pi<float>();
	if (degree >= 360.0f) this->m_angle = (degree - 360) * glm::pi<float>() / 180.0f;
}
void Model::rotateAround(const glm::vec3& position, float theta, float phi, float radius) {
	this->setPosition(position);
	this->translate(glm::vec3(radius * glm::sin(this->m_theta + theta) * glm::cos(phi), radius * glm::sin(this->m_theta + theta) * glm::sin(phi), radius * glm::cos(this->m_theta + theta)));
	this->m_theta += theta;
}

void Model::reset() {
	this->m_position = glm::vec3(0.0f);
	this->m_angle = 0.0f;
	this->m_TM = glm::mat4(1.0f);
	this->m_scale = glm::vec3(1.0f);
}

const std::vector<Mesh>& Model::getMeshData() const {
	return this->m_meshes;
}

void Model::draw(glm::mat4& TM) {
	TM = TM * this->m_TM;
	for (unsigned int i = 0; i < this->m_meshes.size(); i++)
		this->m_meshes[i].draw(TM, this->m_renderMode);
}

void Model::draw() {
	for (unsigned int i = 0; i < this->m_meshes.size(); i++)
		this->m_meshes[i].draw(this->m_TM, this->m_renderMode);
}

void Model::loadModel(const std::string& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		Log::error("{}", importer.GetErrorString());
		return;
	}
	this->m_directory = path.substr(0, path.find_last_of('/'));

	this->processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->m_meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TexMesh> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		} else vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		// Shaders
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		// 1. diffuse maps
		std::vector<TexMesh> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<TexMesh> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// 3. normal maps
		std::vector<TexMesh> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<TexMesh> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	}
	return Mesh(vertices, indices, textures);
}

std::vector<TexMesh> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
	std::vector<TexMesh> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < this->m_textures_loaded.size(); j++) {
			if (std::strcmp(this->m_textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(this->m_textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {  
			TexMesh texture;
			texture.id = TextureFromFile(str.C_Str(), this->m_directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			this->m_textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	stbi_set_flip_vertically_on_load(this->m_flipTexture);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		
		GLuint format = GL_RGB;
		switch (nrComponents) {
			case 4:
				format = GL_RGBA;
				break;
			case 3:
				format = GL_RGB;
				break;
			case 2:
				format = GL_RG;
				break;
			case 1:
				format = GL_RED;
				break;
			default:
				Log::error("Invalid Texture Format");
				break;
		};

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

		/*glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	} else {
		Log::error("Texture failed to load at path: {}", path);
		stbi_image_free(data);
	}

	return textureID;
}