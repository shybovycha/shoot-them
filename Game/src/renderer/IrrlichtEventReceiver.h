#pragma once

#include <memory>

#include <irrlicht.h>

#include "../game/GameActionDispatcher.h"
#include "../game/GameState.h"
#include "../core/StateManager.h"

#include "IrrlichtGameGUI.h"
#include "IrrlichtSceneNode.h"

class IrrlichtEventReceiver : public irr::IEventReceiver
{
public:
    IrrlichtEventReceiver(irr::scene::ISceneManager* _sceneManager, irr::scene::ICameraSceneNode* _camera);

    virtual bool OnEvent(const irr::SEvent& event);

private:
    std::unique_ptr<GameActionDispatcher> actionDispatcher;

    irr::scene::ISceneManager* sceneManager;
    irr::scene::ICameraSceneNode* camera;
};
