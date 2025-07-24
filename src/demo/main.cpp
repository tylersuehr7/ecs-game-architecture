#include "ecs/world.hpp"
#include "components.hpp"
#include "systems.hpp"
#include <iostream>
#include <thread>
#include <chrono>

namespace demo {

/**
 * @brief Creates a player entity with interactive components
 */
game::ecs::Entity* create_player(game::ecs::System* system) {
    auto* player = system->add_entity();
    player->add_component<Name>("Hero");
    player->add_component<Position>(40.0f, 10.0f);  // Center of world
    player->add_component<Velocity>(0.0f, 0.0f);
    player->add_component<Health>(150);
    player->add_component<Renderable>('@', "blue", true);
    
    return player;
}

/**
 * @brief Creates an enemy entity with AI behavior
 */
game::ecs::Entity* create_enemy(game::ecs::System* system, const std::string& name, 
                               float x, float y, game::ecs::EntityID target_id) {
    auto* enemy = system->add_entity();
    enemy->add_component<Name>(name);
    enemy->add_component<Position>(x, y);
    enemy->add_component<Velocity>(0.0f, 0.0f);
    enemy->add_component<Health>(80);
    enemy->add_component<Renderable>('E', "red", true);
    
    // Add AI with patrol behavior
    auto* ai = enemy->add_component<AI>(10.0f);  // 10 unit detection range
    ai->target_entity_id = target_id;
    ai->current_state = AI::State::Patrolling;
    
    // Set up patrol points
    ai->patrol_points.push_back(Position(x - 5, y));
    ai->patrol_points.push_back(Position(x + 5, y));
    ai->patrol_points.push_back(Position(x, y - 3));
    ai->patrol_points.push_back(Position(x, y + 3));
    
    return enemy;
}

/**
 * @brief Creates a wandering NPC entity
 */
game::ecs::Entity* create_npc(game::ecs::System* system, const std::string& name, float x, float y) {
    auto* npc = system->add_entity();
    npc->add_component<Name>(name);
    npc->add_component<Position>(x, y);
    npc->add_component<Velocity>(2.0f, 1.0f);  // Slow random movement
    npc->add_component<Health>(50);
    npc->add_component<Renderable>('N', "green", true);
    
    return npc;
}

/**
 * @brief Creates a temporary effect entity that auto-removes after time
 */
game::ecs::Entity* create_temporary_effect(game::ecs::System* system, float x, float y) {
    auto* effect = system->add_entity();
    effect->add_component<Name>("Magic Effect");
    effect->add_component<Position>(x, y);
    effect->add_component<Renderable>('*', "yellow", true);
    effect->add_component<Timer>(3.0f, true);  // Auto-remove after 3 seconds
    
    return effect;
}

/**
 * @brief Simulates simple player input (for demo purposes)
 */
void simulate_player_input(game::ecs::Entity* player, float delta) {
    static float input_timer = 0.0f;
    static int input_state = 0;
    
    input_timer += delta;
    
    auto* vel = player->get_component<Velocity>();
    if (!vel) return;
    
    // Simple movement pattern for demo
    if (input_timer > 2.0f) {
        input_timer = 0.0f;
        input_state = (input_state + 1) % 4;
        
        const float speed = 8.0f;
        switch (input_state) {
            case 0: vel->dx = speed; vel->dy = 0.0f; break;    // Right
            case 1: vel->dx = 0.0f; vel->dy = speed; break;    // Down
            case 2: vel->dx = -speed; vel->dy = 0.0f; break;   // Left
            case 3: vel->dx = 0.0f; vel->dy = -speed; break;   // Up
        }
    }
}

} // namespace demo

int main() {
    std::cout << "=== ECS Framework Demo ===\n";
    std::cout << "This demo showcases various ECS components and systems working together.\n";
    std::cout << "Watch as entities move, interact, and demonstrate different behaviors!\n\n";
    
    // Create world and add all systems
    game::ecs::World world;
    
    auto* movement_system = world.add_system<demo::MovementSystem>();
    auto* render_system = world.add_system<demo::RenderSystem>();
    auto* health_system = world.add_system<demo::HealthSystem>();
    auto* ai_system = world.add_system<demo::AISystem>();
    auto* timer_system = world.add_system<demo::TimerSystem>();
    
    if (!movement_system || !render_system || !health_system || !ai_system || !timer_system) {
        std::cout << "Failed to add systems to world!\n";
        return 1;
    }
    
    // Initialize world
    if (!world.initialize()) {
        std::cout << "Failed to initialize world!\n";
        return 1;
    }
    
    // Create entities across different systems
    // Note: In a real game, you might have a central entity manager,
    // but for this demo we'll distribute entities across systems
    
    // Create player in movement system
    auto* player = demo::create_player(movement_system);
    auto player_id = player->get_id();
    
    // Create enemies in AI system (they need AI processing)
    demo::create_enemy(ai_system, "Goblin", 10.0f, 5.0f, player_id);
    demo::create_enemy(ai_system, "Orc", 70.0f, 15.0f, player_id);
    
    // Create NPCs in movement system
    demo::create_npc(movement_system, "Villager", 20.0f, 8.0f);
    demo::create_npc(movement_system, "Merchant", 60.0f, 12.0f);
    
    // Create temporary effects in timer system
    demo::create_temporary_effect(timer_system, 30.0f, 5.0f);
    demo::create_temporary_effect(timer_system, 50.0f, 15.0f);
    
    std::cout << "Press Ctrl+C to exit the demo.\n";
    std::cout << "Starting simulation...\n\n";
    
    // Main game loop
    auto last_time = std::chrono::high_resolution_clock::now();
    int frame_count = 0;
    
    try {
        while (true) {
            auto current_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(current_time - last_time);
            float delta = duration.count() / 1000000.0f;  // Convert to seconds
            last_time = current_time;
            
            // Simulate player input
            demo::simulate_player_input(player, delta);
            
            // Update world (this calls tick on all systems)
            world.tick(delta);
            
            // Show additional info every few frames
            if (++frame_count % 30 == 0) {
                std::cout << "\nFrame: " << frame_count << " | Delta: " << delta << "s\n";
                
                // Show system entity counts
                std::cout << "System Entity Counts:\n";
                std::cout << "  Movement: " << movement_system->get_entities().size() << "\n";
                std::cout << "  AI: " << ai_system->get_entities().size() << "\n";
                std::cout << "  Timer: " << timer_system->get_entities().size() << "\n";
            }
            
            // Control frame rate (approximately 60 FPS)
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    } catch (const std::exception& e) {
        std::cout << "Demo terminated: " << e.what() << '\n';
    }
    
    // Cleanup (world destructor handles system shutdown)
    std::cout << "\n=== Demo completed ===\n";
    std::cout << "ECS Framework demonstration finished successfully!\n";
    
    return 0;
} 