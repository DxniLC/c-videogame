
#include "entity.hpp"

#include <game/component/autodestroy.hpp>

template <typename GameCTX>
void EntityManagerSystem<GameCTX>::update(GameCTX &g)
{
    for (auto &destroyCMP : g.template getComponents<AutoDestroyComponent>())
    {
        if (destroyCMP.init)
        {
            if (destroyCMP.mStartTime == AutoDestroyComponent::TimePoint{})
                destroyCMP.mStartTime = std::chrono::system_clock::now();
            else
            {
                std::chrono::duration<float> mDeltaTime = std::chrono::system_clock::now() - destroyCMP.mStartTime;

                if (mDeltaTime.count() >= destroyCMP.Time4Destruction)
                {
                    g.markEntityToRemove(destroyCMP.getEntityKey());
                }
            }
        }
        else if (destroyCMP.range != 0.0f)
        {
            auto *entity = g.getEntityByKey(destroyCMP.getEntityKey());
            auto *render = entity->template getComponent<RenderComponent>();

            if (render)
            {
                auto distance = destroyCMP.initPoint - render->node->globalPosition;

                if (glm::length(distance) >= destroyCMP.range)
                {
                    g.markEntityToRemove(destroyCMP.getEntityKey());
                }
            }
        }
    }

    // Update de eliminacion y creación entidades

    g.update();
}