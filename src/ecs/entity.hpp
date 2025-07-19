#ifndef GAME_ECS_ENTITY_HPP
#define GAME_ECS_ENTITY_HPP

#include "component.hpp"
#include <cstdint>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace game {
namespace ecs {

using EntityID = std::uint64_t;
using EntityComponents = std::unordered_map<std::type_index, std::unique_ptr<Component>>;

class Entity {
    EntityID id_;
    EntityComponents components_;
public:
    explicit Entity(const EntityID id): id_(id) {}
    EntityID get_id() const noexcept { return id_; }

    template<typename T>
    [[nodiscard]] T* get_component() {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit Component");

        const auto index = std::type_index(typeid(T));
        const auto it = components_.find(index);

        if (it == components_.end()) {
            return nullptr;
        }
        
        return static_cast<T*>(it->second.get());
    }

    template<typename T>
    bool has_component() {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit Component");

        const auto index = std::type_index(typeid(T));
        const auto it = components_.find(index);

        return it != components_.end();
    }

    template<typename T, typename... Args>
    [[nodiscard]] T* add_component(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit Component");

        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        auto* component_ptr = component.get();

        const auto index = std::type_index(typeid(T));
        components_.emplace(index, std::move(component));

        return component_ptr;
    }

    template<typename T>
    void remove_component() {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit Component");
        const auto index = std::type_index(typeid(T));
        components_.erase(index);
    }
};

}//ecs
}//game

#endif//GAME_ECS_ENTITY_HPP