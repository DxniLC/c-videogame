#include "debug.hpp"

#include <ImGui/src/imgui_impl_glfw.h>
#include <ImGui/src/imgui_impl_opengl3.h>
#include <ImGui/src/imgui.h>

#include <ecs/manager/entitymanager.hpp>

#include <engines/graphic/SGEngine/SGEngine.hpp>
#include <engines/graphic/SGEngine/scene/entity/TBoundingBox.hpp>

#include <game/CustomFunctions.hpp>

// Components
#include <game/component/frontbox.hpp>

#include <game/component/physics.hpp>
#include <game/component/camera/camera.hpp>
#include <game/component/input/input.hpp>
#include <game/component/render.hpp>
#include <game/component/raycast/raycast.hpp>
#include <game/component/body/body.hpp>
#include <game/component/body/worldbody.hpp>
#include <game/component/body/scenebody.hpp>
#include <game/component/frontbox.hpp>

DebugSystem::DebugSystem(SGDevice *device)
{
    this->device = device;
    engine = dynamic_cast<SGEngine *>(device->getGraphicEngine());
    debugShader = engine->getShaderDebug();

    if (!debugShader)
        std::cout << "\n*** ERROR Shader: Error al construir Debug con su Shader ***\n";
    unsigned int contador{0};
    for (uint32_t i = EntityType::NONE; i < EntityType::END_ENTITY_TYPE; i++)
    {
        tiposMap.emplace(contador++, EntityType(i));
    }
}

void DebugSystem::start()
{
    // HUD initialized ImGui
    engine->initIMGUI(true);
    FrustumCulling = engine->HasOptionActived(SG_TILE_OPTIMIZATION);
    TilesOptimization = engine->HasOptionActived(SG_FRUSTUM_CULLING);
    isRunning = true;
}

void DebugSystem::close(bool hideMouse)
{
    // HUD will close ImGui
    engine->initIMGUI(false);
    if (hideMouse)
        device->getInputReceiver()->setVisible(false);

    isRunning = false;
}

bool DebugSystem::isRun()
{
    return isRunning;
}

void DebugSystem::update(ECS::EntityManager &entityManager) noexcept
{

    drawRaycast(entityManager);
    drawCollisionBoxes(entityManager);
    drawDirection();
    drawTiles();
    if (isRunning)
    {
        engine->preIMGUI();
        drawWindows(entityManager);
        engine->postIMGUI();
    }
}

