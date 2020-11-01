#include "IrrlichtRenderer.h"

IrrlichtRenderer::IrrlichtRenderer() : Renderer(), GameActionHandler(nullptr)
{
}

void IrrlichtRenderer::init(std::shared_ptr<Settings> settings)
{
    irr::video::E_DRIVER_TYPE driverType = irr::video::EDT_OPENGL;

    if (settings->driverName == "DirectX")
    {
        driverType = irr::video::EDT_DIRECT3D9;
    }

    irr::core::dimension2d<irr::u32> resolution = irr::core::dimension2d<irr::u32>(settings->resolutionWidth,
            settings->resolutionHeight);

    device = irr::createDevice(driverType, resolution, settings->colorDepth, settings->fullScreen, settings->stencil,
            settings->vsync);

    device->setWindowCaption(L"ShootThem!");

    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();

    device->getFileSystem()->addFileArchive("resources/packs/data.zip");

    bill = smgr->addBillboardSceneNode();
    bill->setMaterialType(irr::video::EMT_TRANSPARENT_ADD_COLOR);
    bill->setMaterialTexture(0, driver->getTexture("cross.bmp"));
    bill->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    bill->setMaterialFlag(irr::video::EMF_ZBUFFER, false);
    bill->setSize(irr::core::dimension2d<irr::f32>(20.0f, 20.0f));

    irr::video::IGPUProgrammingServices* gpu = driver->getGPUProgrammingServices();

    drunkShaderCallback = new CDrunkShaderCallback();

    irr::s32 drunkShader = gpu->addHighLevelShaderMaterialFromFiles(
            "resources/shaders/identity.vert.glsl", "main", irr::video::EVST_VS_1_1,
            "resources/shaders/drunk.frag.glsl", "main", irr::video::EPST_PS_1_1,
            drunkShaderCallback, irr::video::EMT_SOLID, 0, irr::video::EGSL_DEFAULT
    );

    screenRenderTarget = driver->addRenderTargetTexture(irr::core::dimension2d<irr::u32>(2048, 1024), "RTT0",
            irr::video::ECF_A8R8G8B8);

    screenQuad = new CScreenQuadSceneNode(smgr->getRootSceneNode(), smgr, -1);
    screenQuad->getMaterial(0).MaterialType = (irr::video::E_MATERIAL_TYPE)drunkShader;
    screenQuad->getMaterial(0).setTexture(0, screenRenderTarget);

    gameGUI = std::dynamic_pointer_cast<GameGUI>(std::make_shared<IrrlichtGameGUI>(device, driver, guienv));

    // TODO: move this to scene config too
    smgr->addLightSceneNode(nullptr, irr::core::vector3df(0, 20, 0), irr::video::SColorf(0.5f, 0.5f, 0.5f, 0.5f), 3000,
            0);

    // driver->setFog(irr::video::SColor(0, 138, 125, 81), irr::video::EFT_FOG_LINEAR, 250, 1000, 0, true);

    // TODO: move this to scene config too as a player initial position
    camera = smgr->addCameraSceneNodeFPS(0, 100, 0, 0);
    device->getCursorControl()->setVisible(false);

    timer = device->getTimer();
    timer->start();

    playermesh = smgr->getMesh("rifle.3ds");
    player = smgr->addAnimatedMeshSceneNode(playermesh);

    // TODO: player model offset relative to camera position
    player->setPosition(irr::core::vector3df(0.5f, -1.0f, 1.0f));

    player->setParent(camera);

    // sorry, no better place for this instantiation than here, since this event receiver **has** to be bound to both camera node and scene manager
    eventReceiver = std::make_shared<IrrlichtEventReceiver>(smgr, camera);

    device->setEventReceiver(eventReceiver.get());

    hud = std::make_shared<IrrlichtHUD>(driver, guienv);
    hud->init();
}

void IrrlichtRenderer::render()
{
    if (!device->isWindowActive())
    {
        return;
    }

    auto gameState = StateManager<GameState>::getInstance()->getState();

    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    if (gameState->getCurrentState() == GameStateType::PLAYING)
    {
        // update shader' data
        drunkShaderCallback->setTime(timer->getTime() / 1000.f);

        // render scene to texture, using the shader as a material
        driver->setRenderTarget(screenRenderTarget, true, true, irr::video::SColor(0, 0, 0, 0));
        smgr->drawAll();

        // only render quad with the shader-processed material to the screen
        driver->setRenderTarget(irr::video::ERT_FRAME_BUFFER, false, false, irr::video::SColor(0, 0, 0, 0));
        screenQuad->render();

        updateTimer();

        updateCrosshair();
        updatePostProcessingEffects();

        // lastly, always render HUD on top of everything
        hud->render();
    }

    gameGUI->render();

    driver->endScene();
}

