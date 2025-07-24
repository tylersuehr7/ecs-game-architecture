#ifndef DEMO_COMPONENTS_HPP
#define DEMO_COMPONENTS_HPP

#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include <string>
#include <vector>

namespace demo {

/**
 * @brief Position component for 2D world coordinates
 * 
 * Stores x, y coordinates representing an entity's position in 2D space.
 * This is one of the most common components in game ECS architectures.
 */
struct Position : public game::ecs::Component {
    float x, y;
    
    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

/**
 * @brief Velocity component for movement
 * 
 * Stores velocity in x and y directions (units per second).
 * Combined with Position, this enables movement systems.
 */
struct Velocity : public game::ecs::Component {
    float dx, dy;
    
    Velocity(float dx = 0.0f, float dy = 0.0f) : dx(dx), dy(dy) {}
};

/**
 * @brief Health component for damageable entities
 * 
 * Tracks current and maximum health values.
 * Demonstrates state management within components.
 */
struct Health : public game::ecs::Component {
    int current_health;
    int max_health;
    
    Health(int max_hp = 100) : current_health(max_hp), max_health(max_hp) {}
    
    bool is_alive() const { return current_health > 0; }
    float health_percentage() const { return static_cast<float>(current_health) / max_health; }
};

/**
 * @brief Renderable component for visual entities
 * 
 * Contains display information like character representation and color.
 * Demonstrates string data storage in components.
 */
struct Renderable : public game::ecs::Component {
    char symbol;
    std::string color;
    bool visible;
    
    Renderable(char symbol = '?', const std::string& color = "white", bool visible = true)
        : symbol(symbol), color(color), visible(visible) {}
};

/**
 * @brief Name component for entity identification
 * 
 * Simple string storage for entity names.
 * Useful for debugging and UI display.
 */
struct Name : public game::ecs::Component {
    std::string name;
    
    explicit Name(const std::string& name = "Unnamed") : name(name) {}
};

/**
 * @brief AI component for autonomous behavior
 * 
 * Contains AI state and target information.
 * Demonstrates more complex component data structures.
 */
struct AI : public game::ecs::Component {
    enum class State { Idle, Patrolling, Chasing, Attacking };
    
    State current_state;
    game::ecs::EntityID target_entity_id;
    std::vector<Position> patrol_points;
    size_t current_patrol_index;
    float detection_range;
    
    AI(float detection_range = 5.0f) 
        : current_state(State::Idle)
        , target_entity_id(0)
        , current_patrol_index(0)
        , detection_range(detection_range) {}
};

/**
 * @brief Timer component for time-based behaviors
 * 
 * Tracks elapsed time and duration for temporary effects.
 * Demonstrates time management within components.
 */
struct Timer : public game::ecs::Component {
    float elapsed_time;
    float duration;
    bool auto_remove;
    
    Timer(float duration = 1.0f, bool auto_remove = false) 
        : elapsed_time(0.0f), duration(duration), auto_remove(auto_remove) {}
    
    bool is_finished() const { return elapsed_time >= duration; }
    float progress() const { return std::min(elapsed_time / duration, 1.0f); }
};

} // namespace demo

#endif // DEMO_COMPONENTS_HPP 