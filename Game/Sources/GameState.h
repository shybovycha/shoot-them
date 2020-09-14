#pragma once

#include <string>
#include <memory>
#include <vector>
#include <queue>

#include "Level.h"
#include "PlayerState.h"
#include "QueueAction.h"
#include "Score.h"

class GameState {
public:
    GameState();

    const std::shared_ptr<Score> getCurrentScore() const;

    const E_GAME_STATE getCurrentState() const;

    const std::shared_ptr<Level> getCurrentLevel() const;

    const std::shared_ptr<PlayerState> getPlayerState() const;

    const int getCurrentLevelIndex() const;

    const int getLevelsCnt() const;

    const bool hasActions() const;

    QueueAction* nextAction();

    const std::shared_ptr<Level> getNextLevel() const;

private:
    friend class ActionDispatcher;

    void timeElapsed(unsigned long time);

    void setLevels(std::vector<std::shared_ptr<Level>> _levels);

    void nextLevelLoaded();

    void enqueue(QueueAction* action);

    void setCurrentState(E_GAME_STATE _state);

    E_GAME_STATE currentState;

    std::shared_ptr<Score> currentScore;
    std::shared_ptr<PlayerState> playerState;

    std::queue<QueueAction*> actionQueue;

    std::vector<std::shared_ptr<Level>> levels;
    size_t currentLevel;
};
