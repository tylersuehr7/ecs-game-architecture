# ECS Architecture V1

A lightweight, modern C++ Entity-Component-System (ECS) framework designed for game development. This implementation prioritizes simplicity and ease of use while maintaining good performance, making it ideal for arcade games, indie projects, and educational purposes.

## 🚀 Features

- **🎯 Simple & Clean API** - Easy to learn and use with minimal boilerplate
- **🔒 Type Safety** - Template-based design with compile-time type checking
- **💾 Memory Safe** - Automatic memory management with RAII principles
- **⚡ Performance Focused** - Data-oriented design for efficient processing
- **🛡️ Robust Error Handling** - Clear success/failure indicators for all operations
- **📚 Well Documented** - Comprehensive usage guide and examples
- **🧪 Modern C++** - Uses C++17 features for clean, readable code

## 🏗️ Architecture Overview

The framework consists of four core components:

- **📦 Components** - Pure data structures that define entity properties
- **🎮 Entities** - Lightweight containers that aggregate components
- **⚙️ Systems** - Logic processors that operate on entities with specific components
- **🌍 World** - Manager that coordinates all systems and handles the game loop

## 🔧 Quick Start

### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10 or higher

### Building
```bash
git clone <your-repo-url>
cd ecs-architecture
mkdir build && cd build
cmake ..
make
```

### Basic Example
```cpp
#include "ecs/world.hpp"

// Define components
struct Position : public game::ecs::Component {
    float x, y;
    Position(float x = 0, float y = 0) : x(x), y(y) {}
};

struct Velocity : public game::ecs::Component {
    float dx, dy;
    Velocity(float dx = 0, float dy = 0) : dx(dx), dy(dy) {}
};

// Create a system
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
    // Set up world
    game::ecs::World world;
    auto* movement_system = world.add_system<MovementSystem>();
    world.initialize();
    
    // Create entity
    auto* entity = movement_system->add_entity();
    entity->add_component<Position>(0.0f, 0.0f);
    entity->add_component<Velocity>(1.0f, 0.5f);
    
    // Game loop
    world.tick(1.0f); // Move entity by velocity * delta
    return 0;
}
```

## 📖 Documentation

- **[Complete Usage Guide](ECS_USAGE_GUIDE.md)** - Comprehensive documentation with examples
- **[API Reference](ECS_USAGE_GUIDE.md#api-reference)** - Detailed method documentation
- **[Best Practices](ECS_USAGE_GUIDE.md#best-practices)** - Tips for effective ECS design

## 🎯 Use Cases

This ECS framework is perfect for:

- **🕹️ Arcade Games** - Fast-paced games requiring good performance
- **🎨 Indie Projects** - Small to medium games needing flexible architecture  
- **📚 Learning Projects** - Educational implementation of ECS patterns
- **🧪 Prototyping** - Rapid game development and experimentation
- **🎮 2D Games** - Particularly well-suited for 2D game development

## 📁 Project Structure

```
ecs-architecture1/
├── src/
│   ├── ecs/
│   │   ├── component.hpp    # Base component interface
│   │   ├── entity.hpp       # Entity management
│   │   ├── system.hpp       # System base class
│   │   └── world.hpp        # World coordinator
│   └── main.cpp            # Example usage
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
└── ECS_USAGE_GUIDE.md     # Comprehensive usage guide
```

## 🔨 Building Your Game

1. **Define Components** - Create data structures for your game objects
2. **Implement Systems** - Write logic that processes entities with specific components
3. **Set Up World** - Add your systems to the world manager
4. **Create Entities** - Spawn game objects and attach components
5. **Run Game Loop** - Call `world.tick(delta)` each frame

## 🎮 Example Games

The framework is suitable for building:
- Platformers with Position, Velocity, and Collision components
- Puzzle games with Grid, Movable, and Rule components  
- Shooters with Transform, Weapon, and Health components
- RPGs with Stats, Inventory, and Quest components

## 🧪 Testing

Run the example program to verify your build:
```bash
cd build
./ecs
```

Expected output:
```
Testing ECS Architecture
Initial position: (10, 20)
Position after tick: (11, 22)
ECS Architecture test completed successfully!
```

## 🤝 Contributing

This is a personal learning project, but feedback and suggestions are welcome! Please feel free to:
- Report bugs or issues
- Suggest improvements
- Share your experience using the framework

## 🌟 Acknowledgments

Inspired by modern ECS implementations and game development best practices. Built with simplicity and learning in mind.

## License

> Copyright © 2025 Tyler R. Suehr

This project is provided as-is for educational and personal use.
