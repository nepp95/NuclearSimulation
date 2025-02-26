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

    float m_ControlRodInsertion = 50.0f;
    const float m_ControlRodSpeed = 5.0f;

    int64_t N = 1000000; // Neutron population
    int64_t C = 1000000000000000; // Precursor concentration
    float p0 = 0.01f;
    float p = p0; // Reactivity
    float Q; // Decay heat
    float t = 0.0f; // Total time

    float T0 = 250.0f;
    float T = T0; // Temperature
    float T_coolant = 200.0f; // Coolant temperature
    float P = 1.0f; // Power
};
