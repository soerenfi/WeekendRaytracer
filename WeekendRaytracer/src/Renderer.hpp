#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "Walnut/Image.h"
#include "Walnut/Random.h"

class Renderer
{
  public:
    Renderer() = default;

    void OnResize(uint32_t width, uint32_t height);
    void Render();
    std::shared_ptr<Walnut::Image> GetFinalImage() const;

  private:
    glm::vec4 PixelShader(glm::vec2 coord);

  private:
    std::shared_ptr<Walnut::Image> final_image_;
    uint32_t* image_data_ = nullptr;

    glm::vec3 rayOrigin_{ 0.0f, 0.0f, -1.0f };
    glm::vec3 sphereOrigin_{ 0, 0, 0 };
    glm::vec3 lightDir_ = glm::normalize(glm::vec3(-1, -1, -1));
};