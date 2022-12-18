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
    Viewport() : m_Camera(45.0f, 0.1f, 100.0f)
    {
        m_Camera.setPosition(glm::vec3(0.f, 0.f, 3.f));
        // m_Camera.lookAt(glm::vec3(0.f, 0.f, 0.f));
        {
            Material* metal = m_Scene.addMaterial("Metal1");
            metal->setAlbedo({ 0.8f, 0.0f, 0.0f });
            Material* metal2 = m_Scene.addMaterial("Metal2");
            metal->setAlbedo({ 0.0f, 0.0f, 0.8f });

            std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
            sphere->setPosition({ 0.0f, 0.0f, 0.0f });
            sphere->setRadius(0.5f);
            sphere->setMaterial("Metal1", m_Scene);
            m_Scene.addObject(std::move(sphere));
        }

        {
            std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
            sphere->setPosition({ 1.0f, 0.0f, -5.0f });
            sphere->setRadius(1.0f);
            sphere->setMaterial("Metal2", m_Scene);
            m_Scene.addObject(std::move(sphere));
        }

        Light light;
        light.Position = glm::vec3{ -1.0f, -1.0f, -1.0f };
        light.Intensity = 1;
        m_Scene.lights.push_back(light);
    }
    virtual void OnUpdate(float ts) override
    {
        m_Camera.OnUpdate(ts);
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
        for (size_t i = 0; i < m_Scene.objects.size(); i++)
        {
            ImGui::PushID(i);

            Object* object = m_Scene.objects[i].get();
            ImGui::DragFloat3("Position", glm::value_ptr(object->Position), 0.1f);
            ImGui::DragInt("Material", &object->MaterialIndex, 1.0f, 0, (int)m_Scene.materials.size() - 1);

            ImGui::Separator();
            ImGui::PopID();
        }

        for (size_t i = 0; i < m_Scene.lights.size(); i++)
        {
            ImGui::PushID(i);

            Light& light = m_Scene.lights[i];
            ImGui::DragFloat3("Position", glm::value_ptr(light.Position), 0.1f);
            ImGui::DragFloat("Intensity", &light.Intensity, 0.1f);
            //   ImGui::ColorEdit3("Albedo", glm::value_ptr(light.Color));

            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::End();
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        viewport_width_ = ImGui::GetContentRegionAvail().x;
        viewport_height_ = ImGui::GetContentRegionAvail().y;

        auto image = m_Renderer.GetFinalImage();
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

        m_Renderer.OnResize(viewport_width_, viewport_height_);
        m_Camera.OnResize(viewport_width_, viewport_height_);
        m_Renderer.Render(m_Scene, m_Camera);

        lastRenderTime_ = timer_.ElapsedMillis();
    }

    uint32_t viewport_width_, viewport_height_ = 0;

    Timer timer_;
    float lastRenderTime_{ 0.0f };

    Renderer m_Renderer;
    Camera m_Camera;
    Scene m_Scene;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Weekend Raytracer";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<Viewport>();
    app->SetMenubarCallback(
        [app]()
        {
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