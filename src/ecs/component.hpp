#ifndef GAME_ECS_COMPONENT_HPP
#define GAME_ECS_COMPONENT_HPP

namespace game {
namespace ecs {

class Entity;
/**
 * @brief Base class for all ECS components
 * 
 * Components are pure data containers that can be attached to entities.
 * They don't contain any game logic, only data that systems operate on.
 * Each component automatically stores a pointer to its owner entity.
 */
struct Component {
    Entity* owner{nullptr};
    virtual ~Component() = default;

protected:
    Component() = default;
    Component(const Component&) = default;
    Component& operator=(const Component&) = default;
    Component(Component&&) = default;
    Component& operator=(Component&&) = default;
};

}//ecs
}//game

#endif//GAME_ECS_COMPONENT_HPP