# ECS Framework Demo

This directory contains comprehensive examples demonstrating how to use the ECS (Entity-Component-System) framework.

## Demo Files

### Core Demo Files

- **`components.hpp`** - Defines various component types showcasing different data patterns
- **`systems.hpp`** - Implements systems that process entities with specific component combinations
- **`simple_example.cpp`** - Basic example perfect for beginners learning ECS concepts

### Component Showcase

The demo includes the following components:

| Component | Purpose | Data Stored |
|-----------|---------|-------------|
| `Position` | 2D world coordinates | x, y float values |
| `Velocity` | Movement speed and direction | dx, dy velocity values |
| `Health` | Entity health management | current/max health, utility methods |
| `Renderable` | Visual representation | symbol, color, visibility flag |
| `Name` | Entity identification | string name for debugging/display |
| `AI` | Autonomous behavior | state machine, target tracking, patrol points |
| `Timer` | Time-based effects | elapsed time, duration, auto-removal flag |

### System Showcase

The demo includes the following systems:

| System | Purpose | Processes |
|--------|---------|-----------|
| `MovementSystem` | Updates entity positions | Position + Velocity components |
| `RenderSystem` | Console-based visualization | Position + Renderable components |
| `HealthSystem` | Health regeneration and death | Health components |
| `AISystem` | Autonomous entity behavior | AI + Position + Velocity components |
| `TimerSystem` | Time-based effect management | Timer components |

## Running the Examples

### Simple Example

```bash
# Build and run the simple example
cd build
cmake ..
make
./ecs_example
```

The simple example demonstrates:
- Creating a world and adding systems
- Creating entities with components
- Running simulation steps
- Component manipulation and removal

## Key ECS Concepts Demonstrated

### 1. Component Design
Components are pure data containers:
```cpp
struct Position : public game::ecs::Component {
    float x, y;
    Position(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};
```

### 2. System Logic
Systems contain the game logic:
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

### 3. Component Queries
Systems query for entities with specific component combinations:
```cpp
// Only process entities that have BOTH Position and Velocity
auto* pos = entity->get_component<Position>();
auto* vel = entity->get_component<Velocity>();
if (pos && vel) {
    // Process entity...
}
```

### 4. World Management
The World coordinates all systems:
```cpp
game::ecs::World world;
auto* movement_system = world.add_system<MovementSystem>();
world.initialize();
world.tick(delta_time);  // Updates all systems
```

## Design Patterns Demonstrated

### 1. Data-Driven Design
Components store only data, systems contain only logic.

### 2. Composition over Inheritance
Entities are composed of components rather than inheriting behavior.

### 3. Single Responsibility
Each system has a focused responsibility (movement, rendering, AI, etc.).

### 4. Decoupled Architecture
Systems don't directly depend on each other, communicating through entity state.

## Extending the Demo

To add new functionality:

1. **New Component**: Create a struct inheriting from `Component`
2. **New System**: Create a class inheriting from `System`, implement `tick()`
3. **Entity Factory**: Create helper functions to build entities with component sets
4. **Integration**: Add your system to the world and create entities that use it

## Performance Notes

- Components are stored in hash maps for O(1) lookup by type
- Systems iterate only over entities they own
- Entity creation/destruction is managed per-system
- Component queries are type-safe at compile time

This demo architecture scales well for small to medium-sized games and provides a solid foundation for understanding ECS principles. 