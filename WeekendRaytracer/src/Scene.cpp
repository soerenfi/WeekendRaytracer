#include "Scene.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

void Object::SetMaterial(std::string materialName, Scene& scene) {
  auto it = std::find_if(scene.materials_.begin(), scene.materials_.end(), [&](const Material& mat) -> bool {
    return mat.name_ == materialName;
  });
  if (it == scene.materials_.end()) {
    throw std::logic_error("Material unknown");
  }
  materialIndex = it->index_;
}

const Material& Object::GetMaterial() const {
  if (scene_ == nullptr) {
    throw std::runtime_error("Scene of Object not set!");
  }
  return scene_->materials_.at(materialIndex);
}

bool Sphere::RayIntersection(glm::vec3& rayOrigin, glm::vec3 rayDirection, float& hitDistance) const {
  RayPayload payload;
  glm::vec3 origin = rayOrigin - position;

  float a = glm::dot(rayDirection, rayDirection);
  float b = 2.0f * glm::dot(origin, rayDirection);
  float c = glm::dot(origin, origin) - radius_ * radius_;

  // Quadratic forumula discriminant:
  // b^2 - 4ac

  float discriminant = b * b - 4.0f * a * c;
  if (discriminant < 0.0f) {
    return false;
  }

  // Quadratic formula:
  // (-b +- sqrt(discriminant)) / 2a

  // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
  float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
  if (closestT > 0.0f) {
    hitDistance = closestT;
    return true;
  }
  return false;
}