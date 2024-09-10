#pragma once

#include "../GraphicEngine.hpp"

#include "scene/TNode.hpp"

#include <engines/resource/TResourceManager.hpp>

#include "model/Line.hpp"

#include <memory>

#include <unordered_map>
#include <map>
#include <typeindex>

#include "scene/entity/TCamera.hpp"
#include "scene/entity/light/TLight.hpp"
#include "scene/entity/TBillboard.hpp"
#include "scene/entity/particle/ParticleGenerator.hpp"

#include "HUD/HUDEngine.hpp"

#include "optimizations/Tiles/TileManager.hpp"
#include "optimizations/Frustum/Frustum.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct TNode;
struct TAnimatedMesh;

// ############################
// Custom Flags to active custom options
using filter_type = uint8_t;
enum SGOptionsFlags : filter_type
{
    SG_NONE_FILTER = 0,
    SG_CINEMATIC_BARS = 1 << 1,
    SG_COLOR_FILTER = 1 << 2,
    SG_TILE_OPTIMIZATION = 1 << 3,
    SG_FRUSTUM_CULLING = 1 << 4,
    SG_VEGETATION = 1 << 5,
};
// ############################

// ############################
// Camera and Light Elements for managing the scene
namespace SGScene
{
    struct CameraScene
    {
        TNode *node{nullptr};
        TCamera *camera{nullptr};
    };
    struct LightScene
    {
        TNode *node;
        LightParams *params{nullptr};
    };
};
// ############################

using namespace SGScene;

struct SGEngine : GraphicEngine
{

    explicit SGEngine(int width, int height);

    SGEngine(const SGEngine &) = delete;            // constructor copia
    SGEngine(SGEngine &&) = delete;                 // constructor move
    SGEngine &operator=(const SGEngine &) = delete; // operador = cte
    SGEngine &operator=(SGEngine &&) = delete;      // operador = objeto

    void initScene() override final;
    void clearScene() override final;
    void drop() override final;
    void beginScene() override final;
    void drawAll() override final;
    void endScene() override final;

    void update(float deltaTime);

    void drawUI();

    // ############################
    // NODE MANAGER
    TNode *createNode(TNode *parent = nullptr, Transform transform = Transform());
    TNode *createCube(glm::vec3 size);
    TNode *createCamera(TNode *parent = nullptr, Transform transform = Transform(), CameraParams params = {});
    TNode *createLight(TNode *parent = nullptr, Transform transform = Transform(), LightParams params = {}, const char *filename = nullptr);

    TNode *createModel(const char *path, TNode *parent = nullptr, Transform transform = Transform(), filter_type SGflags = 0);
    TNode *createModel(const char *path, ResourceShader *customShader, TNode *parent = nullptr, Transform transform = Transform(), filter_type SGflags = 0);

    TNode *createAnimatedModel(const char *path, TNode *parent = nullptr, Transform transform = Transform());
    TNode *createAnimatedModel(const char *path, ResourceShader *customShader, TNode *parent = nullptr, Transform transform = Transform());

    TNode *createBoundingBox(TNode *parent = nullptr, Transform transform = Transform(), glm::vec3 size = glm::vec3(0.0f));
    TNode *createBoundingBox(glm::vec3 size);

    TBillboard *createBillboard(const char *path, glm::vec3 position, glm::vec2 size, glm::vec3 locked_axis);

    TNode *createParticleGenerator(std::size_t amount, glm::vec2 sizeParticles, ResourceTexture *textureBase = nullptr, TNode *parent = nullptr, Transform transform = Transform(), int numberSameTime = 1);

    void setWind(glm::vec2 direction, float force, bool status);

    TNode *addModel4Scene(const char *path, ResourceShader *shader = nullptr, filter_type SGflags = 0);

    // ############################

    // ############################
    // Get Resources
    ResourceAnimation *getAnimation(const char *path, TAnimatedMesh *entityAnimated);
    ResourceTexture *getTexture(const char *path);
    ResourceShader *getShader(const char *vertexPath, const char *fragmentPath, bool IsLightShader = false);
    // ############################

    void clearResource(const char *path);

    // ############################
    // Camera Options
    void setCameraActive(TNode::NodeIDType CameraNodeID);
    void setSkyBox(TNode *cameraParent, std::vector<std::string> const &faces);
    std::pair<TNode *, TCamera *> getCurrentCamera() const noexcept
    {
        return std::make_pair(cameraActived.node, cameraActived.camera);
    }
    // ############################

    // ############################
    // OPTIMIZATIONS
    void addTile(glm::vec2 position, glm::vec2 dimension);
    void createTileMesh(glm::vec2 centerPos = {0, 0}, glm::vec2 dimension = {30, 30}, int NumTiles = 8);
    template <typename ElementType>
    constexpr auto const &getTileManager() const noexcept { return tilesManager; }
    TNode *addInstances4Scene(const char *path, std::vector<glm::mat4> matricesTransforms, ResourceShader *customShader = nullptr, filter_type SGflags = 0);

