#pragma once

#include "Ability.hpp"
// #include <ecs/cmp/entity.hpp>
#include <game/component/physics.hpp>
#include <game/component/meshAnimation.hpp>

struct DoubleJumpAbility : public Ability
{

    explicit DoubleJumpAbility()
    {
        abilityID = AbilityID::DOUBLE_JUMP_ABILITY;
    };

    bool useAbility(ECS::Entity &entity) override
    {

        if (!jump)
        {
            auto *physics = entity.getComponent<PhysicsComponent>();
            physics->externForces.y = 1250;
            jump = true;

            auto *audio = entity.getComponent<AudioComponent>();
            if (audio)
                audio->soundsToUpdate.emplace_back("Effects/double_jump", true, nullptr);

            auto *animation = entity.getComponent<MeshAnimationComponent>();
            animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_D_JUMP, true, 0.1f);

            return true;
        }
        return false;
    }

    void resetDoubleJump()
    {
        jump = false;
    }

private:
    bool jump{false};
};