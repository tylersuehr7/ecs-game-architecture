#ifndef GAME_ECS_COMPONENT_HPP
#define GAME_ECS_COMPONENT_HPP

namespace game {
namespace ecs {

class Entity;

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