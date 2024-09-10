#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <ecs/component/component.hpp>
#include <ecs/util/typealiases.hpp>

namespace ECS
{

    struct ComponentVectorBase
    {
        virtual ~ComponentVectorBase() = default;
        virtual Component *deleteComponentByKey(ComponentKey const key) = 0;
    };

    template <typename CMP_Type>
    struct ComponentVector : ComponentVectorBase
    {
        Slotmap<CMP_Type> components{};

        Component *deleteComponentByKey(ComponentKey const key) override
        {
            auto itr = components.erase(key);
            if (itr == components.end())
                return nullptr;
            return itr.base();
        }
    };

    struct ComponentStorage
    {
        ComponentStorage() = delete; // Queremos usar solo el constructor con parametro(C++20)
        explicit ComponentStorage(std::size_t initialSize) : m_initialSize(initialSize) {}

        // eliminar los operadores de asignacion y copia para que salte error y si alguien lo intenta salte en el compilador
        ComponentStorage(const ComponentStorage &) = delete;
        ComponentStorage(ComponentStorage &&) = delete;
        ComponentStorage &operator=(const ComponentStorage &) = delete;
        ComponentStorage &operator=(ComponentStorage &&) = delete;

        template <typename CMP_Type>
        auto createComponent(EntityKey e_id)
        {
            auto &v = getComponents<CMP_Type>();
            auto result = v.emplace_back(e_id);
            result.data.setComponentKey(result.key);
            return result;
        }

        void cleanAll() noexcept
        {
            m_componentsVector.clear();
        }

        Component *deleteComponentBy_Type_And_Key(ComponentTypeID const type_id, ComponentKey const key)
        {
            auto itr = m_componentsVector.find(type_id);
            if (itr == m_componentsVector.end())
                return nullptr;
            return (itr->second)->deleteComponentByKey(key);
        }

        // ###########################################################
        // COMPONENTS VECTOR

        template <typename CMP_Type>
        Slotmap<CMP_Type> &createComponentVector()
        {
            auto typeID = CMP_Type::getComponentTypeID();
            auto v = std::make_unique<ComponentVector<CMP_Type>>();
            v->components.reserve(m_initialSize);
            auto *vecptr = v.get();
            m_componentsVector[typeID] = std::move(v);
            return vecptr->components;
        }

        template <typename CMP_Type>
        Slotmap<CMP_Type> &getComponents()
        {
            Slotmap<CMP_Type> *comvec{nullptr};
            auto typeID = CMP_Type::getComponentTypeID();
            auto itr = m_componentsVector.find(typeID);
            if (itr != m_componentsVector.end())
            {
                auto *v = dynamic_cast<ComponentVector<CMP_Type> *>(itr->second.get()); // ppuede devolver null, hay que comprobar
                comvec = &(v->components);
            }
            else
            {
                comvec = &createComponentVector<CMP_Type>();
            }
            return *comvec;
        }

        template <typename CMP_Type>
        const Slotmap<CMP_Type> &getComponents() const
        {
            Slotmap<CMP_Type> *comvec{nullptr};
            auto typeID = CMP_Type::getComponentTypeID();
            auto itr = m_componentsVector.find(typeID);
            if (itr != m_componentsVector.end())
            {
                auto *v = dynamic_cast<ComponentVector<CMP_Type> *>(itr->second.get());
                comvec = &(v->components);
            }
            else
            {
                throw "*** Error getComponents const ***";
                // comvec = &createComponentVector();
            }
            return *comvec;
        }

        // ###########################################################

        template <typename SingletonCMP, typename... Args>
        SingletonCMP &createSingletonComponent(Args &&...args)
        {
            static_assert(std::is_base_of<SingletonComponent, SingletonCMP>::value, "\n*** ERROR: Invalid Singleton ***\n");
            SingletonCMP *result{nullptr};
            auto typeID = SingletonCMP::getSingletonComponentTypeID();
            auto itr = m_singletonCmpMap.find(typeID);
            if (itr != m_singletonCmpMap.end())
            {
                result = dynamic_cast<SingletonCMP *>(itr->second.get());
            }
            else
            {
                m_singletonCmpMap.emplace(typeID, std::make_unique<SingletonCMP>(std::forward<Args>(args)...));
                result = dynamic_cast<SingletonCMP *>(m_singletonCmpMap[typeID].get());
            }
            if (result == nullptr)
            {
                std::cout << "\nERRROR NULLLLLL " << typeID << "\n";
            }
            return *result;
        }

        template <typename SingletonCMP>
        SingletonCMP *getSingletonComponent()
        {
            static_assert(std::is_base_of<SingletonComponent, SingletonCMP>::value, "\n*** ERROR: Invalid Singleton ***\n");

            SingletonCMP *result{nullptr};

            auto typeID = SingletonCMP::getSingletonComponentTypeID();

            auto itr = m_singletonCmpMap.find(typeID);
            if (itr != m_singletonCmpMap.end())
            {
                result = dynamic_cast<SingletonCMP *>(itr->second.get());
            }

            return result;
        }

    private:
        std::unordered_map<ComponentTypeID, std::unique_ptr<ComponentVectorBase>> m_componentsVector{};

        std::unordered_map<ComponentTypeID, std::unique_ptr<SingletonComponent>> m_singletonCmpMap{};

        std::size_t m_initialSize{100};
    };
} // namespace ECS