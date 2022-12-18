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

    glm::vec3 color(0.0f);
    float multiplier = .5f;

    int bounces = 2;
    for (int i = 0; i < bounces; i++)
    {
        auto payload = TraceRay(ray);

        if (payload.HitDistance < 0.0f)
        {
            glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
            ray.payload.color = skyColor;
            break;
        }

        Object* hitObject = m_Scene->objects[ray.payload.objectIndex].get();
        glm::vec3 lightDir = glm::normalize(glm::vec3{ -1, -1, -1 });
        // glm::vec3 lightDirection = glm::normalize(m_Scene->lights[0].Position - ray.payload.HitPosition);
        float lightIntensity = glm::max(glm::dot(payload.HitNormal, -lightDir), 0.0f);

        ray.payload.color += (hitObject->getMaterial().getAlbedo()) * lightIntensity * multiplier;
        // float ambient = .2f;
        // float diffuse = .6f * glm::max(glm::dot(ray.payload.HitNormal, lightDirection), 0.0f);  // == cos(angle)
        // float specular =
        //     0.2f * std::pow(std::max(0.f, glm::dot(reflect(lightDirection, ray.payload.HitNormal), ray.Direction)),
        //                     hitObject->getMaterial().getMetallic());
        // ray.payload.color *= (ambient + diffuse + specular);

        multiplier *= 0.5f;
        ray.Origin = payload.HitPosition + payload.HitNormal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction, payload.HitNormal);
        //  +
        // m_Scene->objects.at(ray.payload.objectIndex)->getMaterial().getRoughness() *
        // Walnut::Random::Vec3(-0.5f, 0.5f));
    }
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

        if (intersection && hitDistance > 0.0f && hitDistance < ray.payload.HitDistance)
        {
            ray.payload.HitDistance = hitDistance;
            ray.payload.objectIndex = i;
        }
    }

    if (ray.payload.HitDistance > kFarClip)
        return MissShader(ray);

    return ClosestHitShader(ray);
}

RayPayload Renderer::ClosestHitShader(Ray& ray)
{
    const Object* hitObject = m_Scene->objects[ray.payload.objectIndex].get();

    glm::vec3 origin = ray.Origin - hitObject->Position;
    ray.payload.HitPosition = origin + ray.Direction * ray.payload.HitDistance;
    ray.payload.HitNormal = glm::normalize(ray.payload.HitPosition);
    ray.payload.HitPosition += hitObject->Position;

    return ray.payload;
}

RayPayload Renderer::MissShader(Ray& ray)
{
    ray.payload.HitDistance = -1;
    // ray.payload.albedo = queryhrdi;
    return ray.payload;
}
