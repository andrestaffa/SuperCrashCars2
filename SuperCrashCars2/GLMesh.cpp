#include "GLMesh.h"

GLMesh::GLMesh(int renderMode) :
	m_TM(1.0f),
	m_position(0.0f),
	m_scale(1.0f),
	m_angle(0.0f),
	m_theta(0.0f),
	m_renderMode(renderMode)
{}

void GLMesh::createPlane(int size, const glm::vec3& color) {
	std::vector<std::vector<glm::vec3>> vertices;

	int index = 0;
	for (int i = -size/2; i < size - (size/2); i++) {
		vertices.push_back(std::vector<glm::vec3>());
		for (int j = -size/2; j < size - (size/2); j++) {
			vertices[index].push_back(glm::vec3(i, 0.0f, -j));
		}
		index++;
	}

	this->m_process.gpuGeom.bind();
	this->m_process.cpuGeom.verts = generateQuads(vertices);
	this->m_process.cpuGeom.cols.resize(this->m_process.cpuGeom.verts.size(), color);
	this->m_process.gpuGeom.setVerts(this->m_process.cpuGeom.verts);
	this->m_process.gpuGeom.setCols(this->m_process.cpuGeom.cols);
}

void GLMesh::createCube(float scale, const glm::vec3& color) {
	std::vector<glm::vec3> vertices, back_face, bottom_face, top_face, left_face, right_face;
	
	// front face
	vertices.push_back(glm::vec3(-1.0f, -1.0, 1.0f) * scale);
	vertices.push_back(glm::vec3(-1.0f, 1.0, 1.0f) * scale);
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f) * scale);
	vertices.push_back(glm::vec3(-1.0f, -1.0, 1.0f) * scale);
	vertices.push_back(glm::vec3(1.0f, -1.0f, 1.0f) * scale);
	vertices.push_back(glm::vec3(1.0f, 1.0f, 1.0f) * scale);

	back_face = bottom_face = top_face = left_face = right_face = vertices;

	glm::mat4 T, R;

	T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f) * scale);
	for (glm::vec3& v : back_face) {
		v = T * glm::vec4(v, 1.0f);
		vertices.push_back(v);
	}

	R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	for (glm::vec3& v : bottom_face) {
		v = R * glm::vec4(v, 1.0f);
		vertices.push_back(v);
	}

	R = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	for (glm::vec3& v : top_face) {
		v = R * glm::vec4(v, 1.0f);
		vertices.push_back(v);
	}

	R = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (glm::vec3& v : right_face) {
		v = R * glm::vec4(v, 1.0f);
		vertices.push_back(v);
	}

	R = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	for (glm::vec3& v : left_face) {
		v = R * glm::vec4(v, 1.0f);
		vertices.push_back(v);
	}

	this->m_process.gpuGeom.bind();
	this->m_process.cpuGeom.verts = vertices;
	this->m_process.cpuGeom.cols.resize(this->m_process.cpuGeom.verts.size(), color);
	this->m_process.gpuGeom.setVerts(this->m_process.cpuGeom.verts);
	this->m_process.gpuGeom.setCols(this->m_process.cpuGeom.cols);

}

void GLMesh::createSphere(float radius, int numSectors, const glm::vec3& color) {
	std::vector<std::vector<glm::vec3>> vertices, normals;
	std::vector<std::vector<glm::vec2>> textures;

	int sectorCount = numSectors;
	int stackCount = numSectors;

	float x, y, z, r_c;
	float nx, ny, nz;
	float s, t;

	float sectorStep = 2.0f * glm::pi<float>() / sectorCount;
	float stackStep = glm::pi<float>() / stackCount;
	float sectorAngle, stackAngle;

	size_t k = 0;
	for (int i = 0; i <= stackCount; ++i) {
		vertices.push_back(std::vector<glm::vec3>());
		textures.push_back(std::vector<glm::vec2>());
		normals.push_back(std::vector<glm::vec3>());
		stackAngle = glm::pi<float>() / 2 - i * stackStep;
		r_c = radius * glm::cos(stackAngle);
		z = radius * glm::sin(stackAngle);
		for (int j = 0; j <= sectorCount; ++j) {
			// get x, y, z coordinates
			sectorAngle = j * sectorStep;
			x = r_c * cosf(sectorAngle);
			y = r_c * sinf(sectorAngle);
			vertices[k].push_back(glm::vec3(x, y, z));
			// get texture coordinates
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			textures[k].push_back(glm::vec2(s, t));
			// get normal coordinates
			nx = x * 2;
			ny = y * 2;
			nz = z * 2;
			normals[k].push_back(glm::vec3(nx, ny, nz));
		}
		k = k + 1;
	}
	this->m_process.gpuGeom.bind();
	this->m_process.cpuGeom.verts = generateQuads(vertices);
	this->m_process.cpuGeom.cols.resize(this->m_process.cpuGeom.verts.size(), color);
	this->m_process.gpuGeom.setVerts(this->m_process.cpuGeom.verts);
	this->m_process.gpuGeom.setCols(this->m_process.cpuGeom.cols);
}

