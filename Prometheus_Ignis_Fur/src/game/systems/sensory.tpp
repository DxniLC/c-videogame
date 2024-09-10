
#include "sensory.hpp"

#include <game/component/sensory.hpp>

template <typename GameCTX>
SensorySystem<GameCTX>::SensorySystem(EventManager &eventManager) : rules_manager{standardCollision}
{
    eventManager.EventQueueInstance.Subscribe(EventIds::LockSystem, this);
    // Rules to Detection
    // En el metodo asociado al par de entidades, debemos hacer un if para saber si ha sido vision o oido utilizando senseType.
    rules_manager.addRule(EntityType::PLAYER, EntityType::NPC, Player_Npc_Interaction);
}

template <typename GameCTX>
void SensorySystem<GameCTX>::update(GameCTX &g)
{
    if (IsLocked)
        return;

    game_manager = &g;
    // SensoryComponent Checks
    auto &vec_senses = g.template getComponents<SensoryComponent>();
    for (auto &sense : vec_senses)
    {
        sense.iSee=false;
        sense.iHear=false;
        // Check if has direct vision with entity
        CheckVisionRays(sense);
    }
    for (auto itrA = vec_senses.begin(); itrA < vec_senses.end(); itrA++)
    {

        auto *entityA = g.getEntityByKey(itrA->getEntityKey());
        auto *renderA = entityA->template getComponent<RenderComponent>();

        for (auto itrB = itrA + 1; itrB < vec_senses.end(); itrB++)
        {
            auto *entityB = g.getEntityByKey(itrB->getEntityKey());
            auto *renderB = entityB->template getComponent<RenderComponent>();
            
            // *** HEARING
            // Check if A detects B
            CheckHearingSense(*itrA, *renderA->node, *itrB, *renderB->node);
            // Check if B detects A de
            CheckHearingSense(*itrB, *renderB->node, *itrA, *renderA->node);

            // *** SIGHT
            // Check if A detects B
            CheckSightSense(*itrA, *renderA->node, *itrB, *renderB->node);
            // Check if B detects A
            CheckSightSense(*itrB, *renderB->node, *itrA, *renderA->node);
        }
    }
}

template <typename GameCTX>
void SensorySystem<GameCTX>::CheckHearingSense(SensoryComponent &sense_A, TNode &node_A, SensoryComponent &sense_B, TNode &node_B)
{
    if (sense_A.RangeHearing != 0.0f && sense_B.IsListened)
    {
        if (SGFunc::checkCollisionRange(node_A.globalPosition, sense_A.RangeHearing, node_B.globalPosition, 0))
        {
            CollisionResult result{};

            result.entity_A = game_manager->getEntityByKey(sense_A.getEntityKey());
            result.entity_B = game_manager->getEntityByKey(sense_B.getEntityKey());

            senseType = SenseType::HEARING_SENSE;

            rules_manager.applyRule(result);
        }
    }
}

template <typename GameCTX>
void SensorySystem<GameCTX>::CheckSightSense(SensoryComponent &sense_A, TNode &node_A, SensoryComponent &sense_B, TNode &node_B)
{
    // Check if A detects B
    if (sense_A.RangeSight != 0.0f && sense_B.IsVisible)
    {
        if (SGFunc::checkCollisionRange(node_A.globalPosition, sense_A.RangeSight, node_B.globalPosition, 0))
        {
            // Comprobar si esta viendo con la dirección a la que apunto
            // Direction where B is detected
            auto directionB = glm::normalize(node_B.globalPosition - node_A.globalPosition);

            float angle = Transform::getAngle(node_A.direction, directionB, glm::vec3(0, 1, 0));

            if (abs(angle) < sense_A.angleVision)
            {
                // Esta en rango de vision y lo veo, falta comprobar con raycast.
                // Lo mejor seria desde aqui enviar el raycast y que lo procese el sistema de raycast

                RayFlagsType flags{RayCollideFlags::RAY_BODY | RayCollideFlags::RAY_WORLD};

                Raycast rayToThrow{glm::vec3{0.0f}, directionB, sense_A.RangeSight, flags};
                sense_A.raysVision.emplace_back(std::make_pair(sense_B.getEntityKey(), rayToThrow));
            }
        }
    }
}

template <typename GameCTX>
void SensorySystem<GameCTX>::CheckVisionRays(SensoryComponent &senseCMP)
{
    if (not senseCMP.raysVision.empty())
    {
        for (auto const &pair : senseCMP.raysVision)
        {

            auto &ray = pair.second;
            if (ray.result.has_intersection)
            {
                CollisionResult result{};

                result.entity_A = ray.result.entity_A;
                result.entity_B = ray.result.entity_B;
                result.depth = ray.result.distance;
                senseType = SenseType::SIGHT_SENSE;

                rules_manager.applyRule(result);
            }
        }

        senseCMP.raysVision.clear();
    }
}

template <typename GameCTX>
void SensorySystem<GameCTX>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::LockSystem)
    {
        LockSystemData *data{dynamic_cast<LockSystemData *>(eventData)};
        if (data)
            IsLocked = data->status;
        else
            std::cout << "\n***** ERROR LockSystem Sensory *****\n";
    }
}

template <typename GameCTX>
void SensorySystem<GameCTX>::Player_Npc_Interaction(CollisionResult &pts)
{

    auto *sensory = pts.entity_A->getComponent<SensoryComponent>();
    sensory->entityKeySensed = pts.entity_B->getEntityKey();

    if (senseType == HEARING_SENSE)
    {
        sensory->iHear = true;
    }
    else if (senseType == SIGHT_SENSE)
    {
        sensory->iSee = true;
    }
}
