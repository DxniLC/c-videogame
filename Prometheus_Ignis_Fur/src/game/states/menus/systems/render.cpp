
#include "render.hpp"

#include <game/systems/render/debug/debug.hpp>

template <typename Engine>
RenderMenuSystem<Engine>::RenderMenuSystem(Engine *engine, DebugSystem *debug)
{
    this->engine = engine;
    this->debug = debug;
}

template <typename Engine>
void RenderMenuSystem<Engine>::update(ECS::EntityManager &entityManager)
{
    engine->beginScene();
    engine->drawAll();
    if (debug)
        debug->update(entityManager);
    engine->endScene();
}