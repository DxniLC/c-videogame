
#include "SGEngine.hpp"

#include <GLAD/src/glad.h>

#include <GLFW/glfw3.h>

// IMGUI
#include <ImGui/src/imgui_impl_glfw.h>
#include <ImGui/src/imgui_impl_opengl3.h>
#include <ImGui/src/imgui.h>

#include <fstream>
#include <math.h>

#include <glm/gtx/extended_min_max.hpp>

#include "scene/entity/TMesh.hpp"
#include "scene/entity/TSceneMesh.hpp"
#include "scene/entity/TCamera.hpp"
#include "scene/entity/TBoundingBox.hpp"
#include "scene/entity/light/TLight.hpp"
#include "scene/entity/TAnimatedMesh.hpp"
#include "scene/entity/TWindEffect.hpp"

#include <engines/graphic/resource/ResourceModel.hpp>

#include "optimizations/Frustum/Volumes/Sphere.hpp"
#include "optimizations/Frustum/Volumes/AABBNode.hpp"

#include <util/GameTimer.hpp>

#include "optimizations/Tiles/Tile.tpp"
#include "optimizations/Tiles/TileManager.tpp"

SGEngine::SGEngine(int width, int height)
{
    m_width = width;
    m_height = height;

    aspectRatioScreen = (float)width / (float)height;

    TResource::setResourceManager(resourceManager);
    shader = resourceManager.getShader("media/shaders/VertexShader.vert", "media/shaders/Toon/Toon.frag");
    lightShaders.emplace_back(shader);

    debug = resourceManager.getShader("media/shaders/BoundingBox/VertexShader.vert", "media/shaders/BoundingBox/FragmentShader.frag");
    skybox = resourceManager.getShader("media/shaders/SkyBox/VertexShader.vert", "media/shaders/SkyBox/FragmentShader.frag");
    LightMapShader = resourceManager.getShader("media/shaders/VertexShader.vert", "media/shaders/FragmenLightMap.frag");
    LightShader = resourceManager.getShader("media/shaders/VertexShader.vert", "media/shaders/FragmentLight.frag");

    SkeletalShader = resourceManager.getShader("media/shaders/SkeletalShader.vert", "media/shaders/Toon/Toon.frag");
    lightShaders.emplace_back(SkeletalShader);

    HUDImageShader = resourceManager.getShader("media/shaders/VertexHUD.vert", "media/shaders/FragmentHUD.frag");
    HUDTextShader = resourceManager.getShader("media/shaders/FontText.vert", "media/shaders/FontText.frag");

    InstancingShader = resourceManager.getShader("media/shaders/InstancingShader.vert", "media/shaders/Toon/Toon_Outline.frag");
    lightShaders.emplace_back(InstancingShader);

    BillboardShader = resourceManager.getShader("media/shaders/Billboard.vert", "media/shaders/FragmentHUD.frag");

    ParticleShader = resourceManager.getShader("media/shaders/Particle.vert", "media/shaders/FragmentHUD.frag");

    WindShader = resourceManager.getShader("media/shaders/Vegetation.vert", "media/shaders/Toon/Toon.frag");

    WindShaderInstance = resourceManager.getShader("media/shaders/VegetationInstance.vert", "media/shaders/Toon/Toon.frag");

    ResourceFont *DefaultFont = resourceManager.getFont("media/fonts/default/ProggyClean.ttf");

    hudEngine = std::make_unique<HUDEngine>(m_width, m_height, HUDImageShader, HUDTextShader, DefaultFont, &resourceManager);

    initScene();

    glfwSetWindowSizeCallback(m_window, window_resize_callback);
}

void SGEngine::initScene()
{
    if (not scene)
        scene = std::make_unique<TNode>();
}

void SGEngine::clearScene()
{
    scene = nullptr;
    cameraActived.node = nullptr;
    cameraActived.camera = nullptr;
    nodeCameraActivedID = 0;

    cameraRegister.clear();
    lightsRegister.clear();
    hudEngine->clearAll();
    FadeOutRect = nullptr;

    filters = 0;

    tilesManager.clearTiles();
    currentTiles.clear();
}

