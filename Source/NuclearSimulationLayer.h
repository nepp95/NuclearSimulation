#pragma once

#include <EppoCore/Core/Application.h>

using namespace Eppo;

class NuclearSimulationLayer : public Layer
{
public:
    NuclearSimulationLayer();
    ~NuclearSimulationLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(float timestep) override;
    void OnUIRender() override;
};