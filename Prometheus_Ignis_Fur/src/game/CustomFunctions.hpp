#pragma once

#include <game/component/render.hpp>
#include <game/component/body/body.hpp>
#include <game/component/body/worldbody.hpp>
#include <ecs/component/entity.hpp>
#include <game/component/camera/camera.hpp>
#include <game/component/raycast/raycast.hpp>

#include <engines/graphic/SGEngine/model/Polygon.hpp>
#include <engines/graphic/SGEngine/scene/entity/TBoundingBox.hpp>

struct SGFunc
{
    SGFunc() = delete;
    SGFunc(const SGFunc &) = delete;
    SGFunc(SGFunc &&) = delete;
    SGFunc &operator=(const SGFunc &) = delete;
    SGFunc &operator=(SGFunc &&) = delete;

    // TODO: Buscar como hacerlo mas rapido o mejor programado
    inline static TNode &getMainNodeFromEntity(ECS::Entity const &entity)
    {
        TNode *node{nullptr};
        auto *render = entity.getComponent<RenderComponent>();
        if (render)
        {
            node = render->node;
        }
        else
        {
            auto *body = entity.getComponent<BodyComponent>();
            if (body)
            {
                node = body->collider->nodeBox;
            }
            else
            {
                auto *world = entity.getComponent<WorldBodyComponent>();
                if (world)
                {
                    node = world->collider->nodeBox;
                }
                else
                {
                    auto camera = entity.getComponent<CameraComponent>();
                    if (camera)
                    {
                        node = camera->cam;
                    }
                }
            }
        }

        assert(node);

        return *node;
    }

    inline static void SAT_Algorithm(const glm::vec3 &normal, const std::vector<glm::vec3> &globalVertexs, float &minAlong, float &maxAlong)
    {
        minAlong = __FLT_MAX__;
        maxAlong = -__FLT_MAX__;

        for (auto const &vertex : globalVertexs)
        {
            float projectionOnNormal = glm::dot(vertex, normal);
            if (projectionOnNormal < minAlong)
                minAlong = projectionOnNormal;
            if (projectionOnNormal > maxAlong)
                maxAlong = projectionOnNormal;
        }
    }

    inline static bool overlapsInterval(float const &a_min, float const &a_max, float const &b_min, float const &b_max)
    {
        return isBetweenInterval(b_min, a_min, a_max) || isBetweenInterval(a_min, b_min, b_max);
    }

