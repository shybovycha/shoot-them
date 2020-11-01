#include "IrrlichtGameGUI.h"

IrrlichtGameGUI::IrrlichtGameGUI(irr::IrrlichtDevice* _device, irr::video::IVideoDriver* _driver, irr::gui::IGUIEnvironment* _guienv) : GameGUI(), device(_device), driver(_driver), guienv(_guienv)
{
    irr::gui::IGUIFont* font = guienv->getFont("calibri.xml");

    guienv->getSkin()->setFont(font);

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

    const auto gameOverText = L"Game over";
    const auto screenSize = driver->getScreenSize();
    const auto textSize = font->getDimension(gameOverText);

    gameOverLabel = guienv->addStaticText(
            gameOverText,
            irr::core::rect<irr::s32>(
                    (screenSize.Width / 2) - (textSize.Width / 2),
                    (screenSize.Height / 2) - (textSize.Height / 2),
                    (screenSize.Width / 2) + (textSize.Width / 2),
                    (screenSize.Height / 2) + (textSize.Height / 2)
            ),
            false
    );
}

void IrrlichtGameGUI::showCursor()
{
    device->getCursorControl()->setVisible(false);
}

void IrrlichtGameGUI::hideCursor()
{
    device->getCursorControl()->setVisible(true);
}

void IrrlichtGameGUI::showMainMenu()
{
    auto gameState = StateManager<GameState>::getInstance()->getState();

    if (gameState->isGameOver() || !gameState->isGameStarted())
    {
        mainMenuWindow->getElementFromId(CONTINUE_BUTTON_ID)->setEnabled(false);
    }
    else
    {
        mainMenuWindow->getElementFromId(CONTINUE_BUTTON_ID)->setEnabled(true);
    }

    mainMenuWindow->setVisible(true);
}

void IrrlichtGameGUI::hideMainMenu()
{
    hideCursor();

    mainMenuWindow->setVisible(false);
}

void IrrlichtGameGUI::showGameOverMenu()
{
    if (gameOverLabel == nullptr)
    {
        return;
    }

    gameOverLabel->setVisible(true);
}

void IrrlichtGameGUI::hideGameOverMenu()
{
    if (gameOverLabel == nullptr)
    {
        return;
    }

    gameOverLabel->setVisible(false);
}

void IrrlichtGameGUI::render()
{
    guienv->drawAll();
}
