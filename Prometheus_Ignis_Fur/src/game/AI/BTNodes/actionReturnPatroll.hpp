#pragma once

#include <util/BehaviourTree/node.hpp>

#include <iostream>
#include <chrono>

#include <glm/vec3.hpp>

#include <game/component/render.hpp>
#include <game/component/AI.hpp>
#include <game/component/physics.hpp>
#include <util/pathfinding_AStar/Pathfinding.hpp>

struct BTactionreturnPatroll : BTNode_t
{
    BTactionreturnPatroll(){};

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final
    { // override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir

        auto *render = e.getComponent<RenderComponent>();
        auto *ai = e.getComponent<AIComponent>();
        auto *phy = e.getComponent<PhysicsComponent>();

        if (render && ai)
        {
            if (!routeCalculated)
            {

                auto *blackboard = entityManager.template getSingletonComponent<Blackboard>();
                int x = -447 - int(render->node->globalPosition.z);
                int y = 159 + int(render->node->globalPosition.x);

                int xSensed = -447 - int(ai->patrollPoints[0].z);
                int ySensed = 159 + int(ai->patrollPoints[0].x);

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
                        routeCalculated = true;
                    }
                }
            }

            if (position >= int(ai->pathResult.size()))
                position = 0;

            glm::vec3 direction = ai->pathResult[position] - render->node->globalPosition;

            direction.y = 0.f;

            auto orient = glm::normalize(direction);
            if (orient != render->node->direction && not render->node->localTransform.interpolated_need_update)
            {
                render->node->localTransform.setRotateInterpolated(render->node->direction, orient, 2.0f, glm::vec3(0, 1, 0));
            }

            phy->ownForce += orient * 50.f;

            if (glm::length(direction) < 0.2)
            {
                position++;
                if (position >= int(ai->pathResult.size()))
                {

                    return BTNodeStatus_t::fail;
                }
            }
            if (orient != render->node->direction && not render->node->localTransform.interpolated_need_update)
            {
                render->node->localTransform.setRotateInterpolated(render->node->direction, orient, 2.0f, glm::vec3(0, 1, 0));
            }
        }
        return BTNodeStatus_t::running;
    }

private:
    int position{0};
    bool routeCalculated{false};
};