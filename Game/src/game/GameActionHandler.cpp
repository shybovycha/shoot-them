#include "GameActionHandler.h"

GameActionHandler::GameActionHandler(std::unique_ptr<GameGUI> _gameGUI) : ActionHandler(),
        actionDispatcher(std::make_unique<GameActionDispatcher>()),
        gameGUI(std::move(_gameGUI))
{
    StateManager<GameState>::getInstance()->subscribe(this);
}

void GameActionHandler::handleAction(QueueAction* action)
{
    switch (action->getType())
    {
    case QueueActionType::LOAD_FIRST_LEVEL:
        processAction(reinterpret_cast<LoadFirstLevelAction*>(action));
        break;
    case QueueActionType::LOAD_NEXT_LEVEL:
        processAction(reinterpret_cast<LoadNextLevelAction*>(action));
        break;
    case QueueActionType::TARGET_ELIMINATED:
        processAction(reinterpret_cast<TargetEliminatedAction*>(action));
        break;
    case QueueActionType::START_NEW_GAME:
        processAction(reinterpret_cast<StartNewGameAction*>(action));
        break;
    case QueueActionType::QUIT:
        processAction(reinterpret_cast<QuitAction*>(action));
        break;
    case QueueActionType::MAIN_MENU:
        processAction(reinterpret_cast<MainMenuAction*>(action));
        break;
    case QueueActionType::HIDE_MAIN_MENU:
        processAction(reinterpret_cast<HideMainMenuAction*>(action));
        break;
    case QueueActionType::GAME_OVER:
        processAction(reinterpret_cast<GameOverAction*>(action));
        break;
    }
}

void GameActionHandler::processAction(LoadFirstLevelAction* action)
{
    loadLevel(action->getLevel());

    actionDispatcher->firstLevelLoaded();
}

void GameActionHandler::processAction(LoadNextLevelAction* action)
{
    if (action->getNextLevel() == nullptr)
    {
        actionDispatcher->gameOver();
        return;
    }

    // unload existing level data
    unloadLevel(action->getPreviousLevel());

    // load next level
    loadLevel(action->getNextLevel());

    actionDispatcher->nextLevelLoaded();
}

void GameActionHandler::processAction(TargetEliminatedAction* action)
{
    auto gameState = StateManager<GameState>::getInstance()->getState();

    // hide in here, remove when the next level is loaded
    action->getTarget()->setVisible(false);
    actionDispatcher->targetEliminated();

    if (gameState->getCurrentScore()->getTargetsEliminated() >= gameState->getCurrentLevel()->getTargets().size())
    {
        // TODO: show next level menu
        actionDispatcher->loadNextLevel();
    }
}

void GameActionHandler::processAction(StartNewGameAction* action)
{
    auto gameState = StateManager<GameState>::getInstance()->getState();

    gameGUI->hideGameOverMenu();

    if (gameState->isGameStarted())
    {
        unloadLevel(gameState->getCurrentLevel());
    }

    actionDispatcher->loadFirstLevel();
    gameGUI->hideMainMenu();
}

void GameActionHandler::processAction(MainMenuAction* action)
{
    gameGUI->hideGameOverMenu();
    gameGUI->showMainMenu();
}

void GameActionHandler::processAction(QuitAction* action)
{
    quit();
}

void GameActionHandler::processAction(HideMainMenuAction* action)
{
    gameGUI->hideCursor();
    gameGUI->hideMainMenu();
}

void GameActionHandler::processAction(GameOverAction* action)
{
    gameGUI->showGameOverMenu();
}

void GameActionHandler::setGameGUI(std::shared_ptr<GameGUI> _gameGUI)
{
    gameGUI = std::move(_gameGUI);
}
