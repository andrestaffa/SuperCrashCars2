#include <GL/glew.h>
#include <GLFW/glfw3.h>
//
//#include <chrono>
//
//#include "Geometry.h"
//#include "GLDebug.h"
//#include "Log.h"
//#include "ShaderProgram.h"
//#include "Shader.h"
//#include "Window.h"
//
//#include "glm/glm.hpp"
//#include "glm/gtc/type_ptr.hpp"
//
//#include "Utils.h"
//#include "InputManager.h"
//#include "Camera.h"
//#include "GLMesh.h"
//
//#include "PVehicle.h"
//#include "PDynamic.h"
//#include "PStatic.h"
//
//#include "Model.h"
//
//int main(int argc, char** argv) {
//	Log::info("Starting Game...");
//
//	// OpenGL
//	glfwInit();
//	Window window(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, "Super Crash Cars 2");
//	ShaderProgram shader = ShaderProgram("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");
//	std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>(Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT);
//	window.setCallbacks(inputManager);
//
//	Camera editorCamera = Camera(shader, Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT, glm::vec3(-2.0f, 4.0f, 10.0f));
//
//	GLMesh obstacleMesh(shader, GL_FILL), ball(shader, GL_FILL);
//	obstacleMesh.createCube(1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
//	ball.createSphere(1.0f, 25, glm::vec3(1.0f, 0.0f, 0.0f));
//
//	// Physx
//	float throttle = 1.0f;
//	PhysicsManager pm = PhysicsManager(1.0/60.0f);
//	PVehicle player = PVehicle(pm, PxVec3(0.0f, 0.0f, 0.0f));
//	PDyanmic obstacle_d = PDyanmic(pm, PxSphereGeometry(1), PxVec3(-20.0f, 20.0f, -10.0f));
//	PStatic obstacle_s = PStatic(pm, PxBoxGeometry(1.0f, 1.0f, 1.0f), PxVec3(-20.0f, 0.0f, -20.0f));
//
//	Camera playerCamera = Camera(shader, Utils::shared().SCREEN_WIDTH, Utils::shared().SCREEN_HEIGHT);
//	playerCamera.setPitch(-30.0f);
//
//	std::pair<Model, Model> carModel = Model::createJeepModel(shader);
//	Model ground = Model::createGroundModel(shader);
//	
//	// Jump Cooldown and Timer
//	bool cameraToggle = false;
//	std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
//	std::chrono::steady_clock::time_point coolDownBegin = std::chrono::steady_clock::now();
//
//	while (!window.shouldClose()) {
//;
//		currentTime = std::chrono::steady_clock::now();
//
//		pm.simulate();
//		player.update();
//
//		glfwPollEvents();
//		shader.use();
//
//		if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_REPEAT))
//			editorCamera.handleRotation(inputManager->getMousePosition().x, inputManager->getMousePosition().y);
//		else if (inputManager->onMouseButtonAction(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE))
//			editorCamera.resetLastPos();
//
//		if (inputManager->onKeyAction(GLFW_KEY_W, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_W);
//		if (inputManager->onKeyAction(GLFW_KEY_A, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_A);
//		if (inputManager->onKeyAction(GLFW_KEY_S, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_S);
//		if (inputManager->onKeyAction(GLFW_KEY_D, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_D);
//		if (inputManager->onKeyAction(GLFW_KEY_LEFT_SHIFT, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_LEFT_SHIFT);
//		if (inputManager->onKeyAction(GLFW_KEY_LEFT_CONTROL, GLFW_PRESS)) editorCamera.handleTranslation(GLFW_KEY_LEFT_CONTROL);
//
//		if (inputManager->onKeyAction(GLFW_KEY_UP, GLFW_PRESS)) player.accelerate(throttle);
//		if (inputManager->onKeyAction(GLFW_KEY_DOWN, GLFW_PRESS)) player.reverse(throttle * 0.5);
//		if (inputManager->onKeyAction(GLFW_KEY_LEFT, GLFW_PRESS)) player.turnLeft(throttle * 0.5f);
//		if (inputManager->onKeyAction(GLFW_KEY_RIGHT, GLFW_PRESS)) player.turnRight(throttle * 0.5f);
//		if (inputManager->onKeyAction(GLFW_KEY_SPACE, GLFW_PRESS)) player.handbrake();
//
//		if (inputManager->onKeyAction(GLFW_KEY_E, GLFW_PRESS)) {
//			if (std::chrono::duration_cast<std::chrono::microseconds>(currentTime - coolDownBegin).count() > 2000000) {
//				coolDownBegin = std::chrono::steady_clock::now();
//				player.getRigidDynamic()->addForce(PxVec3(0.0, 15000.0, 0.0), PxForceMode::eIMPULSE);
//			}
//		}
//		if (inputManager->onKeyAction(GLFW_KEY_C, GLFW_PRESS)) {
//			if (std::chrono::duration_cast<std::chrono::microseconds>(currentTime - coolDownBegin).count() > 1000000) {
//				coolDownBegin = std::chrono::steady_clock::now();
//				cameraToggle = !cameraToggle;
//			}
//		}
//
//		if (abs(player.getPosition().z) >= 101.0f || abs(player.getPosition().x) >= 101.0) player.removePhysics();
//
//		glEnable(GL_DEPTH_TEST);
//		glEnable(GL_LINE_SMOOTH);
//		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//
//		if (cameraToggle) 
//			editorCamera.render();
//		else {
//			playerCamera.setPosition(glm::vec3(player.getPosition().x, player.getPosition().y + 6.0f, player.getPosition().z + 12.0f));
//			playerCamera.render();
//		}
//
//		ground.draw();
//		player.render(&carModel.first, &carModel.second);
//		obstacle_d.render(ball);
//		obstacle_s.render(obstacleMesh);
//
//		glDisable(GL_FRAMEBUFFER_SRGB);
//		window.swapBuffers();
//
//	}
//
//	player.free();
//	obstacle_d.free();
//	obstacle_s.free();
//	pm.free();
//
//	glfwTerminate();
//	return 0;
//}

