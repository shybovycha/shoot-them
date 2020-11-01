#pragma once

#include <irrlicht.h>

#include "../game/GameGUI.h"
#include "../game/GameState.h"
#include "../core/StateManager.h"

#define NEW_GAME_BUTTON_ID 1
#define CONTINUE_BUTTON_ID 2
#define QUIT_BUTTON_ID 3

class IrrlichtGameGUI : GameGUI
{
public:
    IrrlichtGameGUI(irr::IrrlichtDevice* device, irr::video::IVideoDriver* driver, irr::gui::IGUIEnvironment* guienv);

    virtual void showCursor() override;

    virtual void hideCursor() override;

    virtual void showMainMenu() override;

    virtual void hideMainMenu() override;

    virtual void showGameOverMenu() override;

    virtual void hideGameOverMenu() override;

    virtual void render() override;

private:
    irr::IrrlichtDevice* device = 0;
    irr::gui::IGUIEnvironment* guienv = 0;
    irr::video::IVideoDriver* driver = 0;

    irr::gui::IGUIWindow* msgbox = 0;
    irr::gui::IGUIListBox* hiscoreTable = 0;
    irr::gui::IGUIWindow* mainMenuWindow = 0;
    irr::gui::IGUIStaticText* gameOverLabel = 0;
};
