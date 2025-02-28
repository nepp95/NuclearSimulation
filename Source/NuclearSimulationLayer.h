#pragma once

#include <EppoCore/Core/Application.h>
#include <EppoCore/Platform/GUI/Framebuffer.h>
#include <EppoCore/Platform/GUI/Shader.h>
#include <EppoCore/Platform/GUI/Vertex.h>

using namespace Eppo;

class NuclearSimulationLayer final : public Layer
{
public:
    NuclearSimulationLayer() = default;
    ~NuclearSimulationLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(float timestep) override;
    void OnUIRender() override;

private:
    float m_LastFrameTime = 0.0f;
    float m_LastUpdateTime = 0.0f;

    std::unique_ptr<Framebuffer> m_Framebuffer;
    std::unique_ptr<Shader> m_Shader;

    const float m_ControlRodReactivity = 0.1f;
    const float m_ControlRodSpeed = 10.0f;
    float m_ControlRodInsertion = 50.0f;
    bool m_AutomaticControl = true;

    const float dt = 0.01f; // Fixed Delta Time

    const float Tenv = 300.0f; // Ambient Temperature (C)
    const float T0 = 300.0f; // Initial Core Temperature (C)
    const float a = -0.000005f; // Temperature Coefficient (1/C)
    const float y = 0.1f; // Cooling coefficient (1/s)
    const float e = 0.01f; // Power to Heat coefficient (C/(N * s))
    float T = T0; // Core Temperature (C)

    const float A = 0.003f; // Neutron Generation Time (s)
    float p = -0.01f; // Reactivity
    float N = 1000.0f; // Neutron Density
    float P = 0.0f; // Power Level
    float Ptarget = 3200.0f;
};
