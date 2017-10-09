#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

class InputManager {

public:
	InputManager(GLFWwindow* window);
	~InputManager();

	bool init();
	void setCamera();//Camera* camera);
	void setVerbose(bool setting) { m_setting_verbose = setting; };

	void registerKey(int key, void(*function)());

private:
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow *_window, int button, int action, int mods);
	void error_callback(int error, const char* description);

	GLFWwindow* m_window;
	glm::vec2 m_mouse_position;
	//Camera* m_cam;
	std::vector<int> m_keys;
	std::vector<void(*)()> m_key_function_ptrs;

	// Settings
	bool m_setting_verbose;

	// Wrapper
	class GLFWCallbackWrapper {
		public:
			GLFWCallbackWrapper() = delete;
			GLFWCallbackWrapper(const GLFWCallbackWrapper&) = delete;
			GLFWCallbackWrapper(GLFWCallbackWrapper&&) = delete;
			~GLFWCallbackWrapper() = delete;

			static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
			static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
			static void error_callback(int error, const char* description);
			static void setInputManager(InputManager* inputManager);
		private:
			static InputManager* s_input_manager;
	};
};
#endif INPUT_MANAGER_H
