#pragma once

#include <utility>

#include "../core/ActionHandler.h"
#include "../core/StateManager.h"

#include "GameActionDispatcher.h"
#include "GameState.h"
#include "GameGUI.h"

class GameActionHandler : public ActionHandler
{
public:
    GameActionHandler(std::unique_ptr<GameGUI> gameGUI);

    virtual void handleAction(QueueAction* action) override;

protected:
    void processAction(LoadFirstLevelAction* action);

    void processAction(LoadNextLevelAction* action);

    void processAction(TargetEliminatedAction* action);

    void processAction(StartNewGameAction* action);

    void processAction(MainMenuAction* action);

    void processAction(HideMainMenuAction* action);

    void processAction(QuitAction* action);

    void processAction(GameOverAction* action);

    virtual void loadLevel(std::shared_ptr<Level> level) = 0;

    virtual void unloadLevel(std::shared_ptr<Level> level) = 0;

    virtual void quit() = 0;

    void setGameGUI(std::shared_ptr<GameGUI> gameGUI);

    std::unique_ptr<GameActionDispatcher> actionDispatcher;
    std::shared_ptr<GameGUI> gameGUI;
};
