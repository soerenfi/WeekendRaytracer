#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Renderer.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "tracy/Tracy.hpp"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {
public:
  ExampleLayer()
      : mCamera_(45.0f, 0.1f, 100.0f) {
    {
      Material* metal  = scene_.addMaterial("metal");
      metal->albedo    = {1.0f, 0.0f, 1.0f};
      metal->roughness = 0.0f;
    }
    {
      Material* wood  = scene_.addMaterial("wood");
      wood->albedo    = {0.2f, 0.3f, 1.0f};
      wood->roughness = 0.2f;
    }
    {
      std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
      sphere->position               = {0.0f, 0.0f, -1.0f};
      sphere->radius                 = 1.0f;
      sphere->setMaterial("metal", scene_);
      scene_.addObject(std::move(sphere));
    }
    {
      std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
      sphere->position               = {0.0f, -101.0f, 0.0f};
      sphere->radius                 = 100.0f;
      sphere->setMaterial("wood", scene_);
      scene_.addObject(std::move(sphere));
    }
  }

  void OnUpdate(float ts) override {
    if (mCamera_.OnUpdate(ts))
      renderer_.ResetFrameIndex();
  }

  void OnUIRender() override {
    FrameMarkNamed("UIRender");

    ImGui::Begin("Settings");
    ImGui::Text("Last render: %.3fms", lastRenderTime_);
    if (ImGui::Button("Render")) {
      Render();
    }

    ImGui::Checkbox("Accumulate", &renderer_.GetSettings().Accumulate);

    if (ImGui::Button("Reset"))
      renderer_.ResetFrameIndex();

    ImGui::End();

    ImGui::Begin("Scene");
    for (size_t i = 0; i < scene_.objects_.size(); i++) {
      ImGui::PushID(i);

      Object* object = scene_.objects_[i].get();
      ImGui::DragFloat3("Position", glm::value_ptr(object->position), 0.1f);
      if (dynamic_cast<Sphere*>(object) != nullptr) {
        ImGui::DragFloat("Radius", &dynamic_cast<Sphere*>(object)->radius, 0.1f);
      }
      ImGui::DragInt("Material", &object->materialIndex, 1.0f, 0, (int)scene_.materials_.size() - 1);

      ImGui::Separator();

      ImGui::PopID();
    }

    for (size_t i = 0; i < scene_.materials_.size(); i++) {
      ImGui::PushID(i);

      Material& material = scene_.materials_[i];
      ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
      ImGui::DragFloat("Roughness", &material.roughness, 0.05f, 0.0f, 1.0f);
      ImGui::DragFloat("Metallic", &material.metallic, 0.05f, 0.0f, 1.0f);

      ImGui::Separator();

      ImGui::PopID();
    }

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    viewportWidth_  = ImGui::GetContentRegionAvail().x;
    viewportHeight_ = ImGui::GetContentRegionAvail().y;

    auto image = renderer_.GetFinalImage();
    if (image)
      ImGui::Image(image->GetDescriptorSet(), {(float)image->GetWidth(), (float)image->GetHeight()}, ImVec2(0, 1),
                   ImVec2(1, 0));

    ImGui::End();
    ImGui::PopStyleVar();

    Render();
  }

  void Render() {
    ZoneScopedN("render");

    Timer timer;

    renderer_.OnResize(viewportWidth_, viewportHeight_);
    mCamera_.OnResize(viewportWidth_, viewportHeight_);
    renderer_.Render(scene_, mCamera_);

    lastRenderTime_ = timer.ElapsedMillis();
  }

private:
  Renderer renderer_;
  Camera   mCamera_;
  Scene    scene_;
  uint32_t viewportWidth_ = 0, viewportHeight_ = 0;

  float lastRenderTime_ = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
  Walnut::ApplicationSpecification spec;
  spec.Name = "Ray Tracing";

  Walnut::Application* app = new Walnut::Application(spec);
  app->PushLayer<ExampleLayer>();
  app->SetMenubarCallback([app]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }
  });
  return app;
}