#pragma once

#include "../core/StateManager.h"

#include "GameState.h"
#include "Level.h"

class GameActionDispatcher
{
public:
    void shoot(SceneNode* objectAtCursor);

    void reload();

    void mainMenu();

    void hideMainMenu();

    void targetEliminated();

    void loadNextLevel();

    void loadFirstLevel();

    void nextLevelLoaded();

    void firstLevelLoaded();

    void levelsLoaded(std::vector<std::shared_ptr<Level>> levels);

    void startNewGame();

    void gameOver();

    void quit();
};
