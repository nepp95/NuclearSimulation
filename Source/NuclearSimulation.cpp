#include "NuclearSimulationLayer.h"

#include <EppoCore/Core/Application.h>
#include <EppoCore/Core/Entrypoint.h>

class NuclearSimulation : public Application
{
public:
    explicit NuclearSimulation(ApplicationSpecification specification)
        : Application(std::move(specification))
    {
        const auto layer = std::make_shared<NuclearSimulationLayer>();
        PushLayer(layer);
    }

    ~NuclearSimulation() = default;
};

Application* Eppo::CreateApplication(const ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec{
        .Title = "NuclearSimulation",
        .CommandLineArgs = args
    };

    return new NuclearSimulation(std::move(spec));
}
