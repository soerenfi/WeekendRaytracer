#pragma once

#include <cstdint>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "Walnut/Image.h"
#include "Walnut/Random.h"

class Renderer
{
  public:
    Renderer() = default;

    void OnResize(uint32_t width, uint32_t height)
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

    void Render()
    {
        for (uint32_t y = 0; y < final_image_->GetHeight(); y++)
        {
            for (uint32_t x = 0; x < final_image_->GetWidth(); x++)
            {
                // image_data_[i] = Walnut::Random::UInt();
                glm::vec2 coord{ (float)x / (float)final_image_->GetWidth(),
                                 (float)y / (float)final_image_->GetHeight() };
                coord = coord * 2.0f - 1.0f;
                image_data_[x + y * final_image_->GetWidth()] = PixelShader(coord);
            }
        }
        final_image_->SetData(image_data_);
    }

    std::shared_ptr<Walnut::Image> GetFinalImage() const
    {
        return final_image_;
    }

  private:
    uint32_t PixelShader(glm::vec2 coord)
    {
        glm::vec3 rayOrigin{ 0.0f, 0.0f, -2.0f };
        glm::vec3 rayDirection{ coord.x, coord.y, -1.0f };
        // rayDirection = glm::normalize(rayDirection);
        float radius = 0.5f;

        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(rayOrigin, rayDirection);
        float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant >= 0)
            return 0xffff00ff;
        else
            return 0x0000000;
    }

  private:
    std::shared_ptr<Walnut::Image> final_image_;
    uint32_t* image_data_ = nullptr;
};