    inline static CollisionResult AABB_Collision(glm::vec3 const &globalPosA, glm::vec3 const &sizeA, glm::vec3 const &globalPosB, glm::vec3 const &sizeB)
    {

        CollisionResult collisionpts{};

        auto X_Axis_Delta = abs(globalPosA.x - globalPosB.x);
        auto Y_Axis_Delta = abs(globalPosA.y - globalPosB.y);
        auto Z_Axis_Delta = abs(globalPosA.z - globalPosB.z);

        auto scaled_size_X = (sizeA.x / 2);
        auto scaled_size_Y = (sizeA.y / 2);
        auto scaled_size_Z = (sizeA.z / 2);

        auto box_scaled_size_X = (sizeB.x / 2);
        auto box_scaled_size_Y = (sizeB.y / 2);
        auto box_scaled_size_Z = (sizeB.z / 2);

        auto X_sum_BB = scaled_size_X + box_scaled_size_X;
        auto Y_sum_BB = scaled_size_Y + box_scaled_size_Y;
        auto Z_sum_BB = scaled_size_Z + box_scaled_size_Z;

        if (X_Axis_Delta < X_sum_BB &&
            Y_Axis_Delta < Y_sum_BB &&
            Z_Axis_Delta < Z_sum_BB)
        {

            float X_depth_coll{0};
            float Y_depth_coll{0};
            float Z_depth_coll{0};

            if (globalPosA.x > globalPosB.x)
            {
                X_depth_coll = (globalPosA.x - scaled_size_X) - (globalPosB.x + (box_scaled_size_X));
            }
            else
            {
                X_depth_coll = (globalPosA.x + scaled_size_X) - (globalPosB.x - (box_scaled_size_X));
            }

            if (globalPosA.y > globalPosB.y)
            {
                Y_depth_coll = (globalPosA.y - scaled_size_Y) - (globalPosB.y + (box_scaled_size_Y));
            }
            else
            {
                Y_depth_coll = (globalPosA.y + scaled_size_Y) - (globalPosB.y - (box_scaled_size_Y));
            }

            if (globalPosA.z > globalPosB.z)
            {
                Z_depth_coll = (globalPosA.z - scaled_size_Z) - (globalPosB.z + (box_scaled_size_Z));
            }
            else
            {
                Z_depth_coll = (globalPosA.z + scaled_size_Z) - (globalPosB.z - (box_scaled_size_Z));
            }

            if (abs(X_depth_coll) < abs(Y_depth_coll) && abs(X_depth_coll) < abs(Z_depth_coll))
            {
                collisionpts.depth = X_depth_coll;
                collisionpts.normal = glm::vec3(-1, 0, 0);
            }
            else if (abs(Y_depth_coll) < abs(X_depth_coll) && abs(Y_depth_coll) < abs(Z_depth_coll))
            {
                collisionpts.depth = Y_depth_coll;
                collisionpts.normal = glm::vec3(0, -1, 0);
            }
            else
            {
                collisionpts.depth = Z_depth_coll;
                collisionpts.normal = glm::vec3(0, 0, -1);
            }
            collisionpts.hasCollision = true;
        }
        return collisionpts;
    }

    inline static CollisionResult BB_Polygon_Collision(TNode &nodeOBB, Polygon const &polygon, float const &collisionRange)
    {

        CollisionResult cpts_result{};

        float depth = __FLT_MAX__;

        glm::vec3 normal_result{};

        auto *meshA = nodeOBB.getEntity<TBoundingBox>();
        meshA->updateValues(nodeOBB.matrixTransformGlobal);

        if (not checkCollisionRange(meshA->globalPos, collisionRange, polygon.globalCenterPos, polygon.polygonRange))
        {
            return cpts_result;
        }

        for (std::size_t i = 0; i < meshA->faces.size(); i++)
        {
            float a_Min, a_Max, b_Min, b_Max;
            SAT_Algorithm(meshA->faces[i].normal, meshA->globalVertexs, a_Min, a_Max);
            SAT_Algorithm(meshA->faces[i].normal, polygon.globalVertexs, b_Min, b_Max);
            if (not overlapsInterval(a_Min, a_Max, b_Min, b_Max))
            {
                //  NO INTERSECTION
                return cpts_result;
            }
            else
            {
                // INTERSECTION
                float current_depth = b_Max - a_Min;
                depth = std::min(depth, current_depth);
                if (depth == current_depth)
                {
                    normal_result = meshA->faces[i].normal;
                }
            }
        }

        std::vector<glm::vec3> normales;
        normales.emplace_back(polygon.normal);
        
        std::size_t j{0};
        for (std::size_t i = 0; i < polygon.globalVertexs.size(); i++)
        {
            if (i < polygon.globalVertexs.size() - 1)
            {
                j = i + 1;
            }
            else
            {
                j = 0;
            }
            glm::vec3 point2Point = polygon.globalVertexs[i] - polygon.globalVertexs[j];
            normales.emplace_back(glm::cross(point2Point, polygon.normal));
        }

        for (auto &normal : normales)
        {
            float a_Min, a_Max, b_Min, b_Max;
            SAT_Algorithm(normal, polygon.globalVertexs, a_Min, a_Max);
            SAT_Algorithm(normal, meshA->globalVertexs, b_Min, b_Max);
            if (not overlapsInterval(a_Min, a_Max, b_Min, b_Max))
            {
                //  NO INTERSECTION
                return cpts_result;
            }
            else
            {
                // INTERSECTION
                float current_depth = a_Max - b_Min;
                depth = std::min(depth, current_depth);
                if (depth == current_depth)
                {
                    normal_result = glm::vec3{polygon.normal};
                }
            }
        }

        // INTERSECTION
        cpts_result.hasCollision = true;
        cpts_result.depth = depth;
        cpts_result.normal = normal_result;

        cpts_result.polygonGlobalPos = polygon.globalCenterPos;

        return cpts_result;
    }

