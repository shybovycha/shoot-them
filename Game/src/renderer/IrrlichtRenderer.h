#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include <irrlicht.h>

#include "../core/Renderer.h"
#include "../core/SceneNode.h"
#include "../core/Settings.h"
#include "../game/GameActionDispatcher.h"
#include "../game/GameActionHandler.h"
#include "../game/GameState.h"

#include "CDrunkShaderCallback.h"
#include "CScreenQuadSceneNode.h"
#include "IrrlichtEventReceiver.h"
#include "IrrlichtGameGUI.h"
#include "IrrlichtHUD.h"
#include "IrrlichtSceneNode.h"

class IrrlichtRenderer : public Renderer, public GameActionHandler
{
public:
    IrrlichtRenderer();

    virtual void init(std::shared_ptr<Settings> settings) override;

    virtual void render() override;

    virtual void shutdown() override;

    virtual bool isRunning() override;

    virtual void quit() override;

protected:
    virtual void loadLevel(std::shared_ptr<Level> levelDescriptor) override;

    virtual void unloadLevel(std::shared_ptr<Level> levelDescriptor) override;

private:
    void updateCrosshair();

    void updatePostProcessingEffects();

    void updateTimer();

private:
    std::shared_ptr<IrrlichtEventReceiver> eventReceiver;

    std::shared_ptr<IrrlichtHUD> hud;

    irr::ITimer* timer = 0;

    irr::IrrlichtDevice* device = 0;
    irr::video::IVideoDriver* driver = 0;
    irr::scene::ISceneManager* smgr = 0;
    irr::gui::IGUIEnvironment* guienv = 0;

    irr::scene::ICameraSceneNode* camera = 0;

    irr::scene::IAnimatedMesh* playermesh = 0;
    irr::scene::IAnimatedMeshSceneNode* player = 0;

    irr::scene::IBillboardSceneNode* bill = 0;
    irr::scene::ITriangleSelector* selector = 0;

    irr::video::ITexture* screenRenderTarget = 0;
    CScreenQuadSceneNode* screenQuad = 0;
    CDrunkShaderCallback* drunkShaderCallback = 0;
};
