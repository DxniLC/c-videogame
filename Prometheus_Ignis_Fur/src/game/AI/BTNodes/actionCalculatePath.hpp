#pragma once

#include <util/BehaviourTree/node.hpp>

#include <iostream>
#include <chrono>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>
#include <game/component/AI.hpp>
#include <game/component/physics.hpp>
#include <util/pathfinding_AStar/Pathfinding.hpp>

struct BTActionCalculatePath_t : BTNode_t
{
    BTActionCalculatePath_t(){};

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    {
        if (contadorFrames == 60)
        {
            contadorFrames = 0;
            auto *ai = e.getComponent<AIComponent>();
            auto *render = e.getComponent<RenderComponent>();
            auto *sensory = e.getComponent<SensoryComponent>();
            auto *entitySensed = entityManager.getEntityByKey(sensory->entityKeySensed);
            auto *renderSensed = entitySensed->getComponent<RenderComponent>();

            auto *blackboard = entityManager.template getSingletonComponent<Blackboard>();




            int x = -447 - int(render->node->globalPosition.z);
            int y = 159 + int(render->node->globalPosition.x);

            int xSensed = -447 - int(renderSensed->node->globalPosition.z);
            int ySensed = 159 + int(renderSensed->node->globalPosition.x);



            if (xSensed > 0 && ySensed > 0 && xSensed < int(blackboard->matriz.size()) && ySensed < int(blackboard->matriz[0].size()))
            {
                glm::vec2 init{x, y};
                glm::vec2 end{xSensed, ySensed};

                
                if (blackboard->matriz[end.x][end.y] != 1)
                {
                    if (blackboard->matriz[end.x + 1][end.y] == 1)
                        end.x++;
                    if (blackboard->matriz[end.x - 1][end.y] == 1)
                        end.x--;
                    if (blackboard->matriz[end.x][end.y + 1] == 1)
                        end.y++;
                    if (blackboard->matriz[end.x][end.y - 1] == 1)
                        end.y--;
                }

                if (blackboard->matriz[init.x][init.y] != 1)
                {
                    if (blackboard->matriz[init.x + 1][init.y] == 1)
                        init.x++;
                    if (blackboard->matriz[init.x - 1][init.y] == 1)
                        init.x--;
                    if (blackboard->matriz[init.x][init.y + 1] == 1)
                        init.y++;
                    if (blackboard->matriz[init.x][init.y - 1] == 1)
                        init.y--;
                }

                if (blackboard->matriz[end.x][end.y] == 1)
                {
                    ai->pathResult.clear();
                    std::vector<glm::vec2> givenResult = CalculatePathFinding(init, end, blackboard->matriz);
                    glm::vec3 positionAux = render->node->globalPosition;
                    if (givenResult.size() >= 1)
                    {

                        glm::vec2 aux{givenResult[givenResult.size() - 1]};
                        glm::vec2 lastmove = aux - init;
                        glm::vec2 nextmove = aux - init;

                        float auxSize{1.f};

                        for (int i = int(givenResult.size()) - 2; i >= 0; i--)
                        {
                            nextmove = givenResult[i] - aux;

                            if (lastmove == nextmove)
                            {
                                aux = givenResult[i];

                                auxSize++;
                            }
                            else
                            {

                                positionAux = positionAux + glm::vec3(lastmove.y, 0.0f, -lastmove.x) * auxSize;
                                ai->pathResult.emplace_back(positionAux);


                                auxSize = 1.f;
                                aux = givenResult[i];
                            }
                            lastmove = nextmove;
                        }
                        positionAux = positionAux + glm::vec3(nextmove.y, 0.0f, -nextmove.x) * auxSize;

                        ai->pathResult.emplace_back(positionAux);
                        blackboard->someCalculatedPath = true;
                        ai->resetCount = true;
                    }
                    return BTNodeStatus_t::running;
                }
            }
            else
            {
                return BTNodeStatus_t::success;
            }
        }
        else
        {
            contadorFrames++;
        }
        return BTNodeStatus_t::running;
    }

private:
    int contadorFrames{60};
};