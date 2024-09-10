
#include "PlayerDefault.hpp"

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

#include <engines/graphic/SGEngine/scene/entity/particle/ParticleGenerator.hpp>

#include <ecs/manager/entitymanager.hpp>

#include <game/manager/event/EventManager.hpp>

#include <game/component/physics.hpp>
#include <game/component/camera/camera.hpp>

#include <game/component/input/input.hpp>
#include <game/component/input/controls/FreeCam.hpp>

#include <game/component/ability/ability.hpp>
#include <game/component/sensory.hpp>

#include <game/component/raycast/raycast.hpp>

#include <game/component/meshAnimation.hpp>

#include <game/component/render.hpp>

#include <game/component/powerup/powerup.hpp>

#include <game/animations/CameraAnimation.hpp>
#include <game/manager/animation/AnimationManager.hpp>

#include <game/systems/render/debug/debug.hpp>

#include <glm/gtx/vector_angle.hpp>

// Event Datas
#include <game/events/eventsData/ProjectileEventData.hpp>
#include <game/events/eventsData/StealthData.hpp>
#include <game/events/eventsData/UpdatePowerUpUIData.hpp>
#include <game/events/eventsData/AbilityData.hpp>

// Achievements
#include <game/manager/AchievementManager.hpp>

PlayerDefault::PlayerDefault(TNode *nodePlayer)
{
    // Init values

    // AbstractValues
    this->changeInput = nullptr;

    // Member Values
    entity_cam = nullptr;
    entity_input = nullptr;

    this->control =
        {
            {std::make_pair(GLFW_KEY_W, PRESSED), [&]()
             { MoveFront(); }},
            {std::make_pair(GLFW_KEY_A, PRESSED), [&]()
             { MoveLeft(); }},
            {std::make_pair(GLFW_KEY_S, PRESSED), [&]()
             { MoveBack(); }},
            {std::make_pair(GLFW_KEY_D, PRESSED), [&]()
             { MoveRight(); }},
            {std::make_pair(GLFW_KEY_Q, ONETIME), [&]()
             { ChangePower(); }},
            {std::make_pair(GLFW_KEY_R, ONETIME), [&]()
             { UsePower(); }},
            {std::make_pair(GLFW_KEY_SPACE, ONETIME), [&]()
             { Jump(); }},
            {std::make_pair(GLFW_KEY_LEFT_SHIFT, PRESSED), [&]()
             { Run(); }},
            {std::make_pair(GLFW_KEY_E, ONETIME), [&]()
             { Dash(); }},
            {std::make_pair(GLFW_KEY_F, ONETIME), [&]()
             { Interact(); }},
            {std::make_pair(GLFW_KEY_ESCAPE, ONETIME), [&]()
             { Pause(); }},
            {std::make_pair(GLFW_KEY_F2, ONETIME), [&]()
             { ShowDebug(); }},
            {std::make_pair(GLFW_MOUSE_BUTTON_LEFT, PRESSED), [&]()
             { chargeBow(); }},
            {std::make_pair(GLFW_MOUSE_BUTTON_LEFT, RELEASED), [&]()
             { FireArrow(); }},
            {std::make_pair(GLFW_MOUSE_BUTTON_RIGHT, PRESSED), [&]()
             { AimCamera(); }},
            {std::make_pair(GLFW_MOUSE_BUTTON_RIGHT, RELEASED), [&]()
             { GoCameraNormal(); }},
            {std::make_pair(GLFW_KEY_P, ONETIME), [&]()
             { Prueba(); }},
            {std::make_pair(GLFW_KEY_LEFT_CONTROL, PRESSED), [&]()
             { goStealthMode(true); }},
            {std::make_pair(GLFW_KEY_LEFT_CONTROL, RELEASED), [&]()
             { goStealthMode(false); }},
            {std::make_pair(GLFW_KEY_1, PRESSED), [&]()
             { Teleport(glm::vec3 {-102.3, 23, -454.5}); }},
            {std::make_pair(GLFW_KEY_2, PRESSED), [&]()
             { Teleport(glm::vec3 {-181.3, 37, -577.8}); }},
            {std::make_pair(GLFW_KEY_3, PRESSED), [&]()
             { Teleport(glm::vec3 {-290,17,-615}); }},
            {std::make_pair(GLFW_KEY_4, PRESSED), [&]()
             { Teleport(glm::vec3 {1,1,1}); }},
            {std::make_pair(GLFW_KEY_5, PRESSED), [&]()
             { Teleport(glm::vec3 {-161,62,-209.6}); }},
            {std::make_pair(GLFW_KEY_6, PRESSED), [&]()
             { Teleport(glm::vec3 {-310, 56.8, -99}); }},
            {std::make_pair(GLFW_KEY_7, PRESSED), [&]()
             { Teleport(glm::vec3 {63.6, 28, 59}); }},
            {std::make_pair(GLFW_KEY_8, PRESSED), [&]()
             { Teleport(glm::vec3 {-3, 28, 96}); }},
            {std::make_pair(GLFW_KEY_9, PRESSED), [&]()
             { Teleport(glm::vec3 {142, 32, 66}); }},
            {std::make_pair(GLFW_KEY_0, PRESSED), [&]()
             { Teleport(glm::vec3 {158, 48, -261}); }},
            {std::make_pair(GLFW_KEY_P, PRESSED), [&]()
             { Teleport(glm::vec3 {-4, 36, 76}); }},
        };

    this->nodePlayer = nodePlayer;

    auto screen = receiver->getScreenSizes();
    lastX = screen.x / 2.0f;
    lastY = screen.y / 2.0f;
    receiver->setCursorPos(0.5, 0.5);
    receiver->setCursorPosCallback(mouse_callback);
}

