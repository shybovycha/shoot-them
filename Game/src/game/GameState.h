#pragma once

#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "../core/QueueAction.h"
#include "../core/StateManager.h"

#include "Level.h"
#include "PlayerState.h"
#include "Score.h"

class GameState
{
public:
    GameState();

    const std::shared_ptr<Score> getCurrentScore() const;

    const GameStateType getCurrentState() const;

    const std::shared_ptr<Level> getCurrentLevel() const;

    const std::shared_ptr<PlayerState> getPlayerState() const;

    const int getCurrentLevelIndex() const;

    const int getLevelsCnt() const;

    const bool isGameOver() const;

    const bool isGameStarted() const;

    const std::shared_ptr<Level> getNextLevel() const;

    // modifiers
    void timeElapsed(unsigned long time);

    void setLevels(std::vector<std::shared_ptr<Level>> _levels);

    void nextLevelLoaded();

    void setCurrentState(GameStateType _state);

    void startGame();

    void endGame();

    GameStateType currentState;

    std::shared_ptr<Score> currentScore;
    std::shared_ptr<PlayerState> playerState;

    std::vector<std::shared_ptr<Level>> levels;
    size_t currentLevel;

    bool hasGameStarted = false;
    bool hasGameEnded = false;
};

template class StateManager<GameState>;