void IrrlichtRenderer::updateTimer()
{
    if (timer->isStopped())
    {
        return;
    }

    auto gameState = StateManager<GameState>::getInstance()->getState();

    gameState->getCurrentScore()->tick();

    if (gameState->getCurrentScore()->getCurrentTime() < 1)
    {
        if (gameState->getCurrentLevelIndex() < gameState->getLevelsCnt())
        {
            actionDispatcher->loadNextLevel();
        }
        else
        {
            actionDispatcher->gameOver();
        }
    }
}

void IrrlichtRenderer::shutdown()
{
    timer->stop();

    device->closeDevice();
}

bool IrrlichtRenderer::isRunning()
{
    return device->run();
}

void IrrlichtRenderer::updateCrosshair()
{
    irr::core::line3d<irr::f32> line;
    line.start = camera->getPosition() + (camera->getTarget() - camera->getPosition()).normalize() * 100.f;
    line.end = line.start + (camera->getTarget() - camera->getPosition()).normalize() * 10000.0f;

    irr::core::triangle3df collisionTriangle;
    irr::core::vector3df collisionPoint;
    irr::scene::ISceneNode* node = nullptr;

    if (smgr->getSceneCollisionManager()->getCollisionPoint(line, selector, collisionPoint, collisionTriangle, node))
    {
        bill->setPosition(collisionPoint);
    }
}

void IrrlichtRenderer::updatePostProcessingEffects()
{
    auto gameState = StateManager<GameState>::getInstance()->getState();

    unsigned long time = gameState->getCurrentScore()->getCurrentTime();
    int levelIdx = gameState->getCurrentLevelIndex();
    double k = sin(time / 100.0f) / (10.0f - levelIdx);

    camera->setRotation(
            irr::core::vector3df(
                    camera->getRotation().X + k,
                    camera->getRotation().Y,
                    camera->getRotation().Z
            )
    );
}

void IrrlichtRenderer::loadLevel(std::shared_ptr<Level> levelDescriptor)
{
    auto gameState = StateManager<GameState>::getInstance()->getState();

    irr::scene::IAnimatedMesh* levelMesh = smgr->getMesh(levelDescriptor->getModelFilename().c_str());

    irr::scene::IAnimatedMeshSceneNode* level = smgr->addAnimatedMeshSceneNode(levelMesh);

    std::wostringstream levelName;
    levelName << L"level-" << gameState->getCurrentLevelIndex();
    level->setName(levelName.str().c_str());

    levelDescriptor->setModel(new IrrlichtSceneNode(level));

    auto metaTriangleSelector = smgr->createMetaTriangleSelector();

    metaTriangleSelector->addTriangleSelector(smgr->createTriangleSelector(level));

    irr::scene::IAnimatedMesh* targetMesh = smgr->getMesh("chicken.3ds");

    std::vector<SceneNode*> targets;

    int targetIdx = 0;

    for (const auto& position : levelDescriptor->getTargetPositions())
    {
        auto target = smgr->addAnimatedMeshSceneNode(targetMesh);

        target->setVisible(true);

        target->setMaterialTexture(0, driver->getTexture("Chick02.bmp"));
        target->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, true);
        target->setPosition(irr::core::vector3df(position.getX(), position.getY(), position.getZ()));

        metaTriangleSelector->addTriangleSelector(smgr->createTriangleSelector(target));

        std::wostringstream targetName;
        targetName << "target-";
        targetName << gameState->getCurrentLevelIndex();
        targetName << "-";
        targetName << targetIdx++;
        target->setName(targetName.str().c_str());

        targets.push_back(new IrrlichtSceneNode(target));
    }

    std::vector<SceneNode*> lights;

    for (const auto& position : levelDescriptor->getLightPositions())
    {
        auto lightPosition = irr::core::vector3df(position.getX(), position.getY(), position.getZ());
        auto light = smgr->addLightSceneNode(nullptr, lightPosition);

        lights.push_back(new IrrlichtSceneNode(light));
    }

    selector = metaTriangleSelector;

    gameState->getCurrentScore()->resetCurrentTime();

    levelDescriptor->setTargets(targets);
    levelDescriptor->setLights(lights);
}

void IrrlichtRenderer::unloadLevel(std::shared_ptr<Level> levelDescriptor)
{
    for (auto target : levelDescriptor->getTargets())
    {
        if (target)
        {
            target->remove();
        }
    }

    for (auto light : levelDescriptor->getLights())
    {
        if (light)
        {
            light->remove();
        }
    }

    if (levelDescriptor->getModel())
    {
        levelDescriptor->getModel()->remove();
    }
}

void IrrlichtRenderer::quit()
{
    device->closeDevice();
}
