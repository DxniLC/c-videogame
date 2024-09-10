
#include "ClimbControl.hpp"

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

#include <ecs/manager/entitymanager.hpp>
#include <game/manager/event/EventManager.hpp>

#include <game/component/physics.hpp>
#include <game/component/camera/camera.hpp>
#include <game/component/frontbox.hpp>
#include <game/component/meshAnimation.hpp>

#include <game/component/audio/audio.hpp>

#include <game/component/input/input.hpp>
#include <game/component/body/body.hpp>

#include <game/component/render.hpp>

#include <game/systems/render/debug/debug.hpp>

ClimbControl::ClimbControl(TNode *nodePlayer, glm::vec3 direction, glm::vec2 minLimits, glm::vec2 maxLimits)
{
    // Init values

    // AbstractValues
    this->changeInput = nullptr;

    this->minLimits = minLimits;
    this->maxLimits = maxLimits;

    // Member Values
    entity_cam = nullptr;
    entity_input = nullptr;

    control = {
        {std::make_pair(GLFW_KEY_W, PRESSED), [&]()
         { MoveUp(); }},
        {std::make_pair(GLFW_KEY_A, PRESSED), [&]()
         { MoveLeft(); }},
        {std::make_pair(GLFW_KEY_S, PRESSED), [&]()
         { MoveDown(); }},
        {std::make_pair(GLFW_KEY_D, PRESSED), [&]()
         { MoveRight(); }},
        {std::make_pair(GLFW_KEY_ESCAPE, ONETIME), [&]()
         { Pause(); }},
        {std::make_pair(GLFW_KEY_F, ONETIME), [&]()
         { Fall(); }},
    };

    this->nodePlayer = nodePlayer;
    WallFaceDirection = direction;

    auto screen = receiver->getScreenSizes();
    lastX = screen.x / 2.0f;
    lastY = screen.y / 2.0f;

    receiver->setCursorPos(0.5, 0.5);
    receiver->setCursorPosCallback(mouse_callback);
}

void ClimbControl::doActions(ECS::Entity &entity, ECS::EntityManager &entityManager)
{
    auto *render = entity.getComponent<RenderComponent>();
    if (render)
        entity_cam = entityManager.getEntityByKey(render->camID);

    // Transition
    if (nodePlayer->localTransform.interpolated_need_update)
        return;

    if (!canColide)
    {
        canColide = true;
        auto *body = entity.getComponent<BodyComponent>();
        body->canCollide = true;
        auto *physics = entity.getComponent<PhysicsComponent>();
        if (physics)
            physics->isStatic = false;
    }

    // Input Box

    entity_input = &entity;
    new_force = glm::vec3{0.0f};

    float horizontalAxisPosition = nodePlayer->globalPosition.x;

    if (std::abs(WallFaceDirection.x) > std::abs(WallFaceDirection.z))
        horizontalAxisPosition = nodePlayer->globalPosition.z;

    if (horizontalAxisPosition <= maxLimits.x && horizontalAxisPosition >= minLimits.x &&
        nodePlayer->globalPosition.y <= maxLimits.y + (nodePlayer->dimensionNode.y / 2.0f) && nodePlayer->globalPosition.y >= minLimits.y)
    {
        processKeys();
        // Move with direction camera
        auto *physics = entity_input->getComponent<PhysicsComponent>();
        auto *animation = entity_input->getComponent<MeshAnimationComponent>();

        if (physics)
        {
            if (new_force != glm::vec3(0.0f))
            {
                if (animation)
                    animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_CLIMB);

                auto sizeForce = glm::length(new_force);
                new_force = new_force / sizeForce;
                physics->ownForce += new_force * inputForce;
            }
            physics->ownForce += -physics->ownForce * 0.2f;
            physics->externForces = glm::vec3{0.0f};
        }
    }
    else
    {
        Fall();
    }
}

// TODO: Cambiar la direccion a la direccion que esta mirando el player en ese momento Ya que sino se peta la fuerza y no va bien
// Y agregar la solicion de Alex a empujarla desde cualquier angulo

void ClimbControl::MoveUp()
{
    new_force += glm::vec3(0, 1, 0);
}
void ClimbControl::MoveLeft()
{
    new_force += glm::cross(WallFaceDirection, glm::vec3(0, 1, 0));
}
void ClimbControl::MoveDown()
{
    new_force += glm::vec3(0, -1, 0);
}
void ClimbControl::MoveRight()
{
    new_force += glm::cross(WallFaceDirection, glm::vec3(0, -1, 0));
}
void ClimbControl::Fall()
{
    auto *physics = entity_input->getComponent<PhysicsComponent>();
    physics->gravityBool = true;

    physics->ownForce = glm::vec3{0.0f};
    physics->externForces = glm::vec3{0.0f};

    changeInput = [&](InputComponent &inp)
    {
        inp.controlType = std::make_unique<PlayerDefault>(nodePlayer);
        inp.IsInteracting = false;
        inp.Interact = false;
    };
}

void ClimbControl::Pause()
{
    eventManager->EventQueueInstance.EnqueueLateEvent(EventData{EventIds::Pause});
}

void ClimbControl::mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    if (!DebugSystem::isRun())
    {
        float xoffset = float(xposIn) - lastX;
        float yoffset = float(yposIn) - lastY;

        if (entity_cam)
        {
            auto *camera = entity_cam->template getComponent<CameraComponent>();
            if (camera)
                camera->cam_behaviour->setCameraToUpdate(xoffset, yoffset);
        }

        // cambiar 1280 y 720 a ventana
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        lastX = float(width) / 2.0f;
        lastY = float(height) / 2.0f;
        glfwSetCursorPos(window, lastX, lastY);
    }
}
