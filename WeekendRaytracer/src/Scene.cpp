#include "Scene.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

void Object::setMaterial(std::string materialName, Scene& scene)
{
    std::vector<Material>::iterator it =
        std::find_if(scene.materials.begin(), scene.materials.end(),
                     [&](const Material& m) -> bool { return m.m_Name == materialName; });
    if (it == scene.materials.end())
        throw std::logic_error("Material unknown");
    MaterialIndex = it->m_Index;
}

const Material& Object::getMaterial() const
{
    if (!m_Scene)
        throw std::runtime_error("Scene of Object not set!");
    return m_Scene->materials.at(MaterialIndex);
}

bool Sphere::rayIntersection(const glm::vec3& rayOrigin, const glm::vec3 rayDirection, float& hitDistance) const
{
    RayPayload payload;
    glm::vec3 origin = rayOrigin - Position;

    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * glm::dot(origin, rayDirection);
    float c = glm::dot(origin, origin) - Radius * Radius;

    // Quadratic forumula discriminant:
    // b^2 - 4ac

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return false;

    // Quadratic formula:
    // (-b +- sqrt(discriminant)) / 2a

    // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
    float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    if (closestT > 0.0f)
    {
        hitDistance = closestT;
        return true;
    }
    return false;
}