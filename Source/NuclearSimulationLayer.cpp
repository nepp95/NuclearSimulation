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

    // Timestep in seconds
    float dt = timestep / 1000.0f;
    t += dt;

    dt = 0.01f;

    // Constants
    constexpr float Q0 = 0.05f * 3200.0f;
    constexpr float B = 0.0065f; // Delayed neutron fraction
    constexpr float l = 0.08f; // Decay constant
    constexpr float L = 0.001f; // Neutron generation time
    constexpr float MeV = 200.0f; // Energy per fission
    constexpr float MeV_W = 1.60218e-13f; // Conversion factor (MeV to watts)
    constexpr float tau = 100.0f; // Decay heat time constant
    constexpr float C_heat = 1e6f; // Heat capacity of the core
    constexpr float R_thermal = 1e4f; // Thermal resistance
    constexpr float C_coolant = 4.18e3f; // Coolant heat capacity
    constexpr float M_coolant = 200.0f; // Coolant mass
    constexpr float T_alpha = -2e-4f; // Temperature coefficient of reactivity per C
    constexpr float k_rod = 0.02f; // Control rod efficiency
    constexpr float f_rate = 1.0f; // Fission rate

    // Reactivity
    float d = m_ControlRodInsertion / 100.0f; // value is between 0 and 100
    float p_rod = -k_rod * d;
    p = p0 + T_alpha * (T - T0) + p_rod;

    // Neutron population
    int64_t dN = (p - B) / L * N + l * C;
    N = N + dN * dt;

    // Precursor concentration
    int64_t dC = B / L * N - l * C;
    C = C + dC * dt;

    // Power
    P = N * MeV * MeV_W * f_rate;

    // Decay heat
    Q = Q0 * exp(-t / tau);

    // Temperature
    float dT = (P + Q) / C_heat - (T - T_coolant) / R_thermal;
    T = T + dT * dt;

    // Coolant temperature
    float heatTransfer = P + Q;
    float dT_coolant = heatTransfer / (C_coolant * M_coolant);
    T_coolant = T_coolant + dT_coolant * dt;

    // Auto balance
    if (P < 2000.0f)
        m_ControlRodInsertion -= m_ControlRodSpeed * dt;
    else
        m_ControlRodInsertion += m_ControlRodSpeed * dt;

    m_ControlRodInsertion = std::clamp(m_ControlRodInsertion, 0.0f, 100.0f);

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
    renderer->DrawString(glm::vec2(480.0f, 170.0f), std::format("Coolant Temp: {:.2f} C", T_coolant));
    renderer->DrawString(glm::vec2(20.0f, 210.0f), std::format("Reactivity: {:.5f}", p));
    renderer->DrawString(glm::vec2(20.0f, 250.0f), std::format("Neutron pop: {}", N));
    renderer->DrawString(glm::vec2(20.0f, 290.0f), std::format("Precursor Conc: {}", C));

    // Visualisation start
    renderer->DrawRectangle(glm::vec2(640.0f, 540.0f), glm::vec2(1280.0f, 360.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    // Graphite moderators
    for (uint32_t i = 0; i < 10; i++)
    {
        const float x = (-0.90f + 0.2f * static_cast<float>(i) + 1.0f) / 2.0f * 1280;
        constexpr float y = (0.75f + 1.0f) / 2.0f * 720;
        constexpr float width = 0.05f * 1280;
        constexpr float height = 0.25f * 720;

        renderer->DrawRectangle(glm::vec2(x, y), glm::vec2(width, height), glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    }

    // Control rods
    for (uint32_t i = 0; i < 9; i++)
    {
        const float controlRodHeight = 0.25f + m_ControlRodInsertion / 100.0f * 0.5f;
        const float x = (-0.8f + 0.2f * static_cast<float>(i) + 1.0f) * 0.5f * 1280;
        const float y = (controlRodHeight + 1.0f) * 0.5f
            * 720;
        constexpr float width = 0.025f * 1280; // Correct width for Control rods
        constexpr float height = 0.25f * 720;

        renderer->DrawRectangle(glm::vec2(x, y), glm::vec2(width, height), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
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
    ImGui::Text("Update time: %.5fms", m_LastUpdateTime);
    ImGui::Text("Render time: %.5fms", m_LastFrameTime);
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
