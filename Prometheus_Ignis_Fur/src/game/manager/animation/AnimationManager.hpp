#pragma once

#include <game/animations/AnimationBase.hpp>

#include <game/component/animation.hpp>

#include <unordered_map>
#include <memory>
#include <algorithm>

struct AnimationManager : ECS::SingletonComponentBase<AnimationManager>
{

    using LocalAnimationMap = std::unordered_map<AnimationBase::id_type, std::unique_ptr<AnimationBase>>;
    using MapAnimation = std::unordered_map<unsigned int, LocalAnimationMap>;
    explicit AnimationManager() = default;

    template <typename AnimType>
    [[maybe_unused]] std::pair<LocalAnimationMap::iterator, bool> addAnimation(AnimationComponent &animCMP, unsigned int animationID, AnimType &&anim)
    {
        LocalAnimationMap &localAnimations = getAnimationsByID(animCMP.animID);
        auto status = localAnimations.emplace(animationID, std::make_unique<AnimType>(std::move(anim)));
        if (status.second)
        {
            // std::cout << "meto animacion \n";

            if (animCMP.animationsIDs.empty())
                animCMP.currentAnimation = 0;
            animCMP.animationsIDs.emplace_back(animationID);
        }
        return status;
    }

    LocalAnimationMap &getAnimationsByID(unsigned int id)
    {
        auto itr = m_animations.find(id);

        if (itr != m_animations.end())
        {
            return itr->second;
        }
        else
        {
            LocalAnimationMap localMap{};
            auto itrPair = m_animations.emplace(id, std::move(localMap));
            return itrPair.first->second;
        }
    }

    AnimationBase *getAnimation(AnimationComponent &animCMP, unsigned int animationID)
    {
        LocalAnimationMap &localAnimations = getAnimationsByID(animCMP.animID);
        auto itr = localAnimations.find(animationID);
        if (itr != localAnimations.end())
        {
            return itr->second.get();
        }
        return nullptr;
    }

    MapAnimation::iterator removeAnimation(typename MapAnimation::iterator itr_anim)
    {
        return m_animations.erase(itr_anim);
    }

    void clearAnimations() noexcept { m_animations.clear(); };

private:
    inline static MapAnimation m_animations{};
};
