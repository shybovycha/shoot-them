#pragma once

#include <utility>

#include "../core/ActionHandler.h"

#include "GameActionDispatcher.h"

class GameActionHandler : public ActionHandler
{
public:
    GameActionHandler(std::shared_ptr<ActionDispatcher> actionDispatcher);

    virtual void handle(QueueAction* action, std::shared_ptr<State> state) override;

protected:
    std::shared_ptr<GameActionDispatcher> getActionDispatcher() const;

    void processAction(PlaySoundAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(LoadFirstLevelAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(LoadNextLevelAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(TargetEliminatedAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(StartNewGameAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(MainMenuAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(HideMainMenuAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(QuitAction* action, const std::shared_ptr<GameState>& gameState);

    void processAction(GameOverAction* action, const std::shared_ptr<GameState>& gameState);
};
