#include "Renderer.h"

#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <limits>

#include "Ray.h"
#include "Scene.h"

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
std::shared_ptr<Walnut::Image> Renderer::GetFinalImage() const
{
    return final_image_;
}
// void Renderer::RaySphereIntersection()
// {
// }

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    scene_ = &scene;
    camera_ = &camera;

    for (uint32_t y = 0; y < final_image_->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < final_image_->GetWidth(); x++)
        {
            auto color = PerPixel(x, y);
            image_data_[x + y * final_image_->GetWidth()] = utils::ConvertToRGBA(color);
        }
    }
    final_image_->SetData(image_data_);
}
glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = camera_->GetPosition();
    ray.Direction = camera_->GetRayDirections()[x + y * final_image_->GetWidth()];

    auto payload = TraceRay(ray);

    // if (payload.hitDistance > 1000.f)
    // {
    //     return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    // }

    return glm::vec4(ray.payload.color, 1.0f);
}
glm::vec3 Renderer::reflect(glm::vec3 ray, glm::vec3 normal)
{
    return ray - (glm::vec3(2.0) * glm::dot(ray, normal) * normal);
}

RayPayload Renderer::TraceRay(Ray& ray)
{
    if (ray.payload.traceDepth > kTraceDepth)
    {
        return ray.payload;
    }

    int32_t index = -1;

    for (size_t i = 0; i < scene_->spheres.size(); i++)
    {
        const Sphere& sphere = scene_->spheres[i];
        glm::vec3 origin = ray.Origin - sphere.position;

        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

        // Quadratic forumula discriminant:
        // b^2 - 4ac

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f)
            continue;

        // Quadratic formula:
        // (-b +- sqrt(discriminant)) / 2a

        // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a); // Second hit distance (currently unused)
        float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        if (closestT > 0.0f && closestT < ray.payload.hitDistance)
        {
            ray.payload.hitDistance = closestT;
            ray.payload.objectIndex = i;
        }
    }

    if (ray.payload.hitDistance > kFarClip)
        return MissShader(ray);

    return ClosestHitShader(ray);
}

RayPayload Renderer::ClosestHitShader(Ray& ray)
{
    const Sphere& hitSphere = scene_->spheres[ray.payload.objectIndex];

    glm::vec3 origin = ray.Origin - hitSphere.position;
    ray.payload.hitPosition = origin + ray.Direction * ray.payload.hitDistance;
    ray.payload.hitNormal = glm::normalize(ray.payload.hitPosition);
    ray.payload.hitPosition += hitSphere.position;
    ray.payload.color = (hitSphere.material.albedo);
    // ray.payload.albedo += (hitSphere.albedo * glm::vec3(0.2));

    glm::vec3 lightDirection = glm::normalize(scene_->lights[0].position - ray.payload.hitPosition);
    float ambient = .1f;
    float diffuse = .7f * glm::max(glm::dot(ray.payload.hitNormal, lightDirection), 0.0f);  // == cos(angle)
    float specular =
        .2f * std::pow(std::max(0.f, glm::dot(reflect(lightDirection, ray.payload.hitNormal), ray.Direction)),
                       hitSphere.material.specular);
    ray.payload.color *= (ambient + diffuse);
    // ray.payload.color *= (ambient + diffuse + specular);

    // Ray bounceRay;
    // bounceRay.payload = ray.payload;
    // bounceRay.payload.traceDepth = ray.payload.traceDepth + 1;
    // bounceRay.Origin = ray.payload.hitPosition;
    // bounceRay.Direction = ray.payload.hitNormal;
    // bounceRay.Direction =
    //     ray.Direction - (glm::vec3(2.0) * glm::dot(ray.Direction, ray.payload.hitNormal) * ray.payload.hitNormal);

    // return TraceRay(bounceRay);
    return ray.payload;
}

RayPayload Renderer::MissShader(Ray& ray)
{
    ray.payload.color = glm::vec3(.1f, .1f, .1f);
    // ray.payload.albedo = queryhrdi;
    return ray.payload;
}