void SGEngine::drop()
{
    clearScene();
    dropIMGUI();
}

void SGEngine::beginScene()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    // inicialize camera
    if (cameraActived.node && cameraActived.camera)
    {
        cameraActived.node->getAccumulatedTransform();
        for (auto &map : resourceManager.getShaders())
        {
            auto *shader = dynamic_cast<ResourceShader *>(map.second.get());
            shader->use();
            shader->setMat4("projection", cameraActived.camera->GetProjectionMatrix(aspectRatioScreen));
            shader->setMat4("view", cameraActived.camera->GetViewMatrix());
            shader->setVec3("viewDirection", cameraActived.camera->Front);
            shader->setVec3("viewPos", cameraActived.camera->Position);
        }
        if (filters & SG_TILE_OPTIMIZATION)
            currentTiles = tilesManager.getCurrentTiles(glm::vec2{cameraActived.camera->Position.x, cameraActived.camera->Position.z});
    }
}

void SGEngine::drawAll()
{

    // auto timedCall = [](std::string_view name, auto &&func)
    // {
    //     GameTimer timer;
    //     func();
    //     std::cout << " [" << name << "-" << timer.ellapsedDuration<ChronoTime::Milliseconds>() << "]";
    //     // std::cout << timer.ellapsed() << ";";
    // };

    // ###################################
    // inicialize graphic library
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    // ###################################

    // ###################################
    // inicialize lights
    HasSun = false;

    for (auto &light : lightsRegister)
    {
        if (light.params->actived)
        {
            // Si es luz Sol( Igual para todos )
            if (light.params->type == LightType::DIRECTIONAL_LIGHT)
            {
                HasSun = true;
                for (auto &shader : lightShaders)
                {
                    shader->use();
                    shader->setVec3("dirLight.direction", light.params->direction);
                    shader->setVec3("dirLight.ambient", light.params->ambient);
                    shader->setVec3("dirLight.diffuse", light.params->diffuse);
                    shader->setVec3("dirLight.specular", light.params->specular);
                    shader->setBool("dirLight.Lighted", true);
                }
            }
            else
            {
                auto *sceneMesh = scene->getEntity<TSceneMesh>();
                for (auto &model : sceneMesh->models)
                    for (auto &mesh : model->meshes)
                        mesh->addLightToRender(*light.params);

                // Check Nodes on Range
                for (auto const &node : scene->getChilds())
                {
                    updateLightRecursive(node.get(), light);
                }
            }
        }
    }

    if (not HasSun)
    {
        for (auto &shader : lightShaders)
        {
            shader->use();
            shader->setBool("dirLight.Lighted", false);
        }
    }

    // ###################################

    // calculate deltaTime for animation
    float currentFrame = float(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (HasWind)
        updateWind(deltaTime);

    if (filters & OptimizationFlags)
    {
        // timedCall("OPTIMIZATIONS", [&]()
        //           { updateWithOptimizations(); });
        updateWithOptimizations();
    }
    else
    {
        // timedCall("NO OPTIMIZATIONS", [&]()
        //           { scene->update(deltaTime); });
        scene->update(deltaTime);
    }
    // std::cout << "\n";

    if (cameraActived.camera->skyBox)
        cameraActived.camera->skyBox->draw(cameraActived.camera->GetViewMatrix());

    drawBillboard(deltaTime);

    drawUI();
}

void SGEngine::updateWithOptimizations()
{
    // INIT VALUES
    auto *sceneMesh = scene->getEntity<TSceneMesh>();

    Frustum camFrustum;

    if (filters & SG_FRUSTUM_CULLING)
        camFrustum = createFrustumFromCamera();

    // ###########################################
    // OPTIMIZATIONS
    // PARA ELEMENTOS ESTATICOS
    if (filters & SG_TILE_OPTIMIZATION && not currentTiles.empty())
    {

        for (auto &tile : currentTiles)
        {
            // Instancias del Tile Actual
            for (auto &pairElements : tile->getInstanceElements())
            {
                auto &model = pairElements.first;
                for (auto &mesh : model->meshes)
                {
                    std::vector<glm::mat4> instancesToRender;
                    for (auto &transform : mesh->Tranforms4Instancing)
                    {
                        for (auto &tileTransform : pairElements.second)
                        {
                            if (tileTransform[3] == transform[3])
                            {
                                // Si hay frustum, calculamos las transformaciones que esten en el frustum
                                if (filters & SG_FRUSTUM_CULLING)
                                {
                                    glm::vec3 finalDimension = glm::vec3(
                                                                   glm::length(glm::vec3(transform[0])),
                                                                   glm::length(glm::vec3(transform[1])),
                                                                   glm::length(glm::vec3(transform[2]))) *
                                                               mesh->getDimension();
                                    if (IsElementOnFrustumCamera(camFrustum, transform[3], finalDimension, glm::vec3(transform[2])))
                                        instancesToRender.emplace_back(transform);
                                }
                                else
                                {
                                    instancesToRender.emplace_back(transform);
                                }
                            }
                        }
                    }
                    if (not instancesToRender.empty())
                    {
                        mesh->updateInstances(instancesToRender);
                        model->draw(glm::mat4(1.0f));
                    }
                }
            }

            // Elementos por tiles actuales
            for (auto &pairElement : tile->getElements())
            {
                auto &sceneShader = *pairElement.first->getShader();
                sceneShader.use();
                auto &matrix = scene->matrixTransformGlobal;
                auto *mesh = pairElement.second;
                if (filters & SG_FRUSTUM_CULLING)
                {
                    if (IsElementOnFrustumCamera(camFrustum, mesh->getPosition(), mesh->getDimension(), {0, 0, 1}))
                        mesh->draw(sceneShader, matrix);
                }
                else
                {
                    mesh->draw(sceneShader, matrix);
                }
            }
        }
    }
    // PARA ELEMENTOS ESTATICOS
    else if (filters & SG_FRUSTUM_CULLING)
    {
        Frustum camFrustum = createFrustumFromCamera();
        if (sceneMesh)
        {
            for (auto &model : sceneMesh->models)
            {
                auto &sceneShader = *model->getShader();
                auto &matrix = scene->matrixTransformGlobal;
                if (model->IsInstance())
                {
                    for (auto &mesh : model->meshes)
                    {
                        std::vector<glm::mat4> instancesToRender;
                        for (auto &transform : mesh->Tranforms4Instancing)
                        {
                            glm::vec3 finalDimension = glm::vec3(
                                                           glm::length(glm::vec3(transform[0])),
                                                           glm::length(glm::vec3(transform[1])),
                                                           glm::length(glm::vec3(transform[2]))) *
                                                       mesh->getDimension();
                            if (IsElementOnFrustumCamera(camFrustum, transform[3], finalDimension, glm::vec3(transform[2])))
                            {
                                instancesToRender.emplace_back(transform);
                            }
                        }
                        if (not instancesToRender.empty())
                        {
                            mesh->updateInstances(instancesToRender);
                            model->draw(glm::mat4(1.0f));
                        }
                    }
                }
                else
                {
                    sceneShader.use();
                    for (auto &mesh : model->meshes)
                    {
                        if (IsElementOnFrustumCamera(camFrustum, mesh->getPosition(), mesh->getDimension(), {0, 0, -1}))
                            mesh->draw(sceneShader, matrix);
                    }
                }
            }
        }
    }

    // ###########################################

    // // Recorremos los nodos hijos los cuales pueden ser no estaticos
    for (auto &child : scene->children)
    {
        // El nodo sigue vivo
        if (child->alive())
        {
            /* if (filters & SG_TILE_OPTIMIZATION)
            {
                for (auto &tile : currentTiles)
                {
                    if (tile->IsElementOnTile(child->globalPosition))
                        child->update(deltaTime);
                }
            }
            else  */
            if (filters & SG_FRUSTUM_CULLING)
            {
                if (IsElementOnFrustumCamera(camFrustum, child->globalPosition, child->dimensionNode, child->direction))
                    child->update(deltaTime);
            }
            else
            {
                child->update(deltaTime);
            }
        }
    }
}

void SGEngine::endScene()
{

    if (filters & SG_CINEMATIC_BARS)
    {

        // SCISSOR EFFECT
        glEnable(GL_SCISSOR_TEST);

        glScissor(0, 0, m_width, GLsizei(float(m_height) * CinematicBarsHeight));
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glScissor(0, GLint(float(m_height) * (1 - CinematicBarsHeight)), m_width, GLsizei(float(m_height) * CinematicBarsHeight));
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glDisable(GL_SCISSOR_TEST);
    }

    glfwSwapBuffers(this->m_window);
    glfwPollEvents();
}

void SGEngine::update(float deltaTime)
{
    if (scene)
        scene->updateTransforms(glm::mat4(1.0f), deltaTime);
}

void SGEngine::drawUI()
{

    if (FadeOutRect)
    {
        elapsedTimeFade += deltaTime;
        float factor = glm::clamp(elapsedTimeFade / fadeDuration, 0.0f, 1.0f);
        if (fadeEffect)
            factor = 1.0f - factor;

        FadeOutRect->color = glm::vec4(colorFade, factor);
        if (factor <= 0.0f && fadeEffect)
        {
            elapsedTimeFade = 0.0f;
            FadeOutRect->clean();
            FadeOutRect = nullptr;
            fadeEffect = false;
        }
    }

    if (hudEngine->IsRunning)
        hudEngine->drawUI();
}

TNode *SGEngine::createNode(TNode *parent, Transform transform)
{
    TNode *node{nullptr};
    if (!parent)
        parent = scene.get();
    node = &parent->addChild();
    node->localTransform = transform;
    node->localTransform.need_update = true;
    return node;
}

TNode *SGEngine::createCube(glm::vec3 size)
{
    TNode *node = createNode();
    ResourceModel *model = resourceManager.getCubeResource(size);
    model->setShader(shader);
    std::unique_ptr<TEntity> entity = std::make_unique<TMesh>(model);
    node->setEntity(entity);
    node->originalDimension = size;

    return node;
}

TNode *SGEngine::createCamera(TNode *parent, Transform transform, CameraParams params)
{
    TNode *node = createNode(parent, transform);
    std::unique_ptr<TEntity> entity = std::make_unique<TCamera>(params);
    node->setEntity(entity);
    registerCamera(node);
    if (!cameraActived.camera)
        setCameraActive(node->getID());
    return node;
}

TNode *SGEngine::createLight(TNode *parent, Transform transform, LightParams params, const char *filename)
{
    TNode *node = createNode(parent, transform);
    ResourceModel *model{nullptr};
    if (filename)
        model = resourceManager.getModel(filename);

    std::unique_ptr<TEntity> entity = std::make_unique<TLight>(params, LightShader, model);
    node->setEntity(entity);

    registerLight(node);

    return node;
}

void SGEngine::registerLight(TNode *lightNode)
{
    auto *light = lightNode->getEntity<TLight>();
    LightScene lightScene{lightNode, &light->params};
    lightsRegister.emplace_back(lightScene);
}

void SGEngine::updateLightRecursive(TNode *node, LightScene &light)
{
    auto *mesh = node->getEntity<TMesh>();
    auto *animatedMesh = node->getEntity<TAnimatedMesh>();
    if (node->getID() != light.node->getID() && (mesh || animatedMesh))
    {
        light.params->position = light.node->globalPosition;
        // Comprobar si el nodo esta en rango de la luz
        // SGFUNC SPHERE COLISION (CENTER_A, RADIUS_A, CENTER_B, RADIUS_B)
        auto AB_Distance = glm::length(light.node->globalPosition - node->globalPosition);
        float rangeSum = light.params->RangeLight + std::max({node->dimensionNode.x, node->dimensionNode.y, node->dimensionNode.z});
        if (rangeSum > AB_Distance)
        {
            if (mesh)
                mesh->mesh->addLightToRender(*light.params);
            else
                animatedMesh->mesh->addLightToRender(*light.params);
        }
    }

    for (auto const &child : node->getChilds())
    {
        updateLightRecursive(child.get(), light);
    }
}

TNode *SGEngine::createModel(const char *path, TNode *parent, Transform transform, filter_type SGflags)
{
    return createModel(path, shader, parent, transform, SGflags);
}

TNode *SGEngine::createModel(const char *path, ResourceShader *customShader, TNode *parent, Transform transform, filter_type SGflags)
{
    TNode *node = createNode(parent, transform);
    ResourceModel *model = resourceManager.getModel(path);
    if (SGflags & SG_VEGETATION)
        model->setShader(WindShader);
    else
        model->setShader(customShader);
    assert(model);
    std::unique_ptr<TEntity> entity = std::make_unique<TMesh>(model);
    node->setEntity(entity);
    node->originalDimension = model->dimension;

    return node;
}

TNode *SGEngine::createAnimatedModel(const char *path, TNode *parent, Transform transform)
{
    return createAnimatedModel(path, SkeletalShader, parent, transform);
}

TNode *SGEngine::createAnimatedModel(const char *path, ResourceShader *customShader, TNode *parent, Transform transform)
{
    TNode *node = createNode(parent, transform);
    ResourceModel *model = resourceManager.getModel(path);
    model->setShader(customShader);
    assert(model);
    std::unique_ptr<TEntity> entity = std::make_unique<TAnimatedMesh>(model);
    node->setEntity(entity);
    node->originalDimension = model->dimension;

    return node;
}

TNode *SGEngine::createBoundingBox(TNode *parent, Transform transform, glm::vec3 size)
{
    TNode *node = createNode(parent, transform);

    glm::vec3 sizeResult{size};
    if (size.x == 0.0f && size.y == 0.0f && size.z == 0.0f)
    {

        if (parent)
        {
            auto *modelMesh = parent->getEntity<TMesh>();
            if (modelMesh)
            {
                // auto parentScale = parent->localTransform.scale;
                sizeResult = modelMesh->mesh->dimension;
            }
        }
    }
    else if (size.x != 0.0f && size.y != 0.0f && size.z != 0.0f)
    {
        sizeResult = size;
    }
    else
    {
        return nullptr;
    }
    std::unique_ptr<TEntity> entity = std::make_unique<TBoundingBox>(sizeResult, debug);

    node->setEntity(entity);
    node->originalDimension = sizeResult;

    return node;
}

TNode *SGEngine::createBoundingBox(glm::vec3 size)
{
    return createBoundingBox(nullptr, Transform{}, size);
}

TBillboard *SGEngine::createBillboard(const char *path, glm::vec3 position, glm::vec2 size, glm::vec3 locked_axis)
{
    ResourceTexture *image = resourceManager.getTexture(path);
    assert(image);
    auto &currentBoard = billboard_List.emplace_back(std::make_unique<TBillboard>(image, position, size, locked_axis, BillboardShader));

    return dynamic_cast<TBillboard *>(currentBoard.get());
}

TNode *SGEngine::createParticleGenerator(std::size_t amount, glm::vec2 sizeParticles, ResourceTexture *textureBase, TNode *parent, Transform transform, int numberSameTime)
{
    TNode *node = createNode(parent, transform);

    std::unique_ptr<TEntity> entity = std::make_unique<ParticleGenerator>(amount, sizeParticles, ParticleShader, textureBase, numberSameTime);
    node->setEntity(entity);

    return node;
}

void SGEngine::setWind(glm::vec2 direction, float force, bool status)
{
    HasWind = status;
    WindDirection = direction;
    WindForce = force;
}

void SGEngine::updateWind(float const &deltaTime)
{
    WindTime += deltaTime;

    if (WindTime > LoopAngle)
        WindTime -= float(LoopAngle);

    WindShader->use();
    WindShader->setVec2("WindDirection", WindDirection);
    WindShader->setFloat("CurrentWindTime", WindTime);
    WindShader->setFloat("WindForce", WindForce);
    WindShaderInstance->use();
    WindShaderInstance->setVec2("WindDirection", WindDirection);
    WindShaderInstance->setFloat("CurrentWindTime", WindTime);
    WindShaderInstance->setFloat("WindForce", WindForce);
}

TNode *SGEngine::addModel4Scene(const char *path, ResourceShader *customShader, filter_type SGflags)
{

    if (not scene->entity.get())
    {
        std::unique_ptr<TEntity> entity = std::make_unique<TSceneMesh>();
        scene->setEntity(entity);
    }

    ResourceModel *modelResult{nullptr};

    modelResult = resourceManager.getModel(path);

    if (SGflags & SG_VEGETATION)
        modelResult->setShader(WindShader);
    else
    {
        ResourceShader *resultShader = (customShader) ? customShader : shader;
        modelResult->setShader(resultShader);
    }
    auto *entity = scene->getEntity<TSceneMesh>();
    entity->addModel(modelResult);

    return scene.get();
}

ResourceTexture *SGEngine::getTexture(const char *path)
{
    return resourceManager.getTexture(path);
}

ResourceAnimation *SGEngine::getAnimation(const char *path, TAnimatedMesh *entityAnimated)
{
    return resourceManager.getAnimation(path, entityAnimated->mesh);
}

ResourceShader *SGEngine::getShader(const char *vertexPath, const char *fragmentPath, bool IsLightShader)
{
    ResourceShader *shaderResult{resourceManager.getShader(vertexPath, fragmentPath)};
    if (IsLightShader)
    {
        auto itr = std::find_if(lightShaders.begin(), lightShaders.end(), [shaderResult](const ResourceShader *element)
                                { return element == shaderResult; });

        if (itr == lightShaders.end())
            lightShaders.emplace_back(shaderResult);
    }
    else
    {
        auto itr = std::find_if(lightShaders.begin(), lightShaders.end(), [shaderResult](const ResourceShader *element)
                                { return element == shaderResult; });

        if (itr != lightShaders.end())
            lightShaders.erase(itr);
    }
    return shaderResult;
}

void SGEngine::clearResource(const char *path)
{
    resourceManager.clearResource(path);
}

void SGEngine::setTexture(const char *path, TNode *node)
{
    assert(node);
    auto *mesh = node->getEntity<TMesh>();
    ResourceTexture *texture = resourceManager.getTexture(path);
    assert(texture);
    mesh->setTexture(texture);
}

void SGEngine::setSkyBox(TNode *cameraParent, std::vector<std::string> const &faces)
{
    ResourceTexture *skyboxTexture = resourceManager.getSkyBoxTexture(faces);

    auto *camera = cameraParent->getEntity<TCamera>();

    camera->skyBox = std::make_unique<SkyBox>(skyboxTexture, skybox);
}

void SGEngine::setCameraActive(TNode::NodeIDType CameraNodeID)
{
    auto itr = cameraRegister.find(CameraNodeID);
    if (itr != cameraRegister.end())
    {
        nodeCameraActivedID = CameraNodeID;
        cameraActived.node = (itr->second);
        cameraActived.camera = (itr->second)->getEntity<TCamera>();
    }
    else
        std::cout << "\n*** ERROR setCameraActive: nodeID not found on Camera Register ***\n";
}

void SGEngine::registerCamera(TNode *camera)
{
    cameraRegister.emplace(camera->getID(), camera);
}

TNode *SGEngine::addInstances4Scene(const char *path, std::vector<glm::mat4> matricesTransforms, ResourceShader *customShader, filter_type SGflags)
{
    if (not scene->entity.get())
    {
        std::unique_ptr<TEntity> entity = std::make_unique<TSceneMesh>();
        scene->setEntity(entity);
    }

    ResourceModel *modelResult{nullptr};
    modelResult = resourceManager.getInstancingModel(path, matricesTransforms);

    ResourceShader *resultShader{nullptr};
    if (SGflags & SG_VEGETATION)
        resultShader = WindShaderInstance;
    else
        resultShader = (customShader) ? customShader : InstancingShader;

    modelResult->setShader(resultShader);

    auto *entity = scene->getEntity<TSceneMesh>();
    entity->addModel(modelResult);

    return scene.get();
}

void SGEngine::drawBillboard(float const &deltaTime)
{
    BillboardShader->use();
    for (auto itr = billboard_List.begin(); itr < billboard_List.end();)
    {
        auto *entity = (TBillboard *)(itr->get());
        if (entity->remove)
        {
            itr = billboard_List.erase(itr);
        }
        else if (entity->IsActived)
        {
            entity->deltaTime = deltaTime;
            entity->update();
            itr++;
        }
        else
            itr++;
    }
}

void SGEngine::addTile(glm::vec2 position, glm::vec2 dimension)
{
    auto &tile = tilesManager.createTile(position, dimension);
    auto *sceneMesh = scene->getEntity<TSceneMesh>();

    // La malla de la escena es estatica
    for (auto &model : sceneMesh->models)
    {
        if (model->IsInstance())
        {
            for (auto &mesh : model->meshes)
            {
                for (auto &transform : mesh->Tranforms4Instancing)
                {
                    glm::vec3 currentPosition = transform[3];
                    if (tile.IsElementOnTile(currentPosition))
                    {
                        tile.addElementInstance(model, transform);
                    }
                }
            }
        }
        else
        {

            for (auto &mesh : model->meshes)
            {
                auto currentPosition = mesh->getPosition();
                if (tile.IsElementOnTile(currentPosition))
                {
                    tile.addElement(std::make_pair(model, mesh.get()));
                }
            }
        }
    }
}

void SGEngine::createTileMesh(glm::vec2 centerPos, glm::vec2 dimension, int NumTiles)
{
    tilesManager.clearTiles();

    float startX = centerPos.x - dimension.x / 2.0f;
    float startY = centerPos.y - dimension.y / 2.0f;

    glm::vec2 tileSize{
        dimension.x / float(NumTiles),
        dimension.y / float(NumTiles)};

    for (int i = 0; i < NumTiles; i++)
    {
        for (int j = 0; j < NumTiles; j++)
        {
            glm::vec2 tilePos{
                startX + float(i) * tileSize.x,
                startY + float(j) * tileSize.y};

            addTile(tilePos, tileSize);
        }
    }
}

Frustum SGEngine::createFrustumFromCamera()
{
    Frustum frustum;

    const float &ZNear = cameraActived.camera->ZNear;
    const float &ZFar = cameraActived.camera->ZFar;
    const float FovY = glm::radians(cameraActived.camera->Zoom);

    const glm::vec3 &CameraFront = cameraActived.camera->Front;
    const glm::vec3 &CameraRight = cameraActived.camera->Right;
    const glm::vec3 &CameraUp = cameraActived.camera->Up;

    const glm::vec3 &CameraPosition = cameraActived.camera->Position;

    // Calculamos el punto central del plano cercano
    const float HalfVSide = tanf(FovY * 0.5f) * ZFar;
    const float HalfHSide = HalfVSide * aspectRatioScreen;

    const glm::vec3 FarPlaneDistance = ZFar * CameraFront;

    frustum.nearFace = {CameraPosition + ZNear * CameraFront, CameraFront};
    frustum.farFace = {CameraPosition + FarPlaneDistance, -CameraFront};

    // Restamos porque queremos la direccion hacia abajo
    frustum.topFace = {CameraPosition, glm::cross(CameraRight, FarPlaneDistance - CameraUp * HalfVSide)};
    frustum.bottomFace = {CameraPosition, glm::cross(FarPlaneDistance + CameraUp * HalfVSide, CameraRight)};

    frustum.rightFace = {CameraPosition, glm::cross(FarPlaneDistance - CameraRight * HalfHSide, CameraUp)};
    frustum.leftFace = {CameraPosition, glm::cross(CameraUp, FarPlaneDistance + CameraRight * HalfHSide)};

    return frustum;
}

bool SGEngine::IsElementOnFrustumCamera(Frustum const &camFrustum, glm::vec3 const &position, glm::vec3 const &dimensions, glm::vec3 const &forwardDirection)
{
    float diffDim_XY{dimensions.x / dimensions.y};
    float diffDim_XZ{dimensions.x / dimensions.z};
    float diffDim_YZ{dimensions.y / dimensions.z};

    std::unique_ptr<Volume> VolumenCurrentElement{nullptr};

    if ((diffDim_XY > 2 || diffDim_XY < 0.5) || (diffDim_XZ > 2 || diffDim_XZ < 0.5) || (diffDim_YZ > 2 || diffDim_YZ < 0.5))
    {
        // AABB
        // Scaled orientation
        const glm::vec3 right = glm::cross(forwardDirection, glm::vec3(0, 1, 0)) * (dimensions.x / 2.0f);
        const glm::vec3 up = glm::vec3{0, 1, 0} * (dimensions.y * 2.0f);
        const glm::vec3 forward = forwardDirection * (dimensions.z / 2.0f);

        const float newX = std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, right)) +
                           std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, up)) +
                           std::abs(glm::dot(glm::vec3{1.f, 0.f, 0.f}, forward));

        const float newY = std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, right)) +
                           std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, up)) +
                           std::abs(glm::dot(glm::vec3{0.f, 1.f, 0.f}, forward));

        const float newZ = std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, right)) +
                           std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, up)) +
                           std::abs(glm::dot(glm::vec3{0.f, 0.f, 1.f}, forward));

        VolumenCurrentElement = std::make_unique<AABBNode>(position, newX, newY, newZ);
    }
    else
    {
        // SPHERE
        float range = std::max({dimensions.x, dimensions.y, dimensions.z}) * 0.5f;
        VolumenCurrentElement = std::make_unique<Sphere>(position, range);
    }
    if (VolumenCurrentElement)
    {
        return VolumenCurrentElement->isOnFrustum(camFrustum);
    }
    return false;
}

