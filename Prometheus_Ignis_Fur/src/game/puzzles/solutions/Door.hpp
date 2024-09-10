#pragma once

#include "SolutionPuzzle.hpp"

#include <game/animations/TranslateAnimation.hpp>

struct DoorSolution : public SolutionPuzzle
{
    explicit DoorSolution(TNode *nodeDoor, EntityKey key, glm::vec3 direction, float distance)
        : nodeDoor{nodeDoor}, key{key}, direction{direction}, distance{distance}
    {
    }

    void executeSolution(ECS::EntityManager &entityManager) override final
    {

        if (not isOpen)
        {

            auto *entity = entityManager.getEntityByKey(key);
            if (entity)
            {
                auto *anim = entity->template getComponent<AnimationComponent>();
                auto *audio = entity->template getComponent<AudioComponent>();

                if (audio)
                {

                    if (not OpenedFirstTime)
                    {
                        audio->soundsToUpdate.emplace_back("Music/finish_puzle", true, nullptr);
                        OpenedFirstTime = true;
                    }

                    audio->soundsToUpdate.emplace_back("Effects/big_door_close", false, nullptr);
                    audio->soundsToUpdate.emplace_back("Effects/big_door_open", true, nullptr);
                }

                if (anim)
                {
                    auto *animationManager = entityManager.getSingletonComponent<AnimationManager>();
                    if (animationManager)
                    {
                        auto animationStatus = animationManager->addAnimation(*anim, 0, TranslateAnimation{nodeDoor, key, direction, 150.f, distance});
                        anim->animating = true;
                        if (animationStatus.second == false)
                        {
                            (animationStatus.first)->second->revert(true);
                        }
                    }
                }
            }
            isOpen = true;
        }
    }

    void revertSolution(ECS::EntityManager &entityManager) override final
    {

        if (isOpen)
        {
            auto *entity = entityManager.getEntityByKey(key);
            if (entity)
            {
                auto *anim = entity->template getComponent<AnimationComponent>();
                auto *audio = entity->template getComponent<AudioComponent>();

                if (audio)
                {
                    audio->soundsToUpdate.emplace_back("Effects/big_door_open", false, nullptr);
                    audio->soundsToUpdate.emplace_back("Effects/big_door_close", true, nullptr);

                    if (anim)
                    {
                        auto *animationManager = entityManager.getSingletonComponent<AnimationManager>();
                        if (animationManager)
                        {
                            anim->animating = true;
                            auto animationStatus = animationManager->addAnimation(*anim, 0, TranslateAnimation{nodeDoor, key, -direction, 150.f, distance});
                            if (animationStatus.second == false)
                            {

                                (animationStatus.first)->second->revert(true);
                            }
                        }
                    }
                }
            }

            isOpen = false;
        }
    }

private:
    TNode *nodeDoor;
    EntityKey key;
    glm::vec3 direction{};
    float distance;

    bool isOpen{false};
    bool OpenedFirstTime{false};
};