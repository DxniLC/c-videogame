#pragma once

#include "Ability.hpp"
#include <chrono>
#include <game/component/physics.hpp>
#include <game/component/meshAnimation.hpp>

#include <game/component/audio/audio.hpp>

struct DashAbility : public Ability
{

    explicit DashAbility(){
        abilityID = AbilityID::DASH_ABILITY;
    };

    bool useAbility(ECS::Entity &entity) override
    {
        auto *physics = entity.getComponent<PhysicsComponent>();
        float totalForce = std::abs(physics->ownForce.x) + std::abs(physics->ownForce.z);

        if (checkColdDown() && totalForce != 0)
        {
            auto *animation = entity.getComponent<MeshAnimationComponent>();

            physics->ownForce.x += (physics->ownForce.x / totalForce) * ForceValue;
            physics->ownForce.z += (physics->ownForce.z / totalForce) * ForceValue;

            auto *audio = entity.getComponent<AudioComponent>();
            if (audio)
                audio->soundsToUpdate.emplace_back("Effects/dash", true, nullptr);

            if (animation)
                animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_DASH, true, 0.01f, 0.5f);

            initColdDown();
            return true;
        }
        return false;
    }

    bool checkColdDown()
    {
        std::chrono::duration<float> mDeltaTime = std::chrono::system_clock::now() - mStartTime;

        if (mDeltaTime.count() >= coldTime)
        {
            return true;
        }
        return false;
    }

    void initColdDown()
    {
        mStartTime = std::chrono::system_clock::now();
    }

private:
    float coldTime{2.0f};
    std::chrono::system_clock::time_point mStartTime{};
    const float ForceValue{4000.0f};
};