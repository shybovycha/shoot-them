#include "ApplicationDelegate.h"

ApplicationDelegate::ApplicationDelegate(irr::IrrlichtDevice* _device) :
    device(_device),
    smgr(device->getSceneManager()),
    guienv(device->getGUIEnvironment()),
    driver(device->getVideoDriver()),
    levelsFilename(std::nullopt),
    loadLevelsDialogIsShown(false),
    saveLevelsDialogIsShown(false),
    aboutWindowIsShown(false),
    loadLevelMeshDialogIsShown(false)
{
    gameData = std::make_shared<GameData>(device);
    gameManagerTree = std::make_shared<GameManagerTree>(guienv, gameData);
}

void ApplicationDelegate::init() {
    camera = smgr->addCameraSceneNode();

    auto animator = new CameraSceneNodeAnimator(device->getCursorControl());
    camera->addAnimator(animator);

    initUI();
}

void ApplicationDelegate::initUI() {
    setFont();

    createToolbar();

    createManagerWindow();

    gameManagerTree->init();
}

void ApplicationDelegate::createToolbar() {
    irr::gui::IGUIToolBar* toolbar = guienv->addToolBar();
    toolbar->setMinSize(irr::core::dimension2du(100, 40));

    irr::video::ITexture* openFileIcon = driver->getTexture("Resources/Icons/opened-folder.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::LOAD_LEVELS), nullptr, L"Load game levels", openFileIcon, nullptr, false, true);

    irr::video::ITexture* saveFileIcon = driver->getTexture("Resources/Icons/save.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::SAVE_LEVELS), nullptr, L"Save game levels", saveFileIcon, nullptr, false, true);

    irr::video::ITexture* helpIcon = driver->getTexture("Resources/Icons/help.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::ABOUT), nullptr, L"About", helpIcon, nullptr, false, true);

    irr::video::ITexture* addLevelIcon = driver->getTexture("Resources/Icons/map.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::ADD_LEVEL), nullptr, L"Add level", addLevelIcon, nullptr, false, true);

    irr::video::ITexture* addTargetIcon = driver->getTexture("Resources/Icons/map-pin.png");
    toolbar->addButton(static_cast<irr::s32>(GUIElementId::ADD_TARGET), nullptr, L"Add target", addTargetIcon, nullptr, false, true);
}

void ApplicationDelegate::createManagerWindow() {
    irr::gui::IGUIWindow* managerWindow = guienv->addWindow(
        irr::core::rect<irr::s32>(600, 70, 800, 470),
        false, 
        L"Levels manager",
        nullptr,
        static_cast<irr::s32>(GUIElementId::MANAGER_WINDOW)
    );

    irr::gui::IGUITreeView* gameTree = guienv->addTreeView(
        irr::core::rect<irr::s32>(10, 30, 190, 390), 
        managerWindow, 
        static_cast<irr::s32>(GUIElementId::GAME_LEVEL_TREE)
    );
}

void ApplicationDelegate::update() {
    driver->beginScene(true, true, irr::video::SColor(0, 200, 200, 200));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
}

void ApplicationDelegate::quit() {
    device->closeDevice();
}

void ApplicationDelegate::setFont() {
    irr::gui::IGUIFont* font = guienv->getFont("Resources/Fonts/calibri.xml");
    guienv->getSkin()->setFont(font);
}

void ApplicationDelegate::openSaveLevelsDialog() {
    if (saveLevelsDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Save levels file", true, 0, static_cast<irr::s32>(GUIElementId::SAVE_LEVELS_DIALOG));
    saveLevelsDialogIsShown = true;
}

void ApplicationDelegate::closeSaveLevelsDialog() {
    saveLevelsDialogIsShown = false;
}

void ApplicationDelegate::openLoadLevelsDialog() {
    if (loadLevelsDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Load levels file", true, 0, static_cast<irr::s32>(GUIElementId::LOAD_LEVELS_DIALOG));
    loadLevelsDialogIsShown = true;
}

void ApplicationDelegate::closeLoadLevelsDialog() {
    loadLevelsDialogIsShown = false;
}

void ApplicationDelegate::openAboutWindow() {
    guienv->addMessageBox(L"About", ABOUT_TEXT.c_str());
    aboutWindowIsShown = true;
}

void ApplicationDelegate::closeAboutWindow() {
    aboutWindowIsShown = false;
}

void ApplicationDelegate::openLoadLevelMeshDialog() {
    if (loadLevelMeshDialogIsShown) {
        return;
    }

    guienv->addFileOpenDialog(L"Load level mesh", true, 0, static_cast<irr::s32>(GUIElementId::LOAD_LEVEL_MESH_DIALOG));

    loadLevelMeshDialogIsShown = true;
}

void ApplicationDelegate::closeLoadLevelMeshDialog() {
    loadLevelMeshDialogIsShown = false;
}

void ApplicationDelegate::loadLevels(const std::wstring& filename) {
    gameData->loadFromFile(filename);

    if (gameData->getLevels().size() > 0) {
        levelSelected(gameData->getLevels().at(0)->getId());
    }

    gameManagerTree->rebuild();

    loadLevelsDialogIsShown = false;
}

void ApplicationDelegate::saveLevels() {
    if (levelsFilename != std::nullopt) {
        saveLevels(*levelsFilename);
    }
    else {
        openSaveLevelsDialog();
    }
}

void ApplicationDelegate::saveLevels(const std::wstring& filename) {
    loadLevelsDialogIsShown = false;

    gameData->saveToFile(filename);

    levelsFilename = filename;
}

void ApplicationDelegate::addLevel(const std::wstring& meshFilename) {
    loadLevelMeshDialogIsShown = false;

    irr::scene::ISceneNode* sceneNode = loadMesh(meshFilename);

    if (!sceneNode) {
        return;
    }

    std::shared_ptr<Level> level = gameData->createLevel(meshFilename);
    
    level->setSceneNode(sceneNode);

    levelSelected(level->getId());

    gameManagerTree->rebuild();
}

void ApplicationDelegate::addTarget() {
    if (gameData->getCurrentLevel() == nullptr) {
        guienv->addMessageBox(L"Error", L"You have to select a level before placing a target");
        return;
    }

    irr::core::vector3df targetPosition = getTargetPositionFromCameraView();

    std::shared_ptr<Target> target = gameData->getCurrentLevel()->createTarget(targetPosition);

    // TODO: replace with actual target model
    irr::scene::ISceneNode* targetSceneNode = smgr->addSphereSceneNode(5.0f, 16, 0, -1, targetPosition);

    target->setSceneNode(targetSceneNode);

    targetSelected(target->getId());

    gameManagerTree->rebuild();
}

void ApplicationDelegate::levelSelected(const std::wstring& levelId) {
    for (auto level : gameData->getLevels()) {
        if (level->getSceneNode() != nullptr) {
            level->getSceneNode()->setVisible(false);
        }
    }

    gameData->setCurrentLevel(gameData->getLevelById(levelId));

    if (!gameData->getCurrentLevel()->getSceneNode()) {
        irr::scene::ISceneNode* sceneNode = loadMesh(gameData->getCurrentLevel()->getMeshFilename());

        if (!sceneNode) {
            return;
        }

        gameData->getCurrentLevel()->setSceneNode(sceneNode);
    }

    gameData->getCurrentLevel()->getSceneNode()->setVisible(true);

    // setCameraToOrbit(gameData->getCurrentLevel()->getSceneNode());
}

void ApplicationDelegate::targetSelected(const std::wstring& targetId) {
    if (gameData->getCurrentLevel() == nullptr) {
        return;
    }

    gameData->setCurrentTarget(gameData->getCurrentLevel()->getTargetById(targetId));

    // setCameraToOrbit(gameData->getCurrentTarget()->getSceneNode());
    // TODO: additional behavior
}

void ApplicationDelegate::setCameraToOrbit(irr::scene::ISceneNode* sceneNode) {
    // setCameraToOrbit(sceneNode, (camera->getPosition() - sceneNode->getBoundingBox().getCenter()).getLength());
}

void ApplicationDelegate::setCameraToOrbit(irr::scene::ISceneNode* sceneNode, float distance) {
    // camera->setTarget(sceneNode->getTransformedBoundingBox().getCenter());
    /*irr::scene::ISceneNodeAnimatorCameraMaya* animator = reinterpret_cast<irr::scene::ISceneNodeAnimatorCameraMaya*>(*(camera->getAnimators().begin()));
    animator->setDistance(distance);*/
}

void ApplicationDelegate::gameManagerNodeSelected() {
    GameManagerNodeData* nodeData = gameManagerTree->getSelectedNodeData();

    if (!nodeData) {
        return;
    }

    if (nodeData->getType() == GameManagerNodeDataType::LEVEL) {
        levelSelected(nodeData->getId());
    }
    else {
        targetSelected(nodeData->getId());
    }
}

irr::core::vector3df ApplicationDelegate::getTargetPositionFromCameraView() const {
    irr::core::triangle3df triangle;
    irr::core::vector3df collisionPoint;
    irr::core::line3df ray(camera->getAbsolutePosition(), camera->getAbsolutePosition() + camera->getTarget() * camera->getFarValue());

    const float PICK_DISTANCE = 20.f;

    irr::scene::ISceneNode* collisionNode = smgr->getSceneCollisionManager()->getSceneNodeAndCollisionPointFromRay(ray, collisionPoint, triangle);

    if (collisionNode) {
        return collisionPoint;
    }

    irr::core::vector3df targetPosition = camera->getAbsolutePosition() + camera->getTarget() * PICK_DISTANCE;

    return targetPosition;
}

irr::scene::ISceneNode* ApplicationDelegate::loadMesh(const std::wstring& meshFilename) {
    irr::scene::IMesh * levelMesh = smgr->getMesh(meshFilename.c_str());

    if (!levelMesh) {
        std::wostringstream errorMessage;
        errorMessage << "Could not load level mesh file: " << meshFilename;
        guienv->addMessageBox(L"Error", errorMessage.str().c_str());
        return nullptr;
    }

    irr::scene::ISceneNode* sceneNode = smgr->addMeshSceneNode(levelMesh);
    sceneNode->setName(meshFilename.c_str());
    return sceneNode;
}

//void ApplicationDelegate::moveCamera(irr::core::vector3df direction) {
//    irr::core::matrix4 m;
//    m.buildCameraLookAtMatrixRH(camera->getPosition(), camera->getTarget(), camera->getUpVector());
//    m.setTranslation(direction);
//    irr::core::vector3df newPosition = camera->getPosition();
//    m.transformVect(newPosition);
//    static const float CAMERA_MOVEMENT_SPEED = 100.f;
//    camera->setPosition(newPosition * CAMERA_MOVEMENT_SPEED);
//    camera->updateAbsolutePosition(); // for whatever reason this does not work :(
//}
