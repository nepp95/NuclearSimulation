#include "NuclearSimulationLayer.h"

#include <EppoCore/Core/Timer.h>
#include <EppoCore/Platform/GUI/ApplicationGUI.h>

#include <imgui.h>

void NuclearSimulationLayer::OnAttach()
{
    // Disable V-Sync
    Window::SetVSync(false);

    // Create framebuffer we will render to
    FramebufferSpecification spec{
        .Width = 1280,
        .Height = 720,
    };

    m_Framebuffer = std::make_unique<Framebuffer>(spec);
}

void NuclearSimulationLayer::OnDetach()
{}

void NuclearSimulationLayer::OnUpdate(float timestep)
{
    // Logic
    Timer timer;

    m_LastUpdateTime = timer.GetElapsedMilliseconds();
    timer.Reset();

    // Rendering
    const auto renderer = Application::Get().GetRenderer();

    m_Framebuffer->Bind();

    renderer->BeginScene(glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f));

    // Data visualisation start
    renderer->DrawRectangle(glm::vec2(-0.75f, -0.5f), glm::vec2(0.5f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    // Visualisation start
    renderer->DrawRectangle(glm::vec2(0.0f, 0.5f), glm::vec2(2.0f, 1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Graphite moderators
    for (uint32_t i = 0; i < 10; i++)
        renderer->DrawRectangle(glm::vec2(-0.90f + 0.2f * static_cast<float>(i), 0.75f), glm::vec2(0.1f, 0.5f),
                                glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));

    // Control rods
    for (uint32_t i = 0; i < 9; i++)
        renderer->DrawRectangle(glm::vec2(-0.8f + 0.2f * static_cast<float>(i), 0.25f), glm::vec2(0.05f, 0.5f),
                                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    //

    renderer->EndScene();

    Framebuffer::Unbind();

    m_LastFrameTime = timer.GetElapsedMilliseconds();
}

void NuclearSimulationLayer::OnUIRender()
{
    constexpr ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_None;
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    const ImGuiID dockspaceID = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0, 0), dockFlags);
    //
    // Start of docking space
    //

    // Menu
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Close"))
                Application::Get().Close();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Begin("Settings");
    ImGui::Text("Update time: %.3fms", m_LastUpdateTime);
    ImGui::Text("Render time: %.3fms", m_LastFrameTime);
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Reactor");

    const auto size = ImGui::GetContentRegionAvail();
    ImGui::Image((ImTextureID)m_Framebuffer->GetRendererID(), size);

    ImGui::End();
    ImGui::PopStyleVar();

    //
    // End of docking space
    //
    ImGui::End(); // DockSpace
}