void GLMesh::translate(const glm::vec3& offset) {
	glm::mat4 T = glm::translate(glm::mat4(1.0f), offset);
	this->m_TM = T * this->m_TM;
	this->m_position = this->m_position + offset;
}

void GLMesh::setPosition(const glm::vec3& position) {
	this->translate(-this->m_position);
	this->translate(position);
}

void GLMesh::scale(const glm::vec3& scale) {
	glm::mat4 T_P = glm::translate(glm::mat4(1.0f), this->m_position);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
	glm::mat4 T_N = glm::translate(glm::mat4(1.0f), -this->m_position);
	this->m_TM = T_P * S * T_N * this->m_TM;
	this->m_scale *= scale;
}

void GLMesh::rotate(float angleRadian, const glm::vec3& axis) {
	glm::mat4 T_P = glm::translate(glm::mat4(1.0f), this->m_position);
	glm::mat4 R = glm::rotate(glm::mat4(1.0f), angleRadian, axis);
	glm::mat4 T_N = glm::translate(glm::mat4(1.0f), -this->m_position);
	this->m_TM = T_P * R * T_N * this->m_TM;
	this->m_angle += angleRadian;
	float degree = this->m_angle * 180.0f / glm::pi<float>();
	if (degree >= 360.0f) this->m_angle = (degree - 360) * glm::pi<float>() / 180.0f;
}

void GLMesh::rotateAround(const glm::vec3& position, float theta, float phi, float radius) {
	this->setPosition(position);
	this->translate(glm::vec3(radius * glm::sin(this->m_theta + theta) * glm::cos(phi), radius * glm::sin(this->m_theta + theta) * glm::sin(phi), radius * glm::cos(this->m_theta + theta)));
	this->m_theta += theta;
}

void GLMesh::reset() {
	this->m_position = glm::vec3(0.0f);
	this->m_angle = 0.0f;
	this->m_TM = glm::mat4(1.0f);
	this->m_scale = glm::vec3(1.0f);
}

void GLMesh::destroy() {
	this->m_process.cpuGeom.verts.clear();
	this->m_process.cpuGeom.cols.clear();
}

void GLMesh::render(glm::mat4& TM) {
	TM = TM * this->m_TM;
	GLint modelLoc = glGetUniformLocation(*Utils::instance().shader, "TM");
	this->m_process.gpuGeom.bind();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &TM[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, this->m_renderMode);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(this->m_process.cpuGeom.verts.size()));
}

void GLMesh::render() {
	GLint modelLoc = glGetUniformLocation(*Utils::instance().shader, "TM");
	this->m_process.gpuGeom.bind();
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &this->m_TM[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, this->m_renderMode);
	glDrawArrays(GL_TRIANGLES, 0, GLsizei(this->m_process.cpuGeom.verts.size()));
}

std::vector<glm::vec3> GLMesh::generateQuads(std::vector<std::vector<glm::vec3>>& points) {
	std::vector<glm::vec3> R;
	for (size_t i = 0; i < points.size() - 1; i++) {
		for (size_t j = 0; j < points[i].size() - 1; j++) {
			R.push_back(points[i][j + 1]); // Top Left
			R.push_back(points[i][j]); // Bottom Left
			R.push_back(points[i + 1][j]); // Bottom Right
			R.push_back(points[i][j + 1]); // Top Left
			R.push_back(points[i + 1][j + 1]); // Top Right
			R.push_back(points[i + 1][j]); // Bottom Right
		}
	}
	return R;
}

GLMesh::~GLMesh() {}