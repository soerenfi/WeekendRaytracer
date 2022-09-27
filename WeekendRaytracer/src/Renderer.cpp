#include "Renderer.h"

#include <cstdint>
#include <glm/geometric.hpp>
#include <limits>

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

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    const glm::vec3& rayOrigin = camera.GetPosition();
    Ray ray;
    ray.Origin = camera.GetPosition();
    for (uint32_t y = 0; y < final_image_->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < final_image_->GetWidth(); x++)
        {
            ray.Direction = camera.GetRayDirections()[x + y * final_image_->GetWidth()];
            // image_data_[i] = Walnut::Random::UInt();
            glm::vec4 color = TraceRay(scene, ray);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            image_data_[x + y * final_image_->GetWidth()] = utils::ConvertToRGBA(color);
        }
    }
    final_image_->SetData(image_data_);
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage() const
{
    return final_image_;
}

glm::vec4 Renderer::TraceRay(const Scene& scene, const Ray& ray)
{
	if (scene.spheres.size() == 0)
		return glm::vec4(0, 0, 0, 1);

	const Sphere* closestSphere = nullptr;
	float hitDistance = std::numeric_limits<float>::max();
	
	for (const Sphere& sphere : scene.spheres)
	{
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
		if (closestT > 0 && closestT < hitDistance)
		{
			hitDistance = closestT;
			closestSphere = &sphere;
		}
	}

	if (closestSphere == nullptr)
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::vec3 origin = ray.Origin - closestSphere->position;
	glm::vec3 hitPoint = origin + ray.Direction * hitDistance;
	glm::vec3 normal = glm::normalize(hitPoint);

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
	float lightIntensity = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle)

	glm::vec3 sphereColor = closestSphere->albedo;
	sphereColor *= lightIntensity;
	return glm::vec4(sphereColor, 1.0f);
}

