#ifndef GAME_ECS_SYSTEM_HPP
#define GAME_ECS_SYSTEM_HPP

#include "entity.hpp"
#include <memory>
#include <unordered_map>

namespace game {
namespace ecs {

/**
 * @brief Container mapping entity IDs to their instances
 * 
 * SystemEntities stores all entities managed by a system using
 * EntityID as keys for fast entity lookup. Each system maintains
 * its own collection of entities that it processes during the tick cycle.
 * Entities are stored as unique pointers for automatic memory management.
 */
using SystemEntities = std::unordered_map<EntityID, std::unique_ptr<Entity>>;

/**
 * @brief Base class for all ECS systems that process entities
 * 
 * Systems contain the game logic that operates on entities with specific components.
 * Each system manages its own collection of entities and processes them during the tick cycle.
 * Systems are responsible for initializing, updating, and shutting down their functionality,
 * as well as managing the lifecycle of entities they own.
 */
class System {
    EntityID next_entity_id_{1};
    SystemEntities entities_;

public:
    virtual ~System() = default;

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

    [[nodiscard]] const Entity* get_entity(const EntityID id) const noexcept {
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

    bool remove_entity(const EntityID id) noexcept {
        const auto it = entities_.find(id);
        if (it == entities_.end()) {
            return false; // Entity doesn't exist
        }
        
        entities_.erase(it);
        return true;
    }
};

}//ecs
}//game

#endif//GAME_ECS_SYSTEM_HPP