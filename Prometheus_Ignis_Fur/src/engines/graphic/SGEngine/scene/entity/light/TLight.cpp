
#include "TLight.hpp"

#include <GLAD/src/glad.h>
#include <engines/graphic/resource/ResourceShader.hpp>
#include <engines/graphic/resource/ResourceModel.hpp>

TLight::TLight(LightParams params, ResourceShader *shader, ResourceModel *model) : params(params), shader(shader), model(model)
{
    // cte 0.75 // quadratic 0
    // addEffect(FireLight{0.6, 0.5, 0.075f});
}

void TLight::update()
{

    // shader = debug. Para dibujar rango luz
    if (effect)
        effect->update(params, deltaTime);
    params.position = matrix[3];
    if (model)
    {
        ResourceShader *lastShader{model->setShader(shader)};
        model->draw(matrix);
        model->setShader(lastShader);
    }
}