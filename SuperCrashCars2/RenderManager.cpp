#include "RenderManager.h"

RenderManager::RenderManager(Window* window, std::vector<Camera*>* cameraList, Camera* menuCamera){

	m_window = window;
	//m_camera = camera;
	m_menuCamera = menuCamera;
	m_cameraList = cameraList;
	m_currentViewportActive = 1;

	defaultShader = std::make_shared<ShaderProgram>("shaders/shader_vertex.vert", "shaders/shader_fragment.frag");
	depthShader = std::make_shared<ShaderProgram>("shaders/simpleDepth.vert", "shaders/simpleDepth.frag");
	carShader = std::make_shared<ShaderProgram>("shaders/car.vert", "shaders/car.frag");
	transparentShader = std::make_shared<ShaderProgram>("shaders/transparent.vert", "shaders/transparent.frag");
	powerUpShader = std::make_shared<ShaderProgram>("shaders/powerUp.vert", "shaders/powerUp.frag");

	Utils::instance().shader = defaultShader;

	// Shadows start 
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// end shadows
}

void RenderManager::startFrame(){
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CW);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderManager::endFrame(){
	glDisable(GL_FRAMEBUFFER_SRGB);
	m_window->swapBuffers();
	//glViewport(0, 0, Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
}

bool RenderManager::switchViewport(int playerNumber, int i) { // returns true only on first viewport - used to trigger the timer.
	m_currentViewportActive = i;
	switch (i)
	{
	case 0:		
		switch (playerNumber) {
		case 1: //Full screen
			glViewport(0, 0, Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
			break;
		case 2: //Left Screen
			glViewport(0, 0, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT);
			break;
		case 3: //Whole top part of the screen
			glViewport(Utils::instance().SCREEN_WIDTH / 4, Utils::instance().SCREEN_HEIGHT / 2, Utils::instance().SCREEN_WIDTH /2, Utils::instance().SCREEN_HEIGHT / 2);
			break;
		case 4:	//Top left of the screen
			glViewport(0, Utils::instance().SCREEN_HEIGHT / 2, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT / 2);
			break;
		}
		return true; //time.startRenderTimer(); RETURN TRUE to trigger timer
		break;
	case 1:
		switch (playerNumber) {
		case 2: //Right screen

			glViewport(Utils::instance().SCREEN_WIDTH / 2, 0, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT);
			break;
		case 3: 			// Bottom left screen
			glViewport(0, 0, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT / 2);
			break;
		case 4://Top right of the screen
			glViewport(Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT / 2, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT / 2);
			break;
		}
		break;
	case 2:
		switch (playerNumber) {
		case 3: //Bottom right
			glViewport(Utils::instance().SCREEN_WIDTH / 2, 0, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT / 2);
			break;
		case 4:// Bottom left screen
			glViewport(0, 0, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT / 2);
			break;
		}
		break;
	case 3:
		//Bottom right
		glViewport(Utils::instance().SCREEN_WIDTH / 2, 0, Utils::instance().SCREEN_WIDTH / 2, Utils::instance().SCREEN_HEIGHT / 2);
		break;
	}




	
	return false;
}

void RenderManager::renderShadows(const std::vector<PVehicle*>& vehicleList, const std::vector<PowerUp*>& powerUps) {


	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------

	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view

	Utils::instance().shader = depthShader;
	Utils::instance().shader->use();
	Utils::instance().shader->setInt("shadowMap", 1);
	Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	Utils::instance().shader->setVector4("lightColor", lightColor);
	Utils::instance().shader->setVector3("lightPos", lightPos);
	Utils::instance().shader->setVector3("camPos", m_cameraList->at(m_currentViewportActive)->getPosition());


	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);

	for (PVehicle* carPtr : vehicleList) {
		carPtr->render();
	}

	for (PowerUp* powerUpPtr : powerUps) {
		if (powerUpPtr->active) {
			powerUpPtr->render();
		}
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	switchViewport(GameManager::get().playerNumber, m_currentViewportActive); // change back to the current viewport
	//glViewport(0, 0, Utils::instance().SCREEN_WIDTH, Utils::instance().SCREEN_HEIGHT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

}

void RenderManager::renderCars(const std::vector<PVehicle*>& vehicleList){
	// Cars rendering
	Utils::instance().shader = carShader;
	Utils::instance().shader->use();
	Utils::instance().shader->setInt("shadowMap", 1);
	Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	Utils::instance().shader->setVector4("lightColor", lightColor);
	Utils::instance().shader->setVector3("lightPos", lightPos);
	Utils::instance().shader->setVector3("camPos", m_cameraList->at(m_currentViewportActive)->getPosition());
	m_cameraList->at(m_currentViewportActive)->sendMatricesToShader();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	for (PVehicle* carPtr : vehicleList) {
		Utils::instance().shader->setFloat("damage", carPtr->vehicleAttr.collisionCoefficient * 0.3); // number is how fast car turns red
		Utils::instance().shader->setFloat("flashStrength", carPtr->vehicleParams.flashWhite);
		carPtr->render();
	}
}

// only runs code to setup for rendering a 'normal object'. doesn't actually render anything itself to not pass around too many specific things
void RenderManager::renderNormalObjects(std::vector<Model>& trees, std::vector<Model>& grassPatches) {
	// Other rendering
	Utils::instance().shader = defaultShader;
	Utils::instance().shader->use();
	Utils::instance().shader->setInt("shadowMap", 1);
	Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	Utils::instance().shader->setVector4("lightColor", lightColor);
	Utils::instance().shader->setVector3("lightPos", lightPos);
	Utils::instance().shader->setVector3("camPos", m_cameraList->at(m_currentViewportActive)->getPosition());
	m_cameraList->at(m_currentViewportActive)->sendMatricesToShader();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	for (Model& grass : grassPatches) grass.draw();
	for (Model& tree : trees) tree.draw();
}

void RenderManager::generateLandscape(std::vector<Model>& trees, std::vector<Model>& grassPatches, Model& ground) {

	int counter = 0;
	Model tree = Model("models/tree/tree.obj");
	for (const Mesh& mesh : ground.getMeshData()) {
		for (const Vertex& vertex : mesh.m_vertices) {
			if (counter % 750 == 0) {
				const glm::vec3& position = vertex.Position;
				Model tree_c = Model(tree);
				tree_c.setPosition(glm::vec3(position.x, position.y, position.z));
				tree_c.scale(glm::vec3(2.f));
				trees.push_back(tree_c);
			}
			counter++;
		}
	}

	counter = 0;
	Model grass = Model("models/grass/grass.obj");
	for (const Mesh& mesh : ground.getMeshData()) {
		for (const Vertex& vertex : mesh.m_vertices) {
			if (counter % 20 == 0) {
				const glm::vec3& position = vertex.Position;
				Model grass_c = Model(grass);
				grass_c.setPosition(glm::vec3(position.x, position.y, position.z));
				grass_c.scale(glm::vec3(1.75f));
				grassPatches.push_back(grass_c);
			}
			counter++;
		}
	}

}

void RenderManager::renderTransparentObjects(const std::vector<PVehicle*>& vehicleList, PStatic& sphere, double os, Time& time) {
	// Sphere
	Utils::instance().shader = transparentShader;
	Utils::instance().shader->use();
	Utils::instance().shader->setFloat("opacity", os);
	Utils::instance().shader->setInt("shadowMap", 1);
	Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	Utils::instance().shader->setVector4("lightColor", lightColor);
	Utils::instance().shader->setVector3("lightPos", lightPos);
	Utils::instance().shader->setVector3("camPos", m_cameraList->at(m_currentViewportActive)->getPosition());
	m_cameraList->at(m_currentViewportActive)->sendMatricesToShader();
	for (PVehicle* carPtr : vehicleList) {
		switch (carPtr->m_shieldState) {
		case ShieldPowerUpState::eINACTIVE:
			break;
		case ShieldPowerUpState::eACTIVE:
			carPtr->m_shieldSphere.draw();
			break;
		case ShieldPowerUpState::eEXPIRING:
			if (time.slowOscilator()) carPtr->m_shieldSphere.draw();
			break;
		case ShieldPowerUpState::eLAST_SECOND:
			if (time.fastOscilator()) carPtr->m_shieldSphere.draw();
			break;
		}
	}

	sphere.render();

	

}

void RenderManager::renderPowerUps(const std::vector<PowerUp*>& powerUps, double os) {
	// Power ups
	Utils::instance().shader = powerUpShader;
	Utils::instance().shader->use();
	Utils::instance().shader->setFloat("os", os);
	Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	Utils::instance().shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
	Utils::instance().shader->setVector4("lightColor", lightColor);
	Utils::instance().shader->setVector3("lightPos", lightPos);
	Utils::instance().shader->setVector3("camPos", m_cameraList->at(m_currentViewportActive)->getPosition());
	m_cameraList->at(m_currentViewportActive)->sendMatricesToShader();

	for (PowerUp* powerUpPtr : powerUps) {
		if (powerUpPtr->active) {
			powerUpPtr->render();
		}
	}
}

void RenderManager::useDefaultShader() {
	Utils::instance().shader = defaultShader;
	Utils::instance().shader->use();
	Utils::instance().shader->setVector4("lightColor", lightColor);
	Utils::instance().shader->setVector3("lightPos", lightPos);
	Utils::instance().shader->setVector3("camPos", m_menuCamera->getPosition());
}












