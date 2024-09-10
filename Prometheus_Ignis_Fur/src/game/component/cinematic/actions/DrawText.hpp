#pragma once

#include "CinematicAction.hpp"

#include <util/JSONHelper.hpp>
#include "game/states/menus/components/render.hpp"

struct DrawText : CinematicAction
{
    explicit DrawText(Json::Value const &info, SGEngine *engine)
    {

        text = info["DrawText"]["Text"].asString();
        position = JSONHelper::getVector2(info["DrawText"]["Position"]);

        auto *hudEngine = engine->getHUDEngine();
        // el elemento no cambiara de posicion
        element = hudEngine->addText(text.c_str(), {0.0f, 0.0f, 0.0f}, position, 64, 0);
        assert(element);
        element->IsActive = false;
    }

    bool update([[maybe_unused]] float const &elapsedTime, [[maybe_unused]] ECS::Entity *entity, [[maybe_unused]] SGEngine *engine, float const &deltaTime) override final
    {
        element->IsActive = true;
        currentTimer += deltaTime;
        if (currentTimer >= timeShowing)
        {
            element->clean();
            return true;
        }
        return false;
    }

private:
    float currentTimer{0.0f};
    float timeShowing{3.0f};
    std::string text{};
    glm::vec2 position{};

    HUDElement *element{nullptr};
};