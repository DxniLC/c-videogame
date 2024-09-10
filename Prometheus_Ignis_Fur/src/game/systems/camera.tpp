#include "camera.hpp"
#include <game/component/camera/camera.hpp>

template <typename GameCTX>
void CameraSystem<GameCTX>::update(GameCTX &g, float const &deltaTime)
{
    for (auto &ccmp : g.template getComponents<CameraComponent>())
    {
        if (ccmp.cam_behaviour)
            ccmp.cam_behaviour->updateCamera(g, ccmp, deltaTime);
    }
}