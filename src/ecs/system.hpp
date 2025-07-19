#ifndef GAME_ECS_SYSTEM_HPP
#define GAME_ECS_SYSTEM_HPP

#include "entity.hpp"
#include <memory>
#include <unordered_map>

namespace game {
namespace ecs {

using SystemEntities = std::unordered_map<EntityID, std::unique_ptr<Entity>>;

class System {
    EntityID next_entity_id_{1};
    SystemEntities entities_;

public:
    virtual bool initialize() noexcept {
        return true;
    }

    virtual void tick(const float& delta) noexcept = 0;

    virtual void shutdown() noexcept {
    }

    const SystemEntities& get_entities() const noexcept { return entities_; }
    SystemEntities& get_entities() noexcept { return entities_; }

    bool has_entity(const EntityID id) const noexcept {
        const auto it = entities_.find(id);
        return it != entities_.end();
    }

    [[nodiscard]] Entity* get_entity(const EntityID id) noexcept {
        const auto it = entities_.find(id);
        
        if (it == entities_.end()) {
            return nullptr;
        }

        return it->second.get();
    }

    [[nodiscard]] Entity* add_entity() noexcept {
        const auto new_entity_id = next_entity_id_++;

        auto entity = std::make_unique<Entity>(new_entity_id);
        auto* entity_ptr = entity.get();

        entities_.emplace(new_entity_id, std::move(entity));

        return entity_ptr;
    }

    void remove_entity(const EntityID id) {
        entities_.erase(id);
    }
};

}//ecs
}//game

#endif//GAME_ECS_SYSTEM_HPP