    inline static bool checkCollisionRange(glm::vec3 const &globalAPosition, float const &AColisionRange, glm::vec3 const &globalBPosition, float const &BColisionRange)
    {
        auto AB_Distance = glm::length(globalAPosition - globalBPosition);
        auto rangeSum = AColisionRange + BColisionRange;
        return rangeSum > AB_Distance;
    }

    inline static bool Raycast_Polygon(Raycast &ray, glm::vec3 const &raycastOrigin, glm::vec3 const &normal_plane, glm::vec3 const &center_plane, std::vector<glm::vec3> const &vertexs, std::vector<unsigned int> indexs = {})
    {

        // punto del rayo en el sistema global
        auto r_point = raycastOrigin;

        // **** Para calcular la interseccion necesitamos conocer las ecuaciones ****

        // Ecuacion del plano
        // x - 3y + 2z + d = 0
        // d = -(x - 3y + 2z);
        // 1,-3,2 = normal
        // x,y,z = punto

        // ecuacion_plano: p_normal.X * X + p_normal.Y * Y + p_normal.Z * Z + d = 0;
        // ecuacion_plano: p_normal.X * (r_point.X + r_dir.X * t) +
        // p_normal.Y * (r_point.Y + r_dir.Y * t) +
        // p_normal.Z * (r_point.Z + r_dir.Z * t) + d = 0;

        // punto generico recta: X+dirX*T  Y+dirY*T  Z+dirZ*T

        // ecuacion plano: iX + jY + kZ + d = 0

        // Buscamos T para ver el punto de colision con el plano. por tanto sustituimos en la ecuacion del plano

        // i(X+dirX*T) + j(Y+dirY*T) + k(Z+dirZ*T) + d = 0;

        // i*X + i*dirX*T + j*Y + j*dirY*T + k*Z + k*dirZ*T + d = 0;

        // T(i*dirX+j*dirY+k*dirZ) + i*X + j*Y + k*Z + d = 0;

        // T = -(i*X + j*Y + k*Z + d) / (i*dirX+j*dirY+k*dirZ);

        float den = (normal_plane.x * ray.direction.x +
                     normal_plane.y * ray.direction.y +
                     normal_plane.z * ray.direction.z);

        if (den == 0)
            return false;

        float d = -(normal_plane.x * center_plane.x + normal_plane.y * center_plane.y + normal_plane.z * center_plane.z);

        float num = -(normal_plane.x * r_point.x +
                      normal_plane.y * r_point.y +
                      normal_plane.z * r_point.z + d);

        float T = num / den;

        glm::vec3 intersection = {r_point.x + ray.direction.x * T,
                                  r_point.y + ray.direction.y * T,
                                  r_point.z + ray.direction.z * T};

        // Comprobar rango raycast:

        // intersection.round(3);

        glm::vec3 intersectionDirection = intersection - glm::vec3(r_point);

        // intersectionDirection.round(3);

        glm::vec3 signs = ray.direction * intersectionDirection;

        if (signs.x < 0 || signs.y < 0 || signs.z < 0)
            return false;

        float distance = glm::length(intersection - r_point);

        if (distance > ray.range)
            return false;

        // Comprobar si la interseccion está dentro del rango de la cara del cuadrado en las coordenadas globales

        std::vector<glm::vec2> polygonVertexs;
        glm::vec2 point;

        std::size_t maxVertexs = vertexs.size();

        if (not indexs.empty())
        {
            maxVertexs = indexs.size();
        }

        glm::vec3 auxPlane = normal_plane;

        if (auxPlane.x < 0)
            auxPlane.x = -auxPlane.x;
        if (auxPlane.y < 0)
            auxPlane.y = -auxPlane.y;
        if (auxPlane.z < 0)
            auxPlane.z = -auxPlane.z;

        for (std::size_t j = 0; j < maxVertexs; j++)
        {
            glm::vec3 vertex = vertexs[j];
            if (not indexs.empty())
            {
                vertex = vertexs[indexs[j]];
            }

            if (auxPlane.x > auxPlane.y && auxPlane.x > auxPlane.z)
            {
                polygonVertexs.emplace_back(vertex.y, vertex.z);
                point = {intersection.y, intersection.z};
            }
            else if (auxPlane.y > auxPlane.x && auxPlane.y > auxPlane.z)
            {
                polygonVertexs.emplace_back(vertex.x, vertex.z);
                point = {intersection.x, intersection.z};
            }
            else
            {
                polygonVertexs.emplace_back(vertex.x, vertex.y);
                point = {intersection.x, intersection.y};
            }
        }

        if (inside_polygon(point, polygonVertexs))
        {
            // COLISIONA
            ray.result.distance = distance;
            ray.result.intersection = intersection;
            ray.result.direction = {normal_plane};
            return true;
        }
        return false;
    }

private:
    inline static bool isBetweenInterval(float const &val, float const &lowVal, float const &highVal)
    {
        return val >= lowVal && val <= highVal;
    }

