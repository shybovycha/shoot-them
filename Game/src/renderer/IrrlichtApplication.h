#pragma once

#include "../core/Application.h"
#include "../game/GameActionHandler.h"
#include "../game/GameState.h"

#include "IrrKlangSoundEngine.h"

class IrrlichtApplication : public Application<GameState>
{
public:
    IrrlichtApplication(std::shared_ptr<Renderer> _renderer, std::shared_ptr<ResourceManager> _resourceManager);

private:
    std::unique_ptr<SoundEngine> soundEngine;
};