// ############################
void SGEngine::initIMGUI(bool install_callbacks)
{
    if (not ImGui::GetCurrentContext())
    {

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(m_window, install_callbacks);
        ImGui_ImplOpenGL3_Init(glsl_version);
        Callbacks_Installed = install_callbacks;
    }
    else if (Callbacks_Installed != install_callbacks)
    {
        if (install_callbacks == true)
            ImGui_ImplGlfw_InstallCallbacks(m_window);
        else
            ImGui_ImplGlfw_RestoreCallbacks(m_window);

        Callbacks_Installed = install_callbacks;
    }
}

void SGEngine::dropIMGUI()
{
    if (ImGui::GetCurrentContext())
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void SGEngine::preIMGUI()
{
    if (ImGui::GetCurrentContext())
    {
        // ImGui Pre Render
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
}

void SGEngine::postIMGUI()
{
    if (ImGui::GetCurrentContext())
    {
        // ImGui Post Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
// ############################

void SGEngine::setFade(glm::vec3 color, float duration, bool effect)
{
    elapsedTimeFade = 0.0f;
    fadeDuration = duration;
    colorFade = color;
    fadeEffect = effect;
    if (not FadeOutRect)
        FadeOutRect = hudEngine->addRect({0.5f, 0.5f}, {1.0f, 1.0f}, glm::vec4(colorFade, 0), 10);
    else
        FadeOutRect->color = glm::vec4(colorFade, 0);
}

void SGEngine::getWindowSize(int &width, int &height) noexcept
{
    width = m_width;
    height = m_height;
}

void SGEngine::window_resize_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
{
    m_width = width;
    m_height = height;

    aspectRatioScreen = (float)width / (float)height;

    hudEngine->updateWindow(m_width, m_height);

    glViewport(0, 0, m_width, m_height);
}