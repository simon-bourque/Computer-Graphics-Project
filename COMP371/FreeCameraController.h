#pragma once

#include "Types.h"

class Camera;

class FreeCameraController {
private:
	Camera* m_camera;

	bool m_forwardPressed;
	bool m_backwardPressed;
	bool m_leftPressed;
	bool m_rightPressed;
	bool m_upPressed;
	bool m_downPressed;

	bool m_rollLeftPressed;
	bool m_rollRightPressed;

	bool m_rightMouseButtonPressed;
public:
	FreeCameraController(Camera* camera);
	virtual ~FreeCameraController();

	void update(float32 deltaSeconds);

	void onKey(int32 key, int32 action);
	void onMouseButton(int32 button, int32 action);
};

