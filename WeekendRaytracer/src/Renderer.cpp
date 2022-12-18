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
    if (m_FinalImage)
    {
        m_FinalImage->Resize(width, height);
    }
    else
    {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }
    delete[] m_ImageData;
    m_ImageData = new uint32_t[width * height];
};
std::shared_ptr<Walnut::Image> Renderer::GetFinalImage() const
{
    return m_FinalImage;
}
// void Renderer::RaySphereIntersection()
// {
// }

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    m_Scene = &scene;
    m_Camera = &camera;

    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            auto color = PerPixel(x, y);
            m_ImageData[x + y * m_FinalImage->GetWidth()] = utils::ConvertToRGBA(color);
        }
    }
    m_FinalImage->SetData(m_ImageData);
}
glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
    Ray ray;
    ray.Origin = m_Camera->GetPosition();
    ray.Direction = m_Camera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

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

    for (size_t i = 0; i < m_Scene->objects.size(); i++)
    {
        const Object* object = m_Scene->objects[i].get();

        float hitDistance = -1;
        bool intersection = object->rayIntersection(ray.Origin, ray.Direction, hitDistance);

        if (intersection && hitDistance < ray.payload.hitDistance)
        {
            ray.payload.hitDistance = hitDistance;
            ray.payload.objectIndex = i;
        }
    }

    if (ray.payload.hitDistance > kFarClip)
        return MissShader(ray);

    return ClosestHitShader(ray);
}

RayPayload Renderer::ClosestHitShader(Ray& ray)
{
    const Object* hitObject = m_Scene->objects[ray.payload.objectIndex].get();

    glm::vec3 origin = ray.Origin - hitObject->Position;
    ray.payload.hitPosition = origin + ray.Direction * ray.payload.hitDistance;
    ray.payload.hitNormal = glm::normalize(ray.payload.hitPosition);
    ray.payload.hitPosition += hitObject->Position;
    ray.payload.color = (hitObject->getMaterial().getAlbedo());
    // ray.payload.albedo += (hitSphere.albedo * glm::vec3(0.2));

    glm::vec3 lightDirection = glm::normalize(m_Scene->lights[0].Position - ray.payload.hitPosition);
    float ambient = .1f;
    float diffuse = .7f * glm::max(glm::dot(ray.payload.hitNormal, lightDirection), 0.0f);  // == cos(angle)
    float specular =
        .2f * std::pow(std::max(0.f, glm::dot(reflect(lightDirection, ray.payload.hitNormal), ray.Direction)),
                       hitObject->getMaterial().getMetallic());
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
