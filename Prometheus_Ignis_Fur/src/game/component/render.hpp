#pragma once
#include <ecs/component/component.hpp>

#include <unordered_map>

#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/SGEngine/scene/entity/TBillboard.hpp>

struct TNode;
struct RenderComponent : public ECS::ComponentBase<RenderComponent>
{
    enum RenderType : uint8_t
    {
        CAMERA,
        PARTICLE_1,
        PARTICLE_2,
        PARTICLE_3,
        PARTICLE_4,
        PARTICLE_ENVIRONMENT,
    };

    explicit RenderComponent(EntityKey e_id) : ECS::ComponentBase<RenderComponent>(e_id){};

    void drop() override final
    {
        if (node)
        {
            node->removeNode();
        }
    }

    TNode *node{nullptr};

    TBillboard *board{nullptr};

    EntityKey camID{};

    TNode *getNodeChild(RenderType nodeType)
    {
        auto itr = renderNodeTypes.find(nodeType);
        if (itr != renderNodeTypes.end())
        {
            return node->getChild(itr->second);
        }
        return nullptr;
    }

    std::unordered_map<RenderType, TNode::NodeIDType> renderNodeTypes{};
};