    // ############################

    void setTexture(const char *path, TNode *node);

    // ############################
    // Manage custom options
    void enableSGOptions(filter_type sg_filters)
    {
        if (sg_filters & SG_COLOR_FILTER)
        {
            shader->use();
            shader->setBool("HasFilter", true);
            shader->setVec4("colorFilter", ColorFilter);
        }

        this->filters |= sg_filters;
    }

    void disableSGOptions(filter_type sg_filters)
    {
        if (sg_filters & SG_COLOR_FILTER)
        {
            shader->use();
            shader->setBool("HasFilter", false);
        }
        this->filters &= ~sg_filters;
    }

    [[nodiscard]] constexpr bool HasOptionActived(SGOptionsFlags sg_option) const noexcept { return (filters & sg_option); };

    // ############################

    // ############################
    // Get / Set Options

    std::vector<LightScene> &getLightsRegister() { return lightsRegister; }
    [[nodiscard]] constexpr ResourceShader *getShaderDebug() const noexcept { return debug; }
    HUDEngine *getHUDEngine() const noexcept { return hudEngine.get(); }
    void getWindowSize(int &width, int &height) noexcept;

    // ############################

    // ############################
    // DEBUG IMGUI
    void initIMGUI(bool install_callbacks = false);
    void dropIMGUI();
    void preIMGUI();
    void postIMGUI();
    // ############################

    // True = fade In //// False = fade out
    void setFade(glm::vec3 color, float duration, bool effect);

    constexpr bool IsFadeOut() const noexcept
    {
        if (FadeOutRect)
            return (FadeOutRect->color.a == 1.0f);
        return false;
    }

private:
    // Gestor de recursos
    TResourceManager resourceManager{};

    // Window Properties
    inline static float aspectRatioScreen{16.0f / 9.0f};

    // Scene
    std::unique_ptr<TNode> scene;

    void drawBillboard(float const &deltaTime);
    std::vector<std::unique_ptr<TEntity>> billboard_List;

    // HUD
    inline static std::unique_ptr<HUDEngine> hudEngine{nullptr};

    // ############################
    // Optimizations

    const filter_type OptimizationFlags{SG_TILE_OPTIMIZATION | SG_FRUSTUM_CULLING};

    TileManager<std::pair<ResourceModel *, Mesh *>> tilesManager{};

    std::vector<Tile<std::pair<ResourceModel *, Mesh *>> *> currentTiles;

    Frustum createFrustumFromCamera();
    bool IsElementOnFrustumCamera(Frustum const &camFrustum, glm::vec3 const &position, glm::vec3 const &dimensions, glm::vec3 const &forwardDirection);

    void updateWithOptimizations();

    // ############################

    // Shader actived

    // Shaders with Light
    std::vector<ResourceShader *> lightShaders;

    // ############################
    // ALL SHADERS
    ResourceShader *shader{nullptr};
    ResourceShader *debug{nullptr};
    ResourceShader *skybox{nullptr};
    ResourceShader *LightMapShader{nullptr};
    ResourceShader *LightShader{nullptr};

    ResourceShader *SkeletalShader{nullptr};

    ResourceShader *HUDImageShader{nullptr};
    ResourceShader *HUDTextShader{nullptr};

    ResourceShader *BillboardShader{nullptr};

    ResourceShader *InstancingShader{nullptr};

    ResourceShader *ParticleShader{nullptr};

    ResourceShader *WindShader{nullptr};

    ResourceShader *WindShaderInstance{nullptr};

    void updateWind(float const &deltaTime);

    // ############################

    // ############################
    // Camera
    void registerCamera(TNode *camera);

    CameraScene cameraActived;
    TNode::NodeIDType nodeCameraActivedID;
    std::unordered_map<TNode::NodeIDType, TNode *> cameraRegister;
    // ############################

    // ############################
    // Lights
    void registerLight(TNode *light);

    std::vector<LightScene> lightsRegister{};
    bool HasSun{false};

    void updateLightRecursive(TNode *node, LightScene &light);
    // ############################

    // Delta Time to Animations
    float deltaTime{0.0f};
    float lastFrame{0.0f};

    // ############################
    // UI ImGui
    const char *glsl_version{"#version 330"};
    bool Callbacks_Installed{false};
    // ############################

    // ############################
    // View Filters
    filter_type filters{SG_NONE_FILTER};
    glm::vec4 ColorFilter{1.0f, 0, 0, 1};
    // 0 - 1 to bars
    float CinematicBarsHeight{0.15f};

    glm::vec3 colorFade{0.0f};
    float fadeDuration{0.0f};
    float elapsedTimeFade{0.0f};
    HUDRect *FadeOutRect{nullptr};
    bool fadeEffect{false};

    // Wind Effect

    bool HasWind{false};
    glm::vec2 WindDirection{1, 0};
    float WindForce{0.10f};
    float WindTime{0.0f};

    const double LoopAngle{ 2 * M_PI};

    // ############################

    static void window_resize_callback(GLFWwindow *window, int width, int height);
};