// TODO: Reorganizarlo para que sea entendible
void PlayerDefault::doActions(ECS::Entity &entity, ECS::EntityManager &entityManager)
{

    mDeltaTime = std::chrono::system_clock::now() - mStartTime;

    float deltaTime = mDeltaTime.count();

    auto *render = entity.getComponent<RenderComponent>();

    entity_cam = entityManager.getEntityByKey(render->camID);

    this->entityManager = &entityManager;

    entity_input = &entity;
    new_force = glm::vec3{0.0f};
    extraForce = 1;
    IsListened = false;
    HasJumped = false;

    auto *nodeGen = render->getNodeChild(RenderComponent::RenderType::PARTICLE_1);
    if (nodeGen)
        particleGen = nodeGen->getEntity<ParticleGenerator>();

    auto *camaraNodo = render->getNodeChild(RenderComponent::RenderType::CAMERA);
    if (camaraNodo)
    {
        auto *camEntity = camaraNodo->getEntity<TCamera>();
        if (camEntity)
        {
            camFront = camEntity->Front;
            camRight = camEntity->Right;
        }
    }

    // RESET DASH UI
    if (deltaTime >= 2.0f)
    {
        auto *abilities = entity_input->getComponent<AbilityComponent>();
        if (abilities)
        {
            auto *dash = abilities->getAbility<DashAbility>();
            if (dash)
            {
                auto *eventManager = entityManager.template getSingletonComponent<EventManager>();
                if (eventManager)
                    eventManager->EventQueueInstance.EnqueueLateEvent(AbilityData{dash->abilityID});
            }
        }
    }

    processKeys();

    auto *audio = entity_input->template getComponent<AudioComponent>();
    auto *animation = entity_input->getComponent<MeshAnimationComponent>();
    auto *physics = entity_input->getComponent<PhysicsComponent>();
    auto *input = entity_input->getComponent<InputComponent>();

    if (!physics->isGrounded || physics->waiting4ChangeGrounded)
        IsFalling = true;

    if (physics && new_force != glm::vec3(0.0f))
    {
        auto sizeForce = sqrt(new_force.x * new_force.x + new_force.z * new_force.z);
        new_force = new_force / float(sizeForce);

        inputForce += acceleration;

        if (inputForce >= input->maxInputForce)
        {
            inputForce = input->maxInputForce;
        }

        if (!physics->isGrounded || physics->waiting4ChangeGrounded)
        {
            if (particleGen)
            {
                particleGen->IsRunning = false;
                particleGen->deleteAllParticles();
            }
            if (audio)
                audio->soundsToUpdate.emplace_back("Effects/steps", false, nullptr);

            inputForce = inputForce * 0.60f;
        }
        else
        {
            if (animation)
            {

                if (extraForce == 1)
                {

                    // DesactivaParticulas
                    if (particleGen)
                    {
                        particleGen->IsRunning = false;
                        particleGen->deleteAllParticles();
                    }

                    if (deltaTime >= AnimationColdDown)
                        animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_MOVE);
                }
                else
                {
                    if (particleGen)
                        particleGen->IsRunning = true;

                    if (deltaTime >= AnimationColdDown)
                        animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_RUN);
                }

                if (audio)
                {
                    if (IsFalling)
                    {
                        audio->soundsToUpdate.emplace_back("Effects/jump_fall", true, nullptr);
                        IsFalling = false;
                    }
                    audio->soundsToUpdate.emplace_back("Effects/steps", true, nullptr);
                }
            }

            auto orient = glm::normalize(glm::vec3(new_force.x, 0, new_force.z));
            if (not IsAiming)
                setOrientation(orient);
        }
        physics->ownForce += new_force * inputForce * extraForce;

        IsListened = true;
    }
    else
    {

        if (audio)
        {
            if (!(!physics->isGrounded || physics->waiting4ChangeGrounded))
            {
                if (IsFalling)
                {
                    audio->soundsToUpdate.emplace_back("Effects/jump_fall", true, nullptr);
                    IsFalling = false;
                }
            }
            audio->soundsToUpdate.emplace_back("Effects/steps", false, nullptr);
        }

        if (((physics->ownForce.x == 0.0f && physics->ownForce.z == 0.0f) || (deltaTime >= AnimationColdDown)) && not IsFalling)
        {

            // DesactivaParticulas
            if (particleGen)
            {
                particleGen->IsRunning = false;
                particleGen->deleteAllParticles();
            }

            animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_IDLE, false, 0.2f);
            HasDashed = false;
        }

        inputForce = 20;
    }

    if (not physics->isGrounded && physics->force.y < -100 && (not HasDashed || (HasDashed && deltaTime >= 0.4)))
        animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_ON_AIR, false, 0.2f);

    auto *sensory = entity_input->getComponent<SensoryComponent>();
    if (sensory)
        sensory->IsListened = IsListened;

    if (not IsAiming)
        resetCamera();

    if (HasJumped)
    {
        auto *achieveManager = entityManager.getSingletonComponent<AchievementManager>();
        if (achieveManager)
        {
            achieveManager->achievementData.currentJumps++;
        }
    }
}

