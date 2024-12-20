#include "IrrlichtRenderer.h"

IrrlichtRenderer::IrrlichtRenderer(std::shared_ptr<GameState> _gameState,
        std::shared_ptr<ActionDispatcher> _actionDispatcher) :
        Renderer(_gameState),
        actionDispatcher(_actionDispatcher)
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

    irr::gui::IGUIFont* font = guienv->getFont("calibri.xml");
    guienv->getSkin()->setFont(font);

    // TODO: move this to scene config too
    smgr->addLightSceneNode(nullptr, irr::core::vector3df(0, 20, 0), irr::video::SColorf(0.5f, 0.5f, 0.5f, 0.5f), 3000,
            0);

    // driver->setFog(irr::video::SColor(0, 138, 125, 81), irr::video::EFT_FOG_LINEAR, 250, 1000, 0, true);

    // load sound buffers - they all must remain alive as long as sounds are playing
    std::vector<std::string> sounds {"noammo.wav", "shot.wav", "bell.wav", "reload.wav"};

    for (auto& soundName : sounds)
    {
        auto soundFile = std::format("resources/sounds/{}", soundName);

        auto buffer = std::make_shared<sf::SoundBuffer>();

        if (!buffer->loadFromFile(soundFile))
        {
            auto msg = std::format("Failed to load sound ;{}'", soundFile);
            device->getLogger()->log(msg.c_str(), irr::ELL_ERROR);
            continue;
        }

        soundBuffers[soundFile] = std::move(buffer);
    }

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
    eventReceiver = std::make_shared<IrrlichtEventReceiver>(gameState, actionDispatcher, smgr, camera);

    device->setEventReceiver(eventReceiver.get());

    hud = std::make_shared<IrrlichtHUD>(driver, guienv, gameState);
    hud->init();
}

void IrrlichtRenderer::processActionQueue()
{
    while (gameState->hasActions())
    {
        auto action = gameState->nextAction();

        switch (action->getType())
        {
        case QueueActionType::LOAD_FIRST_LEVEL:
            processAction(reinterpret_cast<LoadFirstLevelAction*>(action));
            break;
        case QueueActionType::LOAD_NEXT_LEVEL:
            processAction(reinterpret_cast<LoadNextLevelAction*>(action));
            break;
        case QueueActionType::PLAY_SOUND:
            processAction(reinterpret_cast<PlaySoundAction*>(action));
            break;
        case QueueActionType::TARGET_ELIMINATED:
            processAction(reinterpret_cast<TargetEliminatedAction*>(action));
            break;
        case QueueActionType::START_NEW_GAME:
            processAction(reinterpret_cast<StartNewGameAction*>(action));
            break;
        case QueueActionType::QUIT:
            processAction(reinterpret_cast<QuitAction*>(action));
            break;
        case QueueActionType::MAIN_MENU:
            processAction(reinterpret_cast<MainMenuAction*>(action));
            break;
        case QueueActionType::HIDE_MAIN_MENU:
            processAction(reinterpret_cast<HideMainMenuAction*>(action));
            break;
        case QueueActionType::GAME_OVER:
            processAction(reinterpret_cast<GameOverAction*>(action));
            break;
        }
    }
}

void IrrlichtRenderer::processAction(PlaySoundAction* action)
{
    auto& buffer = soundBuffers[action->getSoundFile()];

    if (!buffer)
    {
        auto str = std::format("Failed to load sound {}", action->getSoundFile());
        device->getLogger()->log(str.c_str(), irr::ELL_ERROR);
        return;
    }

    auto sound = std::make_shared<sf::Sound>(*buffer);

    sound->play();

    playingSounds.push(std::move(sound));
}

void IrrlichtRenderer::processAction(LoadFirstLevelAction* action)
{
    loadLevel(action->getLevel());

    actionDispatcher->firstLevelLoaded();
}

void IrrlichtRenderer::processAction(LoadNextLevelAction* action)
{
    if (action->getNextLevel() == nullptr)
    {
        actionDispatcher->gameOver();
        return;
    }

    // unload existing level data
    unloadLevel(action->getPreviousLevel());

    // load next level
    loadLevel(action->getNextLevel());

    actionDispatcher->nextLevelLoaded();
}

void IrrlichtRenderer::processAction(TargetEliminatedAction* action)
{
    // hide in here, remove when the next level is loaded
    action->getTarget()->setVisible(false);
    actionDispatcher->targetEliminated();

    if (gameState->getCurrentScore()->getTargetsEliminated() >= gameState->getCurrentLevel()->getTargets().size())
    {
        // TODO: show next level menu
        actionDispatcher->loadNextLevel();
    }
}

