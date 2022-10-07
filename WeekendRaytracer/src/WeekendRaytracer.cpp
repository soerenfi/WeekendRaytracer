#include <cstdint>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "Camera.h"
#include "Renderer.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"

using namespace Walnut;

class Viewport : public Walnut::Layer
{
  public:
    Viewport() : camera_(45.0f, 0.1f, 100.0f)
    {
        camera_.setPosition(glm::vec3(0.f, 0.f, 3.f));
        // camera_.lookAt(glm::vec3(0.f, 0.f, 0.f));
        {
            std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
            sphere->setPosition({ 0.0f, 0.0f, 0.0f });
            sphere->setRadius(0.5f);

            Material metal;
            metal.setAlbedo({ 1.0f, 0.0f, 1.0f });
            sphere->setMaterial(metal);
            scene_.addObject(std::move(sphere));
        }

        // {
        //     std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
        //     sphere.setPosition({ 1.0f, 0.0f, -5.0f });
        //     sphere.setRadius(1.0f);

        //     Material metal;
        //     metal.setAlbedo({ 1.0f, 0.0f, 1.0f });
        //     sphere.setMaterial(metal);
        //     scene_.addObject(sphere);
        // }
        Light light;
        light.position_ = glm::vec3{ -1.0f, -1.0f, -1.0f };
        scene_.lights.push_back(light);
    }
    virtual void OnUpdate(float ts) override
    {
        camera_.OnUpdate(ts);
    }
    virtual void OnUIRender() override
    {
        ImGui::Begin("Settings");
        ImGui::Text("Last Render: %.3fms", lastRenderTime_);
        if (ImGui::Button("Render"))
        {
            Render();
        }

        ImGui::Begin("Scene");
        for (size_t i = 0; i < scene_.lights.size(); i++)
        {
            ImGui::PushID(i);

            Light& light = scene_.lights[i];
            ImGui::DragFloat3("Position", glm::value_ptr(light.position_), 0.1f);
            ImGui::DragFloat("Intensity", &light.intensity_, 0.1f);
            // ImGui::ColorEdit3("Albedo", glm::value_ptr(light.color_));

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::End();
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        viewport_width_ = ImGui::GetContentRegionAvail().x;
        viewport_height_ = ImGui::GetContentRegionAvail().y;

        auto image = renderer_.GetFinalImage();
        if (image)
            ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
                         ImVec2(0, 1), ImVec2(1, 0));

        // ImGui::ShowDemoWindow();

        ImGui::End();
        ImGui::PopStyleVar(

        );

        Render();
    }

    void Render()
    {
        timer_.Reset();

        renderer_.OnResize(viewport_width_, viewport_height_);
        camera_.OnResize(viewport_width_, viewport_height_);
        renderer_.Render(scene_, camera_);

        lastRenderTime_ = timer_.ElapsedMillis();
    }

    uint32_t viewport_width_, viewport_height_ = 0;

    Timer timer_;
    float lastRenderTime_{ 0.0f };

    Renderer renderer_;
    Camera camera_;
    Scene scene_;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Weekend Raytracer";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<Viewport>();
    app->SetMenubarCallback([app]() {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
    return app;
}