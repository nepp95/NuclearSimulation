#pragma once

#include "Reactor.h"

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

    std::vector<Vertex> m_Vertices;

    Reactor m_Reactor;
};
