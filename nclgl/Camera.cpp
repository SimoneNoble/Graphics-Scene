#include "Camera.h"
#include "Window.h"
#include <algorithm>
#include <iostream>
using namespace std;

void Camera::UpdateCamera(int &startTime, int &toggleCamera, float dt) {

	if (toggleCamera != 1) {
		Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

		Vector3 forward = rotation * Vector3(0, 0, -1);
		Vector3 right = rotation * Vector3(1, 0, 0);

		float speed = 300.0f * dt;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_3)) {
			toggleCamera = 1;
			return;
		}

		if (startTime != 2) {
			timeKeeper = 0;
			startTime = 2;
		}

		timeKeeper += dt;

		// pitch and yaw
		yaw += 0.06f;
		pitch += 0.05f;

		pitch = std::min(pitch, 90.0f);
		pitch = std::max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}

		// linear movement
		position += (forward * 0.9);
		position += (right * 0.1);

		// takes roughly 55-60 seconds to load:
		if (timeKeeper > 60.0f) {
			yaw += 0.4f;
		}

		if (timeKeeper > 65.0f) {
			pitch -= 0.15f;
			position += (forward * 5.0);
			yaw -= 0.15f;
		}

		if (timeKeeper > 70.0f) {
			pitch = 0.0f;
			yaw = 0.005f;
			position -= (forward * 5.9);
		}

		cout << timeKeeper << endl;

		return;

	}

	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);

	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float speed = 1000 * dt;
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
		position.y += speed;
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y -= speed;
	}
}
Matrix4 Camera::BuildViewMatrix() {
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) * Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) * Matrix4::Translation(-position);
}