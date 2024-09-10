#pragma once

#include <game/manager/event/EventTrigger.hpp>
#include <game/component/physics.hpp>
#include <game/manager/event/EventManager.hpp>
#include <game/events/eventsData/AbilityData.hpp>

struct ChangeGroundEvent : public EventTrigger
{
    explicit ChangeGroundEvent(ECS::Entity &A, bool ground) : ent_A(A), isGround(ground) {}

    void handle([[maybe_unused]] ECS::EntityManager &entityMan) override
    {
        auto *physics = ent_A.getComponent<PhysicsComponent>();

        if (physics)
        {
            if (!physics->isGrounded && isGround)
            {
                auto *abilities = ent_A.getComponent<AbilityComponent>();
                if (abilities)
                {
                    auto *doublejump = abilities->getAbility<DoubleJumpAbility>();
                    if (doublejump)
                    {
                        doublejump->resetDoubleJump();
                        auto *eventManager = entityMan.template getSingletonComponent<EventManager>();
                        if (eventManager)
                            eventManager->EventQueueInstance.EnqueueLateEvent(AbilityData{doublejump->abilityID});
                    }
                }
                physics->isGrounded = isGround;
            }
            if (physics->waiting4ChangeGrounded)
            {
                auto thisTime(std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count());
                auto timeSinceChanging = thisTime - physics->lastGrounded;
                if (timeSinceChanging > CoyoteTime)
                {
                    physics->isGrounded = isGround;
                    physics->waiting4ChangeGrounded = false;
                }
            }
            else if (physics->isGrounded && !isGround)
            {
                physics->lastGrounded = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
                physics->waiting4ChangeGrounded = true;
            }
        }
    }
    ECS::Entity ent_A;
    bool isGround{false};
    const float CoyoteTime{0.2f};
};