//#include <GL/glew.h>
//#include <glfw3.h>
#include <iostream>
#include <cstdlib>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    glViewport(0.0f, 0.0f, screenWidth, screenHeight); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); // essentially set coordinate system
    glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity(); // same as above comment






    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Render OpenGL here




        int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        //std::cout << present << std::endl;

        if (1 == present)
        {
            int axesCount;
            const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
            //std::cout << axesCount << std::endl;

            //std::cout << "Left Stick X Axis: " << axes[0] << std::endl; // tested with PS4 controller connected via micro USB cable
            //std::cout << "Left Stick Y Axis: " << axes[1] << std::endl; // tested with PS4 controller connected via micro USB cable
            //std::cout << "Right Stick X Axis: " << axes[2] << std::endl; // tested with PS4 controller connected via micro USB cable
            //std::cout << "Right Stick Y Axis: " << axes[3] << std::endl; // tested with PS4 controller connected via micro USB cable
            //std::cout << "Left Trigger/L2: " << axes[4] << std::endl; // tested with PS4 controller connected via micro USB cable
            //std::cout << "Right Trigger/R2: " << axes[5] << std::endl; // tested with PS4 controller connected via micro USB cable

            int buttonCount;
            const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
            if (GLFW_PRESS == buttons[0])
            {
                std::cout << "A/X Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[1])
            {
                std::cout << "B/O Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[2])
            {
                std::cout << "X/triangle Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[3])
            {
                std::cout << "Y/square Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[4])
            {
                std::cout << "LB Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[5])
            {
                std::cout << "RB Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[6])
            {
                std::cout << "opt1 Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[7])
            {
                std::cout << "opt2 Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[8])
            {
                std::cout << "left joystick Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[9])
            {
                std::cout << "right joystick Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[10])
            {
                std::cout << "up button Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[11])
            {
                std::cout << "right button Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[12])
            {
                std::cout << "down joystick Pressed" << std::endl;
            }
            if (GLFW_PRESS == buttons[13])
            {
                std::cout << "left joystick Pressed" << std::endl;
            }

            const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
            //std::cout << name << std::endl;
        }




        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

