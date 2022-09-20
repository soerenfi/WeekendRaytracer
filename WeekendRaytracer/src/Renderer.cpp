#include "Renderer.h"

#include <cstdint>
#include <glm/geometric.hpp>

namespace utils
{
static uint32_t ConvertToRGBA(const glm::vec4& color)
{
    uint8_t r = (uint8_t)(color.r * 255.0f);
    uint8_t g = (uint8_t)(color.g * 255.0f);
    uint8_t b = (uint8_t)(color.b * 255.0f);
    uint8_t a = (uint8_t)(color.a * 255.0f);

    uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
    return result;
}

}  // namespace utils

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (final_image_)
    {
        final_image_->Resize(width, height);
    }
    else
    {
        final_image_ = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }
    delete[] image_data_;
    image_data_ = new uint32_t[width * height];
};

void Renderer::Render()
{
    for (uint32_t y = 0; y < final_image_->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < final_image_->GetWidth(); x++)
        {
            // image_data_[i] = Walnut::Random::UInt();
            glm::vec2 coord{ (float)x / (float)final_image_->GetWidth(), (float)y / (float)final_image_->GetHeight() };
            coord = coord * 2.0f - 1.0f;
            glm::vec4 color = PixelShader(coord);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            image_data_[x + y * final_image_->GetWidth()] = utils::ConvertToRGBA(color);
        }
    }
    final_image_->SetData(image_data_);
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage() const
{
    return final_image_;
}

glm::vec4 Renderer::PixelShader(glm::vec2 coord)
{
    // rayDirection = glm::normalize(rayDirection);
    float radius = 0.5f;
    glm::vec3 rayDirection{ coord.x, coord.y, -1.0f };

    glm::vec3 originCentre = rayOrigin_ - sphereOrigin_;

    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(originCentre, rayDirection);
    float c = glm::dot(originCentre, originCentre) - radius * radius;

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return glm::vec4(0, 0, 0, 1);

    float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);  // Second hit distance (currently unused)

    glm::vec3 hitPoint = rayOrigin_ + rayDirection * closestT;
    glm::vec3 normal = glm::normalize(hitPoint);

    float lightIntensity = glm::max(glm::dot(normal, lightDir_), 0.0f);  // == cos(angle)

    glm::vec3 sphereColor(1, 0, 1);
    sphereColor *= lightIntensity;
    return glm::vec4(sphereColor, 1.0f);
}
