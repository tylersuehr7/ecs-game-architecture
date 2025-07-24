#include "ecs/world.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <iostream>

/**
 * @brief Simple example demonstrating basic ECS usage
 * 
 * This example shows:
 * 1. Creating a world and adding systems
 * 2. Creating entities and adding components
 * 3. Running the simulation for a few steps
 * 4. Demonstrating component queries and updates
 */

int main() {
    std::cout << "=== Simple ECS Example ===\n\n";
    
    // Step 1: Create world and add systems
    game::ecs::World world;
    auto* movement_system = world.add_system<demo::MovementSystem>();
    
    if (!movement_system) {
        std::cout << "Failed to add movement system!\n";
        return 1;
    }
    
    // Step 2: Initialize world
    if (!world.initialize()) {
        std::cout << "Failed to initialize world!\n";
        return 1;
    }
    
    std::cout << "World initialized successfully!\n\n";
    
    // Step 3: Create entities with components
    std::cout << "Creating entities...\n";
    
    // Create a moving entity
    auto* moving_entity = movement_system->add_entity();
    moving_entity->add_component<demo::Name>("Moving Box");
    moving_entity->add_component<demo::Position>(0.0f, 0.0f);
    moving_entity->add_component<demo::Velocity>(2.0f, 1.0f);
    
    // Create a stationary entity  
    auto* static_entity = movement_system->add_entity();
    static_entity->add_component<demo::Name>("Static Block");
    static_entity->add_component<demo::Position>(5.0f, 3.0f);
    // Note: No velocity component, so it won't move
    
    // Create an entity with health
    auto* health_entity = movement_system->add_entity();
    health_entity->add_component<demo::Name>("Health Demo");
    health_entity->add_component<demo::Position>(10.0f, 5.0f);
    health_entity->add_component<demo::Health>(50);
    
    std::cout << "Created " << movement_system->get_entities().size() << " entities.\n\n";
    
    // Step 4: Show initial state
    std::cout << "=== Initial State ===\n";
    for (auto& [id, entity] : movement_system->get_entities()) {
        auto* name = entity->get_component<demo::Name>();
        auto* pos = entity->get_component<demo::Position>();
        auto* vel = entity->get_component<demo::Velocity>();
        auto* health = entity->get_component<demo::Health>();
        
        if (name && pos) {
            std::cout << name->name << " at (" << pos->x << ", " << pos->y << ")";
            if (vel) {
                std::cout << " moving at (" << vel->dx << ", " << vel->dy << ")";
            }
            if (health) {
                std::cout << " with " << health->current_health << " HP";
            }
            std::cout << "\n";
        }
    }
    
    // Step 5: Run simulation for a few frames
    std::cout << "\n=== Running Simulation ===\n";
    const float time_step = 1.0f;  // 1 second per step
    
    for (int step = 1; step <= 3; ++step) {
        std::cout << "\n--- Step " << step << " ---\n";
        
        // Update world
        world.tick(time_step);
        
        // Show updated positions
        for (auto& [id, entity] : movement_system->get_entities()) {
            auto* name = entity->get_component<demo::Name>();
            auto* pos = entity->get_component<demo::Position>();
            
            if (name && pos) {
                std::cout << name->name << " now at (" << pos->x << ", " << pos->y << ")\n";
            }
        }
    }
    
    // Step 6: Demonstrate component manipulation
    std::cout << "\n=== Component Manipulation ===\n";
    
    // Add velocity to the static entity
    static_entity->add_component<demo::Velocity>(-1.0f, 0.5f);
    std::cout << "Added velocity to Static Block\n";
    
    // Modify existing component
    auto* moving_vel = moving_entity->get_component<demo::Velocity>();
    if (moving_vel) {
        moving_vel->dx *= -1;  // Reverse direction
        moving_vel->dy *= -1;
        std::cout << "Reversed Moving Box velocity\n";
    }
    
    // Run one more step to see changes
    std::cout << "\n--- Final Step ---\n";
    world.tick(time_step);
    
    for (auto& [id, entity] : movement_system->get_entities()) {
        auto* name = entity->get_component<demo::Name>();
        auto* pos = entity->get_component<demo::Position>();
        
        if (name && pos) {
            std::cout << name->name << " final position: (" << pos->x << ", " << pos->y << ")\n";
        }
    }
    
    // Step 7: Demonstrate component removal
    std::cout << "\n=== Component Removal ===\n";
    
    // Remove velocity from moving entity
    bool removed = moving_entity->remove_component<demo::Velocity>();
    std::cout << "Removed velocity from Moving Box: " << (removed ? "Success" : "Failed") << "\n";
    
    // Check if component is gone
    auto* removed_vel = moving_entity->get_component<demo::Velocity>();
    std::cout << "Moving Box has velocity: " << (removed_vel ? "Yes" : "No") << "\n";
    
    std::cout << "\n=== Example Complete ===\n";
    std::cout << "This example demonstrated:\n";
    std::cout << "- World and system creation\n";
    std::cout << "- Entity creation and component attachment\n";
    std::cout << "- System processing (movement updates)\n";
    std::cout << "- Component querying and manipulation\n";
    std::cout << "- Component removal\n\n";
    
    std::cout << "Check out demo_main.cpp for a more complex interactive example!\n";
    
    return 0;
} 