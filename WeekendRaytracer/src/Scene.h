#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Ray.h"

class Material
{
  public:
    const glm::vec3& albedo() const
    {
        return albedo_;
    }
    void setAlbedo(glm::vec3 albedo)
    {
        albedo_ = albedo;
    }

  private:
    glm::vec3 albedo_{ 1.0f };
    float specular_{ 0.1f };
};

struct Light
{
    glm::vec3 position_;
    glm::vec3 color_{ 1.0f };
    float intensity_;
};

class Object
{
  public:
    virtual const glm::vec3& position() const
    {
        return position_;
    }
    virtual const Material& material() const
    {
        return material_;
    }
    virtual void setPosition(glm::vec3 pos)
    {
        position_ = pos;
    }
    virtual void setMaterial(Material mat)
    {
        material_ = mat;
    }

  protected:
    glm::vec3 position_{ 0.0f };
    Material material_;

  public:
    virtual bool rayIntersection(const glm::vec3& rayOrigin, const glm::vec3 rayDirection, float& hitDistance) const = 0;
};

class Sphere : public Object
{
  public:
    void setRadius(float radius)
    {
        radius_ = radius;
    }

  private:
    float radius_ = 0.5f;

  public:
    virtual bool rayIntersection(const glm::vec3& rayOrigin, const glm::vec3 rayDirection,
                                 float& hitDistance) const override
    {
        RayPayload payload;
        glm::vec3 origin = rayOrigin - position_;

        float a = glm::dot(rayDirection, rayDirection);
        float b = 2.0f * glm::dot(origin, rayDirection);
        float c = glm::dot(origin, origin) - radius_ * radius_;

        // Quadratic forumula discriminant:
        // b^2 - 4ac

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f)
            return false;

        // Quadratic formula:
        // (-b +- sqrt(discriminant)) / 2a

        // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
        float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        // if (closestT > 0.0f && closestT < ray.payload.hitDistance)
        // {
        hitDistance = closestT;
        // }
        return true;
    }
};

class Scene
{
  public:
    void addObject(std::unique_ptr<Object> object)
    {
        objects.push_back(std::move(object));
    }

  public:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Light> lights;
};