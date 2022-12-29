#include "Renderer.h"

#include <glm/gtx/string_cast.hpp>

#include <execution>

#include "Walnut/Random.h"

namespace Utils {

static uint32_t ConvertToRGBA(const glm::vec4& color) {
  uint8_t r = (uint8_t)(color.r * 255.0f);
  uint8_t g = (uint8_t)(color.g * 255.0f);
  uint8_t b = (uint8_t)(color.b * 255.0f);
  uint8_t a = (uint8_t)(color.a * 255.0f);

  uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
  return result;
}

}  // namespace Utils

void Renderer::OnResize(uint32_t width, uint32_t height) {
  if (finalImage_) {
    // No resize necessary
    if (finalImage_->GetWidth() == width && finalImage_->GetHeight() == height)
      return;

    finalImage_->Resize(width, height);
  } else {
    finalImage_ = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
  }

  delete[] imageData_;
  imageData_ = new uint32_t[width * height];

  delete[] accumulationData_;
  accumulationData_ = new glm::vec4[width * height];

  imageHorizontalIter_.resize(width);
  imageVerticalIter_.resize(height);
  for (uint32_t i = 0; i < width; i++) {
    imageHorizontalIter_[i] = i;
  }
  for (uint32_t i = 0; i < height; i++) {
    imageVerticalIter_[i] = i;
  }
}

void Renderer::Render(const Scene& scene, const Camera& camera) {
  activeScene_  = &scene;
  activeCamera_ = &camera;

  if (frameIndex_ == 1) {
    memset(accumulationData_, 0, finalImage_->GetWidth() * finalImage_->GetHeight() * sizeof(glm::vec4));
  }

#define MT 1
#if MT
  std::for_each(std::execution::par, imageVerticalIter_.begin(), imageVerticalIter_.end(), [this](uint32_t y) {
    std::for_each(std::execution::par, imageHorizontalIter_.begin(), imageHorizontalIter_.end(), [this, y](uint32_t x) {
      glm::vec4 color = PerPixel(x, y);
      accumulationData_[x + y * finalImage_->GetWidth()] += color;

      glm::vec4 accumulatedColor = accumulationData_[x + y * finalImage_->GetWidth()];
      accumulatedColor /= (float)frameIndex_;

      accumulatedColor                            = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
      imageData_[x + y * finalImage_->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
    });
  });

#else

  for (uint32_t y = 0; y < finalImage_->GetHeight(); y++) {
    for (uint32_t x = 0; x < finalImage_->GetWidth(); x++) {
      glm::vec4 color = PerPixel(x, y);
      accumulationData_[x + y * finalImage_->GetWidth()] += color;

      glm::vec4 accumulatedColor = accumulationData_[x + y * finalImage_->GetWidth()];
      accumulatedColor /= (float)m_FrameIndex;

      accumulatedColor                            = glm::clamp(accumulatedColor, glm::vec4(0.0f), glm::vec4(1.0f));
      imageData_[x + y * finalImage_->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
    }
  }
#endif

  finalImage_->SetData(imageData_);

  if (settings_.Accumulate) {
    frameIndex_++;
  } else {
    frameIndex_ = 1;
  }
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
  Ray ray;
  ray.origin    = activeCamera_->GetPosition();
  ray.direction = activeCamera_->GetRayDirections()[x + y * finalImage_->GetWidth()];

  glm::vec3 color(0.0f);
  float     multiplier = 1.0f;

  int bounces = 5;
  for (int i = 0; i < bounces; i++) {
    RayPayload payload = TraceRay(ray);
    if (payload.hitDistance < 0.0f) {
      glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
      color += skyColor * multiplier;
      break;
    }

    glm::vec3 lightDir       = glm::normalize(glm::vec3(-1, -1, -1));
    float     lightIntensity = glm::max(glm::dot(payload.hitNormal, -lightDir), 0.0f);  // == cos(angle)

    const Object*   object   = activeScene_->objects_[payload.objectIndex].get();
    const Material& material = activeScene_->materials_[object->materialIndex];

    glm::vec3 sphereColor = material.albedo;
    sphereColor *= lightIntensity;
    color += sphereColor * multiplier;

    multiplier *= 0.5f;
    ray.origin = payload.hitPosition + payload.hitNormal * 0.0001f;
    ray.direction =
      glm::reflect(ray.direction, payload.hitNormal + material.roughness * Walnut::Random::Vec3(-0.5f, 0.5f));
  }

  return glm::vec4(color, 1.0f);
}

RayPayload Renderer::TraceRay(const Ray& ray) {
  RayPayload payload;
  // int        closestSphere = -1;
  float hitDistance   = std::numeric_limits<float>::max();
  payload.hitDistance = std::numeric_limits<float>::max();

  for (size_t i = 0; i < activeScene_->objects_.size(); i++) {
    const Object* object = activeScene_->objects_[i].get();

    bool intersection = object->rayIntersection(ray, hitDistance);
    if (!intersection) {
      continue;
    }
    payload.hitDistance = hitDistance;
    payload.objectIndex = i;
  }

  if (payload.objectIndex < 0)
    return Miss(ray, payload);

  return ClosestHit(ray, payload);
}

RayPayload Renderer::ClosestHit(const Ray& ray, RayPayload payload) {
  const Object* closestObject = activeScene_->objects_[payload.objectIndex].get();

  glm::vec3 origin    = ray.origin - closestObject->position;
  payload.hitPosition = origin + ray.direction * payload.hitDistance;
  payload.hitNormal   = glm::normalize(payload.hitPosition);

  payload.hitPosition += closestObject->position;

  return payload;
}

RayPayload Renderer::Miss(const Ray& ray, RayPayload payload) {
  payload.hitDistance = -1.0f;
  return payload;
}
