#ifndef DEMO_SYSTEMS_HPP
#define DEMO_SYSTEMS_HPP

#include "ecs/system.hpp"
#include "components.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace demo {

/**
 * @brief Handles entity movement based on position and velocity
 * 
 * This system processes all entities that have both Position and Velocity components,
 * updating their positions each frame. Demonstrates basic component querying.
 */
class MovementSystem : public game::ecs::System {
public:
    void tick(const float& delta) noexcept override {
        for (auto& [id, entity] : get_entities()) {
            auto* pos = entity->get_component<Position>();
            auto* vel = entity->get_component<Velocity>();
            
            if (pos && vel) {
                pos->x += vel->dx * delta;
                pos->y += vel->dy * delta;
            }
        }
    }
};

/**
 * @brief Renders entities to console output
 * 
 * This system finds all entities with Position and Renderable components
 * and prints their representation. Demonstrates component combination querying.
 */
class RenderSystem : public game::ecs::System {
    static constexpr int WORLD_WIDTH = 80;
    static constexpr int WORLD_HEIGHT = 20;
    
public:
    void tick(const float& delta) noexcept override {
        // Clear screen (simple console rendering)
        system("clear || cls");
        
        // Create world grid
        std::vector<std::vector<char>> world(WORLD_HEIGHT, std::vector<char>(WORLD_WIDTH, '.'));
        
        // Place entities on grid
        for (auto& [id, entity] : get_entities()) {
            auto* pos = entity->get_component<Position>();
            auto* renderable = entity->get_component<Renderable>();
            
            if (pos && renderable && renderable->visible) {
                int x = static_cast<int>(std::round(pos->x));
                int y = static_cast<int>(std::round(pos->y));
                
                // Bounds check
                if (x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT) {
                    world[y][x] = renderable->symbol;
                }
            }
        }
        
        // Render world
        for (const auto& row : world) {
            for (char cell : row) {
                std::cout << cell;
            }
            std::cout << '\n';
        }
        
        // Show entity info
        std::cout << "\nEntities:\n";
        for (auto& [id, entity] : get_entities()) {
            auto* name = entity->get_component<Name>();
            auto* pos = entity->get_component<Position>();
            auto* health = entity->get_component<Health>();
            
            if (name && pos) {
                std::cout << name->name << " at (" << pos->x << ", " << pos->y << ")";
                if (health) {
                    std::cout << " HP: " << health->current_health << "/" << health->max_health;
                }
                std::cout << '\n';
            }
        }
    }
};

/**
 * @brief Manages entity health and death
 * 
 * This system processes entities with Health components, handling health
 * regeneration and entity removal when health reaches zero.
 */
class HealthSystem : public game::ecs::System {
    float health_regen_rate_ = 1.0f; // HP per second
    
public:
    void tick(const float& delta) noexcept override {
        std::vector<game::ecs::EntityID> entities_to_remove;
        
        for (auto& [id, entity] : get_entities()) {
            auto* health = entity->get_component<Health>();
            
            if (health) {
                // Health regeneration (if not at max)
                if (health->current_health < health->max_health && health->current_health > 0) {
                    health->current_health = std::min(
                        health->max_health,
                        health->current_health + static_cast<int>(health_regen_rate_ * delta)
                    );
                }
                
                // Mark dead entities for removal
                if (!health->is_alive()) {
                    entities_to_remove.push_back(id);
                }
            }
        }
        
        // Remove dead entities
        for (auto entity_id : entities_to_remove) {
            auto* entity = get_entity(entity_id);
            if (entity) {
                auto* name = entity->get_component<Name>();
                if (name) {
                    std::cout << name->name << " has died!\n";
                }
            }
            remove_entity(entity_id);
        }
    }
};

/**
 * @brief Simple AI system for autonomous entity behavior
 * 
 * This system processes entities with AI components, implementing basic
 * state machine behavior. Demonstrates more complex system logic.
 */
