#pragma once

#include <glm/glm.hpp>

#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Scheduler.h"
#include "Walnut/Image.h"

class Renderer {
public:
  struct Settings {
    bool Accumulate = true;
  };

public:
  Renderer() = default;

  void OnResize(uint32_t width, uint32_t height);
  void Render(const Scene& scene, const Camera& camera);

  std::shared_ptr<Walnut::Image> GetFinalImage() const {
    return finalImage_;
  }

  void ResetFrameIndex() {
    frameIndex_ = 1;
  }
  Settings& GetSettings() {
    return settings_;
  }

private:
  glm::vec4 PerPixel(uint32_t x, uint32_t y);  // RayGen

  RayPayload TraceRay(const Ray& ray);
  RayPayload ClosestHit(const Ray& ray, RayPayload payload);
  RayPayload Miss(const Ray& ray, RayPayload payload);

private:
  std::shared_ptr<Walnut::Image> finalImage_;
  Settings                       settings_;

  std::vector<uint32_t> imageHorizontalIter_, imageVerticalIter_;

  ThreadPool threads_;

  const Scene*  activeScene_      = nullptr;
  const Camera* activeCamera_     = nullptr;
  uint32_t*     imageData_        = nullptr;
  glm::vec4*    accumulationData_ = nullptr;

  uint32_t frameIndex_ = 1;
};