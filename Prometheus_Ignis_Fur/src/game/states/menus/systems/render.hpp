
#pragma once

struct DebugSystem;
namespace ECS
{
    struct EntityManager;
};

template <typename Engine>
struct RenderMenuSystem
{
    explicit RenderMenuSystem(Engine *engine, DebugSystem *debug);
    void update(ECS::EntityManager &entityManager);

private:
    int m_width{1280}, m_height{720};
    Engine *engine{nullptr};
    DebugSystem *debug{nullptr};
};
