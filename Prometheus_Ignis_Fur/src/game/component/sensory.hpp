#pragma once

#include <ecs/component/component.hpp>

#include <game/component/raycast/RaycastBase.hpp>

struct SensoryComponent : public ECS::ComponentBase<SensoryComponent>
{
    explicit SensoryComponent(EntityKey e_key) : ECS::ComponentBase<SensoryComponent>(e_key){};

    // Caracteristica de si te pueden ver o te escuchan
    bool IsVisible{true};
    bool IsListened{false};
    
    bool iSee{false};
    bool iHear{false};
    EntityKey entityKeySensed;

    float RangeSight{8.0f};
    float RangeHearing{5.0f};

    float angleVision{60.0f};

    std::vector<std::pair<EntityKey, Raycast>> raysVision{};
};