# ECS Framework Usage Guide

## Table of Contents
- [Introduction](#introduction)
- [Core Concepts](#core-concepts)
- [Getting Started](#getting-started)
- [API Reference](#api-reference)
- [Best Practices](#best-practices)
- [Common Patterns](#common-patterns)
- [Examples](#examples)

## Introduction

This Entity-Component-System (ECS) framework provides a flexible and performant architecture for building games and simulations. ECS promotes composition over inheritance, making your code more modular, testable, and maintainable.

### Key Benefits
- **Modularity**: Components define data, systems define behavior
- **Performance**: Data-oriented design enables efficient processing
- **Flexibility**: Easy to add/remove functionality without complex inheritance hierarchies
- **Testability**: Systems can be tested independently

## Core Concepts

### Entity
An **Entity** is simply a unique identifier that aggregates components. Entities themselves contain no data or behavior - they're just containers for components.

```cpp
// Entities are created through Systems
auto* entity = system->add_entity();
auto entity_id = entity->get_id();
```

### Component
A **Component** is pure data that defines an aspect of an entity. Components inherit from the base `Component` class and contain only data members.

```cpp
struct Position : public game::ecs::Component {
    float x, y, z;
    Position(float x = 0.0f, float y = 0.0f, float z = 0.0f) 
        : x(x), y(y), z(z) {}
};

struct Health : public game::ecs::Component {
    int current_health;
    int max_health;
    Health(int max_hp) : current_health(max_hp), max_health(max_hp) {}
};
```

### System
A **System** contains the logic that operates on entities with specific component combinations. Systems inherit from the base `System` class and implement the `tick()` method.

```cpp
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
```

### World
The **World** manages all systems and provides the main game loop coordination. It handles system lifecycle and orchestrates the tick updates.

```cpp
game::ecs::World world;
auto* movement_system = world.add_system<MovementSystem>();
world.initialize();

// Game loop
while (running) {
    world.tick(delta_time);
}
```

## Getting Started

### 1. Include Headers
```cpp
#include "ecs/component.hpp"
#include "ecs/entity.hpp" 
#include "ecs/system.hpp"
#include "ecs/world.hpp"
```

### 2. Define Components
```cpp
struct Transform : public game::ecs::Component {
    float x, y, rotation;
    Transform(float x = 0, float y = 0, float rot = 0) 
        : x(x), y(y), rotation(rot) {}
};

struct Sprite : public game::ecs::Component {
    std::string texture_path;
    int width, height;
    Sprite(const std::string& path, int w, int h) 
        : texture_path(path), width(w), height(h) {}
};
```

### 3. Create Systems
```cpp
class RenderSystem : public game::ecs::System {
public:
    void tick(const float& delta) noexcept override {
        for (auto& [id, entity] : get_entities()) {
            auto* transform = entity->get_component<Transform>();
            auto* sprite = entity->get_component<Sprite>();
            
            if (transform && sprite) {
                // Render sprite at transform position
                render_sprite(sprite->texture_path, transform->x, transform->y);
            }
        }
    }
};
```

### 4. Set Up World and Game Loop
```cpp
int main() {
    game::ecs::World world;
    
    // Add systems
    auto* render_system = world.add_system<RenderSystem>();
    auto* movement_system = world.add_system<MovementSystem>();
    
    // Initialize
    if (!world.initialize()) {
        return -1;
    }
    
    // Create entities (through any system)
    auto* player = render_system->add_entity();
    player->add_component<Transform>(100, 100, 0);
    player->add_component<Sprite>("player.png", 32, 32);
    
    // Game loop
    while (game_running) {
        float delta = calculate_delta_time();
        world.tick(delta);
    }
    
    return 0;
}
```

## API Reference

### Entity Methods

#### Component Management
```cpp
// Add a component (returns nullptr if component already exists)
template<typename T, typename... Args>
T* add_component(Args&&... args);

// Get a component (returns nullptr if not found)
template<typename T>
T* get_component();
template<typename T>
const T* get_component() const;

// Check if entity has a component
template<typename T>
bool has_component() const;

// Remove a component (returns true if removed, false if not found)
template<typename T>
bool remove_component();

// Get entity ID
EntityID get_id() const noexcept;
```

### System Methods

#### Entity Management
```cpp
// Add a new entity to this system
Entity* add_entity() noexcept;

// Get entity by ID
Entity* get_entity(EntityID id) noexcept;
const Entity* get_entity(EntityID id) const noexcept;

// Check if entity exists
bool has_entity(EntityID id) const noexcept;

// Remove entity (returns true if removed, false if not found)
bool remove_entity(EntityID id) noexcept;

// Get all entities
const SystemEntities& get_entities() const noexcept;
SystemEntities& get_entities() noexcept;
```

#### Lifecycle Methods
```cpp
// Called once when world initializes (override if needed)
virtual bool initialize() noexcept;

// Called every frame - MUST implement
virtual void tick(const float& delta) noexcept = 0;

// Called when system is removed or world shuts down (override if needed)
virtual void shutdown() noexcept;
```

### World Methods

#### System Management
```cpp
// Add a system (returns nullptr if system already exists)
template<typename T, typename... Args>
T* add_system(Args&&... args) noexcept;

// Get a system
template<typename T>
T* get_system() noexcept;
template<typename T>
const T* get_system() const noexcept;

// Check if system exists
template<typename T>
bool has_system() const noexcept;

// Remove a system (returns true if removed, false if not found)
template<typename T>
bool remove_system() noexcept;
```

#### Lifecycle Management
```cpp
// Initialize all systems
bool initialize() noexcept;

// Update all systems
void tick(const float& delta) noexcept;

// Shutdown all systems and cleanup
void shutdown() noexcept;
```

## Best Practices

### 1. Component Design
- **Keep components as pure data structures** - no methods except constructors
- **Use meaningful names** that describe what the component represents
- **Keep components small and focused** - prefer multiple small components over large ones

```cpp
// Good: Small, focused components
struct Position : public game::ecs::Component {
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Velocity : public game::ecs::Component {
    float dx, dy;
    Velocity(float dx = 0, float dy = 0) : dx(dx), dy(dy) {}
};

// Avoid: Large, unfocused components
struct Player : public game::ecs::Component {
    float x, y, dx, dy;
    int health, mana;
    std::string name;
    // ... too many responsibilities
};
```

### 2. System Design
- **One responsibility per system** - each system should handle one type of behavior
- **Process entities in batches** - iterate through all relevant entities in each tick
- **Use early returns** for entities missing required components

```cpp
class CollisionSystem : public game::ecs::System {
public:
    void tick(const float& delta) noexcept override {
        auto& entities = get_entities();
        
        for (auto& [id1, entity1] : entities) {
            auto* pos1 = entity1->get_component<Position>();
            auto* collider1 = entity1->get_component<Collider>();
            
            if (!pos1 || !collider1) continue; // Early return
            
            for (auto& [id2, entity2] : entities) {
                if (id1 >= id2) continue; // Avoid duplicate checks
                
                auto* pos2 = entity2->get_component<Position>();
                auto* collider2 = entity2->get_component<Collider>();
                
                if (!pos2 || !collider2) continue;
                
                if (check_collision(pos1, collider1, pos2, collider2)) {
                    handle_collision(entity1, entity2);
                }
            }
        }
    }
};
```

### 3. Error Handling
- **Always check return values** from add/remove operations
- **Validate component pointers** before using them
- **Handle initialization failures** gracefully

```cpp
// Good error handling
auto* health_comp = entity->add_component<Health>(100);
if (!health_comp) {
    // Component already exists or other error
    std::cerr << "Failed to add health component\n";
    return false;
}

auto* existing_health = entity->get_component<Health>();
if (existing_health) {
    existing_health->current_health -= damage;
}
```

### 4. Memory Management
- **Let the framework handle memory** - don't manually delete components or entities
- **Use RAII principles** in component constructors/destructors
- **Be careful with component owner pointers** - they're automatically managed

## Common Patterns

### 1. Component Queries
Filter entities by component combinations:

```cpp
void PhysicsSystem::tick(const float& delta) noexcept {
    for (auto& [id, entity] : get_entities()) {
        // Only process entities with both Position and RigidBody
        auto* pos = entity->get_component<Position>();
        auto* body = entity->get_component<RigidBody>();
        
        if (pos && body) {
            update_physics(pos, body, delta);
        }
    }
}
```

### 2. Component Communication
Components can reference each other through the entity:

```cpp
void HealthSystem::tick(const float& delta) noexcept {
    for (auto& [id, entity] : get_entities()) {
        auto* health = entity->get_component<Health>();
        if (!health) continue;
        
        if (health->current_health <= 0) {
            // Trigger death animation if available
            if (auto* animator = entity->get_component<Animator>()) {
                animator->play_animation("death");
            }
            
            // Schedule for removal
            entities_to_remove.push_back(id);
        }
    }
    
    // Remove dead entities
    for (auto id : entities_to_remove) {
        remove_entity(id);
    }
    entities_to_remove.clear();
}
```

### 3. System Coordination
Systems can interact through shared world state:

```cpp
class InputSystem : public game::ecs::System {
    std::vector<InputEvent> events_;
    
public:
    void tick(const float& delta) noexcept override {
        events_ = poll_input_events();
        // Process input and update components
    }
    
    const std::vector<InputEvent>& get_events() const { return events_; }
};

class GameLogicSystem : public game::ecs::System {
    World* world_;
    
public:
    GameLogicSystem(World* world) : world_(world) {}
    
    void tick(const float& delta) noexcept override {
        auto* input_system = world_->get_system<InputSystem>();
        if (input_system) {
            for (const auto& event : input_system->get_events()) {
                handle_input_event(event);
            }
        }
    }
};
```

## Examples

### Simple 2D Game Entity
```cpp
// Create a simple game character
auto* player = game_system->add_entity();
player->add_component<Position>(100.0f, 200.0f);
player->add_component<Velocity>(0.0f, 0.0f);
player->add_component<Sprite>("player.png", 32, 32);
player->add_component<Health>(100);
player->add_component<Input>(); // Tag component for input handling

// Add some pickups
auto* coin = game_system->add_entity();
coin->add_component<Position>(150.0f, 150.0f);
coin->add_component<Sprite>("coin.png", 16, 16);
coin->add_component<Collectible>(10); // 10 points
```

### Multi-System Game
```cpp
int main() {
    game::ecs::World world;
    
    // Add systems in order of execution priority
    auto* input_system = world.add_system<InputSystem>();
    auto* physics_system = world.add_system<PhysicsSystem>();
    auto* collision_system = world.add_system<CollisionSystem>();
    auto* game_logic_system = world.add_system<GameLogicSystem>(&world);
    auto* animation_system = world.add_system<AnimationSystem>();
    auto* render_system = world.add_system<RenderSystem>();
    auto* audio_system = world.add_system<AudioSystem>();
    
    if (!world.initialize()) {
        std::cerr << "Failed to initialize world\n";
        return -1;
    }
    
    // Game loop
    auto last_time = std::chrono::high_resolution_clock::now();
    while (game_running) {
        auto current_time = std::chrono::high_resolution_clock::now();
        float delta = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;
        
        world.tick(delta);
        
        // Cap framerate
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
    }
    
    return 0;
}
```

---

## Tips for Success

1. **Start simple** - Begin with basic Position/Velocity components and a MovementSystem
2. **Profile early** - ECS can be very fast, but poor system design can hurt performance
3. **Use composition** - Prefer multiple small components over inheritance hierarchies
4. **Test systems independently** - Each system should be unit testable
5. **Document component contracts** - Make clear what components systems expect

This ECS framework provides a solid foundation for building scalable, maintainable games and simulations. The separation of data (components) and logic (systems) makes your code more modular and easier to reason about.
