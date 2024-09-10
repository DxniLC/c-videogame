
#include "TWindEffect.hpp"

#include <engines/graphic/resource/ResourceModel.hpp>
#include <engines/graphic/resource/ResourceShader.hpp>

#include <iostream>

void TWindEffect::update()
{
    WindTime += deltaTime;

    if (WindTime > LoopAngle)
        WindTime -= float(LoopAngle);

    auto *shader = mesh->getShader();

    if (shader)
    {
        shader->use();
        shader->setVec2("WindDirection", WindDirection);
        shader->setFloat("CurrentWindTime", WindTime);
        shader->setFloat("WindForce", WindForce);
    }

    mesh->draw(matrix);
}