

#include "animation.hpp"

#include <game/component/animation.hpp>

template <typename GameCTX>
void AnimationSystem<GameCTX>::update(GameCTX &g, float dTime)
{
    auto *animationManager = g.template getSingletonComponent<AnimationManager>();
    if (not animationManager)
        return;
        
    for (auto &animCMP : g.template getComponents<AnimationComponent>())
    {

        if (animCMP.animating && not animCMP.animationsIDs.empty())
        {

            auto &animationsMap = animationManager->getAnimationsByID(animCMP.animID);

            auto animationID = animCMP.animationsIDs[std::size_t(animCMP.currentAnimation)];

            auto &animation = animationsMap[animationID];

            bool IsEnded = animation->update(g, dTime);

            if (IsEnded)
            {
                // Si soy la ultima animacion y voy hacia arriba, o si soy primera y voy hacia bajo
                if ((animationID == animCMP.animationsIDs.back() && not animCMP.inverseOrder) || (animationID == animCMP.animationsIDs.front() && animCMP.inverseOrder))
                {
                    // Revertir animacion e ir hacia atrás
                    if (animCMP.IsLooping)
                    {
                        animCMP.inverseOrder = not animCMP.inverseOrder;
                        animation->revert(animCMP.inverseOrder);
                    }
                    else if (animation->remove)
                    {
                        // Si es ultimo, borrar ultimo y desplazar al anterior
                        if (animationID == animCMP.animationsIDs.back())
                        {
                            animationsMap.erase(static_cast<unsigned int>(animCMP.currentAnimation));
                            animCMP.animationsIDs.erase(animCMP.animationsIDs.begin() + animCMP.currentAnimation--);
                        }
                        else
                        {
                            // Borrar para que siga y se vuelva a hacer la misma id ya que se ha desplazado
                            animationsMap.erase(static_cast<unsigned int>(animCMP.currentAnimation));
                            animCMP.animationsIDs.erase(animCMP.animationsIDs.begin() + animCMP.currentAnimation);
                        }
                    }
                }
                else
                {
                    // Comprobar si queremos eliminar la animacion de en medio
                    if (animation->remove)
                    {
                        animationsMap.erase(static_cast<unsigned int>(animCMP.currentAnimation));
                        // Borrar para que siga y se vuelva a hacer la misma id ya que se ha desplazado
                        animCMP.animationsIDs.erase(animCMP.animationsIDs.begin() + animCMP.currentAnimation);
                    }
                    else
                    {
                        // Pasar a la siguiente animacion
                        if (not animCMP.inverseOrder)
                            animCMP.currentAnimation++;
                        else
                            animCMP.currentAnimation--;
                    }
                }
            }
        }
    }
}
