#pragma once

#include <glm/exponential.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"

class Renderer {
public:
  Renderer() = default;

  void OnResize(uint32_t width, uint32_t height);
  void Render(const Scene& scene, const Camera& cam);
  std::shared_ptr<Walnut::Image> GetFinalImage() const;

private:
  glm::vec4 PerPixel(uint32_t x, uint32_t y);
  RayPayload TraceRay(Ray& ray);
  RayPayload ClosestHitShader(Ray& ray);
  RayPayload MissShader(Ray& ray);
  static glm::vec3 Reflect(glm::vec3 ray, glm::vec3 normal);

  std::shared_ptr<Walnut::Image> finalImage_;
  uint32_t* imageData_ = nullptr;

  const Scene* scene_ = nullptr;
  const Camera* camera_ = nullptr;
  // glm::vec3 sphereOrigin_{ 0.0f, 0.0f, 0.0f };
  static constexpr int16_t K_TRACE_DEPTH = 2;
  static constexpr float K_FAR_CLIP = 1000.0f;
};