#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Material
{
    glm::vec3 albedo{ 1.0f };
    float specular{ 0.1f };
};

struct Light
{
    glm::vec3 position;
    glm::vec3 color{ 1.0f };
    float intensity;
};

struct Sphere
{
    glm::vec3 position{ 0.0f };
    float radius = 0.5f;

    Material material;

    bool RayIntersect(const glm::vec3& origin, const glm::vec3& direction, float firstHit)
    {
        return true;
    }
};

struct Scene
{
    std::vector<Sphere> spheres;
    std::vector<Light> lights;
};