    inline static bool inside_polygon(glm::vec2 point, std::vector<glm::vec2> const &poligon_vertexs)
    {
        unsigned int countIntersections{0};
        for (std::size_t i = 0; i < poligon_vertexs.size(); i++)
        {
            auto j = i + 1;
            if (j == poligon_vertexs.size())
                j = 0;

            auto edge = poligon_vertexs[i] - poligon_vertexs[j];

            if (edge.y <= 0.001 && edge.y >= -0.001f)
                continue;

            // Comprobar que almenos una de las X de la arista este en rango de colision
            if (poligon_vertexs[i].x < point.x && poligon_vertexs[j].x < point.x)
                continue;

            // Comprobar si son paralelos

            // Ecuacion explícita recta: y = mx + n
            // m = 0
            // y = n -> punto con recta hacia derecha o izquierda

            // y = mx + n

            // ecuacion de la recta que tiene el punto de interseccion(recta hacia derecha)

            // y = n --> m = 0;

            // ecuacion de la recta actual

            // calculamos m ya que tenemos dos puntos de la recta

            // m' = y2-y1 / x2-x1

            // m' = (poligon_vertexs[j].y - poligon_vertexs[i].y) / (poligon_vertexs[j].x - poligon_vertexs[i].x);

            // n' = y' - m'x';

            // n' = poligon_vertexs[i].y - m * poligon_vertexs[i].x

            // la interseccion en la arista con la recta solo puede ser en la y de nuestro punto de interseccion. por tanto la colision con la recta de la arista es:

            // x' = - (n' - y) / m'
            float x_intersect{poligon_vertexs[i].x};
            float y_intersect{point.y};

            float num = (poligon_vertexs[j].y - poligon_vertexs[i].y);
            float den = (poligon_vertexs[j].x - poligon_vertexs[i].x);

            if (den != 0)
            {
                float m = num / den;
                float n = poligon_vertexs[i].y - m * poligon_vertexs[i].x;

                x_intersect = -(n - y_intersect) / m;
            }
            if (x_intersect > point.x)
            {
                if (y_intersect <= std::max(poligon_vertexs[i].y, poligon_vertexs[j].y) && y_intersect >= std::min(poligon_vertexs[i].y, poligon_vertexs[j].y))
                    countIntersections++;
            }
        }

        if (countIntersections % 2 == 0)
            return false;

        return true;
    }
};
