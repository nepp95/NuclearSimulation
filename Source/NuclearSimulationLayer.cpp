#include "NuclearSimulationLayer.h"

#include <EppoCore/Core/Timer.h>
#include <EppoCore/Platform/GUI/ApplicationGUI.h>

#include <imgui.h>

void NuclearSimulationLayer::OnAttach()
{
    // Disable V-Sync
    Window::SetVSync(true);

    // Create framebuffer we will render to
    FramebufferSpecification spec{
        .Width = 1280,
        .Height = 720,
    };

    m_Framebuffer = std::make_unique<Framebuffer>(spec);
}

void NuclearSimulationLayer::OnDetach()
{}

void NuclearSimulationLayer::OnUpdate(const float timestep)
{
    // Logic
    Timer timer;

    // Reactivity
    const float prod = m_ControlRodInsertion / 100.0f * (0.2f - -0.2f) + -0.2f;
    p = prod * a * (T - T0);

    // Neutron Density
    const float dN = (p / A) * N * dt;
    N += dN;

    // Core Temperature
    const float dT = (e * N - y * (T - Tenv)) * dt;
    T += dT;

    // Power
    P = N;

    // Auto balance
    if (m_AutomaticControl)
    {
        if (P < Ptarget && T < 550.0f && p < 0.00002f)
            m_ControlRodInsertion -= m_ControlRodSpeed * timestep;
        else if (p > -0.00003f)
            m_ControlRodInsertion += m_ControlRodSpeed * timestep;

        m_ControlRodInsertion = std::clamp(m_ControlRodInsertion, 0.0f, 100.0f);
    }

    // Rendering
    m_LastUpdateTime = timer.GetElapsedMilliseconds();
    timer.Reset();

    const auto renderer = Application::Get().GetRenderer();

    m_Framebuffer->Bind();

    renderer->BeginScene(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f));

    // Data visualisation start
    renderer->DrawRectangle(glm::vec2(640.0f, 180.0f), glm::vec2(1280.0f, 360.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    renderer->DrawString(glm::vec2(20.0f, 40.0f), "Reactor Simulation");

    renderer->DrawString(glm::vec2(20.0f, 130.0f), std::format("Power: {:.2f} MW", P));
    renderer->DrawString(glm::vec2(20.0f, 170.0f), std::format("Temp: {:.2f} C", T));
    renderer->DrawString(glm::vec2(20.0f, 210.0f), std::format("Reactivity: {:.5f}", p));

    // Visualisation start
    renderer->DrawRectangle(glm::vec2(640.0f, 540.0f), glm::vec2(1280.0f, 360.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Graphite moderators
    for (uint32_t i = 0; i < 10; i++)
    {
        const float xPos = (-0.90f + 0.2f * static_cast<float>(i) + 1.0f) / 2.0f * 1280;
        constexpr float yPos = (0.75f + 1.0f) / 2.0f * 720;
        constexpr float width = 0.05f * 1280;
        constexpr float height = 0.25f * 720;

        renderer->DrawRectangle(glm::vec2(xPos, yPos), glm::vec2(width, height), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    }

    // Control rods
    for (uint32_t i = 0; i < 9; i++)
    {
        const float controlRodHeight = 0.25f + m_ControlRodInsertion / 100.0f * 0.5f;
        const float xPos = (-0.8f + 0.2f * static_cast<float>(i) + 1.0f) * 0.5f * 1280;
        const float yPos = (controlRodHeight + 1.0f) * 0.5f
            * 720;
        constexpr float width = 0.025f * 1280; // Correct width for Control rods
        constexpr float height = 0.25f * 720;

        renderer->DrawRectangle(glm::vec2(xPos, yPos), glm::vec2(width, height), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

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

    ImGui::Text("Control Rod Level: %.2f", m_ControlRodInsertion);
    ImGui::Checkbox("Automatic Control", &m_AutomaticControl);

    constexpr ImGuiSliderFlags flags = ImGuiSliderFlags_AlwaysClamp;
    ImGui::SliderFloat("##ControlRod", &m_ControlRodInsertion, 0.0f, 100.0f, "%.2f", flags);
    ImGui::Text("Target Power Level");
    ImGui::SliderFloat("##PowerLevel", &Ptarget, 0.0f, 3200.0f, "%.0f", flags);

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Reactor");

    const auto size = ImGui::GetContentRegionAvail();
    ImGui::Image(reinterpret_cast<ImTextureID>(m_Framebuffer->GetRendererID()), size);

    ImGui::End();
    ImGui::PopStyleVar();

    //
    // End of docking space
    //
    ImGui::End(); // DockSpace
}
