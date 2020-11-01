#include "GameActionHandler.h"

GameActionHandler::GameActionHandler(std::shared_ptr<ActionDispatcher> _actionDispatcher) : ActionHandler(std::move(_actionDispatcher))
{
}

std::shared_ptr<GameActionDispatcher> GameActionHandler::getActionDispatcher() const
{
    return std::dynamic_pointer_cast<GameActionDispatcher>(actionDispatcher);
}

void GameActionHandler::handle(QueueAction* action, std::shared_ptr<State> state)
{
    switch (action->getType())
    {
    case QueueActionType::LOAD_FIRST_LEVEL:
        processAction(reinterpret_cast<LoadFirstLevelAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::LOAD_NEXT_LEVEL:
        processAction(reinterpret_cast<LoadNextLevelAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::PLAY_SOUND:
        processAction(reinterpret_cast<PlaySoundAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::TARGET_ELIMINATED:
        processAction(reinterpret_cast<TargetEliminatedAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::START_NEW_GAME:
        processAction(reinterpret_cast<StartNewGameAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::QUIT:
        processAction(reinterpret_cast<QuitAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::MAIN_MENU:
        processAction(reinterpret_cast<MainMenuAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::HIDE_MAIN_MENU:
        processAction(reinterpret_cast<HideMainMenuAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    case QueueActionType::GAME_OVER:
        processAction(reinterpret_cast<GameOverAction*>(action), std::dynamic_pointer_cast<GameState>(state));
        break;
    }
}

void GameActionHandler::processAction(PlaySoundAction* action, const std::shared_ptr<GameState>& gameState)
{
    soundEngine->play2D(action->getSoundFile().c_str(), false);
}

void GameActionHandler::processAction(LoadFirstLevelAction* action, const std::shared_ptr<GameState>& gameState)
{
    loadLevel(action->getLevel());

    getActionDispatcher()->firstLevelLoaded();
}

void GameActionHandler::processAction(LoadNextLevelAction* action, const std::shared_ptr<GameState>& gameState)
{
    if (action->getNextLevel() == nullptr)
    {
        getActionDispatcher()->gameOver();
        return;
    }

    // unload existing level data
    unloadLevel(action->getPreviousLevel());

    // load next level
    loadLevel(action->getNextLevel());

    getActionDispatcher()->nextLevelLoaded();
}

void GameActionHandler::processAction(TargetEliminatedAction* action, const std::shared_ptr<GameState>& gameState)
{
    // hide in here, remove when the next level is loaded
    action->getTarget()->setVisible(false);
    getActionDispatcher()->targetEliminated();

    if (gameState->getCurrentScore()->getTargetsEliminated() >= gameState->getCurrentLevel()->getTargets().size())
    {
        // TODO: show next level menu
        getActionDispatcher()->loadNextLevel();
    }
}

void GameActionHandler::processAction(StartNewGameAction* action, const std::shared_ptr<GameState>& gameState)
{
    if (gameOverLabel != nullptr)
    {
        gameOverLabel->remove();
        gameOverLabel = nullptr;
    }

    if (gameState->isGameStarted())
    {
        unloadLevel(gameState->getCurrentLevel());
    }

    getActionDispatcher()->loadFirstLevel();
    device->getCursorControl()->setVisible(false);
    mainMenuWindow->setVisible(false);
}

void GameActionHandler::processAction(MainMenuAction* action, const std::shared_ptr<GameState>& gameState)
{
    if (gameOverLabel != nullptr)
    {
        gameOverLabel->setVisible(false);
    }

    mainMenuWindow->setVisible(true);

    if (gameState->isGameOver() || !gameState->isGameStarted())
    {
        mainMenuWindow->getElementFromId(CONTINUE_BUTTON_ID)->setEnabled(false);
    }
    else
    {
        mainMenuWindow->getElementFromId(CONTINUE_BUTTON_ID)->setEnabled(true);
    }

    device->getCursorControl()->setVisible(true);
}

void GameActionHandler::processAction(QuitAction* action, const std::shared_ptr<GameState>& gameState)
{
    device->closeDevice();
}

void GameActionHandler::processAction(HideMainMenuAction* action, const std::shared_ptr<GameState>& gameState)
{
    device->getCursorControl()->setVisible(false);
    mainMenuWindow->setVisible(false);
}

void GameActionHandler::processAction(GameOverAction* action, const std::shared_ptr<GameState>& gameState)
{
    gameOverLabel->setVisible(true);
}
