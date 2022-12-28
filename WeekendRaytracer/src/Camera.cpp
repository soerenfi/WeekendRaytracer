#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

#include "Walnut/Input/Input.h"

using namespace Walnut;

Camera::Camera(float verticalFOV, float nearClip, float farClip)
    : verticalFov_(verticalFOV)
    , nearClip_(nearClip)
    , farClip_(farClip) {
  forwardDirection_ = glm::vec3(0, 0, -1);
  position_ = glm::vec3(0, 0, 3);
}

void Camera::OnUpdate(float ts) {
  glm::vec2 mousePos = Input::GetMousePosition();
  glm::vec2 delta = (mousePos - lastMousePosition_) * 0.002f;
  lastMousePosition_ = mousePos;

  if (!Input::IsMouseButtonDown(MouseButton::Right)) {
    Input::SetCursorMode(CursorMode::Normal);
    return;
  }

  Input::SetCursorMode(CursorMode::Locked);

  bool moved = false;

  constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
  glm::vec3 rightDirection = glm::cross(forwardDirection_, upDirection);

  float speed = 5.0f;

  // Movement
  if (Input::IsKeyDown(KeyCode::W)) {
    position_ += forwardDirection_ * speed * ts;
    moved = true;
  } else if (Input::IsKeyDown(KeyCode::S)) {
    position_ -= forwardDirection_ * speed * ts;
    moved = true;
  }
  if (Input::IsKeyDown(KeyCode::A)) {
    position_ -= rightDirection * speed * ts;
    moved = true;
  } else if (Input::IsKeyDown(KeyCode::D)) {
    position_ += rightDirection * speed * ts;
    moved = true;
  }
  if (Input::IsKeyDown(KeyCode::Q)) {
    position_ -= upDirection * speed * ts;
    moved = true;
  } else if (Input::IsKeyDown(KeyCode::E)) {
    position_ += upDirection * speed * ts;
    moved = true;
  }

  // Rotation
  if (delta.x != 0.0f || delta.y != 0.0f) {
    float pitchDelta = delta.y * GetRotationSpeed();
    float yawDelta = delta.x * GetRotationSpeed();

    glm::quat q = glm::normalize(
      glm::cross(glm::angleAxis(-pitchDelta, rightDirection), glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
    forwardDirection_ = glm::rotate(q, forwardDirection_);

    moved = true;
  }

  if (moved) {
    RecalculateView();
    RecalculateRayDirections();
  }
}

void Camera::OnResize(uint32_t width, uint32_t height) {
  if (width == viewportWidth_ && height == viewportHeight_)
    return;

  viewportWidth_ = width;
  viewportHeight_ = height;

  RecalculateProjection();
  RecalculateRayDirections();
}

float Camera::GetRotationSpeed() {
  return 0.3f;
}

void Camera::RecalculateProjection() {
  projection_ =
    glm::perspectiveFov(glm::radians(verticalFov_), (float)viewportWidth_, (float)viewportHeight_, nearClip_, farClip_);
  inverseProjection_ = glm::inverse(projection_);
}

void Camera::RecalculateView() {
  view_ = glm::lookAt(position_, position_ + forwardDirection_, glm::vec3(0, 1, 0));
  inverseView_ = glm::inverse(view_);
}

void Camera::RecalculateRayDirections() {
  rayDirections_.resize(viewportWidth_ * viewportHeight_);

  for (uint32_t y = 0; y < viewportHeight_; y++) {
    for (uint32_t x = 0; x < viewportWidth_; x++) {
      glm::vec2 coord = {static_cast<float>(x) / static_cast<float>(viewportWidth_),
                         static_cast<float>(y) / static_cast<float>(viewportHeight_)};
      coord = coord * 2.0f - 1.0f;

      glm::vec4 target = inverseProjection_ * glm::vec4(coord.x, coord.y, 1, 1);
      glm::vec3 rayDirection =
        glm::vec3(inverseView_ * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));  // World space
      rayDirections_[x + y * viewportWidth_] = rayDirection;
    }
  }
}