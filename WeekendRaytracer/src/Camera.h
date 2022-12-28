#pragma once

#include <glm/glm.hpp>

#include <vector>

class Camera {
public:
  Camera(float verticalFOV, float nearClip, float farClip);  // specify units

  void OnUpdate(float ts);
  void OnResize(uint32_t width, uint32_t height);

  const glm::mat4& GetProjection() const {
    return projection_;
  }
  const glm::mat4& GetInverseProjection() const {
    return inverseProjection_;
  }
  const glm::mat4& GetView() const {
    return view_;
  }
  const glm::mat4& GetInverseView() const {
    return inverseView_;
  }
  // void setPosition(const glm::vec3& position)
  // {
  //     m_Position = position;
  // }
  // void lookAt(const glm::vec3 at)
  // {
  //     m_ForwardDirection = at;
  // }
  const glm::vec3& GetPosition() const {
    return position_;
  }
  const glm::vec3& GetDirection() const {
    return forwardDirection_;
  }

  const std::vector<glm::vec3>& GetRayDirections() const {
    return rayDirections_;
  }

  float GetRotationSpeed();

private:
  void RecalculateProjection();
  void RecalculateView();
  void RecalculateRayDirections();

  glm::mat4 projection_{1.0f};
  glm::mat4 view_{1.0f};
  glm::mat4 inverseProjection_{1.0f};
  glm::mat4 inverseView_{1.0f};

  float verticalFov_ = 45.0f;
  float nearClip_ = 0.1f;
  float farClip_ = 100.0f;

  glm::vec3 position_{0.0f, 0.0f, 0.0f};
  glm::vec3 forwardDirection_{0.0f, 0.0f, 0.0f};

  // Cached ray directions
  std::vector<glm::vec3> rayDirections_;

  glm::vec2 lastMousePosition_{0.0f, 0.0f};

  uint32_t viewportWidth_ = 0, viewportHeight_ = 0;
};