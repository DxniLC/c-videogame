
#include "FreeCam.hpp"

#include <ecs/manager/entitymanager.hpp>
#include <game/manager/event/EventManager.hpp>

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

FreeCam::FreeCam(TNode *cameraActived)
{
    this->control = {
        {std::make_pair(GLFW_KEY_W, PRESSED),[&](){MoveFront();}},
        {std::make_pair(GLFW_KEY_A, PRESSED),[&](){MoveLeft();}},
        {std::make_pair(GLFW_KEY_S, PRESSED),[&](){MoveBack();}},
        {std::make_pair(GLFW_KEY_D, PRESSED),[&](){MoveRight();}},
        {std::make_pair(GLFW_KEY_SPACE, PRESSED),[&](){MoveUp();}},
        {std::make_pair(GLFW_KEY_LEFT_CONTROL, PRESSED),[&](){MoveDown();}},
    };
    camera = cameraActived;
    auto screen = receiver->getScreenSizes();
    lastX = screen.x / 2.0f;
    lastY = screen.y / 2.0f;
    firstMouse = true;
    receiver->setCursorPosCallback(mouse_callback);
}

void FreeCam::doActions([[maybe_unused]] ECS::Entity &entity, [[maybe_unused]] ECS::EntityManager &entityManager)
{
    processKeys();
}

void FreeCam::MoveFront()
{
    auto *cam = camera->getEntity<TCamera>();
    camera->localTransform.setPosition(cam->setTranslate(Movement::FORWARD));
}

void FreeCam::MoveLeft()
{
    auto *cam = camera->getEntity<TCamera>();
    camera->localTransform.setPosition(cam->setTranslate(Movement::LEFT));
}

void FreeCam::MoveBack()
{
    auto *cam = camera->getEntity<TCamera>();
    camera->localTransform.setPosition(cam->setTranslate(Movement::BACKWARD));
}

void FreeCam::MoveRight()
{
    auto *cam = camera->getEntity<TCamera>();
    camera->localTransform.setPosition(cam->setTranslate(Movement::RIGHT));
}

void FreeCam::MoveUp()
{
    auto *cam = camera->getEntity<TCamera>();
    camera->localTransform.setPosition(cam->setTranslate(Movement::UP));
}

void FreeCam::MoveDown()
{
    auto *cam = camera->getEntity<TCamera>();
    camera->localTransform.setPosition(cam->setTranslate(Movement::DOWN));
}

void FreeCam::mouse_callback([[maybe_unused]] GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = float(xposIn);
    float ypos = float(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // float xoffset = xpos - lastX;
    // float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    // send camera update
    // auto &cam = camera->getEntity<TCamera>();
    // cam.setRotation(xoffset, yoffset);
}