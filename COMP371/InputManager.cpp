#include "InputManager.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

InputManager* InputManager::_instance = 0;

InputManager::InputManager():
	m_setting_verbose(false)
{
	m_window = glfwGetCurrentContext();
}

InputManager::~InputManager()
{
	glfwSetKeyCallback(m_window, NULL);
	glfwSetCursorPosCallback(m_window, NULL);
	glfwSetMouseButtonCallback(m_window, NULL);
	glfwSetErrorCallback(NULL);
}

InputManager* InputManager::instance()
{
	if (_instance == nullptr)
	{
		_instance = new InputManager();
		_instance->init();
	}
	return _instance;
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

void InputManager::registerKeyCallback(std::function<void(int, int)> function)
{
	m_key_callbacks.push_back(function);
}

void InputManager::registerMouseBtnCallback(std::function<void(int, int)> function)
{
	m_mouse_btn_callbacks.push_back(function);
}

void InputManager::setCursorInvisible(bool setting)
{
	m_setting_cursor_invisible = setting;
	m_setting_cursor_invisible ? glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL) : glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS || (action == GLFW_REPEAT && m_setting_key_repeat))
	{
		switch (key)
		{
			case GLFW_KEY_F1:
				setCursorInvisible(!m_setting_cursor_invisible);
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

	for (auto& it : m_key_callbacks)
		it(key, action);
}


void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	m_mouse_position = glm::vec2(xpos, ypos);
	if(m_setting_verbose) std::cout << "Mouse at (" << m_mouse_position.x << "," << m_mouse_position.y << ")" << std::endl;
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	for (auto& it : m_mouse_btn_callbacks)
		it(button, action);
}

void InputManager::error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

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