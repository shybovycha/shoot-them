#include "IrrlichtApplication.h"

IrrlichtApplication::IrrlichtApplication(std::shared_ptr<Renderer> _renderer,
        std::shared_ptr<ResourceManager> _resourceManager)
        : Application<GameState>(std::move(_renderer), std::move(_resourceManager)),
          soundEngine(std::make_unique<IrrKlangSoundEngine>())
{
}
