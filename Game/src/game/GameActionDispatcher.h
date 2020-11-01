#pragma once

#include "../core/ActionDispatcher.h"

#include "GameState.h"
#include "Level.h"

class GameActionDispatcher : public ActionDispatcher
{
public:
    GameActionDispatcher(const std::shared_ptr<GameState> &gameState);

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

protected:
    std::shared_ptr<GameState> getState() const;
};
