#include <glm/gtc/type_ptr.hpp>

#include <cstdint>
#include <memory>
#include <tuple>

#include "Camera.h"
#include "Renderer.h"
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"

class Viewport : public Walnut::Layer {
public:
  Viewport()
      : camera_(45.0f, 0.1f, 100.0f) {
    // m_Camera.SetPosition(glm::vec3(0.f, 0.f, 3.f));
    // m_Camera.lookAt(glm::vec3(0.f, 0.f, 0.f));
    {
      Material* metal = scene_.AddMaterial("Metal1");
      metal->SetAlbedo({0.8f, 0.0f, 0.0f});
      metal->SetMetallic(1.0f);
      Material* metal2 = scene_.AddMaterial("Metal2");
      metal->SetAlbedo({0.0f, 0.0f, 0.8f});
    }
    {
      std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
      sphere->SetPosition({0.0f, 0.0f, -1.0f});
      sphere->SetRadius(1.0f);
      sphere->SetMaterial("Metal1", scene_);
      scene_.AddObject(std::move(sphere));
    }

    {
      std::unique_ptr<Sphere> sphere = std::make_unique<Sphere>();
      sphere->SetPosition({1.0f, -101.0f, -5.0f});
      sphere->SetRadius(100.0f);
      sphere->SetMaterial("Metal2", scene_);
      scene_.AddObject(std::move(sphere));
    }

    {
      Light light;
      light.position = glm::vec3{-1.0f, 3.0f, -2.0f};
      light.intensity = 1;
      scene_.lights_.push_back(light);
    }
  }

  void OnUpdate(float ts) override {
    camera_.OnUpdate(ts);
  }

  void OnUIRender() override {
    ImGui::Begin("Settings");
    ImGui::Text("Last Render: %.3fms", lastRenderTime_);
    if (ImGui::Button("Render")) {
      Render();
    }

    ImGui::Begin("Scene");
    for (size_t i = 0; i < scene_.objects_.size(); i++) {
      ImGui::PushID(i);

      Object* object = scene_.objects_[i].get();
      ImGui::DragFloat3("Position", glm::value_ptr(object->position), 0.1f);
      ImGui::DragInt("Material", &object->materialIndex, 1.0f, 0, (int)scene_.materials_.size() - 1);

      ImGui::Separator();
      ImGui::PopID();
    }

    ImGui::Separator();

    for (size_t j = 0; j < scene_.lights_.size(); j++) {
      ImGui::PushID(j);

      Light& light = scene_.lights_[j];
      // ImGui::DragFloat3("Position", glm::value_ptr(light.Position), 0.1f);
      ImGui::DragFloat("Intensity", &light.intensity, 0.1f);

      ImGui::Separator();
      ImGui::PopID();
    }
    ImGui::Separator();

    for (size_t k = 0; k < scene_.materials_.size(); k++) {
      ImGui::PushID(k);

      Material& mat = scene_.materials_[k];
      ImGui::ColorEdit3("Albedo", glm::value_ptr(mat.albedo_));
      ImGui::SliderFloat("Metallic", &mat.metallic_, 0.0f, 100.f);

      ImGui::Separator();
      ImGui::PopID();
    }

    ImGui::End();
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    viewportWidth_ = ImGui::GetContentRegionAvail().x;
    viewportHeight_ = ImGui::GetContentRegionAvail().y;

    auto image = renderer_.GetFinalImage();
    if (image) {
      ImGui::Image(image->GetDescriptorSet(),
                   {static_cast<float>(image->GetWidth()), static_cast<float>(image->GetHeight())}, ImVec2(0, 1),
                   ImVec2(1, 0));
    }

    // ImGui::ShowDemoWindow();

    ImGui::End();
    ImGui::PopStyleVar(

    );

    Render();
  }

  void Render() {
    timer_.Reset();

    renderer_.OnResize(viewportWidth_, viewportHeight_);
    camera_.OnResize(viewportWidth_, viewportHeight_);
    renderer_.Render(scene_, camera_);

    lastRenderTime_ = timer_.ElapsedMillis();
  }

private:
  uint32_t viewportWidth_{}, viewportHeight_ = 0;

  Walnut::Timer timer_;
  float lastRenderTime_{0.0f};

  Renderer renderer_;
  Camera camera_;
  Scene scene_;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv) {
  std::ignore = argc;
  std::ignore = argv;
  Walnut::ApplicationSpecification spec;
  spec.Name = "Weekend Raytracer";

  Walnut::Application* app = new Walnut::Application(spec);
  app->PushLayer<Viewport>();
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