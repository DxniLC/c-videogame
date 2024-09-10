#pragma once

#include "AnimationBase.hpp"

#include <game/component/animation.hpp>

#include <ecs/manager/entitymanager.hpp>

#include <game/CustomFunctions.hpp>

struct TranslateAnimation : public AnimationBase
{
    explicit TranslateAnimation(TNode *node, EntityKey key, glm::vec3 direction, float velocity, float maxRange, bool loop = false)
    {
        this->node = node;
        this->key = key;
        this->direction = direction;
        this->steps = abs(velocity);
        this->maxRange = maxRange;
        finalSteps = maxRange;
        this->loop = loop;
    }

    bool update(ECS::EntityManager &g, float dTime) override
    {
        auto *entity = g.getEntityByKey(this->key);

        if (not entity)
        {
            return true;
        }
        auto *physics = entity->getComponent<PhysicsComponent>();

        if (currentSteps >= finalSteps)
        {

            if (loop)
            {
                currentSteps = 0;
                direction = -direction;
            }
            else
            {

                auto *anim = entity->template getComponent<AnimationComponent>();
                anim->animating = false;

                if (physics)
                    physics->ownForce = glm::vec3(0.0f);
                return true;
            }
        }

        auto velocity = steps * dTime * dTime;
        currentSteps += velocity;

        if (physics)
        {


            physics->ownForce = direction * steps;
        }
        else
        {


            TNode &nodoMain = SGFunc::getMainNodeFromEntity(*entity);
            auto position = nodoMain.localTransform.position;
           
            position += (direction * velocity);
            this->node->localTransform.setPosition(position);
        }

        return false;
    }

    void revert(bool directionAnimation) override
    {
        if (currentSteps != 0)
        {
            if (directionAnimation)
            {
                direction = -direction;
                currentSteps = finalSteps - currentSteps;
                finalSteps = maxRange;
            }
            else
            {
                direction = -direction;
                finalSteps = currentSteps;
                currentSteps = 0;
            }
        }
    }

    glm::vec3 direction{};
    float steps{};
    float currentSteps{0};
    float finalSteps{};
    bool loop{};
    float maxRange{};
};