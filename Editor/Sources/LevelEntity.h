#pragma once

#include <memory>
#include <string>

#include <irrlicht.h>

enum class LevelEntityType {
    TARGET,
    LIGHT
};

class LevelEntity {
public:
    ~LevelEntity();

    irr::core::vector3df getPosition() const;

    std::wstring getId() const;

    void setPosition(irr::core::vector3df newPosition);

    void setSceneNode(irr::scene::ISceneNode* sceneNode);

    irr::scene::ISceneNode* getSceneNode() const;

    LevelEntityType getType() const;

protected:
    LevelEntity(irr::core::vector3df position, std::wstring id, LevelEntityType type);

    irr::core::vector3df position;
    std::wstring id;

    irr::scene::ISceneNode* sceneNode;

    LevelEntityType type;
};
