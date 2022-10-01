#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Material
{
    glm::vec3 albedo{ 1.0f };
    float specular{ 1.0f };
};

struct Sphere
{
    glm::vec3 position{ 0.0f };
    float radius = 0.5f;

    Material material;

    bool RayIntersect(const glm::vec3& origin, const glm::vec3& direction, float firstHit)
    {
    }
};

struct Scene
{
    std::vector<Sphere> spheres;
};