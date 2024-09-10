
#include "render.hpp"

#include <game/component/render.hpp>

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>
#include <engines/graphic/SGEngine/scene/entity/TBoundingBox.hpp>

#include "debug/debug.hpp"
#include "HUD/HUD.hpp"

// Event Data
#include <game/events/eventsData/StealthData.hpp>

template <typename GameCTX, typename Engine>
RenderSystem<GameCTX, Engine>::RenderSystem(Engine *engine, HUDSystem &hud, DebugSystem &debug, EventManager &eventManager)
{
    this->engine = engine;
    this->hud = &hud;
    this->debug = &debug;

    eventManager.EventQueueInstance.Subscribe(EventIds::StealthMode, this);
}

template <typename GameCTX, typename Engine>
void RenderSystem<GameCTX, Engine>::update(GameCTX &g)
{
    engine->beginScene();
    engine->drawAll();

    hud->draw(g);
    debug->update(g);

    engine->endScene();
}

template <typename GameCTX, typename Engine>
void RenderSystem<GameCTX, Engine>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::StealthMode)
    {
        StealthData *data{dynamic_cast<StealthData *>(eventData)};
        if (data)
        {
            if (data->active)
            {
                engine->enableSGOptions(SG_CINEMATIC_BARS);
            }
            else
            {
                engine->disableSGOptions(SG_CINEMATIC_BARS);
            }
        }
        else
            std::cout << "\n***** ERROR EventStealthData Render *****\n";
    }
}

template <typename GameCTX, typename Engine>
bool RenderSystem<GameCTX, Engine>::StealthMode(bool active)
{

    return false;
}

