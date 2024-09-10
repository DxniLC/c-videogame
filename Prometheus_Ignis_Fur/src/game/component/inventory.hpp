
#pragma once

#include <ecs/component/component.hpp>

#include <game/manager/ObjectsTypes.hpp>

#include <unordered_map>

struct InventoryComponent : public ECS::ComponentBase<InventoryComponent>
{
    explicit InventoryComponent(EntityKey e_id) : ECS::ComponentBase<InventoryComponent>(e_id){};

    void addElement(ObjectType element_type, unsigned int quantity = 1)
    {
        auto itr = inventoryData.emplace(element_type, quantity);
        if (!itr.second)
            itr.first->second = itr.first->second + quantity;
    }

    bool removeElement(ObjectType element_type, unsigned int quantity = 1)
    {
        auto itr = inventoryData.find(element_type);
        if (itr != inventoryData.end())
        {
            itr->second = itr->second - quantity;
            return true;
        }
        return false;
    }

// private:
    std::unordered_map<ObjectType, unsigned int> inventoryData{};
};