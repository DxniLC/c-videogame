#pragma once

#include "entity/TEntity.hpp"
#include <vector>
#include <memory>

#include "Transform.hpp"
struct SGEngine;
struct TNode
{
    friend SGEngine;

    using NodeIDType = unsigned int;
    using tags_type = uint8_t;
    enum TransformFlags : tags_type
    {
        TRANSLATE = 1 << 0, // 0001
        ROTATE = 1 << 1,    // 0010
        SCALE = 1 << 2,     // 0100
        ALL = (1 << 4) - 1  // 1111
    };
    explicit TNode();
    explicit TNode(TNode *parent);

    TNode(const TNode &) = delete;            // constructor copia
    TNode(TNode &&) = delete;                 // constructor move
    TNode &operator=(const TNode &) = delete; // operador = cte
    TNode &operator=(TNode &&) = delete;      // operador = objeto

    TNode &addChild(/* TransformFlags flags */);
    void removeNode();
    void setEntity(std::unique_ptr<TEntity> &ent);

    // TODO: Change this
    template <typename EntityType>
    EntityType *getEntity()
    {
        return dynamic_cast<EntityType *>(entity.get());
    }

    TNode *getParent();

    void update(float &deltaTime);
    void updateTransforms(glm::mat4 matrixAcum = glm::mat4(1.0f), float deltaTime = 1.0f);

    void setGlobalPosition(glm::vec3 position);

    glm::vec3 getGlobalScale() const noexcept;

    Transform localTransform{};
    glm::vec3 globalPosition{};
    glm::vec3 direction{0, 0, 1};
    glm::mat4 matrixTransformGlobal{glm::mat4(1.0f)};

    glm::vec3 dimensionNode{1};

    [[nodiscard]] std::vector<std::unique_ptr<TNode>> &getChilds() noexcept;
    [[nodiscard]] TNode *getChild(NodeIDType IDNode) noexcept;

    constexpr bool alive() const noexcept { return m_alive; };
    constexpr NodeIDType getID() const noexcept { return nodeID; };

    constexpr void setFlags(TransformFlags const f) noexcept { flags = f; }
    constexpr bool hasFlag(TransformFlags f) noexcept { return flags & static_cast<TransformFlags>(f); }

    inline static float deltaTime{1.0f};

    bool IsActived{true};

private:
    inline static NodeIDType nextNodeID{0};
    NodeIDType nodeID{nextNodeID++};
    bool m_alive{true};

    std::unique_ptr<TEntity> entity{nullptr};

    TNode *parentNode{nullptr};
    std::vector<std::unique_ptr<TNode>> children;

    tags_type flags{TransformFlags::ALL};
    glm::mat4 getMatrixChildNeeded(tags_type const &childFlags);

    void getAccumulatedTransform();

    glm::vec3 originalDimension{};
};