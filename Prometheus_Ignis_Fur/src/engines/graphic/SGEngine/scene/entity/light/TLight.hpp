#pragma once

#include "../TEntity.hpp"

#include <glm/glm.hpp>

#include <memory>

#include "../../LightEffects/LightEffect.hpp"
#include "../../LightEffects/FireLight.hpp"

// Maximo de luces que se calculan a la vez. Cambiar en el Frag
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct ResourceShader;
struct ResourceModel;
struct TLight : public TEntity
{
    explicit TLight(LightParams params, ResourceShader *shader, ResourceModel *model);

    template <typename EffectType>
    void addEffect(EffectType &&lightEffect)
    {
        effect = std::make_unique<EffectType>(std::move(lightEffect));
    }

    void update() override final;

    LightParams params{};

private:
    ResourceShader *shader{nullptr};
    ResourceModel *model{nullptr};
    std::unique_ptr<LightEffect> effect{nullptr};
};