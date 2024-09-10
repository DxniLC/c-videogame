#include "ThirdPerson.hpp"

#include <ecs/manager/entitymanager.hpp>

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <game/component/camera/camera.hpp>

#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

#include <math.h>

#include <game/component/frontbox.hpp>
#include <game/component/raycast/raycast.hpp>

// TODO: Hay que usar DeltaTime para un movimiento fluido, sino no va fluido

void ThirdPersonBehaviour::updateCamera(ECS::EntityManager &g, CameraComponent &ccmp, float const &deltaTime)
{

    auto *camera = ccmp.cam->template getEntity<TCamera>();

    camera->Yaw += xoffset * camera->MouseSensitivity * deltaTime;
    camera->Pitch += yoffset * camera->MouseSensitivity * deltaTime;

    // Camera limitations
    if (camera->Pitch > 60.0f)
        camera->Pitch = 60.0f;
    if (camera->Pitch < -50.0f)
        camera->Pitch = -50.0f;

    float xf = ccmp.distance * cos(glm::radians(camera->Yaw)) * cos(glm::radians(camera->Pitch));
    float yf = ccmp.distance * sin(glm::radians(camera->Pitch));
    float zf = ccmp.distance * sin(glm::radians(camera->Yaw)) * cos(glm::radians(camera->Pitch));

    // std::cout << "\nCamera Local: (" << xf << ", " << yf << ", " << zf << ")";
    // std::cout << "\nCamera Global: (" << camera->Position.x << ", " << camera->Position.y << ", " << camera->Position.z << ")";

    // Local Position
    ccmp.cam->localTransform.setPosition(xf, yf, zf);

    // std::cout << "\nCamera Target To: (" << ccmp.cam->getParent()->globalPosition.x << ", " << ccmp.cam->getParent()->globalPosition.y << ", " << ccmp.cam->getParent()->globalPosition.z << ")";

    // TODO: Mejorar camara para pitch y yaw y reajustar el frontbox

    // camera->Position += glm::vec3(xf, yf, zf);

    // camera->setTarget(ccmp.cam->getParent()->globalPosition);

    // Calculate Camera Directions on Local
    camera->Front = glm::normalize(-glm::vec3(xf, yf, zf) + ccmp.localFocus);
    auto rightDir = glm::cross(camera->Front, camera->WorldUp);
    if (not(rightDir.x == 0 and rightDir.y == 0 and rightDir.z == 0))
        camera->Right = glm::normalize(rightDir); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    auto upDir = glm::cross(camera->Right, camera->Front);
    if (not(upDir.x == 0 and upDir.y == 0 and upDir.z == 0))
        camera->Up = glm::normalize(upDir);

    // std::cout << "\nCamera Direction: (" << camera->Front.x << ", " << camera->Front.y << ", " << camera->Front.z << ")";
    // std::cout << "\nCamera Rotation: (" << camera->Yaw << ", " << camera->Pitch << ")";

    auto *entity_focus = g.getEntityByKey(ccmp.e_key);

    // UPDATE FRONTBOX
    ccmp.cam->getParent()->updateTransforms();
    auto *front = entity_focus->getComponent<FrontBoxComponent>();
    auto posFrontBox = ccmp.cam->getParent()->globalPosition + glm::normalize(camera->Front * glm::vec3(1, 0, 1)) * front->range;
    front->collider->nodeBox->localTransform.setPosition(posFrontBox - front->collider->nodeBox->getParent()->globalPosition);
    front->collider->nodeBox->localTransform.setRotate(0, -camera->Yaw, 0);

    // UPDATE RAYCAST
    auto *raycmp = entity_focus->getComponent<RaycastComponent>();
    for (auto &ray : raycmp->rays)
    {
        if (ray.functionID == FunctionID::RAYCAMERA)
        {
            ray.direction = -camera->Front;
            break;
        }
    }

    this->need_update = true;
    
    xoffset = 0;
    yoffset = 0;
}