#pragma once

#include <ecs/component/component.hpp>

#include <memory>

#include <game/component/input/controls/FreeCam.hpp>
#include <game/component/input/controls/PlayerDefault.hpp>
#include <game/component/input/controls/BoxControl.hpp>
#include <game/component/input/controls/ClimbControl.hpp>

struct InputComponent : public ECS::ComponentBase<InputComponent>
{
    explicit InputComponent(EntityKey e_id) : ECS::ComponentBase<InputComponent>(e_id){};

    // TODO: MEJORAR ESTO PARA LA IMPLEMENTACION DEL MANDO.
    std::unique_ptr<InputControl> controlType{nullptr};
    InputReceiver *receiver{nullptr};
    bool Interact{false};
    bool IsInteracting{false};

    float jumpForce{1000.0f};
    float maxInputForce{75.0f};
};