void IrrlichtRenderer::processAction(StartNewGameAction* action)
{
    if (gameOverLabel != nullptr)
    {
        gameOverLabel->remove();
        gameOverLabel = nullptr;
    }

    if (gameState->isGameStarted())
    {
        unloadLevel(gameState->getCurrentLevel());
    }

    actionDispatcher->loadFirstLevel();
    device->getCursorControl()->setVisible(false);
    mainMenuWindow->setVisible(false);
}

void IrrlichtRenderer::processAction(MainMenuAction* action)
{
    if (gameOverLabel != nullptr)
    {
        gameOverLabel->setVisible(false);
    }

    mainMenuWindow->setVisible(true);

    if (gameState->isGameOver() || !gameState->isGameStarted())
    {
        mainMenuWindow->getElementFromId(CONTINUE_BUTTON_ID)->setEnabled(false);
    }
    else
    {
        mainMenuWindow->getElementFromId(CONTINUE_BUTTON_ID)->setEnabled(true);
    }

    device->getCursorControl()->setVisible(true);
}

void IrrlichtRenderer::processAction(QuitAction* action)
{
    device->closeDevice();
}

void IrrlichtRenderer::processAction(HideMainMenuAction* action)
{
    device->getCursorControl()->setVisible(false);
    mainMenuWindow->setVisible(false);
}

void IrrlichtRenderer::processAction(GameOverAction* action)
{
    const auto text = L"Game over";
    const auto screenSize = driver->getScreenSize();
    const auto font = guienv->getSkin()->getFont();
    const auto textSize = font->getDimension(text);

    gameOverLabel = guienv->addStaticText(
            L"Game over",
            irr::core::rect<irr::s32>(
                    (screenSize.Width / 2) - (textSize.Width / 2),
                    (screenSize.Height / 2) - (textSize.Height / 2),
                    (screenSize.Width / 2) + (textSize.Width / 2),
                    (screenSize.Height / 2) + (textSize.Height / 2)
            ),
            false
    );
}

void IrrlichtRenderer::render()
{
    if (!device->isWindowActive())
    {
        return;
    }

    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    if (gameState->getCurrentState() == GameStateType::MAIN_MENU)
    {
        device->getCursorControl()->setVisible(true);
        renderMainMenu();
    }
    else if (gameState->getCurrentState() == GameStateType::PLAYING)
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
    else if (gameState->getCurrentState() == GameStateType::END_GAME)
    {
        renderEndGameMenu();
    }
    else if (gameState->getCurrentState() == GameStateType::END_LEVEL)
    {
        renderEndLevelMenu();
    }

    guienv->drawAll();

    driver->endScene();
}

void IrrlichtRenderer::renderMainMenu()
{
    if (mainMenuWindow)
    {
        return;
    }

    mainMenuWindow = guienv->addWindow(
            irr::core::rect<irr::s32>(100, 100, 315, 265),
            false,
            L"Main menu"
    );

    mainMenuWindow->getCloseButton()->remove();

    guienv->addButton(
            irr::core::rect<irr::s32>(35, 35, 180, 60),
            mainMenuWindow,
            NEW_GAME_BUTTON_ID,
            L"New game"
    );

    irr::gui::IGUIButton* continueButton = guienv->addButton(
            irr::core::rect<irr::s32>(35, 70, 180, 95),
            mainMenuWindow,
            CONTINUE_BUTTON_ID,
            L"Back to the game"
    );

    continueButton->setEnabled(false);

    guienv->addButton(
            irr::core::rect<irr::s32>(35, 105, 180, 130),
            mainMenuWindow,
            QUIT_BUTTON_ID,
            L"Quit"
    );
}

void IrrlichtRenderer::renderEndGameMenu()
{
    // nothing to do here, really
}

void IrrlichtRenderer::renderEndLevelMenu()
{
    // TODO: implement
}

void IrrlichtRenderer::updateTimer()
{
    if (timer->isStopped())
    {
        return;
    }

    gameState->getCurrentScore()->tick();

    if (gameState->getCurrentScore()->getCurrentTime() < 1)
    {
        if (gameState->getCurrentLevelIndex() < gameState->getLevelsCnt())
        {
            actionDispatcher->loadNextLevel();
        }
        else
        {
            renderEndGameMenu();
        }
    }
}

void IrrlichtRenderer::shutdown()
{
    timer->stop();

    device->closeDevice();
    // soundEngine->drop();
}

bool IrrlichtRenderer::isRunning()
{
    return device->run();
}

// TODO: implement this endgame screen
void IrrlichtRenderer::showResult()
{
    /*irr::core::stringw title = L"Level complete!";

    int points = gameState->getCurrentScore()->getTargetsEliminated();
    int targetCnt = gameState->getCurrentLevel()->getTargets().size();

    int shots = gameState->getCurrentScore()->getShots();*/
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
