#include <cstdint>
#include <memory>

#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {
    public:
    virtual void OnUIRender () override {
        ImGui::Begin ("Settings");
        if (ImGui::Button ("Render")) {
            Render ();
        }
        ImGui::End ();

        ImGui::Begin ("Viewport");

        viewportWidth_  = ImGui::GetContentRegionAvail ().x;
        viewportHeight_ = ImGui::GetContentRegionAvail ().y;

        if (image_)
            ImGui::Image (image_->GetDescriptorSet (), { (float)image_->GetWidth (), (float)image_->GetHeight () });

        ImGui::End ();
    }

    void Render () {
        if (!image_ || viewportHeight_ != image_->GetWidth () || viewportWidth_ != image_->GetWidth ()) {
            image_ = std::make_shared<Image> (viewportWidth_, viewportHeight_, ImageFormat::RGBA);
            delete[] imageData_;
            imageData_ = new uint32_t[viewportWidth_ * viewportHeight_];

            for (uint32_t i = 0; i < viewportWidth_ * viewportHeight_; i++) {
                imageData_[i] = 0xffff00ff;
            }
            image_->SetData (imageData_);
        }
    }

    uint32_t viewportWidth_{ 0 };
    uint32_t viewportHeight_{ 0 };

    std::shared_ptr<Walnut::Image> image_;
    uint32_t* imageData_ = nullptr;
};

Walnut::Application* Walnut::CreateApplication (int argc, char** argv) {
    Walnut::ApplicationSpecification spec;
    spec.Name = "Weekend Raytracer";

    Walnut::Application* app = new Walnut::Application (spec);
    app->PushLayer<ExampleLayer> ();
    app->SetMenubarCallback ([app] () {
        if (ImGui::BeginMenu ("File")) {
            if (ImGui::MenuItem ("Exit")) {
                app->Close ();
            }
            ImGui::EndMenu ();
        }
    });
    return app;
}