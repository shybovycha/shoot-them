#include "GameActionDispatcher.h"

GameActionDispatcher::GameActionDispatcher(const std::shared_ptr<GameState> &state)
        : ActionDispatcher(state)
{
}

std::shared_ptr<GameState> GameActionDispatcher::getState() const
{
    return std::dynamic_pointer_cast<GameState>(state);
}

void GameActionDispatcher::shoot(SceneNode* objectAtCursor)
{
    if (getState()->getPlayerState()->getCurrentAmmo() <= 0)
    {
        dispatch(new PlaySoundAction("resources/sounds/noammo.wav"));
        return;
    }

    dispatch(new PlaySoundAction("resources/sounds/shot.wav"));

    getState()->getPlayerState()->shoot();

    if (objectAtCursor == getState()->getCurrentLevel()->getModel())
    {
        return;
    }

    for (auto target : getState()->getCurrentLevel()->getTargets())
    {
        if (*target != *objectAtCursor)
        {
            continue;
        }

        dispatch(new TargetEliminatedAction(target));
        dispatch(new PlaySoundAction("resources/sounds/bell.wav"));

        break;
    }
}

void GameActionDispatcher::reload()
{
    getState()->getPlayerState()->reload();
    dispatch(new PlaySoundAction("resources/sounds/reload.wav"));
}

void GameActionDispatcher::mainMenu()
{
    getState()->setCurrentState(GameStateType::MAIN_MENU);
    dispatch(new MainMenuAction());
}

void GameActionDispatcher::hideMainMenu()
{
    getState()->setCurrentState(GameStateType::PLAYING);
    dispatch(new HideMainMenuAction());
}

void GameActionDispatcher::targetEliminated()
{
    getState()->getCurrentScore()->targetEliminated();
}

void GameActionDispatcher::loadNextLevel()
{
    dispatch(new LoadNextLevelAction(getState()->getCurrentLevel(), getState()->getNextLevel()));
}

void GameActionDispatcher::loadFirstLevel()
{
    dispatch(new LoadFirstLevelAction(getState()->getCurrentLevel()));
}

void GameActionDispatcher::firstLevelLoaded()
{
    getState()->startGame();
    getState()->setCurrentState(GameStateType::PLAYING);
}

void GameActionDispatcher::nextLevelLoaded()
{
    getState()->nextLevelLoaded();
    getState()->getCurrentScore()->resetTargetEliminated();
}

void GameActionDispatcher::levelsLoaded(std::vector<std::shared_ptr<Level>> levels)
{
    getState()->setLevels(levels);
}

void GameActionDispatcher::startNewGame()
{
    dispatch(new StartNewGameAction());
}

void GameActionDispatcher::quit()
{
    dispatch(new QuitAction());
}

void GameActionDispatcher::gameOver()
{
    getState()->setCurrentState(GameStateType::END_GAME);
    dispatch(new GameOverAction());
    getState()->endGame();
}
