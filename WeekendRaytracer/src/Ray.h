#pragma once

#include <glm/glm.hpp>

struct RayPayload {
  glm::vec3 hitPosition;
  glm::vec3 hitNormal;
  float     hitDistance = std::numeric_limits<float>::max();
  int       objectIndex{-1};
};
struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};
