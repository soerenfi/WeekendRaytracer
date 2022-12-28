#include "Renderer.h"

#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

#include <cstdint>
#include <iostream>
#include <limits>

#include "Ray.h"
#include "Scene.h"

namespace utils {
static uint32_t ConvertToRGBA(const glm::vec4& color) {
  auto r = static_cast<uint8_t>(color.r * 255.0f);
  auto g = static_cast<uint8_t>(color.g * 255.0f);
  auto b = static_cast<uint8_t>(color.b * 255.0f);
  auto a = static_cast<uint8_t>(color.a * 255.0f);

  uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
  return result;
}

}  // namespace utils

void Renderer::OnResize(uint32_t width, uint32_t height) {
  if (finalImage_) {
    finalImage_->Resize(width, height);
  } else {
    finalImage_ = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
  }
  delete[] imageData_;
  imageData_ = new uint32_t[width * height];
};
std::shared_ptr<Walnut::Image> Renderer::GetFinalImage() const {
  return finalImage_;
}
// void Renderer::RaySphereIntersection()
// {
// }

void Renderer::Render(const Scene& scene, const Camera& camera) {
  scene_ = &scene;
  camera_ = &camera;

  for (uint32_t y = 0; y < finalImage_->GetHeight(); y++) {
    for (uint32_t x = 0; x < finalImage_->GetWidth(); x++) {
      auto color = PerPixel(x, y);
      imageData_[x + y * finalImage_->GetWidth()] = utils::ConvertToRGBA(color);
    }
  }
  finalImage_->SetData(imageData_);
}
glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
  Ray ray;
  ray.origin = camera_->GetPosition();
  ray.direction = camera_->GetRayDirections()[x + y * finalImage_->GetWidth()];

  glm::vec3 color(0.0f);
  float multiplier = .5f;

  int bounces = 2;
  for (int i = 0; i < bounces; i++) {
    auto payload = TraceRay(ray);

    if (payload.hitDistance < 0.0f) {
      glm::vec3 skyColor = glm::vec3(0.3f, 0.4f, 0.5f);
      ray.payload.color = skyColor;
      break;
    }
    Object* hitObject = scene_->objects_[ray.payload.objectIndex].get();
    const Material& hitMaterial = hitObject->GetMaterial();

    glm::vec3 lightDir = glm::normalize(glm::vec3{-1, -1, -1});
    // glm::vec3 lightDirection = glm::normalize(m_Scene->lights[0].Position - ray.payload.HitPosition);
    float lightIntensity = glm::max(glm::dot(payload.hitNormal, -lightDir), 0.0f);

    ray.payload.color += (hitMaterial.GetAlbedo()) * lightIntensity * multiplier;
    // float ambient = .2f;
    // float diffuse = .6f * glm::max(glm::dot(ray.payload.HitNormal, lightDirection), 0.0f);  // == cos(angle)
    // float specular =
    //     0.2f * std::pow(std::max(0.f, glm::dot(reflect(lightDirection, ray.payload.HitNormal), ray.Direction)),
    //                     hitObject->getMaterial().getMetallic());
    // ray.payload.color *= (ambient + diffuse + specular);

    multiplier *= 0.5f;
    ray.origin = payload.hitPosition + payload.hitNormal * 0.0001f;
    ray.direction = glm::reflect(ray.direction, payload.hitNormal);
    //  +
    // m_Scene->objects.at(ray.payload.objectIndex)->getMaterial().getRoughness() *
    // Walnut::Random::Vec3(-0.5f, 0.5f));
  }
  return glm::vec4(ray.payload.color, 1.0f);
}

glm::vec3 Renderer::Reflect(glm::vec3 ray, glm::vec3 normal) {
  return ray - (glm::vec3(2.0) * glm::dot(ray, normal) * normal);
}

RayPayload Renderer::TraceRay(Ray& ray) {
  for (size_t i = 0; i < scene_->objects_.size(); i++) {  // TODO(soerenfi) Add Object Index
    const Object* object = scene_->objects_[i].get();

    float hitDistance = std::numeric_limits<float>::max();
    bool intersection = object->RayIntersection(ray.origin, ray.direction, hitDistance);

    if (intersection && hitDistance < ray.payload.hitDistance) {
      ray.payload.hitDistance = hitDistance;
      ray.payload.objectIndex = i;
    }
  }

  if (ray.payload.hitDistance > K_FAR_CLIP) {
    return MissShader(ray);
  }

  return ClosestHitShader(ray);
}

RayPayload Renderer::ClosestHitShader(Ray& ray) {
  const Object* hitObject = scene_->objects_[ray.payload.objectIndex].get();

  glm::vec3 origin = ray.origin - hitObject->position;
  ray.payload.hitPosition = origin + ray.direction * ray.payload.hitDistance;
  ray.payload.hitNormal = glm::normalize(ray.payload.hitPosition);
  ray.payload.hitPosition += hitObject->position;

  return ray.payload;
}

RayPayload Renderer::MissShader(Ray& ray) {
  ray.payload.hitDistance = -1.0f;
  // ray.payload.albedo = queryhrdi;
  return ray.payload;
}
