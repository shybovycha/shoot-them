#pragma once

class GameGUI
{
public:
    GameGUI();

    virtual void showCursor() = 0;

    virtual void hideCursor() = 0;

    virtual void showMainMenu() = 0;

    virtual void hideMainMenu() = 0;

    virtual void showGameOverMenu() = 0;

    virtual void hideGameOverMenu() = 0;

    virtual void render() = 0;
};
