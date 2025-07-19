#ifndef GAME_ECS_WORLD_HPP
#define GAME_ECS_WORLD_HPP

#include "system.hpp"
#include <memory>
#include <type_traits>
#include <typeindex>
#include <unordered_map>

namespace game {
namespace ecs {

/**
 * @brief Container mapping system types to their instances
 * 
 * WorldSystems stores all systems registered with the world using
 * type_index as keys for fast system lookup by type. Each world
 * can have at most one system of each type, and systems are
 * stored as unique pointers for automatic memory management.
 */
using WorldSystems = std::unordered_map<std::type_index, std::unique_ptr<System>>;

/**
 * @brief Central coordinator for the ECS architecture
 * 
 * World manages the lifecycle of all systems in the ECS framework.
 * It handles system registration, initialization, updating, and shutdown.
 * The World class serves as the main entry point for the ECS framework,
 * coordinating the execution of all systems during the game loop.
 * Each world can have at most one system of each type.
 */
class World {
    WorldSystems systems_;

public:
    World() = default;

    ~World() {
        shutdown();
    }

    bool initialize() noexcept {
        bool all_systems_initialized = true;
        
        for (auto& [_, system] : systems_) {
            if (!system->initialize()) {
                all_systems_initialized = false;
                break;
            }
        }

        return all_systems_initialized;
    }

    void tick(const float& delta) noexcept {
        for (auto& [_, system] : systems_) {
            system->tick(delta);
        }
    }

    void shutdown() noexcept {
        for (auto& [_, system] : systems_) {
            system->shutdown();
        }
        systems_.clear();
    }

    template<typename T>
    bool has_system() const noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");
        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);
        return it != systems_.end();
    }

    template<typename T>
    [[nodiscard]] T* get_system() noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);

        if (it == systems_.end()) {
            return nullptr;
        }

        return static_cast<T*>(it->second.get());
    }

    template<typename T>
    [[nodiscard]] const T* get_system() const noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);

        if (it == systems_.end()) {
            return nullptr;
        }

        return static_cast<const T*>(it->second.get());
    }

    template<typename T, typename... Args>
    [[nodiscard]] T* add_system(Args&&... args) noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");

        // Check if system already exists
        const auto index = std::type_index(typeid(T));
        if (systems_.find(index) != systems_.end()) {
            return nullptr; // System already exists
        }

        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        auto* system_ptr = system.get();
        
        systems_.emplace(index, std::move(system));

        return system_ptr;
    }

    template<typename T>
    bool remove_system() noexcept {
        static_assert(std::is_base_of_v<System, T>, "T must inherit System");
        const auto index = std::type_index(typeid(T));
        const auto it = systems_.find(index);

        if (it == systems_.end()) {
            return false; // System doesn't exist
        }

        // Call system shutdown lifecycle event
        it->second->shutdown();

        systems_.erase(it);
        return true;
    }
};

}//ecs
}//game

#endif//GAME_ECS_WORLD_HPP