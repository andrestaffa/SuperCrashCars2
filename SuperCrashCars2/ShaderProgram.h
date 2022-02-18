#pragma once

#include "Shader.h"

#include "GLHandles.h"

#include <GL/glew.h>
#include "glm/glm.hpp"

#include <string>


class ShaderProgram {

public:
	ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

	// Because we're using the ShaderProgramHandle to do RAII for the shader for us
	// and our other types are trivial or provide their own RAII
	// we don't have to provide any specialized functions here. Rule of zero
	//
	// https://en.cppreference.com/w/cpp/language/rule_of_three
	// https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#Rc-zero

	// Public interface
	bool recompile();
	void use() const { glUseProgram(programID); }

	void setBool(const std::string& name, bool value) const { glUniform1i(glGetUniformLocation(*this, name.c_str()), (int)value); }
	void setInt(const std::string& name, int value) const { glUniform1i(glGetUniformLocation(*this, name.c_str()), value); }
	void setFloat(const std::string& name, float value) const { glUniform1f(glGetUniformLocation(*this, name.c_str()), value); }

	void setVector4(const std::string& name, glm::vec4 value) const { glUniform4f(glGetUniformLocation(*this, name.c_str()), value.x, value.y, value.z, value.w); }
	void setVector3(const std::string& name, glm::vec3 value) const { glUniform3f(glGetUniformLocation(*this, name.c_str()), value.x, value.y, value.z); }

	void setMat4(const std::string& name, glm::mat4 value) const { glUniformMatrix4fv(glGetUniformLocation(*this, name.c_str()), 1, GL_FALSE, &value[0][0]); }

	void friend attach(ShaderProgram& sp, Shader& s);

	operator GLuint() const {
		return programID;
	}

private:
	ShaderProgramHandle programID;

	Shader vertex;
	Shader fragment;

	bool checkAndLogLinkSuccess() const;
};
