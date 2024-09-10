
#include "BoxControl.hpp"

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

#include <ecs/manager/entitymanager.hpp>
#include <game/manager/event/EventManager.hpp>

#include <game/component/physics.hpp>
#include <game/component/camera/camera.hpp>
#include <game/component/frontbox.hpp>

#include <game/component/input/input.hpp>
#include <game/component/body/body.hpp>

#include <game/component/meshAnimation.hpp>

#include <game/component/render.hpp>

#include <game/component/audio/audio.hpp>

#include <game/systems/render/debug/debug.hpp>

BoxControl::BoxControl(TNode *nodePlayer, EntityKey boxKey, TNode *nodeBox, glm::vec3 direction)
{
    // Init values
    iPushed = false;

    // AbstractValues
    this->changeInput = nullptr;

    // Member Values
    entity_cam = nullptr;
    entity_input = nullptr;
    entity_box = nullptr;

    control = {
        {std::make_pair(GLFW_KEY_W, PRESSED), [&]()
         { PushPull(); }},
        {std::make_pair(GLFW_KEY_A, PRESSED), [&]()
         { PushPull(); }},
        {std::make_pair(GLFW_KEY_S, PRESSED), [&]()
         { PushPull(); }},
        {std::make_pair(GLFW_KEY_D, PRESSED), [&]()
         { PushPull(); }},
        {std::make_pair(GLFW_KEY_ESCAPE, ONETIME), [&]()
         { Pause(); }},
        {std::make_pair(GLFW_KEY_F, ONETIME), [&]()
         { LeftBox(); }},
    };

    this->boxKey = boxKey;
    this->nodePlayer = nodePlayer;
    this->nodeBox = nodeBox;
    direction_to_move = direction;

    auto screen = receiver->getScreenSizes();
    lastX = screen.x / 2.0f;
    lastY = screen.y / 2.0f;

    receiver->setCursorPos(0.5, 0.5);
    receiver->setCursorPosCallback(mouse_callback);
}

void BoxControl::doActions(ECS::Entity &entity, ECS::EntityManager &entityManager)
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
    entity_box = entityManager.getEntityByKey(boxKey);
    eventManager = entityManager.getSingletonComponent<EventManager>();

    auto incrementoX = nodePlayer->globalPosition.x - nodeBox->globalPosition.x;
    auto incrementoY = nodePlayer->globalPosition.y - nodeBox->globalPosition.y;
    auto incrementoZ = nodePlayer->globalPosition.z - nodeBox->globalPosition.z;

    if ((incrementoX < -2.1f || incrementoX > 2.1f) || (incrementoY < -1.f || incrementoY > 1.f) || (incrementoZ < -2.1f || incrementoZ > 2.1f))
    {
        LeftBox();
    }
    bool sameKey = false;
    if (new_force != glm::vec3{0.0f})
    {

        for (std::size_t i = 0; i < keysPressed.size(); i++)
        {
            if (keysPressed[i].first == lastKeyPressed)
                sameKey = true;
        }
    }

    if (!sameKey)
    {
        new_force = glm::vec3{0.0f};

        // Custom Key Update
        for (std::size_t i = 0; i < keysPressed.size(); i++)
        {
            auto itr = control.find(keysPressed[i]);
            if (itr != control.end())
            {
                // Tengo que sacar la direccion del cubo para que solo se mueva a lo largo de esa. (Direccion seria el centro de la box con el player)
                currentKey = keysPressed[i].first;
                itr->second();
            }
        }
        lastKeyPressed = currentKey;
    }

    auto *audio = entity_input->getComponent<AudioComponent>();
    auto *physics = entity_input->getComponent<PhysicsComponent>();
    if (physics && new_force != glm::vec3{0.0f})
    {
        iPushed = true;

        auto sizeForce = sqrt(new_force.x * new_force.x + new_force.z * new_force.z);
        new_force = new_force / float(sizeForce);
        physics->ownForce += new_force * inputForce;

        if (audio)
            audio->soundsToUpdate.emplace_back("Effects/box_move", true, nullptr); // TODO: Mirar de mover al player en base a la masa de la caja entonces para cajas mas pesadas se mueve mas lento y podemos proporcionar una habilidad que una caja muy pesada se mueva si tienes esa habilidad. Por ejemplo una caja de hormigon

        auto *box_phy = entity_box->getComponent<PhysicsComponent>();
        box_phy->ownForce = physics->ownForce;
    }
    else
    {
        if (audio)
            audio->soundsToUpdate.emplace_back("Effects/box_move", false, nullptr);
    }
    if (!iPushed)
    {
        auto *animation = entity_input->getComponent<MeshAnimationComponent>();
        animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_IDLE);
    }
}

// TODO: Cambiar la direccion a la direccion que esta mirando el player en ese momento Ya que sino se peta la fuerza y no va bien
// Y agregar la solicion de Alex a empujarla desde cualquier angulo

void BoxControl::PushPull()
{
    iPushed = true;

    // Caculamos el angulo entre la direccion a la que se ha pulsado con respecto la camara

    auto *render = entity_input->template getComponent<RenderComponent>();

    auto *camaraNodo = render->getNodeChild(RenderComponent::RenderType::CAMERA);
    auto *animation = entity_input->getComponent<MeshAnimationComponent>();
    animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_PUSH, false);

    auto *camEntity = camaraNodo->getEntity<TCamera>();
    glm::vec3 directionInput;
    switch (currentKey)
    {
    case GLFW_KEY_W:
        new_force -= glm::vec3(direction_to_move.x, 0, direction_to_move.z);
        return;
        break;
    case GLFW_KEY_A:
        directionInput = -glm::vec3{camEntity->Right.x, 0, camEntity->Right.z};

        break;
    case GLFW_KEY_S:
        new_force += glm::vec3(direction_to_move.x, 0, direction_to_move.z);
        return;
        break;
    case GLFW_KEY_D:
        directionInput = glm::vec3{camEntity->Right.x, 0, camEntity->Right.z};
        break;
    }

    double angle = std::abs(acos(glm::dot(directionInput, direction_to_move) / (glm::length(directionInput) * glm::length(direction_to_move))) * (180 / M_PI));
    if (angle > 135)
        new_force -= glm::vec3(direction_to_move.x, 0, direction_to_move.z);
    else if (angle < 45)
        new_force += glm::vec3(direction_to_move.x, 0, direction_to_move.z);
}

void BoxControl::LeftBox()
{
    changeInput = [&](InputComponent &inp)
    {
        auto *audio = entity_input->getComponent<AudioComponent>();
        if (audio)
            audio->soundsToUpdate.emplace_back("Effects/box_move", false, nullptr);
        inp.controlType = std::make_unique<PlayerDefault>(nodePlayer);
        inp.IsInteracting = false;
    };
}

void BoxControl::ShowDebug()
{
    if (!DebugSystem::isRun())
    {
        DebugSystem::start();
    }
    else
    {
        DebugSystem::close();
    }
}

void BoxControl::Pause()
{
    eventManager->EventQueueInstance.EnqueueLateEvent(EventData{EventIds::Pause});
}

void BoxControl::mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
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
