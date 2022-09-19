#include <cstdint>
#include <memory>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
  public:
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

        viewportWidth_ = ImGui::GetContentRegionAvail().x;
        viewportHeight_ = ImGui::GetContentRegionAvail().y;

        if (image_)
            ImGui::Image(image_->GetDescriptorSet(), { (float)image_->GetWidth(), (float)image_->GetHeight() });

        ImGui::End();
        ImGui::PopStyleVar();

        Render();
    }

    void Render()
    {
        timer_.Reset();
        if (!image_ || viewportHeight_ != image_->GetWidth() || viewportWidth_ != image_->GetWidth())
        {
            image_ = std::make_shared<Image>(viewportWidth_, viewportHeight_, ImageFormat::RGBA);
            delete[] imageData_;
            imageData_ = new uint32_t[viewportWidth_ * viewportHeight_];

            for (uint32_t i = 0; i < viewportWidth_ * viewportHeight_; i++)
            {
                imageData_[i] = Random::UInt();
                imageData_[i] |= 0xff000000;
            }
            image_->SetData(imageData_);
            lastRenderTime_ = timer_.ElapsedMillis();
        }
    }

    uint32_t viewportWidth_{ 0 };
    uint32_t viewportHeight_{ 0 };

    std::shared_ptr<Walnut::Image> image_;
    uint32_t* imageData_ = nullptr;

    Timer timer_;
    float lastRenderTime_{ 0 };
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Weekend Raytracer";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<ExampleLayer>();
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