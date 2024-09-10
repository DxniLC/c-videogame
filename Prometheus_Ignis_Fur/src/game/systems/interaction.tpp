#include "interaction.hpp"

#include <game/manager/rules/Rules.hpp>
#include <ecs/component/EntityTypes.hpp>

#include <game/component/frontbox.hpp>
#include <game/component/audio/audio.hpp>
#include <game/component/input/input.hpp>
#include <game/events/eventsData/PowerUpData.hpp>

// #include <game/component/BoxControl.hpp>

template <typename GameCTX>
InteractionSystem<GameCTX>::InteractionSystem(EventManager &eventManager) : hoverRules{standardCollision}, interactionRules{standardCollision}
{
    eventManager.EventQueueInstance.Subscribe(EventIds::LockSystem, this);

    // Interaction Rules
    interactionRules.addRule(EntityType::PLAYER, EntityType::BOX, Player_Box);
    interactionRules.addRule(EntityType::PLAYER, EntityType::CLIMBING_WALL, Player_Climb);
    interactionRules.addRule(EntityType::PLAYER, EntityType::CHEST, Player_Chest);

    // Hover Rules
    hoverRules.addRule(EntityType::PLAYER, (EntityType::BOX | EntityType::CHEST), ShowControl);
}

template <typename GameCTX>
void InteractionSystem<GameCTX>::update(GameCTX &g)
{
    if (IsLocked)
        return;

    eventManager = g.template getSingletonComponent<EventManager>();
    for (auto &fbox : g.template getComponents<FrontBoxComponent>())
    {
        auto e_fbox = g.getEntityByKey(fbox.getEntityKey());
        auto *input = e_fbox->template getComponent<InputComponent>();
        if (fbox.result.entity_B)
        {
            if (input)
            {
                // Player
                if (input->Interact && fbox.result.normal.y > -0.01 && fbox.result.normal.y < 0.01)
                {
                    interactionRules.applyRule(fbox.result);
                    if (fbox.currentBoard)
                    {
                        fbox.currentBoard->clear();
                        fbox.currentBoard = nullptr;
                    }
                }
                else
                {
                    if (not input->IsInteracting)
                        hoverRules.applyRule(fbox.result);
                }
            }
            else
            {
                // IA????
            }
        }
        else
        {
            if (fbox.currentBoard && not input->IsInteracting)
            {
                fbox.currentBoard->clear();
                fbox.currentBoard = nullptr;
            }
        }
    }
}

template <typename GameCTX>
void InteractionSystem<GameCTX>::standardCollision([[maybe_unused]] CollisionResult &pts) {}

// TODO: Mejorar este metodo
template <typename GameCTX>
void InteractionSystem<GameCTX>::Player_Box(CollisionResult &pts)
{

    // PLAYER
    auto *body = pts.entity_A->template getComponent<BodyComponent>();

    // BOX
    auto *renderB = pts.entity_B->getComponent<RenderComponent>();
    auto *body_e_interacted = pts.entity_B->template getComponent<BodyComponent>();
    auto *meshBox = body_e_interacted->collider->nodeBox->template getEntity<TBoundingBox>();

    auto *input = pts.entity_A->template getComponent<InputComponent>();
    input->IsInteracting = true;

    auto *audio = pts.entity_A->template getComponent<AudioComponent>();
    if (audio)
        audio->soundsToUpdate.emplace_back("Effects/steps", false, nullptr);

    auto *render = pts.entity_A->getComponent<RenderComponent>();

    auto direction = pts.normal;

    auto boxPlayerDirection = render->node->globalPosition - renderB->node->globalPosition;
    float sizeFaceCenter{0};

    // TODO: Las cajas no rotan
    for (auto const &face : meshBox->faces)
    {

        float angle{46.f};

        if (face.normal.y < -0.01 || 0.01f < face.normal.y)
            continue;

        float den = glm::length(boxPlayerDirection * glm::vec3(1, 0, 1)) * glm::length(face.normal);
        if (den != 0.f)
            angle = std::acos(glm::dot(boxPlayerDirection * glm::vec3(1, 0, 1), face.normal) / den) * float(180.f / M_PI);

        if (angle <= 45.f)
        {
            sizeFaceCenter = glm::length(face.center - renderB->node->globalPosition);
            direction = face.normal;
        }
    }

    auto finalPosition = renderB->node->globalPosition + direction * (sizeFaceCenter * 2);
    auto direction2Move = (finalPosition - render->node->globalPosition) * glm::vec3(1, 0, 1);

    render->node->localTransform.setRotateInterpolated(render->node->direction, -direction, 0.5f, glm::vec3(0, 1, 0));
    // Al colisionar con algo, no rota apropiadamente
    body->canCollide = false;

    auto *physics = pts.entity_A->getComponent<PhysicsComponent>();

    // No physics while animation is true
    if (physics)
        physics->isStatic = true;

    render->node->localTransform.setPositionInterpolated(direction2Move, glm::length(direction2Move), 0.01f);

    input->controlType = std::make_unique<BoxControl>(render->node, pts.entity_B->getEntityKey(), renderB->node, direction);
}

