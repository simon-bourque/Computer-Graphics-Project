#include "InputManager.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

InputManager::InputManager(GLFWwindow* window):
	m_setting_verbose(false)
{
	m_window = window;
}

InputManager::~InputManager()
{
	glfwSetKeyCallback(m_window, NULL);
	glfwSetCursorPosCallback(m_window, NULL);
	glfwSetMouseButtonCallback(m_window, NULL);
	glfwSetErrorCallback(NULL);
}

bool InputManager::init()
{
	GLFWCallbackWrapper::setInputManager(this);
	glfwSetKeyCallback(m_window, GLFWCallbackWrapper::key_callback);
	glfwSetCursorPosCallback(m_window, GLFWCallbackWrapper::mouse_callback);
	glfwSetMouseButtonCallback(m_window, GLFWCallbackWrapper::mouse_button_callback);
	glfwSetErrorCallback(GLFWCallbackWrapper::error_callback);
	return true;
}

void InputManager::setCamera()//Camera* camera)
{
	//m_camera = camera;
}

/*
	Had to read https://stackoverflow.com/questions/15128444/c-calling-a-function-from-a-vector-of-function-pointers-inside-a-class-where-t
*/
void InputManager::registerDebugKey(int key, void(*function)())
{
	std::tuple<int, void(*)> t;
	std::get<0>(t) = key;
	std::get<1>(t) = function;
	m_debug_keys.push_back(t);
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS || (action == GLFW_REPEAT && m_setting_key_repeat))
	{
		switch (key)
		{
			case GLFW_KEY_W:
				if (m_setting_verbose) std::cout << "W pressed" << std::endl;
				break;
			case GLFW_KEY_S:
				if (m_setting_verbose) std::cout << "S pressed" << std::endl;
				break;
			case GLFW_KEY_A:
				if (m_setting_verbose) std::cout << "A pressed" << std::endl;
				break;
			case GLFW_KEY_D:
				if (m_setting_verbose) std::cout << "D pressed" << std::endl;
				break;
		}
	}

	for(int i = 0; i < m_debug_keys.size(); i++)
	{
		if (std::get<0>(m_debug_keys.at(i)) == key && action == GLFW_PRESS)
		{
			if (m_setting_verbose) std::cout << "debug key " << key << " pressed" << std::endl;
			std::get<1>(m_debug_keys.at(i));
		}
	}
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	m_mouse_position = glm::vec2(xpos, ypos);
	if(m_setting_verbose) std::cout << "Mouse at (" << m_mouse_position.x << "," << m_mouse_position.y << ")" << std::endl;

	// forward xpos and ypos to camera
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// potentially alter the camera's state if necessary
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (m_setting_verbose) std::cout << "Left Mouse pressed." << std::endl;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		if (m_setting_verbose) std::cout << "Left Mouse released." << std::endl;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		if (m_setting_verbose) std::cout << "Middle Mouse pressed." << std::endl;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		if (m_setting_verbose) std::cout << "Middle Mouse released." << std::endl;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (m_setting_verbose) std::cout << "Right Mouse pressed." << std::endl;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		if (m_setting_verbose) std::cout << "Right Mouse released." << std::endl;
	}
}

void InputManager::error_callback(int error, const char* description)
{
	fputs(description, stderr);
}


// Wrapper
/*
	Idea is from https://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback
*/

void InputManager::GLFWCallbackWrapper::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	s_input_manager->key_callback(window, key, scancode, action, mods);
}

void InputManager::GLFWCallbackWrapper::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	s_input_manager->mouse_callback(window, xpos, ypos);
}

void InputManager::GLFWCallbackWrapper::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	s_input_manager->mouse_button_callback(window, button, action, mods);
}

void InputManager::GLFWCallbackWrapper::error_callback(int error, const char* description)
{
	s_input_manager->error_callback(error, description);
}

void InputManager::GLFWCallbackWrapper::setInputManager(InputManager* inputManager)
{
	GLFWCallbackWrapper::s_input_manager = inputManager;
}

InputManager* InputManager::GLFWCallbackWrapper::s_input_manager = nullptr;