#include "FreeCameraController.h"

#include "Camera.h"
#include "InputManager.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

glm::vec2 getMouseAxis();

FreeCameraController::FreeCameraController(Camera* camera) : 
	m_camera(camera),
	m_forwardPressed(false),
	m_backwardPressed(false),
	m_leftPressed(false),
	m_rightPressed(false),
	m_upPressed(false),
	m_downPressed(false),
	m_rollLeftPressed(false),
	m_rollRightPressed(false),
	m_rightMouseButtonPressed(false)
{
	InputManager::instance()->registerKeyCallback(std::bind(&FreeCameraController::onKey, this, std::placeholders::_1, std::placeholders::_2));
	InputManager::instance()->registerMouseBtnCallback(std::bind(&FreeCameraController::onMouseButton, this, std::placeholders::_1, std::placeholders::_2));
}

FreeCameraController::~FreeCameraController() {}

void FreeCameraController::update(float32 deltaSeconds) {
	const static float32 SPEED = 100.0f;
	const static float32 ROTATE_SPEED = 0.01f;

	float32 dx = 0;
	float32 dy = 0;
	float32 dz = 0;
	
	if (m_forwardPressed) {
		--dz;
	}
	if (m_backwardPressed) {
		++dz;
	}
	if (m_leftPressed) {
		--dx;
	}
	if (m_rightPressed) {
		++dx;
	}
	if (m_upPressed) {
		++dy;
	}
	if (m_downPressed) {
		--dy;
	}

	glm::vec3 deltaPos(dx,dy,dz);
	deltaPos = (glm::normalize(deltaPos) * SPEED * deltaSeconds);

	if (dx != 0 || dy != 0 || dz != 0) {
		m_camera->transform.translateLocal(deltaPos.x, deltaPos.y, deltaPos.z);
	}

	// ###############################
	glm::vec2 mouseAxis = getMouseAxis() * ROTATE_SPEED;
	if (m_rightMouseButtonPressed) {

		if (abs(mouseAxis.x) > abs(mouseAxis.y)) {
			mouseAxis.y = 0;
		}
		else {
			mouseAxis.x = 0;
		}

		m_camera->transform.rotateLocal(mouseAxis.y, 0, 0);
		m_camera->transform.rotate(0, -mouseAxis.x, 0);
	}

	float32 roll = 0;
	if (m_rollLeftPressed) {
		++roll;
	}
	if (m_rollRightPressed) {
		--roll;
	}

	if (roll != 0) {
		m_camera->transform.rotateLocal(0, 0, roll * ROTATE_SPEED * 100 * deltaSeconds);
	}
}

void FreeCameraController::onKey(int32 key, int32 action) {
	if (key == GLFW_KEY_W) {
		if (action == GLFW_PRESS) {
			m_forwardPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_forwardPressed = false;
		}
	}
	if (key == GLFW_KEY_S) {
		if (action == GLFW_PRESS) {
			m_backwardPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_backwardPressed = false;
		}
	}
	if (key == GLFW_KEY_A) {
		if (action == GLFW_PRESS) {
			m_leftPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_leftPressed = false;
		}
	}
	if (key == GLFW_KEY_D) {
		if (action == GLFW_PRESS) {
			m_rightPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_rightPressed = false;
		}
	}
	if (key == GLFW_KEY_SPACE) {
		if (action == GLFW_PRESS) {
			m_upPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_upPressed = false;
		}
	}
	if (key == GLFW_KEY_LEFT_CONTROL) {
		if (action == GLFW_PRESS) {
			m_downPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_downPressed = false;
		}
	}
	if (key == GLFW_KEY_Q) {
		if (action == GLFW_PRESS) {
			m_rollLeftPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_rollLeftPressed = false;
		}
	}
	if (key == GLFW_KEY_E) {
		if (action == GLFW_PRESS) {
			m_rollRightPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_rollRightPressed = false;
		}
	}
}

void FreeCameraController::onMouseButton(int32 button, int32 action) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			m_rightMouseButtonPressed = true;
		}
		else if (action == GLFW_RELEASE) {
			m_rightMouseButtonPressed = false;
		}
	}
}