#pragma once

#include <ecs/component/component.hpp>
#include "abilities/Ability.hpp"
#include <memory>
// Abilities
#include "abilities/Dash.hpp"
#include "abilities/DoubleJump.hpp"

struct AbilityComponent : public ECS::ComponentBase<AbilityComponent>
{
    explicit AbilityComponent(EntityKey e_id) : ECS::ComponentBase<AbilityComponent>(e_id){};

    Ability *addAbility(std::unique_ptr<Ability> ability)
    {
        abilitySize++;
        return abilities.emplace_back(std::move(ability)).get();
    }

    template <typename AbilityType>
    AbilityType *getAbility()
    {
        AbilityType *result{nullptr};
        for (auto &ability : abilities)
        {
            result = dynamic_cast<AbilityType *>(ability.get());
            if (result)
            {
                return result;
            }
        }
        return result;
    }

    std::size_t abilitySize{0};

private:
    std::vector<std::unique_ptr<Ability>> abilities{};
};
