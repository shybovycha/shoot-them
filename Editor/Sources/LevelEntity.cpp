#include "LevelEntity.h"

#include <utility>

LevelEntity::LevelEntity(const irr::core::vector3df& _position, std::wstring _id, LevelEntityType _type) : id(std::move(_id)), position(_position), type(_type), sceneNode(nullptr) {}

irr::core::vector3df LevelEntity::getPosition() const {
    return position;
}

std::wstring LevelEntity::getId() const {
    return id;
}

void LevelEntity::setPosition(const irr::core::vector3df& newPosition) {
    position = newPosition;
}

void LevelEntity::setSceneNode(irr::scene::ISceneNode* _sceneNode) {
    sceneNode = _sceneNode;
}

irr::scene::ISceneNode* LevelEntity::getSceneNode() const {
    return sceneNode;
}

LevelEntityType LevelEntity::getType() const {
    return type;
}