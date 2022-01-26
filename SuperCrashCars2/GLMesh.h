#pragma once

#include <vector>

#include "Geometry.h"
#include "ShaderProgram.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Utils.h"

struct Process { CPU_Geometry cpuGeom; GPU_Geometry gpuGeom; };

class GLMesh {

public:
	GLMesh(int renderMode = GL_LINE);
	~GLMesh();

	void createPlane(int size, const glm::vec3& color);
	void createCube(float scale, const glm::vec3& color);
	void createSphere(float radius, int numSectors, const glm::vec3& color);

	void translate(const glm::vec3& offset);
	void setPosition(const glm::vec3& position);
	void scale(const glm::vec3& scale);
	void rotate(float angleRadian, const glm::vec3& axis);
	void rotateAround(const glm::vec3& position, float theta, float phi, float radius);
	void reset();
	void destroy();


	void render(glm::mat4& TM);
	void render();

private:
	Process m_process;

	glm::mat4 m_TM;
	glm::vec3 m_position;
	glm::vec3 m_scale;
	float m_angle;
	float m_theta;

	int m_renderMode;

	std::vector<glm::vec3> generateQuads(std::vector<std::vector<glm::vec3>>& points);

};