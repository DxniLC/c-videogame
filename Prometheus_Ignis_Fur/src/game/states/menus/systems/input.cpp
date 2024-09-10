#include "input.hpp"

#include "../components/render.hpp"
#include "../components/animation.hpp"
#include "../components/input.hpp"

#include <game/systems/render/debug/debug.hpp>

template <typename GameCTX>
void InputMenuSystem<GameCTX>::update(GameCTX &g)
{

    // Switch DEBUG
    if (receiver->isKeyPressOneTime(291) == 1)
    {
        if (!DebugSystem::isRun())
        {
            DebugSystem::start();
        }
        else
        {
            DebugSystem::close(false);
        }
    }
    else
    {

        for (auto &input : g.template getComponents<InputMenuComponent>())
        {

            input.isHovered = false;

            auto *entity = g.getEntityByKey(input.getEntityKey());
            auto *animation = entity->template getComponent<AnimationMenuComponent>();
            auto *render = entity->template getComponent<RenderMenuComponent>();

            // if (typeid(*render->hudElement) == typeid(HUDRect))
            // {
            //     IsHovering = CheckIsHover(render->hudElement->getPosition(), render->hudElement->getSize(), 1);
            // }
            // else
            IsHovering = CheckIsHover(render->hudElement->getPosition(), render->hudElement->getSize());

            if (input.isClicked)
            {
                if (not animation || (not animation->animationElement->IsOnAnimation() && animation->startClickAnimation))
                {
                    input.actionFunc();
                    input.isClicked = false;
                    break;
                }
            }
            else if (IsHovering)
            {
                if (not input.isClicked && receiver->isKeyPressed(0)) // TODO: 0 = left Button
                {
                    // Start animation click
                    input.isClicked = true;
                    receiver->setStandardCursorShape(MouseCursorShape::ArrowCursor);
                }
                else
                {
                    receiver->setStandardCursorShape(MouseCursorShape::HandCursor);
                    // Start animation hover
                    input.isHovered = true;
                }

                break;
            }
        }
        if (not IsHovering)
            receiver->setStandardCursorShape(MouseCursorShape::ArrowCursor);
    }

    // if (inp.controlType->exit)
    // {
    //     exitInput = true;
    // }
}

template <typename GameCTX>
bool InputMenuSystem<GameCTX>::CheckIsHover(glm::vec2 position, glm::vec2 sizeElement)
{
    mousePos = receiver->getCursorPos();

    if ((mousePos.x <= position.x + sizeElement.x && mousePos.x >= position.x) &&
        (mousePos.y >= position.y && mousePos.y <= position.y + sizeElement.y))
    {

        return true;
    }

    return false;
}
