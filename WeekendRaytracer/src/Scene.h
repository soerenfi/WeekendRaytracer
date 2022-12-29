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
  float roughness = 1.0f;
  float metallic = 0.0f;

  // private:
  //   Material(const std::string name, int index)
  //       : index_(index)
  //       , name_(name) {}

  int index_{-1};
  std::string name_{};

  friend class Object;
  friend class Scene;
};

class Object {
public:
  virtual ~Object() = default;
  // virtual void SetPosition(glm::vec3 pos) {
  //   Position = pos;
  // }
  // virtual void SetMaterial(Material* mat) {
  //   materialIndex = mat->index_;
  // }
  // virtual void SetMaterial(std::string materialName, Scene& scene);
  // const Material& GetMaterial() const;

  glm::vec3 position{0.0f};
  int materialIndex{-1};

  virtual bool RayIntersection(const Ray& ray, float& hitDistance) const = 0;

private:
  Scene* scene_ = nullptr;

  friend class Scene;
};

class Sphere : public Object {
public:
  virtual ~Sphere() = default;
  bool RayIntersection(const Ray& ray, float& hitDistance) const override;

  float radius_ = 0.5f;
};

// struct Scene {
//   std::vector<Sphere> Spheres;
//   std::vector<Material> Materials;
// };

class Scene {
public:
  void AddObject(std::unique_ptr<Object> object) {
    object->scene_ = this;
    // objects_.push_back(std::move(object));
  }
  Material* AddMaterial(std::string name) {
    // Material mat(name, materials_.size());
    // materials_.push_back(std::move(mat));
    return &materials_.back();
  }

public:
  // std::vector<std::unique_ptr<Object>> objects_;
  std::vector<Sphere> spheres_;
  // std::vector<Light> lights_;
  std::vector<Material> materials_;

  friend class Object;
  friend class Renderer;
  friend class Viewport;
};