#include "GameActionDispatcher.h"

void GameActionDispatcher::shoot(SceneNode* objectAtCursor)
{
    if (StateManager<GameState>::getInstance()->getState()->getPlayerState()->getCurrentAmmo() <= 0)
    {
        StateManager<GameState>::getInstance()->dispatch(new PlaySoundAction("resources/sounds/noammo.wav"));
        return;
    }

    StateManager<GameState>::getInstance()->dispatch(new PlaySoundAction("resources/sounds/shot.wav"));

    StateManager<GameState>::getInstance()->getState()->getPlayerState()->shoot();

    if (objectAtCursor == StateManager<GameState>::getInstance()->getState()->getCurrentLevel()->getModel())
    {
        return;
    }

    for (auto target : StateManager<GameState>::getInstance()->getState()->getCurrentLevel()->getTargets())
    {
        if (*target != *objectAtCursor)
        {
            continue;
        }

        StateManager<GameState>::getInstance()->dispatch(new TargetEliminatedAction(target));
        StateManager<GameState>::getInstance()->dispatch(new PlaySoundAction("resources/sounds/bell.wav"));

        break;
    }
}

void GameActionDispatcher::reload()
{
    StateManager<GameState>::getInstance()->getState()->getPlayerState()->reload();
    StateManager<GameState>::getInstance()->dispatch(new PlaySoundAction("resources/sounds/reload.wav"));
}

void GameActionDispatcher::mainMenu()
{
    StateManager<GameState>::getInstance()->getState()->setCurrentState(GameStateType::MAIN_MENU);
    StateManager<GameState>::getInstance()->dispatch(new MainMenuAction());
}

void GameActionDispatcher::hideMainMenu()
{
    StateManager<GameState>::getInstance()->getState()->setCurrentState(GameStateType::PLAYING);
    StateManager<GameState>::getInstance()->dispatch(new HideMainMenuAction());
}

void GameActionDispatcher::targetEliminated()
{
    StateManager<GameState>::getInstance()->getState()->getCurrentScore()->targetEliminated();
}

void GameActionDispatcher::loadNextLevel()
{
    StateManager<GameState>::getInstance()->dispatch(new LoadNextLevelAction(StateManager<GameState>::getInstance()->getState()->getCurrentLevel(), StateManager<GameState>::getInstance()->getState()->getNextLevel()));
}

void GameActionDispatcher::loadFirstLevel()
{
    StateManager<GameState>::getInstance()->dispatch(new LoadFirstLevelAction(StateManager<GameState>::getInstance()->getState()->getCurrentLevel()));
}

void GameActionDispatcher::firstLevelLoaded()
{
    StateManager<GameState>::getInstance()->getState()->startGame();
    StateManager<GameState>::getInstance()->getState()->setCurrentState(GameStateType::PLAYING);
}

void GameActionDispatcher::nextLevelLoaded()
{
    StateManager<GameState>::getInstance()->getState()->nextLevelLoaded();
    StateManager<GameState>::getInstance()->getState()->getCurrentScore()->resetTargetEliminated();
}

void GameActionDispatcher::levelsLoaded(std::vector<std::shared_ptr<Level>> levels)
{
    StateManager<GameState>::getInstance()->getState()->setLevels(levels);
}

void GameActionDispatcher::startNewGame()
{
    StateManager<GameState>::getInstance()->dispatch(new StartNewGameAction());
}

void GameActionDispatcher::quit()
{
    StateManager<GameState>::getInstance()->dispatch(new QuitAction());
}

void GameActionDispatcher::gameOver()
{
    StateManager<GameState>::getInstance()->getState()->setCurrentState(GameStateType::END_GAME);
    StateManager<GameState>::getInstance()->dispatch(new GameOverAction());
    StateManager<GameState>::getInstance()->getState()->endGame();
}
