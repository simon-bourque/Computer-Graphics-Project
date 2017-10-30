#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <vector>
#include <tuple>
#include <functional>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputManager {

public:
	static InputManager* instance();

	void registerDebugKey(int key, void(*function)());
	void registerKeyCallback(std::function<void(int, int)> function);
	void registerMouseBtnCallback(std::function<void(int, int)> function);

	void setVerbose(bool setting) { m_setting_verbose = setting; };
	void setKeyRepeat(bool setting) { m_setting_key_repeat = setting; };
	void setCursorInvisible(bool setting);

	glm::vec2 getMousePosition() { return m_mouse_position; };

private:
	InputManager();
	~InputManager();

	bool init();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback(GLFWwindow *_window, int button, int action, int mods);
	void error_callback(int error, const char* description);

	static InputManager* _instance;

	GLFWwindow* m_window;
	glm::vec2 m_mouse_position;

	std::vector <std::tuple<int, void(*)>> m_debug_keys;
	std::vector<std::function<void(int,int)>> m_key_callbacks;
	std::vector<std::function<void(int, int)>> m_mouse_btn_callbacks;

	// Settings
	bool m_setting_verbose;
	bool m_setting_key_repeat;
	bool m_setting_cursor_invisible;

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
