#pragma once

#include <ecs/util/typealiases.hpp>

#include "component.hpp"

#include <unordered_map>

#include "EntityTypes.hpp"

namespace ECS
{
    struct EntityManager;
    struct Entity
    {
        friend EntityManager;

        explicit Entity() = default;

        template <typename CMP_Type>
        void addComponent(ComponentKey c_key, CMP_Type &c)
        {
            auto type = c.getComponentTypeID();
            components[type] = std::make_pair(c_key, &c);
        }

        template <typename CMP_Type>
        CMP_Type *getComponent()
        {
            auto type = CMP_Type::getComponentTypeID();
            auto itr = components.find(type);
            if (itr != components.end())
            {
                return dynamic_cast<CMP_Type *>(itr->second.second);
            }
            return nullptr;
        }

        template <typename CMP_Type>
        const CMP_Type *getComponent() const
        {
            auto p = const_cast<Entity *>(this)->getComponent<CMP_Type>();
            return const_cast<const CMP_Type *>(p);
        }


        EntityType type{EntityType::NONE};

        constexpr EntityKey getEntityKey() const noexcept { return key; }

    private:
        EntityKey key{};
        constexpr void setEntityKey(EntityKey key) noexcept { this->key = key; }
        void updateComponent(ComponentTypeID c_type, Component *cmp_ptr)
        {
            auto itr = components.find(c_type);
            if (itr != components.end())
                itr->second.second = cmp_ptr;
        }
        std::unordered_map<ComponentTypeID, std::pair<ComponentKey, Component *>> components;
    };

} // namespace ECS
