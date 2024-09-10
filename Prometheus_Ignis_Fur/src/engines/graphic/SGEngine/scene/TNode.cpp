
#include "TNode.hpp"

#include <iostream>

TNode::TNode()
{
    children.reserve(8);
}

TNode::TNode(TNode *parent) : parentNode(parent)
{
    children.reserve(8);
}

TNode &TNode::addChild()
{
    return *children.emplace_back(std::make_unique<TNode>(this));
}

void TNode::removeNode()
{
    m_alive = false;
}

void TNode::updateTransforms(glm::mat4 matrixAcum, float deltaTime)
{
    this->deltaTime = deltaTime;
    if (localTransform.need_update || localTransform.interpolated_need_update)
    {
        matrixTransformGlobal = matrixAcum * localTransform.calculateTransformMatrix(deltaTime);
        entity->matrix = matrixTransformGlobal;

        globalPosition = glm::vec3(matrixTransformGlobal[3]);

        direction = glm::vec3(matrixTransformGlobal[2]);

        // Falta la escala de los padres
        if (localTransform.need_scale_update)
        {
            dimensionNode = originalDimension * localTransform.scale;
            localTransform.need_scale_update = false;
        }

        for (auto &child : children)
            child->localTransform.need_update = true;

        localTransform.need_update = false;
    }

    for (auto itr_child = children.begin(); itr_child < children.end();)
    {
        // El nodo sigue vivo
        // (*itr_child) = itr_child->get();
        if ((*itr_child)->alive())
        {
            // Vemos los flags del hijo por si no quiere alguna transformacion
            auto childFlags = (*itr_child)->flags;
            // Hacemos update de los nodos
            (*itr_child)->updateTransforms(getMatrixChildNeeded(childFlags));
            // Siguiene hijo
            ++itr_child;
        }
        else
        {
            // liberamos el nodo hijo
            itr_child->reset();
            // Lo eliminamos de los hijos y seguimos
            itr_child = children.erase(itr_child);
        }
    }
}

void TNode::update(float &deltaTime)
{
    // Hacemos update de la TEntity
    if (entity)
    {
        entity->deltaTime = deltaTime;
        if (IsActived)
            entity->update();
    }

    // Recorremos los nodos hijos
    for (auto &child : children)
    {
        // El nodo sigue vivo
        if (child->alive())
        {
            // Hacemos update de los nodos
            child->update(deltaTime);
        }
    }
}

void TNode::setGlobalPosition(glm::vec3 position)
{
    if (position != globalPosition)
    {
        TNode *node{parentNode};
        if (!node)
            node = this;

        localTransform.setPosition(position - node->globalPosition);
        globalPosition = position;
    }
}

glm::vec3 TNode::getGlobalScale() const noexcept
{
    return {glm::length(matrixTransformGlobal[0]), glm::length(matrixTransformGlobal[1]), glm::length(matrixTransformGlobal[2])};
}

void TNode::setEntity(std::unique_ptr<TEntity> &ent)
{
    assert(!entity);
    entity = std::move(ent);
}

TNode *TNode::getParent()
{
    return parentNode;
}

std::vector<std::unique_ptr<TNode>> &TNode::getChilds() noexcept
{
    return children;
}

// PRIVATE
// Get Acumulated Transform and update Camera
void TNode::getAccumulatedTransform()
{
    glm::mat4 acumTransform = glm::mat4(1.0f);
    TNode *currentNode = this;
    while (currentNode)
    {
        acumTransform = acumTransform * currentNode->localTransform.calculateTransformMatrix(deltaTime);
        currentNode = currentNode->parentNode;
    }
    localTransform.need_update = false;
    matrixTransformGlobal = acumTransform;
    globalPosition = glm::vec3(matrixTransformGlobal[3]);
    entity->matrix = matrixTransformGlobal;
    entity->update();
}

// TODO: Completar el metodo
glm::mat4 TNode::getMatrixChildNeeded(tags_type const &childFlags)
{
    glm::mat4 result(1.0f);
    if (childFlags == TransformFlags::ALL)
        return matrixTransformGlobal;
    else
    {
        if (childFlags & TransformFlags::TRANSLATE)
        {
            result = glm::translate(result, glm::vec3(matrixTransformGlobal[3]));
        }
        if (childFlags & TransformFlags::ROTATE)
        {
        }
        if (childFlags & TransformFlags::SCALE)
        {
            glm::vec3 scaling = glm::vec3(
                glm::length(glm::vec3(matrixTransformGlobal[0])),
                glm::length(glm::vec3(matrixTransformGlobal[1])),
                glm::length(glm::vec3(matrixTransformGlobal[2])));
            result = glm::scale(result, scaling);
        }
    }
    return result;
}

TNode *TNode::getChild(NodeIDType IDNode) noexcept
{
    TNode *resultNode{nullptr};
    for (auto &child : children)
    {
        if (child->nodeID == IDNode)
        {
            resultNode = child.get();
            break;
        }
    }
    return resultNode;
}