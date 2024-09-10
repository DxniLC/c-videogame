
#pragma once

#include <unordered_map>

#include <ecs/component/EntityTypes.hpp>
#include <ecs/component/entity.hpp>
#include <vector>
#include <ecs/util/typealiases.hpp>
#include <engines/graphic/SGEngine/scene/TNode.hpp>
#include <engines/graphic/resource/ResourceShader.hpp>
#include <engines/graphic/SGEngine/model/Line.hpp>

#include <util/GameTimer.hpp>

#include <engines/device.hpp>

namespace ECS
{
    struct EntityManager;
} // namespace ECS
struct ImGuiContext;
struct DebugSystem
{
    explicit DebugSystem(SGDevice *device);

    static void start();

    static void close(bool hideMouse = true);

    [[nodiscard]] static bool isRun();

    void update(ECS::EntityManager &entityManager) noexcept;

    // void drawDebug(ECS::EntityManager &entityManager) noexcept;
    bool profilingSystems{false};

private:
    inline static SGEngine *engine{nullptr};
    inline static SGDevice *device{nullptr};

    inline static ImGuiContext *context{nullptr};
    inline static bool isRunning{false};
    inline static const char *glsl_version{"#version 130"};

    std::size_t indice{0};
    bool checkCmp{true};
    bool positionWindow{false};
    bool forcesWindow{false};
    bool colisionWindow{false};
    bool transformWindow{false};
    bool finalize{false};
    bool cameraWindow{false};
    bool entidad{false};
    bool inputWindow{false};
    bool raycastWindow{false};
    bool renderWindow{false};
    bool lightsWindow{false};

    int numberOfLight{0};
    bool nodeLight{false};

    // EntityTag tagselected{PLAYER};
    int tagselected{0};

    float modelScale[3]{1, 1, 1};
    float modelRotate[3]{0, 0, 0};
    float modelTranslate[3]{0, 0, 0};

    std::vector<EntityKey> entitiesSelected;
    std::vector<EntityType> typesSelected;

    std::pair<std::size_t, EntityKey> currentEntitySelected;

    std::vector<TNode *> nodesToWindow;

    std::unordered_map<unsigned int, EntityType> tiposMap{};

    bool boundBoxWorld{false};
    bool boundBoxEntity{false};
    bool boundFrontBox{false};
    bool boundPolygonWorld{false};

    bool drawingBoxWorld{false};
    bool drawingBoxEntity{false};
    bool drawingFrontBox{false};
    bool drawingPolygonWorld{false};

    bool raycast{false};
    bool drawingRay{false};

    bool rayDirection{false};
    bool drawingDirection{false};
    std::pair<TNode *, Line> directionEntity{};

    bool boxTile{false};
    bool drawingTile{false};

    inline static bool FrustumCulling{false};
    inline static bool TilesOptimization{false};

    bool CinematicBar{false};
    bool ColorFilter{false};

    ResourceShader *debugShader{nullptr};
    void drawCollisionBoxes(ECS::EntityManager &entityManager);
    void drawRaycast(ECS::EntityManager &entityManager);
    std::vector<Line> rays_to_render;
    void drawDirection();
    void drawTiles();
    std::vector<Line> tiles_to_render;

    // EntityKey entity_index;
    bool addNodeWindow(TNode *node);

    void drawNodeWindow(TNode *node);
    void drawWindows(ECS::EntityManager &entityManager) noexcept;

    int entityTypeSelected{-1};
    int entityTypeLastSelected{entityTypeSelected};

    const char *entityTypeList[14]{
        "NONE",
        "PLAYER",
        "BOX",
        "WORLD",
        "CAMERA",
        "PPLATE",
        "PLATFORM",
        "CONSUMABLE",
        "CHECKPOINT",
        "FALL_VOID",
        "SPIKES",
        "PROJECTILE",
        "CLIMBING_WALL",
        "LOAD_LEVEL"};

    const char *tiposDeLuz[3]{
        "DIRECTIONAL LIGHT",
        "POINT LIGHT",
        "SPOT LIGHT"};
};

// TODO:
/*

    Implementar una ventana debug que contenga informacion general como mostrar todas las bounding box, mostrar los raycast, dejar de dibjar la maya y dibujar el wireframe.
    En esta ventana se mostrarÃ¡ los FPS actuales y a poder ser el tiempo de cada loop y una opcion para calcular tiempos de cada sistema y mostrarlos por el debug.

    Si queremos acceder a una entidad en especial, mostrar una lista de tipos de entidad y a la lista seleccionada, recorrer las entidades de ese tipo (una vez solo)

    Posibilidad de habilitar las bounding box a entidades por separado
    En esto incluye que si marco una esta se mantenga hasta que desactive todas las activas o hasta que vaya a desactivarla

    Lo mismo con el raycast, poder mostrar todos a la vez o por separado con la posibilidad de que se dibuje las que yo quiera y con un boton para quitar todas

    Para cada entidad seleccionada, poder ver sus componentes y acceder a una ventana con los datos del componente. POr ejemplo si estoy seleccionando la entidad Enemigo, ID 2,
    me salga una lista de los componentes que tiene y poder abrir una ventana de cada componente y ver los datos.

*/