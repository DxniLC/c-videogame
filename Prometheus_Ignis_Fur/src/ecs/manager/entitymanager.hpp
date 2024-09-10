#pragma once

#include <vector>

#include <ecs/util/typealiases.hpp>
#include <ecs/manager/cmpstorage.hpp>
#include <ecs/component/entity.hpp>

namespace ECS
{
    using VecEntities = Slotmap<Entity>;

    struct EntityManager
    {
        static constexpr std::size_t maxEntities{500};
        explicit EntityManager()
        {
            m_entities.reserve(maxEntities);
        }

        EntityManager(const EntityManager &) = delete;
        EntityManager(EntityManager &&) = delete;
        EntityManager &operator=(const EntityManager &) = delete;
        EntityManager &operator=(EntityManager &&) = delete;

        Entity &createEntity()
        {
            auto result = m_entities.emplace_back();
            result.data.setEntityKey(result.key);
            return result.data;
        }

        void deleteEntityByKey(EntityKey key)
        {
            auto *entity = &m_entities.get(key);
            if (entity)
            {
                // Eliminamos componentes y reajustamos punteros
                for (auto &cmp : entity->components)
                {
                    cmp.second.second->drop();
                    auto *cmp_ptr = m_components.deleteComponentBy_Type_And_Key(cmp.first, cmp.second.first);
                    if (!cmp_ptr)
                        continue;
                    auto *movedEntity{getEntityByKey(cmp_ptr->getEntityKey())};
                    movedEntity->updateComponent(cmp.first, cmp_ptr);
                }

                // Eliminamos Entity
                m_entities.erase(key);
            }
        }

        template <typename CMP_Type>
        CMP_Type &addComponent(Entity &e)
        {
            CMP_Type *cmp_ptr{e.getComponent<CMP_Type>()};
            if (!cmp_ptr)
            {
                auto cmp = m_components.createComponent<CMP_Type>(e.getEntityKey());
                e.addComponent(cmp.key, cmp.data);
                cmp_ptr = &cmp.data;
            }
            return *cmp_ptr;
        }

        template <typename CMP_Type, typename... Args>
        CMP_Type &addSingletonComponent(Args &&...args)
        {
            CMP_Type *cmp_ptr{nullptr};

            cmp_ptr = &m_components.createSingletonComponent<CMP_Type>(args...);

            return *cmp_ptr;
        }

        template <typename CMP_Type>
        CMP_Type *getSingletonComponent()
        {
            return m_components.getSingletonComponent<CMP_Type>();
        }

        // ENTITIES

        const VecEntities &getEntities() const { return m_entities; };
        VecEntities &getEntities() { return m_entities; };

        Entity *getEntityByKey(EntityKey e_key)
        {
            Entity *entity{nullptr};

            entity = &m_entities.get(e_key);

            return entity;
        }

        // COMPONENTS

        template <typename CMP_Type>
        const Slotmap<CMP_Type> &getComponents() const
        {
            return m_components.getComponents<CMP_Type>();
        }

        template <typename CMP_Type>
        Slotmap<CMP_Type> &getComponents()
        {
            return m_components.getComponents<CMP_Type>();
        }

        void update()
        {
            destroy_marked_entities();
            incorporate_new_entities();
        }

        void markEntityToRemove(EntityKey key)
        {
            entities_to_delete.emplace_back(key);
        }

        void cleanAll() noexcept
        {
            m_entities.clear();
            m_components.cleanAll();
            new_entities.clear();
            entities_to_delete.clear();
        }

        constexpr bool empty() const noexcept { return (m_entities.size() == 0); }

    private:
        VecEntities m_entities{};
        ComponentStorage m_components{maxEntities};
        std::vector<Entity> new_entities{};
        std::vector<EntityKey> entities_to_delete{};

        void incorporate_new_entities()
        {
            for (auto &e : new_entities)
                m_entities.emplace_back(e);
            new_entities.clear();
        }

        void destroy_marked_entities()
        {
            for (auto &e_key : entities_to_delete)
                deleteEntityByKey(e_key);
            entities_to_delete.clear();
        }
    };
} // namespace ECS