void DebugSystem::drawWindows(ECS::EntityManager &entityManager) noexcept
{
    auto &io{ImGui::GetIO()};
    Slotmap<ECS::Entity> &entities = entityManager.getEntities();
    CameraComponent *cComp{nullptr};
    PhysicsComponent *pComp{nullptr};
    BodyComponent *bComp{nullptr};
    InputComponent *iComp{nullptr};
    RaycastComponent *rComp{nullptr};
    RenderComponent *reComp{nullptr};

    { // MAIN WINDOW
        ImGui::SetNextWindowPos(ImVec2(20, 20));
        ImGui::SetNextWindowSize(ImVec2(400, 300));
        ImGui::Begin("General Debug", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        ImGui::Text("FPS: %2f", io.Framerate);
        ImGui::Text("ms/f: %2f", 1000.0f / io.Framerate);

        ImGui::Text("\n----- OPTIMIZATIONS -----");
        ImGui::Checkbox("Frustum Culling", &FrustumCulling);
        if (FrustumCulling)
            engine->enableSGOptions(SG_FRUSTUM_CULLING);
        else
            engine->disableSGOptions(SG_FRUSTUM_CULLING);

        ImGui::Checkbox("Tiles Optmization", &TilesOptimization);
        if (TilesOptimization)
            engine->enableSGOptions(SG_TILE_OPTIMIZATION);
        else
            engine->disableSGOptions(SG_TILE_OPTIMIZATION);

        ImGui::Text("\n----- FILTERS -----");
        ImGui::Checkbox("Cinematic Bars", &CinematicBar);
        if (CinematicBar)
            engine->enableSGOptions(SG_CINEMATIC_BARS);
        else
            engine->disableSGOptions(SG_CINEMATIC_BARS);

        ImGui::Checkbox("Color Filter", &ColorFilter);
        if (ColorFilter)
            engine->enableSGOptions(SG_COLOR_FILTER);
        else
            engine->disableSGOptions(SG_COLOR_FILTER);

        ImGui::Text("\n----- BOUNDING BOXES -----");

        ImGui::Checkbox("WorldBounds", &boundBoxWorld);
        ImGui::SameLine();
        ImGui::Checkbox("EntityBounds", &boundBoxEntity);
        ImGui::SameLine();
        ImGui::Checkbox("FrontBox", &boundFrontBox);

        ImGui::Text("\n----- WORLD PLANES -----");
        ImGui::Checkbox("WorldPlanes", &boundPolygonWorld);

        ImGui::Text("\n----- LIGHTS WINDOW -----");

        ImGui::Text("Lights");
        ImGui::SameLine();
        if (ImGui::Button("Lights"))
        {
            numberOfLight = 0;

            lightsWindow = !lightsWindow;
        }

        ImGui::Text("\n----- ALL RAYCAST -----");
        ImGui::Checkbox("Show Rays", &raycast);

        ImGui::Text("\n----- Show Tiles -----");
        ImGui::Checkbox("Show Tiles", &boxTile);

        ImGui::Text("\n----- PROFILING SYSTEMS -----");
        ImGui::Checkbox("Profiling Systems", &profilingSystems);

        // Desplegable con los distintos tipos de entidad
        // Cuando se seleccione una, mostrar ventana de informacion sobre ese tipo de entidad(Informacion general sobre entidad)
        // Poder recorrer entidades de ese tipo.

        ImGui::Combo("\nEntity Type Filter", &entityTypeSelected, entityTypeList, IM_ARRAYSIZE(entityTypeList));

        if (entityTypeSelected != entityTypeLastSelected)
        {
            inputWindow = false;
            renderWindow = false;
            cameraWindow = false;
            forcesWindow = false;
            colisionWindow = false;
            raycastWindow = false;
            nodesToWindow.clear();

            entityTypeLastSelected = entityTypeSelected;
            auto typeSelected = tiposMap[(unsigned int)(entityTypeSelected)];
            entitiesSelected.clear();
            int convertedTypeSelected;
            if (typeSelected != 0)
            {
                convertedTypeSelected = 1 << typeSelected;
            }
            else
            {
                convertedTypeSelected = 0;
            }
            // std::cout << convertedTypeSelected << " una entidad de un tipo \n";

            {
                for (auto &e : entities)
                {
                    // std::cout << e.type << " dentro \n";

                    if (convertedTypeSelected == EntityType::NONE)
                        entitiesSelected.emplace_back(e.getEntityKey());

                    else if (convertedTypeSelected & int(e.type))
                    {
                        entitiesSelected.emplace_back(e.getEntityKey());
                    }
                }
                if (entitiesSelected.size() > 0)
                {
                    currentEntitySelected = std::make_pair(0, entitiesSelected[0]);
                    entidad = true;
                }
                else
                    entidad = false;
            }
        }

        ImGui::End();

        // LA VENTANA DE LAS ENTIDADES
        if (entidad)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 450));
            ImGui::Begin("Debug Entities");

            if (ImGui::Button("<-") && currentEntitySelected.first > 0)
            {
                inputWindow = false;
                renderWindow = false;
                cameraWindow = false;
                forcesWindow = false;
                colisionWindow = false;
                raycastWindow = false;
                nodesToWindow.clear();
                currentEntitySelected.second = entitiesSelected[--currentEntitySelected.first];
            }
            ECS::Entity &e1 = entities.get(currentEntitySelected.second);

            ImGui::SameLine();
            ImGui::Text("De una a una %i ID: ", currentEntitySelected.second.id);

            ImGui::SameLine();
            if (ImGui::Button("->") && currentEntitySelected.first < entitiesSelected.size() - 1)
            {
                inputWindow = false;
                renderWindow = false;
                cameraWindow = false;
                forcesWindow = false;
                colisionWindow = false;
                raycastWindow = false;
                nodesToWindow.clear();
                currentEntitySelected.first++;
                // std::cout << "el size es de " << entitiesSelected.size() << "  Estamos en la posicion:" << currentEntitySelected.first << "\n";
                currentEntitySelected.second = entitiesSelected[currentEntitySelected.first];
            }

            int indice = 0;

            if (e1.type != 0)
            {
                uint32_t numeroDelTipo = e1.type;
                indice = (int)log2(numeroDelTipo);
            }
            ImGui::Text("El tipo de entidad es %s ID: ", entityTypeList[indice]);

            ImGui::Text("Delete Entity: ");
            ImGui::SameLine();
            if (ImGui::Button("delete"))
            {
                inputWindow = false;
                renderWindow = false;
                cameraWindow = false;
                forcesWindow = false;
                colisionWindow = false;
                raycastWindow = false;
                entidad = false;

                entityManager.markEntityToRemove(e1.getEntityKey());
                entityTypeLastSelected = -1;
                // Seleccionar el primero disponible
            }

            ImGui::Text("Posicion de la entidad");

            TNode &nodoMain = SGFunc::getMainNodeFromEntity(e1);

            auto posicionEntidad = nodoMain.globalPosition;
            if (ImGui::InputFloat3("Transform position", &posicionEntidad.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
                nodoMain.setGlobalPosition(posicionEntidad);

            ImGui::Text("Dimension Node [%.3f, %.3f, %.3f]", nodoMain.dimensionNode.x, nodoMain.dimensionNode.y, nodoMain.dimensionNode.z);

            pComp = e1.getComponent<PhysicsComponent>();
            if (pComp)
            {
                ImGui::Text("Physic Component");
                ImGui::SameLine();
                if (ImGui::Button("Physic"))
                {
                    if (!forcesWindow)
                        forcesWindow = true;

                    else
                        forcesWindow = false;
                }
            }

            reComp = e1.getComponent<RenderComponent>();
            if (reComp)
            {
                ImGui::Text("Render Component");
                ImGui::SameLine();
                if (ImGui::Button("Render"))
                {
                    if (!renderWindow)
                        renderWindow = true;
                    else
                        renderWindow = false;
                }
            }

            bComp = e1.getComponent<BodyComponent>();
            if (bComp)
            {
                ImGui::Text("Body Component");
                ImGui::SameLine();
                if (ImGui::Button("Body"))
                {
                    if (!colisionWindow)
                        colisionWindow = true;

                    else
                        colisionWindow = false;
                }
            }

            cComp = e1.getComponent<CameraComponent>();
            if (cComp)
            {
                ImGui::Text("Camera Component");
                ImGui::SameLine();
                if (ImGui::Button("Camera"))
                {
                    if (!cameraWindow)
                        cameraWindow = true;

                    else
                        cameraWindow = false;
                }
            }

            iComp = e1.getComponent<InputComponent>();
            if (iComp)
            {
                ImGui::Text("Input Component");
                ImGui::SameLine();
                if (ImGui::Button("Input"))
                {
                    if (!inputWindow)
                        inputWindow = true;

                    else
                        inputWindow = false;
                }
            }

            rComp = e1.getComponent<RaycastComponent>();
            if (rComp)
            {
                ImGui::Text("Raycast Component");
                ImGui::SameLine();
                if (ImGui::Button("Raycast"))
                {
                    if (!raycastWindow)
                        raycastWindow = true;

                    else
                        raycastWindow = false;
                }
            }

            ImGui::End();
        }

        if (renderWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(140, 100));
            ImGui::Begin("Render Window");
            ImGui::Text("Id de la camara %i", reComp->camID.id);

            auto *node = reComp->node;
            if (node)
            {
                if (ImGui::Button("Nodo"))
                {
                    addNodeWindow(node);
                }

                ImGui::Checkbox("Show Direction", &rayDirection);
                if (rayDirection && not drawingDirection)
                {
                    directionEntity.first = node;
                    directionEntity.second = {node->globalPosition, node->direction, 5.f};
                    directionEntity.second.setup(glm::vec4(0.7, 0, 0.9, 1), 8.0f);
                    drawingDirection = true;
                }
            }

            ImGui::End();
        }

        if (raycastWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 250));
            ImGui::Begin("Raycast Window");

            for (unsigned int i = 0; i < rComp->rays.size(); i++)
            {
                ImGui::Text("Rayo: %i\n", i);

                ImGui::Text("-POSICION: ");
                float step{1};

                ImGui::InputFloat(" Posicion X", &rComp->rays[i].origin.x, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::InputFloat(" Posicion Y", &rComp->rays[i].origin.y, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::InputFloat(" Posicion Z", &rComp->rays[i].origin.z, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

                ImGui::Text("-DIRECCION: ");

                ImGui::InputFloat(" Direccion X", &rComp->rays[i].direction.x, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::InputFloat(" Direccion Y", &rComp->rays[i].direction.y, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
                ImGui::InputFloat(" Direccion Z", &rComp->rays[i].direction.z, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

                ImGui::InputFloat("\nRango", &rComp->rays[i].range, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

                if (true)
                {
                    const char *tipoFunc = "Tipo de la funcion";

                    // Este es el if en el que habria que comprobar cual es el tipo de raycast
                    ImGui::Text("El tipo de raycast es: %s", tipoFunc);
                }
            }

            ImGui::End();
        }

        if (lightsWindow)
        {

            auto registerOfLights = engine->getLightsRegister();
            if (ImGui::Button("<-") && numberOfLight > 0)
            {
                numberOfLight--;
            }
            ImGui::SameLine();
            ImGui::Text("Esta es la luz numero: %i", numberOfLight);
            ImGui::SameLine();
            if (ImGui::Button("->") && numberOfLight < int(registerOfLights.size() - 1))
            {
                numberOfLight++;
            }

            auto luzActual = registerOfLights[std::size_t(numberOfLight)];
            auto tipo = luzActual.params->type;
            int numeroDelTipo = tipo;

            if (ImGui::Combo("\nEntity Type Filter", &numeroDelTipo, tiposDeLuz, IM_ARRAYSIZE(tiposDeLuz)))
            {
                switch (numeroDelTipo)
                {
                case 0:
                    luzActual.params->type = LightType::DIRECTIONAL_LIGHT;
                    break;
                case 1:
                    luzActual.params->type = LightType::POINT_LIGHT;
                    break;
                case 2:
                    luzActual.params->type = LightType::SPOT_LIGHT;
                    break;
                }
            }

            if (ImGui::Button("Node Luz"))
            {
                nodeLight = not nodeLight;
                if (nodeLight)
                    addNodeWindow(luzActual.node);
            }

            ImGui::InputFloat3("Ambiente", &luzActual.params->ambient.x, "%.8f");
            ImGui::InputFloat3("Difusa", &luzActual.params->diffuse.x, "%.8f");
            ImGui::InputFloat3("Especular", &luzActual.params->specular.x, "%.8f");
            ImGui::Checkbox("Esta activa", &luzActual.params->actived);
            ImGui::Checkbox("Tiene atenuacion", &luzActual.params->HasAttenuation);

            switch (numeroDelTipo)
            {
            case 0:
                ImGui::InputFloat3("Direccion", &luzActual.params->direction.x);

                break;
            case 1:
                ImGui::InputFloat("Constate", &luzActual.params->constant, {}, {}, "%.8f");
                ImGui::InputFloat("Linear", &luzActual.params->linear, {}, {}, "%.8f");

                ImGui::InputFloat("Quadratica", &luzActual.params->quadratic, {}, {}, "%.8f");

                break;
            case 2:
                ImGui::InputFloat3("Direccion", &luzActual.params->direction.x);

                float angleCutOff = luzActual.params->getCutOffAngle();
                ImGui::SliderFloat("Cut Off", &angleCutOff, 0.0f, 90.0f, "%.8f");

                float angleOuterCutOff = luzActual.params->getOuterCutOffAngle();
                ImGui::SliderFloat("Outer Cut Off", &angleOuterCutOff, 0.0f, 90.0f, "%.8f");

                luzActual.params->setCutOff(angleCutOff);
                luzActual.params->setOuterCutOff(angleOuterCutOff);

                break;
            }
        }

        if (inputWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 150));

            ImGui::Begin("Input Window");
            if (true)
            {
                const char *tipoS = "Tipo del input";
                ImGui::Text("El tipo de input es: %s", tipoS);
            }

            ImGui::End();
        }

        if (cameraWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(250, 250));

            ImGui::Begin("Camera Window");

            ImGui::Text("Distancia %2f", cComp->distance);
            ImGui::Text("Key de la camara %i", cComp->e_key.id);

            auto *node = cComp->cam;
            if (node)
            {
                if (ImGui::Button("Nodo"))
                {
                    addNodeWindow(node);
                }
            }
            ImGui::End();
        }

        if (forcesWindow)
        {

            ImGui::SetNextWindowSize(ImVec2(190, 420));

            ImGui::Begin("Físicas window");

            float step = 200;
            float gravitychange = 10;

            ImGui::Text("------ FUERZAS ------");

            ImGui::Text("Fuerza X: %2f", pComp->force.x);
            ImGui::InputFloat("X", &pComp->force.x, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::Text("Fuerza Y: %2f", pComp->force.y);
            ImGui::InputFloat("Y", &pComp->force.y, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::Text("Fuerza Z: %2f", pComp->force.z);
            ImGui::InputFloat("Z", &pComp->force.z, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::Text("\n------ VELOCIDADES ------");
            ImGui::Text("Velocidad en X: %2f", pComp->velocity.x);
            ImGui::Text("Velocidad en Y: %2f", pComp->velocity.y);
            ImGui::Text("Velocidad en Z: %2f", pComp->velocity.z);

            ImGui::Text("\n------ EXTRA ------");
            ImGui::Text("Mass: %2f", pComp->mass);
            ImGui::Checkbox("IsGrounded: ", &pComp->isGrounded);

            ImGui::Text("\n------ GRAVEDAD ------");
            ImGui::Text("Gravedad: %2f", pComp->gravity.y);
            ImGui::InputFloat("Y", &pComp->gravity.y, gravitychange, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

            ImGui::Checkbox("On/Off", &pComp->gravityBool);

            ImGui::End();
        }

        if (colisionWindow)
        {
            ImGui::SetNextWindowSize(ImVec2(300, 200));

            ImGui::Begin("Colision window");
            float step = 50;

            ImGui::Text("Friction: %2f", bComp->friction);
            ImGui::InputFloat("Z", &bComp->friction, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
            ImGui::Text("Bounciness: %2f", bComp->bounciness);
            ImGui::InputFloat("Z", &bComp->bounciness, step, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);

            // ImGui::Text("----- BOUNDING BOXES -----");
            // ImGui::Checkbox("On/Off", &boundBox);

            ImGui::End();
        }

        for (unsigned int i = 0; i < nodesToWindow.size(); i++)
        {
            drawNodeWindow(nodesToWindow[i]);
        }
    }
}

void DebugSystem::drawNodeWindow(TNode *node)
{
    std::string name = std::to_string(node->getID());
    ImGui::SetNextWindowSize(ImVec2(300, 400));
    ImGui::Begin(name.c_str());

    auto posicionDelNodo = node->globalPosition;
    if (ImGui::InputFloat3("Entity Position", &posicionDelNodo.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        node->setGlobalPosition(posicionDelNodo);

    ImGui::Text("\nTRANSFORM");

    ImGui::Text("-Rotacion");

    auto rotacionTransform = node->localTransform.rotation;
    if (ImGui::InputFloat3("Transform rotation", &rotacionTransform.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        node->localTransform.setRotate(rotacionTransform);

    ImGui::Text("-Escala");

    auto scaleTransform = node->localTransform.scale;
    if (ImGui::InputFloat3("Transform scale", &scaleTransform.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        node->localTransform.setScale(scaleTransform);

    ImGui::Text("-Posicion");
    auto positionTransform = node->localTransform.position;
    if (ImGui::InputFloat3("Transform position", &positionTransform.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
        node->localTransform.setPosition(positionTransform);

    ImGui::Text("Dimension Node [%.3f, %.3f, %.3f]", node->dimensionNode.x, node->dimensionNode.y, node->dimensionNode.z);

    ImGui::Text("\nId del nodo: %u", node->getID());
    if (node->getParent())
    {
        if (ImGui::Button("Mi nodo padre que tiene la id"))
        {
            addNodeWindow(node->getParent());
        }
    }
    else
    {
        ImGui::Text("(no tiene padre)");
    }

    ImGui::End();
}

bool DebugSystem::addNodeWindow(TNode *node)
{
    bool add{true};
    for (unsigned int i = 0; i < nodesToWindow.size(); i++)
    {
        if (nodesToWindow[i] == node)
        {
            add = false;
            nodesToWindow.erase(nodesToWindow.begin() + i);
        }
    }
    if (add)
        nodesToWindow.emplace_back(node);
    return add;
}

void DebugSystem::drawCollisionBoxes(ECS::EntityManager &entityManager)
{
    if (boundBoxWorld && not drawingBoxWorld)
    {
        for (auto &bodyCmp : entityManager.getComponents<WorldBodyComponent>())
        {
            auto *boundingMesh = bodyCmp.collider->nodeBox->getEntity<TBoundingBox>();
            boundingMesh->setup(glm::vec4(1, 1, 0, 1));
        }
        drawingBoxWorld = true;
    }
    else if (not boundBoxWorld && drawingBoxWorld)
    {
        for (auto &bodyCmp : entityManager.getComponents<WorldBodyComponent>())
        {
            auto *boundingMesh = bodyCmp.collider->nodeBox->getEntity<TBoundingBox>();
            boundingMesh->drop();
        }
        drawingBoxWorld = false;
    }

    if (boundBoxEntity && not drawingBoxEntity)
    {
        for (auto &bodyCmp : entityManager.getComponents<BodyComponent>())
        {
            auto *boundingMesh = bodyCmp.collider->nodeBox->getEntity<TBoundingBox>();
            boundingMesh->setup();
        }
        drawingBoxEntity = true;
    }
    else if (not boundBoxEntity && drawingBoxEntity)
    {
        for (auto &bodyCmp : entityManager.getComponents<BodyComponent>())
        {
            auto *boundingMesh = bodyCmp.collider->nodeBox->getEntity<TBoundingBox>();
            boundingMesh->drop();
        }
        drawingBoxEntity = false;
    }

    if (boundFrontBox && not drawingFrontBox)
    {
        for (auto &frontCmp : entityManager.getComponents<FrontBoxComponent>())
        {
            auto *boundingMesh = frontCmp.collider->nodeBox->getEntity<TBoundingBox>();
            boundingMesh->setup(glm::vec4(0, 1, 1, 1));
        }
        drawingFrontBox = true;
    }
    else if (not boundFrontBox && drawingFrontBox)
    {
        for (auto &frontCmp : entityManager.getComponents<FrontBoxComponent>())
        {
            auto *boundingMesh = frontCmp.collider->nodeBox->getEntity<TBoundingBox>();
            boundingMesh->drop();
        }
        drawingFrontBox = false;
    }

    if (boundPolygonWorld && not drawingPolygonWorld)
    {
        for (auto &scene : entityManager.getComponents<SceneBodyComponent>())
        {
            for (auto &polygon : scene.world_bounds)
            {
                polygon.setup(glm::vec4(1, 1, 0, 1));
            }
        }
        drawingPolygonWorld = true;
    }
    else if (not boundPolygonWorld && drawingPolygonWorld)
    {
        for (auto &scene : entityManager.getComponents<SceneBodyComponent>())
        {
            for (auto &polygon : scene.world_bounds)
            {
                polygon.drop();
            }
        }
        drawingPolygonWorld = false;
    }

    if (drawingPolygonWorld)
    {
        for (auto &scene : entityManager.getComponents<SceneBodyComponent>())
        {
            for (auto &polygon : scene.world_bounds)
            {
                polygon.draw(*debugShader);
            }
        }
    }
}

void DebugSystem::drawRaycast(ECS::EntityManager &entityManager)
{
    if (raycast && !drawingRay)
    {
        for (auto &rayCmp : entityManager.getComponents<RaycastComponent>())
        {
            for (auto &ray : rayCmp.rays)
            {
                rays_to_render.emplace_back(ray.origin, ray.direction, ray.range).setup();
            }
        }
        drawingRay = true;
    }
    else if (!raycast && drawingRay)
    {
        rays_to_render.clear();
        drawingRay = false;
    }
    if (drawingRay)
    {
        for (auto &rayCmp : entityManager.getComponents<RaycastComponent>())
        {
            auto *entity = entityManager.getEntityByKey(rayCmp.getEntityKey());
            TNode &nodoMain = SGFunc::getMainNodeFromEntity(*entity);

            auto globalPos = nodoMain.globalPosition;
            for (auto &ray : rayCmp.rays)
            {
                for (auto &rayLine : rays_to_render)
                {
                    rayLine.updateLine(ray.origin + globalPos, ray.direction, ray.range);
                    rayLine.draw(*debugShader);
                }
            }
        }
    }
}

void DebugSystem::drawDirection()
{
    if (!rayDirection && drawingDirection)
    {
        directionEntity.second.drop();
        drawingDirection = false;
    }
    else if (drawingDirection)
    {
        directionEntity.second.updateLine(directionEntity.first->globalPosition, directionEntity.first->direction, 2.0f);
        directionEntity.second.draw(*debugShader);
    }
}

void DebugSystem::drawTiles()
{

    if (boxTile & not drawingTile)
    {
        auto const &tileManager = engine->getTileManager<Mesh *>();
        tiles_to_render.reserve(tileManager.getTiles().size() * 8);

        glm::vec4 TileColor1{1, 0, 0, 1};
        glm::vec4 TileColor2{1, 0.5, 0, 1};

        auto currentColor = TileColor1;

        for (auto const &tile : tileManager.getTiles())
        {
            tiles_to_render.emplace_back(glm::vec3(tile.xMax, 0, tile.yMin), glm::vec3(tile.xMin, 0, tile.yMin)).setup(currentColor, 7.5f);
            tiles_to_render.emplace_back(glm::vec3(tile.xMin, 0, tile.yMax), glm::vec3(tile.xMin, 0, tile.yMin)).setup(currentColor, 7.5f);
            tiles_to_render.emplace_back(glm::vec3(tile.xMin, 0, tile.yMax), glm::vec3(tile.xMax, 0, tile.yMax)).setup(currentColor, 7.5f);
            tiles_to_render.emplace_back(glm::vec3(tile.xMax, 0, tile.yMin), glm::vec3(tile.xMax, 0, tile.yMax)).setup(currentColor, 7.5f);

            tiles_to_render.emplace_back(glm::vec3(tile.xMax, 0, tile.yMin), glm::vec3{0, 1, 0}, 75.0f).setup(currentColor, 7.5f);
            tiles_to_render.emplace_back(glm::vec3(tile.xMin, 0, tile.yMax), glm::vec3{0, 1, 0}, 75.0f).setup(currentColor, 7.5f);
            tiles_to_render.emplace_back(glm::vec3(tile.xMin, 0, tile.yMin), glm::vec3{0, 1, 0}, 75.0f).setup(currentColor, 7.5f);
            tiles_to_render.emplace_back(glm::vec3(tile.xMax, 0, tile.yMax), glm::vec3{0, 1, 0}, 75.0f).setup(currentColor, 7.5f);

            if (currentColor == TileColor1)
                currentColor = TileColor2;
            else
                currentColor = TileColor1;
        }

        drawingTile = true;
    }
    else if (not boxTile && drawingTile)
    {
        tiles_to_render.clear();
        drawingTile = false;
    }
    if (drawingTile)
    {
        // std::cout << "\n TAMANO TILES: " << tiles_to_render.size() << "\n";
        for (auto &tile : tiles_to_render)
        {
            tile.draw(*debugShader);
        }
    }
}