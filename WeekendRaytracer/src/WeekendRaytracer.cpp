#include <cstdint>
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
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        viewport_width_ = ImGui::GetContentRegionAvail().x;
        viewport_height_ = ImGui::GetContentRegionAvail().y;

        auto image = renderer_.GetFinalImage();
        if (image)
            ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
                         ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();

        Render();
    }

    void Render()
    {
        timer_.Reset();

        renderer_.OnResize(viewport_width_, viewport_height_);
        camera_.OnResize(viewport_width_, viewport_height_);
        renderer_.Render(camera_);

        lastRenderTime_ = timer_.ElapsedMillis();
    }

    uint32_t viewport_width_, viewport_height_ = 0;

    Timer timer_;
    float lastRenderTime_{ 0.0f };

    Renderer renderer_;
    Camera camera_;
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