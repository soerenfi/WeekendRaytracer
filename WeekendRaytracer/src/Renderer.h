#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <glm/exponential.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"

class Renderer
{
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

    glm::vec3 reflect(glm::vec3 ray, glm::vec3 normal);

  private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;

    const Scene* m_Scene = nullptr;
    const Camera* m_Camera = nullptr;
    // glm::vec3 sphereOrigin_{ 0.0f, 0.0f, 0.0f };
    static constexpr int16_t kTraceDepth = 1;
    static constexpr float kFarClip = 1000.0f;
};