template <typename GameCTX>
void InteractionSystem<GameCTX>::Player_Climb(CollisionResult &pts)
{
    // PLAYER
    auto *body = pts.entity_A->template getComponent<BodyComponent>();

    // BOX
    auto *body_e_interacted = pts.entity_B->template getComponent<BodyComponent>();
    auto *meshBox = body_e_interacted->collider->nodeBox->template getEntity<TBoundingBox>();

    auto *input = pts.entity_A->template getComponent<InputComponent>();
    input->IsInteracting = true;

    auto *audio = pts.entity_A->template getComponent<AudioComponent>();
    if (audio)
        audio->soundsToUpdate.emplace_back("Effects/steps", false, nullptr);

    auto *render = pts.entity_A->getComponent<RenderComponent>();

    auto direction = pts.normal;

    auto boxPlayerDirection = render->node->globalPosition - body_e_interacted->collider->nodeBox->globalPosition;
    // float sizeFaceCenter{0};

    // TODO: Las cajas no rotan
    for (auto const &face : meshBox->faces)
    {

        float angle{46.f};

        if (face.normal.y < -0.01 || 0.01f < face.normal.y)
            continue;

        float den = glm::length(boxPlayerDirection * glm::vec3(1, 0, 1)) * glm::length(face.normal);
        if (den != 0.f)
            angle = std::acos(glm::dot(boxPlayerDirection * glm::vec3(1, 0, 1), face.normal) / den) * float(180.f / M_PI);

        if (angle <= 45.f)
        {
            // sizeFaceCenter = glm::length(face.center - renderB->node->globalPosition);
            direction = face.normal;
        }
    }

    render->node->localTransform.setRotateInterpolated(render->node->direction, -direction, 0.5f, glm::vec3(0, 1, 0));
    // Al colisionar con algo, no rota apropiadamente
    body->canCollide = false;

    auto *physics = pts.entity_A->getComponent<PhysicsComponent>();

    // No physics while animation is true
    if (physics)
    {
        physics->gravityBool = false;
        physics->externForces = glm::vec3{0.0f};
        physics->force = glm::vec3{0.0f};
        physics->ownForce = glm::vec3{0.0f};
        physics->isStatic = true;
    }

    glm::vec2 minLimits;
    glm::vec2 maxLimits;

    std::cout << "\nSIZE BOX: " << meshBox->size.x << ", " << meshBox->size.y << ", " << meshBox->size.z;

    if (abs(direction.x) > abs(direction.z))
    {
        std::cout << "\nFaceDirection = X";
        minLimits = {
            body_e_interacted->collider->nodeBox->globalPosition.z - meshBox->size.z / 2.0f,
            body_e_interacted->collider->nodeBox->globalPosition.y - meshBox->size.y / 2.0f};

        maxLimits = {
            body_e_interacted->collider->nodeBox->globalPosition.z + meshBox->size.z / 2.0f,
            body_e_interacted->collider->nodeBox->globalPosition.y + meshBox->size.y / 2.0f};
    }
    else
    {
        std::cout << "\nFaceDirection = Z";
        minLimits = {
            body_e_interacted->collider->nodeBox->globalPosition.x - meshBox->size.x / 2.0f,
            body_e_interacted->collider->nodeBox->globalPosition.y - meshBox->size.y / 2.0f};

        maxLimits = {
            body_e_interacted->collider->nodeBox->globalPosition.x + meshBox->size.x / 2.0f,
            body_e_interacted->collider->nodeBox->globalPosition.y + meshBox->size.y / 2.0f};
    }

    std::cout << "\nMIN: " << minLimits.x << ", " << minLimits.y;
    std::cout << "\nMAX: " << maxLimits.x << ", " << maxLimits.y;
    std::cout << "\nPOSITION PLAYER: " << render->node->globalPosition.x << ", " << render->node->globalPosition.y << ", " << render->node->globalPosition.z << "\n";

    input->controlType = std::make_unique<ClimbControl>(render->node, direction, minLimits, maxLimits);
}

template <typename GameCTX>
void InteractionSystem<GameCTX>::ShowControl(CollisionResult &pts)
{
    glm::vec3 position{0, 0, 0};
    auto *render = pts.entity_B->template getComponent<RenderComponent>();
    if (render)
    {
        position = render->node->globalPosition;
        position.y += render->node->dimensionNode.y * 1.25f;
    }

    auto *frontBox = pts.entity_A->template getComponent<FrontBoxComponent>();

    if (not frontBox->currentBoard)
    {
        glm::vec3 axis{0.0f};
        glm::vec2 size{1.0f, 1.0f};
        eventManager->EventQueueInstance.EnqueueLateEvent(ShowBoardData{"media/textures/Billboard/letraF.png", position, size, axis, pts.entity_A->getEntityKey()});
    }
}

template <typename GameCTX>
void InteractionSystem<GameCTX>::Player_Chest(CollisionResult &pts)
{

    auto *object = pts.entity_B->template getComponent<ObjectComponent>();

    if (object && object->type != ObjectType::NONE_OBJECT)
    {
        auto *powerUp = pts.entity_A->template getComponent<PowerUpComponent>();
        if (powerUp && powerUp->powerUps.size() < 3)
        {
            PowerUpId idPower{PowerUpId::MORESPEED};
            if (object->type == ObjectType::POWER_JUMP)
            {
                powerUp->powerUps.emplace_back(std::make_unique<MoreJump>());
                idPower = PowerUpId::MOREJUMP;
            }
            eventManager->EventQueueInstance.EnqueueLateEvent(PowerUpData{idPower});
            object->type = ObjectType::NONE_OBJECT;

            auto *render = pts.entity_B->template getComponent<RenderComponent>();
            if (render)
            {
                auto *child = render->node->getChilds()[0].get();
                child->IsActived = true;
                render->node->IsActived = false;
            }
        }

        auto *audio = pts.entity_B->template getComponent<AudioComponent>();
        if (audio)
            audio->soundsToUpdate.emplace_back("Effects/open_chest", true, nullptr);
    }
}

template <typename GameCTX>
void InteractionSystem<GameCTX>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::LockSystem)
    {
        LockSystemData *data{dynamic_cast<LockSystemData *>(eventData)};
        if (data)
            IsLocked = data->status;
        else
            std::cout << "\n***** ERROR LockSystem Interaction *****\n";
    }
}
