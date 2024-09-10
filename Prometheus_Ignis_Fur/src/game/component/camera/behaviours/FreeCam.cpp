#include "FreeCam.hpp"

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <game/component/camera/camera.hpp>

#include <engines/graphic/SGEngine/scene/entity/TCamera.hpp>

void FreeCameraBehaviour::updateCamera([[maybe_unused]] ECS::EntityManager &g, CameraComponent &ccmp, [[maybe_unused]] float const &deltaTime)
{
    // WARNING_TO_FIX
    // g not used && deltaTime

    auto *camera = ccmp.cam->template getEntity<TCamera>();
    camera->Front = glm::normalize(ccmp.globalFocus - camera->Position);
    // std::cout << "\nCAMERA FRONT: " << camera->Front.x << ", " << camera->Front.y << ", " << camera->Front.z << "\n";
    auto rightDir = glm::cross(camera->Front, camera->WorldUp);
    if (not(rightDir.x == 0 and rightDir.y == 0 and rightDir.z == 0))
        camera->Right = glm::normalize(rightDir); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    auto upDir = glm::cross(camera->Right, camera->Front);
    if (not(upDir.x == 0 and upDir.y == 0 and upDir.z == 0))
        camera->Up = glm::normalize(upDir);
}