void PlayerDefault::MoveFront()
{
    // TODO: Buscar el hijo correspondiente

    new_force += glm::vec3(camFront.x, 0, camFront.z);

    glm::vec3 newDirection{camFront.x, 0, camFront.z};
    newDirection = glm::normalize(newDirection);
}

void PlayerDefault::MoveLeft()
{
    new_force -= camRight;
}

void PlayerDefault::MoveBack()
{
    new_force -= glm::vec3(camFront.x, 0, camFront.z);
}

void PlayerDefault::MoveRight()
{
    new_force += camRight;
}

void PlayerDefault::ChangePower()
{
    auto *powerCMP = entity_input->template getComponent<PowerUpComponent>();
    if (powerCMP && not powerCMP->IsUsingPowerUp)
    {
        powerCMP->PowerUpIndex = (powerCMP->PowerUpIndex < 2) ? powerCMP->PowerUpIndex + 1 : 0;
        auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
        if (eventManager)
            eventManager->EventQueueInstance.EnqueueLateEvent(UpdatePowerUpUIData{powerCMP->PowerUpIndex, false, false});
    }
}

void PlayerDefault::UsePower()
{
    auto *powerCMP = entity_input->template getComponent<PowerUpComponent>();
    if (powerCMP && not powerCMP->powerUps.empty() && not powerCMP->IsUsingPowerUp)
    {
        if (powerCMP->PowerUpIndex < int(powerCMP->powerUps.size()))
        {
            auto &element = powerCMP->powerUps[powerCMP->PowerUpIndex];
            element->usePowerUp(*entity_input);
            powerCMP->IsUsingPowerUp = true;

            auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
            if (eventManager)
                eventManager->EventQueueInstance.EnqueueLateEvent(UpdatePowerUpUIData{powerCMP->PowerUpIndex, true, false});
        }
    }
}