class AISystem : public game::ecs::System {
public:
    void tick(const float& delta) noexcept override {
        for (auto& [id, entity] : get_entities()) {
            auto* ai = entity->get_component<AI>();
            auto* pos = entity->get_component<Position>();
            auto* vel = entity->get_component<Velocity>();
            
            if (ai && pos && vel) {
                switch (ai->current_state) {
                    case AI::State::Idle:
                        handleIdleState(ai, pos, vel, delta);
                        break;
                    case AI::State::Patrolling:
                        handlePatrolState(ai, pos, vel, delta);
                        break;
                    case AI::State::Chasing:
                        handleChaseState(ai, pos, vel, delta, id);
                        break;
                    case AI::State::Attacking:
                        handleAttackState(ai, pos, vel, delta, id);
                        break;
                }
            }
        }
    }
    
private:
    void handleIdleState(AI* ai, Position* pos, Velocity* vel, float delta) {
        // Stop movement
        vel->dx = vel->dy = 0.0f;
        
        // Transition to patrol if patrol points exist
        if (!ai->patrol_points.empty()) {
            ai->current_state = AI::State::Patrolling;
        }
    }
    
    void handlePatrolState(AI* ai, Position* pos, Velocity* vel, float delta) {
        if (ai->patrol_points.empty()) {
            ai->current_state = AI::State::Idle;
            return;
        }
        
        const auto& target_point = ai->patrol_points[ai->current_patrol_index];
        float dx = target_point.x - pos->x;
        float dy = target_point.y - pos->y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance < 1.0f) {
            // Reached patrol point, move to next
            ai->current_patrol_index = (ai->current_patrol_index + 1) % ai->patrol_points.size();
        } else {
            // Move toward patrol point
            float speed = 10.0f;
            vel->dx = (dx / distance) * speed;
            vel->dy = (dy / distance) * speed;
        }
    }
    
    void handleChaseState(AI* ai, Position* pos, Velocity* vel, float delta, game::ecs::EntityID chaser_id) {
        // Find target entity
        auto* target = get_entity(ai->target_entity_id);
        if (!target) {
            ai->current_state = AI::State::Idle;
            return;
        }
        
        auto* target_pos = target->get_component<Position>();
        if (!target_pos) {
            ai->current_state = AI::State::Idle;
            return;
        }
        
        float dx = target_pos->x - pos->x;
        float dy = target_pos->y - pos->y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        if (distance > ai->detection_range) {
            // Lost target
            ai->current_state = AI::State::Patrolling;
            vel->dx = vel->dy = 0.0f;
        } else if (distance < 2.0f) {
            // Close enough to attack
            ai->current_state = AI::State::Attacking;
        } else {
            // Chase target
            float speed = 15.0f;
            vel->dx = (dx / distance) * speed;
            vel->dy = (dy / distance) * speed;
        }
    }
    
    void handleAttackState(AI* ai, Position* pos, Velocity* vel, float delta, game::ecs::EntityID attacker_id) {
        // Stop movement during attack
        vel->dx = vel->dy = 0.0f;
        
        // Find target and damage it
        auto* target = get_entity(ai->target_entity_id);
        if (target) {
            auto* target_health = target->get_component<Health>();
            auto* target_pos = target->get_component<Position>();
            
            if (target_health && target_pos) {
                float dx = target_pos->x - pos->x;
                float dy = target_pos->y - pos->y;
                float distance = std::sqrt(dx * dx + dy * dy);
                
                if (distance <= 2.0f) {
                    // Deal damage
                    target_health->current_health -= static_cast<int>(50.0f * delta); // 50 DPS
                    if (target_health->current_health <= 0) {
                        ai->current_state = AI::State::Idle;
                    }
                } else {
                    // Target moved away, resume chasing
                    ai->current_state = AI::State::Chasing;
                }
            }
        } else {
            ai->current_state = AI::State::Idle;
        }
    }
};

/**
 * @brief Manages timer components and time-based effects
 * 
 * This system processes entities with Timer components, updating their
 * elapsed time and optionally removing them when finished.
 */
class TimerSystem : public game::ecs::System {
public:
    void tick(const float& delta) noexcept override {
        std::vector<game::ecs::EntityID> entities_to_process;
        
        // First pass: update timers
        for (auto& [id, entity] : get_entities()) {
            auto* timer = entity->get_component<Timer>();
            
            if (timer) {
                timer->elapsed_time += delta;
                
                if (timer->is_finished() && timer->auto_remove) {
                    entities_to_process.push_back(id);
                }
            }
        }
        
        // Second pass: remove entities with finished auto-remove timers
        for (auto entity_id : entities_to_process) {
            remove_entity(entity_id);
        }
    }
};

} // namespace demo

#endif // DEMO_SYSTEMS_HPP 