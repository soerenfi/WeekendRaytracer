#pragma once

#include <glm/glm.hpp>

struct RayPayload {
  float hitDistance = std::numeric_limits<float>::max();
  glm::vec3 hitPosition;
  glm::vec3 hitNormal;
  int objectIndex{-1};
  // glm::vec3 color = glm::vec3(0.0f);
  //   uint16_t traceDepth = 0;
};
struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};
