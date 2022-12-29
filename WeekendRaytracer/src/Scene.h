#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

class Scene;
struct Ray;
struct RayPayload;

struct Material {
  glm::vec3 albedo{1.0f};
  float     roughness = 1.0f;
  float     metallic  = 0.0f;

private:
  Material(const std::string name, int index)
      : index_(index)
      , name_(name) {}

  int         index_{-1};
  std::string name_{};

  friend class Object;
  friend class Scene;
};

class Object {
public:
  virtual ~Object() = default;
  virtual void setMaterial(std::string materialName, Scene& scene);

  Material& getMaterial() const;

  glm::vec3 position{0.0f};
  int       materialIndex{-1};

  virtual bool rayIntersection(const Ray& ray, float& hitDistance) const = 0;

private:
  Scene* scene_ = nullptr;

  friend class Scene;
};

class Sphere : public Object {
public:
  virtual ~Sphere() = default;
  bool rayIntersection(const Ray& ray, float& hitDistance) const override;

  float radius = 0.5f;
};

class Scene {
public:
  void addObject(std::unique_ptr<Object> object) {
    object->scene_ = this;
    objects_.push_back(std::move(object));
  }
  Material* addMaterial(std::string name) {
    Material mat(name, materials_.size());
    materials_.push_back(std::move(mat));
    return &materials_.back();
  }

public:
  std::vector<std::unique_ptr<Object>> objects_;
  // std::vector<Light> lights_;
  std::vector<Material> materials_;

  friend class Object;
  friend class Renderer;
  friend class Viewport;
};