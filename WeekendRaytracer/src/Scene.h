#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Ray.h"

class Scene;
class Material {
public:
  void SetAlbedo(glm::vec3 albedo) {
    albedo_ = albedo;
  }
  void SetMetallic(float metallic) {
    metallic_ = metallic;
  }
  const glm::vec3& GetAlbedo() const {
    return albedo_;
  }
  const float& GetRoughness() const {
    return roughness_;
  }
  const float& GetMetallic() const {
    return metallic_;
  }

private:
  Material(const std::string name, int index)
      : index_(index)
      , name_(name) {}

  glm::vec3 albedo_{1.0f};
  float roughness_ = 1.0f;
  float metallic_ = 0.0f;
  int index_;
  std::string name_{};

  friend class Scene;
  friend class Object;
  friend class Viewport;
};

struct Light {
  glm::vec3 position;
  glm::vec3 color{1.0f};
  float intensity;
};

class Object {
public:
  virtual ~Object() = default;
  virtual void SetPosition(glm::vec3 pos) {
    position = pos;
  }
  virtual void SetMaterial(Material* mat) {
    materialIndex = mat->index_;
  }
  virtual void SetMaterial(std::string materialName, Scene& scene);
  const Material& GetMaterial() const;

  glm::vec3 position{0.0f};
  int materialIndex{-1};

  virtual bool RayIntersection(glm::vec3& rayOrigin, glm::vec3 rayDirection, float& hitDistance) const = 0;

private:
  Scene* scene_ = nullptr;

  friend class Scene;
};

class Sphere : public Object {
public:
  virtual ~Sphere() = default;
  void SetRadius(float radius) {
    radius_ = radius;
  }

public:
  bool RayIntersection(glm::vec3& rayOrigin, glm::vec3 rayDirection, float& hitDistance) const override;

private:
  float radius_ = 0.5f;
};

class Scene {
public:
  void AddObject(std::unique_ptr<Object> object) {
    object->scene_ = this;
    objects_.push_back(std::move(object));
  }
  Material* AddMaterial(std::string name) {
    Material mat(name, materials_.size());
    materials_.push_back(std::move(mat));
    return &materials_.back();
  }

protected:
  std::vector<std::unique_ptr<Object>> objects_;
  std::vector<Light> lights_;
  std::vector<Material> materials_;

  friend class Object;
  friend class Renderer;
  friend class Viewport;
};