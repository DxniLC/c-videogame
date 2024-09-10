#pragma once

#include <game/manager/puzzle/Puzzle.hpp>
#include <game/component/puzzle.hpp>

#include <vector>
#include <memory>

#include <game/manager/animation/AnimationManager.hpp>
#include <ecs/manager/entitymanager.hpp>
#include <game/animations/TranslateAnimation.hpp>

#include "solutions/createEntity.hpp"
#include "solutions/createPlatform.hpp"
#include "solutions/Door.hpp"

struct PlatePuzzle : public Puzzle
{
    explicit PlatePuzzle()
    {
        this->puzzleID = this->nextPuzzleID++;
    }

    void prehandle(ECS::EntityManager &g) final
    {
        for (auto &plate : plates)
        {
            auto *entity = g.getEntityByKey(plate.first);
            auto *puzzle_cmp = entity->template getComponent<PuzzleComponent>();
            puzzle_cmp->actived = false;
        }
    }

    void handle(ECS::EntityManager &g) final
    {
        // TODO: Cuando hay dos placas juntas y te pones una al lado de la otra se bugea, podemos probar con un timer o algo
        for (auto &plate : plates)
        {
            auto *entity = g.getEntityByKey(plate.first);
            auto *puzzle_cmp = entity->template getComponent<PuzzleComponent>();
            auto *anim_cmp = entity->template getComponent<AnimationComponent>();
            auto *audio = entity->template getComponent<AudioComponent>();
            if (puzzle_cmp->actived != plate.second && !anim_cmp->animating)
            {
                glm::vec3 direction{0, 1, 0};
                float velocity = 75.f;
                float range = 0.3f;
                if (puzzle_cmp->actived)
                {
                    platesActived++;
                    direction = -direction;

                    if (audio)
                        audio->soundsToUpdate.emplace_back("Effects/plate_down", true, nullptr);
                }
                else
                {
                    if (audio)
                        audio->soundsToUpdate.emplace_back("Effects/plate_up", true, nullptr);

                    platesActived--;
                }
                auto *render = entity->template getComponent<RenderComponent>();
                // ThrowAnimation
                AnimationManager *animationManager = g.getSingletonComponent<AnimationManager>();
                if (animationManager)
                {
                    auto animation = animationManager->addAnimation(*anim_cmp, 0, TranslateAnimation{render->node, entity->getEntityKey(), direction, velocity, range});

                    if (!puzzle_cmp->actived)
                    {
                        (animation.first)->second->revert(true);
                    }
                    else
                    {
                        (animation.first)->second->revert(false);
                    }
                }

                anim_cmp->animating = true;
                plate.second = puzzle_cmp->actived;

                if (platesActived == plates.size())
                {
                    solution->executeSolution(g);
                }
                else
                    solution->revertSolution(g);
            }
        }
    }

    void addPlate(ECS::Entity &entity)
    {
        auto *pzl_cmp = entity.getComponent<PuzzleComponent>();
        pzl_cmp->puzzleID = this->puzzleID;
        plates.emplace_back(std::make_pair(entity.getEntityKey(), false));
    }

    template <typename SolutionPuzzle>
    void addSolution(SolutionPuzzle &solution)
    {
        this->solution = std::make_unique<SolutionPuzzle>(std::move(solution));
    }

private:
    std::vector<std::pair<EntityKey, bool>> plates{};
    std::unique_ptr<SolutionPuzzle> solution{nullptr};
    unsigned int platesActived{0};
};