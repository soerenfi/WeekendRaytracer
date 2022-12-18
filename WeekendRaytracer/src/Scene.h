#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Ray.h"

class Scene;
class Material
{
  public:
    void setAlbedo(glm::vec3 albedo)
    {
        m_Albedo = albedo;
    }

    const glm::vec3& getAlbedo() const
    {
        return m_Albedo;
    }
    const float& getRoughness() const
    {
        return m_Roughness;
    }
    const float& getMetallic() const
    {
        return m_Metallic;
    }

  private:
    Material(const std::string name, int Index)
    {
        m_Name = name;
        m_Index = Index;
    }

    glm::vec3 m_Albedo{ 1.0f };
    float m_Roughness = 1.0f;
    float m_Metallic = 0.0f;
    int m_Index;
    std::string m_Name{};

    friend class Scene;
    friend class Object;
    friend class Viewport;
};

struct Light
{
    glm::vec3 Position;
    glm::vec3 Color{ 1.0f };
    float Intensity;
};

class Object
{
  public:
    virtual void setPosition(glm::vec3 pos)
    {
        Position = pos;
    }
    virtual void setMaterial(Material* mat)
    {
        MaterialIndex = mat->m_Index;
    }
    virtual void setMaterial(std::string materialName, Scene& scene);

    const Material& getMaterial() const;
    glm::vec3 Position{ 0.0f };
    int MaterialIndex{ -1 };

  public:
    virtual bool rayIntersection(const glm::vec3& rayOrigin, const glm::vec3 rayDirection,
                                 float& hitDistance) const = 0;

  private:
    Scene* m_Scene = nullptr;

    friend class Scene;
};

class Sphere : public Object
{
  public:
    void setRadius(float radius)
    {
        Radius = radius;
    }

    float Radius = 0.5f;

  public:
    virtual bool rayIntersection(const glm::vec3& rayOrigin, const glm::vec3 rayDirection,
                                 float& hitDistance) const override;
};

class Scene
{
  public:
    void addObject(std::unique_ptr<Object> object)
    {
        object->m_Scene = this;
        objects.push_back(std::move(object));
    }
    Material* addMaterial(std::string name)
    {
        Material mat(name, materials.size());
        materials.push_back(std::move(mat));
        return &materials.back();
    }

  protected:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Light> lights;
    std::vector<Material> materials;

    friend class Object;
    friend class Renderer;
    friend class Viewport;
};