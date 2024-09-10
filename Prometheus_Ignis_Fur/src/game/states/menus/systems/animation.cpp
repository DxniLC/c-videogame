
#include "animation.hpp"

#include "../components/animation.hpp"
#include "../components/render.hpp"
#include "../components/input.hpp"
#include "../components/camera.hpp"

#include <chrono>

template <typename GameCTX>
void AnimationMenuSystem<GameCTX>::update(GameCTX &g, float deltaTime)
{

    // Animacion de camara rotando
    for (auto &scene : g.template getComponents<CameraMenuComponent>())
    {
        scene.camera.first->setGlobalPosition(scene.Position);
        scene.Rotation.y += scene.RotationSpeed * deltaTime;
        // std::cout << "\nROTATION CAMERA: " << scene.camera.second->Yaw + 90;
        // std::cout << "\nROTATION NODE: " << scene.camera.first->localTransform.rotation.y << "\n";
        scene.camera.first->localTransform.setRotate(-scene.Rotation);
        scene.camera.second->setRotation(scene.Rotation);
    }

    for (auto &animation : g.template getComponents<AnimationMenuComponent>())
    {

        auto *entity = g.getEntityByKey(animation.getEntityKey());
        auto *input = entity->template getComponent<InputMenuComponent>();

        if (input)
        {
            if (animation.startHoverAnimation && animation.animationElement && not animation.animationElement->IsOnAnimation() && input->isClicked) // Si se hace click, inicia animacion y no se puede seleccionar ni nada mas
            {
                animation.ClickAnimation(animation);
                animation.startClickAnimation = true;
                break;
            }
            if (not input->isClicked)
            {
                if (input->isHovered && not animation.startHoverAnimation) // Si estoy hover y no se ha iniciado animacion
                {
                    animation.HoverAnimation(animation, false);
                    animation.startHoverAnimation = true;
                }
                else if (not input->isHovered && animation.startHoverAnimation) // Si estaba hover y ahora ya no, invierto animacion
                {
                    animation.HoverAnimation(animation, true);
                    animation.startHoverAnimation = false;
                    auto *cmpRend = entity->template getComponent<RenderMenuComponent>();
                    if (cmpRend)
                        cmpRend->hudElement->addAnimationScale(glm::vec2(1, 1), 2);
                }
            }
        }
    }
}