void PlayerDefault::Jump()
{

    auto *physics = entity_input->getComponent<PhysicsComponent>();
    auto *animation = entity_input->getComponent<MeshAnimationComponent>();
    auto *input = entity_input->getComponent<InputComponent>();

    auto thisTime = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    auto timeSinceLastJump = thisTime - timeLastJump;

    if (physics->isGrounded && timeSinceLastJump > JumpColdDown)
    {

        // DesactivaParticulas
        if (particleGen)
        {
            particleGen->IsRunning = false;
            particleGen->deleteAllParticles();
        }

        timeLastJump = thisTime;

        // Queremos que pare la caida al saltar
        if (physics->externForces.y < 0)
            physics->externForces.y = input->jumpForce;
        else
            physics->externForces.y += input->jumpForce;

        animation->animationEntity->playAnimation(PlayerAnimation::PLAYER_JUMP, true, 0.1f);

        auto *audio = entity_input->template getComponent<AudioComponent>();
        if (audio)
        {
            audio->soundsToUpdate.emplace_back("Effects/steps", false, nullptr);
            audio->soundsToUpdate.emplace_back("Effects/jump", true, nullptr);
        }

        IsListened = true;
        HasJumped = true;
    }
    else
    {
        auto *abilities = entity_input->getComponent<AbilityComponent>();
        if (abilities)
        {
            auto *double_jump = abilities->getAbility<DoubleJumpAbility>();
            if (double_jump)
            {
                IsListened = double_jump->useAbility(*entity_input);
                if (IsListened)
                {
                    auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
                    if (eventManager)
                        eventManager->EventQueueInstance.EnqueueLateEvent(AbilityData{double_jump->abilityID, true});
                }
            }
        }
    }
    // Para que funcione free cam, la camara tiene que tener input y el comportamiento de freecam.
    // Si hago un bool de activado, podria hacer que se habilite el input de la camara , creando si inp_type ya que si esta desactivado
    // puede rallarse el callback
    // changeInput = [](InputComponent &inp)
    // {
    //     inp.controlType = std::make_unique<FreeCam>(receiver, nodePlayer->getChilds()[0].get());
    // };
}

void PlayerDefault::Run()
{
    extraForce = 1.70f;
}

void PlayerDefault::Dash()
{
    auto *abilities = entity_input->getComponent<AbilityComponent>();
    if (abilities)
    {
        auto *dash = abilities->getAbility<DashAbility>();
        if (dash)
        {
            IsListened = dash->useAbility(*entity_input);
            if (IsListened)
            {
                auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
                if (eventManager)
                    eventManager->EventQueueInstance.EnqueueLateEvent(AbilityData{dash->abilityID, true});
                HasDashed = true;
                mStartTime = std::chrono::system_clock::now();
            }
        }
    }
}

void PlayerDefault::Interact()
{
    auto *input = entity_input->template getComponent<InputComponent>();
    input->Interact = true;
}

void PlayerDefault::chargeBow()
{

    if (inputBowForce < 2.5f && currentFrames <= 140)
    {
        inputBowForce = float(currentFrames) * 0.04f;
        currentFrames++;
    }
}

void PlayerDefault::Prueba()
{

    // auto *powerCMP = entity_input->template getComponent<PowerUpComponent>();
    // if (powerCMP)
    // {
    //     /*auto &element = powerCMP->powerUps.emplace_back(std::make_unique<MoreJump>());
    //     element->usePowerUp(*entity_input);*/

    //     if (powerCMP->MaxPowerUpIndex >= 2)
    //         powerCMP->MaxPowerUpIndex = 2;
    //     else
    //     {
    //         auto &element = powerCMP->powerUps.emplace_back(std::make_unique<MoreSpeed>());
    //         powerCMP->MaxPowerUpIndex++;
    //     }
    // }

    // unsigned int ID{0};
    // eventManager->notifyEvent("InitCinematic", ID, 10.0f);

    // auto *camCMP = entity_cam->template getComponent<CameraComponent>();

    // auto *anim = entity_cam->template getComponent<AnimationComponent>();
    // anim->animating = true;

    // AnimationManager::addAnimation(*anim, 1, CameraAnimation{camCMP->cam, *entity_cam, {0, 30, 0}, {0, 0, 0}, 0.1f, 0.1f});
}

