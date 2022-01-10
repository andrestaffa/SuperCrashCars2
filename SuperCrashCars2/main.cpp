#include <PxPhysicsAPI.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"


#include "InputManager.h"

int main() {
	Log::info("Starting Game...");

	glfwInit();
	Window window(800, 800, "Super Crash Cars 2");
	GLDebug::enable();

	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(800, 800);
	window.setCallbacks(inputManager);

	ShaderProgram shader("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");

	while (!window.shouldClose()) {

		glfwPollEvents();

		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		shader.use();

		glDisable(GL_FRAMEBUFFER_SRGB);
		window.swapBuffers();

	}

	glfwTerminate();
	return 0;
}