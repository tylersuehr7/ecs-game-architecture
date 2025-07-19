#include "ecs/component.hpp"
#include "ecs/entity.hpp"
#include "ecs/system.hpp"
#include "ecs/world.hpp"
#include <iostream>

// Example components
struct Position : public game::ecs::Component {
    float x, y;
    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct Velocity : public game::ecs::Component {
    float dx, dy;
    Velocity(float dx = 0.0f, float dy = 0.0f) : dx(dx), dy(dy) {}
};

// Example system
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

int main() {
    std::cout << "Testing ECS Architecture\n";
    
    // Create world and add system
    game::ecs::World world;
    auto* movement_system = world.add_system<MovementSystem>();
    
    if (!movement_system) {
        std::cout << "Failed to add movement system\n";
        return 1;
    }
    
    // Initialize world
    if (!world.initialize()) {
        std::cout << "Failed to initialize world\n";
        return 1;
    }
    
    // Create entity and add components
    auto* entity = movement_system->add_entity();
    auto* pos = entity->add_component<Position>(10.0f, 20.0f);
    auto* vel = entity->add_component<Velocity>(1.0f, 2.0f);
    
    // Verify component owner pointers are set correctly
    if (pos->owner != entity || vel->owner != entity) {
        std::cout << "Component owner pointers not set correctly\n";
        return 1;
    }
    
    std::cout << "Initial position: (" << pos->x << ", " << pos->y << ")\n";
    
    // Simulate one frame
    world.tick(1.0f);
    
    std::cout << "Position after tick: (" << pos->x << ", " << pos->y << ")\n";
    
    // Test component removal
    bool removed = entity->remove_component<Velocity>();
    if (!removed) {
        std::cout << "Failed to remove velocity component\n";
        return 1;
    }
    
    // Verify component is gone
    if (entity->has_component<Velocity>()) {
        std::cout << "Velocity component still exists after removal\n";
        return 1;
    }
    
    std::cout << "ECS Architecture test completed successfully!\n";
    return 0;
}