void PlayerDefault::FireArrow()
{
    // Create Entity to Throw
    if (IsAiming)
    {
        auto *camCMP = entity_cam->template getComponent<CameraComponent>();
        auto *camera = camCMP->cam->getEntity<TCamera>();
        auto direction = camera->Front;
        auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
        if (eventManager)
            eventManager->EventQueueInstance.EnqueueLateEvent(ProjectileEventData{nodePlayer->globalPosition + (direction * 3.0f), direction, 2000.f * inputBowForce, true, true});

        inputBowForce = 0.0f;
        currentFrames = 0.0f;
    }
}

void PlayerDefault::AimCamera()
{
    auto *anim = entity_cam->template getComponent<AnimationComponent>();
    anim->animating = true;
    auto *camCMP = entity_cam->template getComponent<CameraComponent>();
    auto *camera = camCMP->cam->getEntity<TCamera>();

    float zoomTarget = 2.5f;
    float speed{0.1f};

    glm::vec3 localFocusTarget;

    localFocusTarget.x = (camera->Right.x * 1.f);
    localFocusTarget.y = 0.75f;
    localFocusTarget.z = (camera->Right.z * 1.f);

    auto *animationManager = entityManager->getSingletonComponent<AnimationManager>();
    assert(animationManager);

    auto animationStatus = animationManager->addAnimation(*anim, 0, CameraAnimation{camCMP->cam, *entity_cam, zoomTarget, localFocusTarget, speed, speed});

    if (not IsAiming)
    {
        auto *audio = entity_input->template getComponent<AudioComponent>();
        audio->soundsToUpdate.emplace_back("Effects/bow_out", true, nullptr);
    }

    if (animationStatus.second == false)
    {
        // UPDATE X
        CameraAnimation *animation = dynamic_cast<CameraAnimation *>((animationStatus.first)->second.get());
        animation->distanceTarget = zoomTarget;
        animation->focusTarget = localFocusTarget;
        animation->revert(true);
    }

    setOrientation(glm::normalize(glm::vec3(camera->Front.x, 0, camera->Front.z)));

    IsAiming = true;
}

void PlayerDefault::GoCameraNormal()
{
    IsAiming = false;
}

void PlayerDefault::resetCamera()
{
    auto *anim = entity_cam->template getComponent<AnimationComponent>();

    if (not IsAiming && anim->animating)
    {

        auto *animationManager = entityManager->getSingletonComponent<AnimationManager>();
        if (animationManager)
        {
            auto *animationBase = animationManager->getAnimation(*anim, 0);

            if (animationBase)
            {
                animationBase->revert(false);
            }
        }
    }
}

void PlayerDefault::goStealthMode(bool active)
{
    if (IsStealth != active)
    {
        auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
        if (eventManager)
            eventManager->EventQueueInstance.EnqueueLateEvent(StealthData{active});
        IsStealth = active;
    }
    // Setear animacion agahado o cualquier cosa como puede ser el efecto de las barras negras de cinematica
}

void PlayerDefault::ShowDebug()
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

void PlayerDefault::Pause()
{
    auto *eventManager = entityManager->template getSingletonComponent<EventManager>();
    if (eventManager)
        eventManager->EventQueueInstance.EnqueueLateEvent(EventData{EventIds::Pause});
}

void PlayerDefault::setOrientation(glm::vec3 const &newDirection)
{
    // calculamos el nuevo angulo de rotacion
    if (newDirection != nodePlayer->direction && not nodePlayer->localTransform.interpolated_need_update)
    {
        nodePlayer->localTransform.setRotateInterpolated(nodePlayer->direction, newDirection, 2.0f, glm::vec3(0, 1, 0));
    }
}

void PlayerDefault::mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
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

void PlayerDefault::Teleport(glm::vec3 postp)
{
    nodePlayer->localTransform.setPosition(postp);
}
