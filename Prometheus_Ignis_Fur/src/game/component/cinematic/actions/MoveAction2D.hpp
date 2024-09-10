#pragma once

#include "CinematicAction.hpp"

#include <util/JSONHelper.hpp>

struct MoveAction2D : CinematicAction
{
    explicit MoveAction2D(Json::Value const &info)
    {
        positionTarget = JSONHelper::getVector2(info["Move2D"]["positionMove"]);
        force = info["Move2D"]["force"].asFloat();
    }

    bool update([[maybe_unused]] float const &elapsedTime, ECS::Entity *entity, [[maybe_unused]] SGEngine *engine, [[maybe_unused]] float const &deltaTime) override final
    {

        auto *animation = entity->template getComponent<MeshAnimationComponent>();
        animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_MOVE);
        auto *render = entity->template getComponent<RenderComponent>();
        auto *physics = entity->template getComponent<PhysicsComponent>();
        auto *audio = entity->template getComponent<AudioComponent>();
        glm::vec2 positionXZ{render->node->globalPosition.x, render->node->globalPosition.z};

        glm::vec2 leftDistance{positionTarget - positionXZ};

        glm::vec2 directionXZ{glm::normalize(leftDistance)};

        glm::vec3 direction{directionXZ.x, 0, directionXZ.y};

        physics->ownForce = force * direction;

        float currentDistance = glm::length(leftDistance);

        // seteo la direccion de donde mira el player

        // std::cout << currentDistance << " : " << leftDistance.x << " : " << leftDistance.y << " distancia que queda \n";
        //const float margin{1.0f};
        const float margin{0.1f};

        float angle = glm::degrees(glm::angle(render->node->direction, direction));
        if (std::round(angle) > 1)
            render->node->localTransform.setRotateInterpolated(render->node->direction, direction, 0.5, glm::vec3(0, 1, 0));

        if (currentDistance <= margin)
        {
            render->node->setGlobalPosition(glm::vec3(positionTarget.x, render->node->globalPosition.y, positionTarget.y));
            physics->force = glm::vec3{0.0f};
            physics->ownForce = glm::vec3{0.0f};
            animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_IDLE);
            audio->soundsToUpdate.emplace_back("Effects/steps", false, nullptr);


            return true;
        }
        return false;
    }

private:
    glm::vec2 positionTarget{};
    float force{0};
};