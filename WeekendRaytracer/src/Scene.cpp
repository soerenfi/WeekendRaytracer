#include "Scene.h"

#include <glm/gtx/string_cast.hpp>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Ray.h"

void Object::setMaterial(std::string materialName, Scene& scene) {
  auto it = std::find_if(scene.materials_.begin(), scene.materials_.end(), [&](const Material& mat) -> bool {
    return mat.name_ == materialName;
  });
  if (it == scene.materials_.end()) {
    throw std::logic_error("Material unknown");  // TODO (soeren) does not work
  }
  materialIndex = it->index_;
}

// const Material& Object::getMaterial() const {
//   if (scene_ == nullptr) {
//     throw std::runtime_error("Scene of Object not set!");
//   }
//   return scene_->materials_.at(materialIndex);
// }

bool Sphere::rayIntersection(const Ray& ray, float& hitDistance) const {
  glm::vec3 origin = ray.origin - position;

  float a = glm::dot(ray.direction, ray.direction);
  float b = 2.0f * glm::dot(origin, ray.direction);
  float c = glm::dot(origin, origin) - radius * radius;

  // Quadratic forumula discriminant:
  // b^2 - 4ac

  float discriminant = b * b - 4.0f * a * c;
  // std::cout << "pos2 " << glm::to_string(Position) << std::endl;
  // std::cout << "disc2 " << discriminant << std::endl;
  if (discriminant < 0.0f) {
    return false;
  }

  // Quadratic formula:
  // (-b +- sqrt(discriminant)) / 2a

  // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
  float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
  if (closestT > 0.0f && closestT < hitDistance) {
    hitDistance = closestT;
    // payload.hitDistance = closestT;
    // payload.objectIndex=  index;
    return true;
  }
  return false;
}