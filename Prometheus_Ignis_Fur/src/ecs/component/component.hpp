#pragma once

#include <ecs/util/typealiases.hpp>

namespace ECS
{
    struct ComponentStorage;
    struct Component
    {
        friend ComponentStorage;

        explicit Component(EntityKey eID) : entityID{eID} {}

        virtual ~Component() = default;

        virtual void drop(){};

        constexpr EntityKey getEntityKey() const noexcept { return entityID; }
        constexpr ComponentKey getComponentKey() const noexcept { return componentKey; };

    protected:
        inline static ComponentTypeID nextTypeID{0};

    private:
        ComponentKey componentKey{0};
        EntityKey entityID{0};
        constexpr void setComponentKey(ComponentKey key) noexcept { this->componentKey = key; }
    };

    template <typename CMP_Type>
    struct ComponentBase : Component
    {
        explicit ComponentBase(EntityKey eID) : Component{eID} {}
        virtual ~ComponentBase() = default;
        static ComponentTypeID getComponentTypeID() noexcept
        {
            static ComponentTypeID typeID{++nextTypeID};
            return typeID;
        }
    };

    struct SingletonComponent
    {
        virtual ~SingletonComponent() = default;

    protected:
        inline static ComponentTypeID nextID{0};
    };

    template <typename CMP_Type>
    struct SingletonComponentBase : SingletonComponent
    {
        virtual ~SingletonComponentBase() = default;
        static ComponentTypeID getSingletonComponentTypeID() noexcept
        {
            static ComponentTypeID typeID{nextID++};
            return typeID;
        }
    };

} // namespace ECS
