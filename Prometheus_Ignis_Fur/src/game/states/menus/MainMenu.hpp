#pragma once

#include "../State.hpp"

// Device and Engine
#include <engines/device.hpp>

// Managers
#include <ecs/manager/entitymanager.hpp>

// Timer
#include <util/GameTimer.hpp>

// Systems
#include "systems/render.cpp"
#include "systems/input.cpp"
#include "systems/animation.cpp"

#include "../LevelManager.hpp"

#include <game/particles/FireParticle.hpp>

#include <chrono>
#include <iostream>
#include <glm/glm.hpp>
#include <game/data/GameDataManager.hpp>

struct MainMenu : StateBase
{

    explicit MainMenu(StateManager &sm, SGDevice &device)
        : m_states(sm), device{device}, lastTime{device.getTime()}, engine{dynamic_cast<SGEngine *>(device.getGraphicEngine())},
          hudEngine{engine->getHUDEngine()}, audioEngine{device.getAudioEngine()}, debugSystem{&device},
          renderSystem{engine, &debugSystem}, inputSystem{device.getInputReceiver()}
    {

        device.getInputReceiver()->setVisible(true);

        // LOAD SCENE
        loadScene();

        // LOAD HUD
        loadHUD();

        // Sounds to Load
        loadSounds();
    }

    ~MainMenu()
    {

        engine->clearResource("media/models/Menu/borde_menu.obj");
        engine->clearResource("media/models/Menu/fuego_torch_pared.obj");

        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu1.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu2.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu3.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu4.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu5.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu6.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu7.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu8.png");
        engine->clearResource("media/textures/Menu/MainSelect/AnimMenu9.png");
        engine->clearResource("media/textures/Menu/MainClick/AnimMenuClick1.png");
        engine->clearResource("media/textures/Menu/MainClick/AnimMenuClick2.png");
        engine->clearResource("media/textures/Menu/MainClick/AnimMenuClick3.png");
        engine->clearResource("media/textures/Menu/MainClick/AnimMenuClick4.png");
        engine->clearResource("media/textures/Menu/MainClick/AnimMenuClick5.png");
        engine->clearResource("media/textures/Menu/MainClick/AnimMenuClick6.png");
        engine->clearResource("media/textures/Menu/MainClick/AnimMenuClick7.png");
    }

    void update() override final
    {
        // Calculate DeltaTime
        double currentTime = device.getTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        // Update Audio

        audioEngine->update();

        // Update Render
        engine->update(deltaTime);

        renderSystem.update(entityManager);

        // Update Input
        inputSystem.update(entityManager);

        // Update Animation

        animationSystem.update(entityManager, deltaTime);

        if (need_update)
        {
            hudEngine->clearAll();

            // Problema camara esta aqui
            entityManager.cleanAll();

            if (functionMenu)
            {
                functionMenu();
                loadCamera();
                functionMenu = nullptr;
            }

            need_update = false;
        }

        // timer to wait 60fps
        timer.waitUntil_ns(NSPF);
    }

    bool alive() override final { return m_alive; }

private:
    bool m_alive{true};
    StateManager &m_states;

    // Device
    SGDevice &device;

    GameTimer timer;

    uint64_t FPS{60};
    uint64_t NSPF{1000000000 / FPS};

    double lastTime;

    // Engine
    // TODO: Change SGEngine to abstract
    SGEngine *engine{nullptr};
    HUDEngine *hudEngine{nullptr};
    AudioEngine *audioEngine{nullptr};
    DebugSystem debugSystem;

    // Managers
    ECS::EntityManager entityManager{};
    GameDataManager dataManager{};

    // Systems
    RenderMenuSystem<SGEngine> renderSystem;
    InputMenuSystem<ECS::EntityManager> inputSystem;
    AnimationMenuSystem<ECS::EntityManager> animationSystem{};

    ResourceFont *font{nullptr};

    enum AudioType : int
    {
        AUDIO_MUSIC = 0,
        AUDIO_AMBIENT,
        AUDIO_EFFECTS
    };

    std::vector<HUDRect *> volumeBars{};

    bool need_update{false};

    std::function<void()> functionMenu{nullptr};

    std::vector<ResourceTexture *> spritesHover;

    std::vector<ResourceTexture *> spritesClick;

    std::vector<const char *> typesOfScreen = {
        "Pantalla completa",
        "Modo ventana"};
    int actualType{1};
    glm::vec2 maxSizes{};
    glm::vec2 actualSize{};

    HUDText *textType;
    HUDText *textSize;

    AudioInstance *move{nullptr};
    AudioInstance *select{nullptr};

    std::chrono::steady_clock::time_point lastChangeTime{};
    std::chrono::steady_clock::time_point lastChangeTimeApply{};

    const std::chrono::duration<double> changeDelay{0.2};
    const std::chrono::duration<double> changeDelayApply{2};

    void loadCamera()
    {
        auto &camera = entityManager.createEntity();
        auto &sceneCamera = entityManager.addComponent<CameraMenuComponent>(camera);

        sceneCamera.camera = engine->getCurrentCamera();
        sceneCamera.Position = {0, 5, 0};
        sceneCamera.Rotation = -engine->getCurrentCamera().first->localTransform.rotation;
        sceneCamera.RotationSpeed = 5.0f;
    }

    void loadScene()
    {

        engine->clearScene();

        engine->initScene();

        engine->addModel4Scene("media/levels/Inicio/BaseInicio.obj", engine->getShader("media/shaders/VertexShader.vert", "media/shaders/Toon/Toon.frag", true));
        engine->createCamera(nullptr);

        loadCamera();

        LightParams fireParams;
        fireParams.type = LightType::POINT_LIGHT;
        fireParams.ambient = glm::vec3(0.5f);
        fireParams.diffuse = glm::vec3(0.8, 0.3, 0.1);
        fireParams.constant = 1.f;
        fireParams.linear = 0.3f;
        fireParams.quadratic = 0;
        fireParams.HasAttenuation = true;

        // PANEL MENU

        Transform lightTrans;
        lightTrans.position = {0, 0, -5};
        lightTrans.scale = {0.3, 0.3, 0.3};
        auto *node = engine->createModel("media/models/Menu/borde_menu.obj", engine->getShader("media/shaders/VertexShader.vert", "media/shaders/Toon/Toon.frag", true), engine->getCurrentCamera().first, lightTrans);

        fireParams.type = LightType::POINT_LIGHT;

        lightTrans.position = {7.95f, 1.7, 0};
        lightTrans.scale = {0.15, 0.15, 0.15};
        lightTrans.rotation = {0, -90, 0};

        auto *nodeLight = engine->createLight(node, lightTrans, fireParams, "media/models/Menu/fuego_torch_pared.obj");
        lightTrans.position.x *= -1;
        lightTrans.rotation.y *= -1;

        auto *light = nodeLight->getEntity<TLight>();
        light->addEffect(FireLight{0.3f, 0.2f, 0.13f});

        auto *nodeGen = engine->createParticleGenerator(5, {0.1, 0.1}, nullptr, nodeLight, Transform());
        auto *particleGen = nodeGen->getEntity<ParticleGenerator>();
        particleGen->addEffect(std::make_unique<FireParticle>());
        particleGen->addColor(glm::vec4{0.5f});
        particleGen->addParticleLife(2.0f);
        particleGen->IsLooping = true;

        nodeLight = engine->createLight(node, lightTrans, fireParams, "media/models/Menu/fuego_torch_pared.obj");

        light = nodeLight->getEntity<TLight>();
        light->addEffect(FireLight{0.3f, 0.2f, 0.13f});

        nodeGen = engine->createParticleGenerator(5, {0.1, 0.1}, nullptr, nodeLight, Transform());
        particleGen = nodeGen->getEntity<ParticleGenerator>();
        particleGen->addEffect(std::make_unique<FireParticle>());
        particleGen->addColor(glm::vec4{0.5f});
        particleGen->addParticleLife(2.0f);
        particleGen->IsLooping = true;
    }

    void loadHUD()
    {

        font = hudEngine->getFont("media/fonts/Archeologicaps.ttf");

        spritesHover = {
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu1.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu2.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu3.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu4.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu5.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu6.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu7.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu8.png"),
            engine->getTexture("media/textures/Menu/MainSelect/AnimMenu9.png"),
        };

        spritesClick = {
            engine->getTexture("media/textures/Menu/MainClick/AnimMenuClick1.png"),
            engine->getTexture("media/textures/Menu/MainClick/AnimMenuClick2.png"),
            engine->getTexture("media/textures/Menu/MainClick/AnimMenuClick3.png"),
            engine->getTexture("media/textures/Menu/MainClick/AnimMenuClick4.png"),
            engine->getTexture("media/textures/Menu/MainClick/AnimMenuClick5.png"),
            engine->getTexture("media/textures/Menu/MainClick/AnimMenuClick6.png"),
            engine->getTexture("media/textures/Menu/MainClick/AnimMenuClick7.png"),
        };

        // NEW GAME ELEMENT

        auto &entity1 = entityManager.createEntity();

        auto &animation = entityManager.addComponent<AnimationMenuComponent>(entity1);
        animation.animationElement = hudEngine->addImage(nullptr, {0.5f, 0.50f}, {0.3, 0.1}, -1, true);
        animation.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animation.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);

        animation.HoverAnimation = [&](AnimationMenuComponent &anim, bool inverse)
        {
            if (move != nullptr)
                move->start();

            anim.animationElement->playAnimation(0, inverse);
        };

        animation.ClickAnimation = [&](AnimationMenuComponent &anim)
        {
            if (select != nullptr)
                select->start();
            anim.animationElement->playAnimation(1);
        };

        auto &render = entityManager.addComponent<RenderMenuComponent>(entity1);

        std::string newGame{"NEW GAME"};

        if (dataManager.HasASaveGame())
            newGame = "CONTINUE";

        render.hudElement = hudEngine->addText(newGame.c_str(), {0.0f, 0.0f, 0.0f}, {0.5f, 0.50f}, 32, 0, font);

        auto &input = entityManager.addComponent<InputMenuComponent>(entity1);
        input.actionFunc = [&]()
        {
            need_update = true;
            functionMenu = [&]()
            {
                m_states.replaceState<LevelManager>(m_states, device);
            };
        };

        // OPTIONS ELEMENT

        auto &entity3 = entityManager.createEntity();

        auto &animation3 = entityManager.addComponent<AnimationMenuComponent>(entity3);
        animation3.animationElement = hudEngine->addImage(nullptr, {0.5f, 0.65f}, {0.3, 0.1}, -1, true);
        animation3.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animation3.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);

        animation3.HoverAnimation = [&](AnimationMenuComponent &anim, bool inverse)
        {
            if (move)
                move->start();

            anim.animationElement->playAnimation(0, inverse);
        };

        animation3.ClickAnimation = [&](AnimationMenuComponent &anim)
        {
            if (select)
                select->start();

            anim.animationElement->playAnimation(1);
        };

        auto &render3 = entityManager.addComponent<RenderMenuComponent>(entity3);
        render3.hudElement = hudEngine->addText("OPTIONS", {0.0f, 0.0f, 0.0f}, {0.5f, 0.65f}, 32, 0, font);

        auto &input3 = entityManager.addComponent<InputMenuComponent>(entity3);
        input3.actionFunc = [&]()
        {
            need_update = true;
            functionMenu = [&]()
            { loadOptions(); };
        };

        // EXIT ELEMENT

        auto &entity2 = entityManager.createEntity();

        auto &animation2 = entityManager.addComponent<AnimationMenuComponent>(entity2);
        animation2.animationElement = hudEngine->addImage(nullptr, {0.35f, 0.8f}, {0.15, 0.1}, -1, true);
        animation2.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animation2.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);

        animation2.HoverAnimation = [&](AnimationMenuComponent &anim, bool inverse)
        {
            if (move)
                move->start();

            anim.animationElement->playAnimation(0, inverse);
        };

        animation2.ClickAnimation = [&](AnimationMenuComponent &anim)
        {
            if (select)
                select->start();

            anim.animationElement->playAnimation(1);
        };

        auto &render2 = entityManager.addComponent<RenderMenuComponent>(entity2);
        render2.hudElement = hudEngine->addText("EXIT", {0.0f, 0.0f, 0.0f}, {0.35f, 0.8f}, 32, 0, font);

        auto &input2 = entityManager.addComponent<InputMenuComponent>(entity2);
        input2.actionFunc = [&]()
        {
            m_alive = false;
        };

        // TITLE ELEMENT

        auto &entity6 = entityManager.createEntity();

        auto &render6 = entityManager.addComponent<RenderMenuComponent>(entity6);
        render6.hudElement = hudEngine->addText("PROMETHEUS", {0.0f, 0.0f, 0.0f}, {0.5f, 0.25f}, 64, 0, font);

        // SUBTITLE ELEMENT

        auto &entity4 = entityManager.createEntity();

        auto &render4 = entityManager.addComponent<RenderMenuComponent>(entity4);
        render4.hudElement = hudEngine->addText("IGNIS FUR", {0.0f, 0.0f, 0.0f}, {0.5f, 0.35f}, int(32.0f * 0.9f), 0, font);

        // VERSION ELEMENT
        auto &entityKey = entityManager.createEntity();
        auto &renderKey = entityManager.addComponent<RenderMenuComponent>(entityKey);

        auto userInfo = dataManager.getUserInfo();

        std::string keyInfo{"User Key: " + userInfo.second};
        renderKey.hudElement = hudEngine->addText(keyInfo.c_str(), {0.0f, 0.0f, 0.0f}, {0.55f, 0.75f}, 32, 5);

        auto &entityUser = entityManager.createEntity();
        auto &renderUser = entityManager.addComponent<RenderMenuComponent>(entityUser);

        std::string userName{"Welcome: " + userInfo.first};
        renderUser.hudElement = hudEngine->addText(userName.c_str(), {0.0f, 0.0f, 0.0f}, {0.55f, 0.80f}, 32, 5);
    }

    // TODO: Habra que hacer algo para que se comparta con el menu de Pausa
    void loadOptions()
    {

        // #############################
        // TITLE ELEMENT

        auto &entity = entityManager.createEntity();

        auto &render = entityManager.addComponent<RenderMenuComponent>(entity);
        render.hudElement = hudEngine->addText("AJUSTES", {0.0f, 0.0f, 0.0f}, {0.5f, 0.25f}, 64, 0, font);
        // #############################

        // #############################
        // EXIT ELEMENT
        auto &entity2 = entityManager.createEntity();

        auto &animation2 = entityManager.addComponent<AnimationMenuComponent>(entity2);
        animation2.animationElement = hudEngine->addImage(nullptr, {0.35f, 0.8f}, {0.15, 0.1}, -1, true);
        animation2.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animation2.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);

        animation2.HoverAnimation = [&](AnimationMenuComponent &anim, bool inverse)
        {
            if (move)
                move->start();
            anim.animationElement->playAnimation(0, inverse);
        };

        animation2.ClickAnimation = [&](AnimationMenuComponent &anim)
        {
            if (select)
                select->start();
            anim.animationElement->playAnimation(1);
        };

        auto &render2 = entityManager.addComponent<RenderMenuComponent>(entity2);
        render2.hudElement = hudEngine->addText("EXIT", {0.0f, 0.0f, 0.0f}, {0.35f, 0.8f}, 32, 0, font);

        auto &input2 = entityManager.addComponent<InputMenuComponent>(entity2);
        input2.actionFunc = [&]()
        {
            need_update = true;
            volumeBars.clear();
            functionMenu = [&]()
            { loadHUD(); };
        };

        // #############################

        // #############################
        // AUDIO ELEMENTS

        hudEngine->addText("MUSIC", {0.0f, 0.0f, 0.0f}, {0.37f, 0.58f}, 20, 0, font);
        hudEngine->addText("AMBIENT", {0.0f, 0.0f, 0.0f}, {0.50f, 0.58f}, 20, 0, font);
        hudEngine->addText("EFFECTS", {0.0f, 0.0f, 0.0f}, {0.63f, 0.58f}, 20, 0, font);

        glm::vec2 ContainerSize{0.11f, 0.04f};
        glm::vec2 ContainerPos{0.37f, 0.63f};

        glm::vec4 BackgroundColor{0, 0, 0, 0.5};
        glm::vec4 ColorValue{1.0f};

        for (int i = 0; i < 3; i++)
        {
            auto &entityBar = entityManager.createEntity();
            auto &renderBar = entityManager.addComponent<RenderMenuComponent>(entityBar);

            renderBar.hudElement = hudEngine->addRect(ContainerPos, ContainerSize, BackgroundColor, 0, true);

            volumeBars.emplace_back(hudEngine->addRect({ContainerPos.x - (ContainerSize.x / 2.0f), ContainerPos.y - (ContainerSize.y / 2.0f)}, {(ContainerSize.x / 2.0f), ContainerSize.y}, ColorValue, 1, false));

            auto &inputBar = entityManager.addComponent<InputMenuComponent>(entityBar);
            inputBar.actionFunc = [&, ContainerSize, ContainerPos, i]()
            {
                glm::vec2 cursorPos = device.getInputReceiver()->getCursorPosRelative();

                float value = cursorPos.x - (ContainerPos.x - (ContainerSize.x / 2.0f));
                float volume = value / (ContainerSize.x / 10.0f);

                setVolume(volume, i);
            };

            ContainerPos.x += ContainerSize.x + 0.02f;
        }

        loadScreenOptions();
    }

    void
    loadSounds()
    {
        audioEngine->cleanAll();
        move = audioEngine->loadSoundEvent("Menus/move");
        select = audioEngine->loadSoundEvent("Menus/open");
        audioEngine->loadSoundEvent("Music/menu")->start();
    }

    void loadScreenOptions()
    {
        // SCREEN ELEMENTS
        auto &entityTypeScreenL = entityManager.createEntity();
        auto &renderTypeScreenL = entityManager.addComponent<RenderMenuComponent>(entityTypeScreenL);
        const char *TextToRenderL = "<-";
        renderTypeScreenL.hudElement = hudEngine->addText(TextToRenderL, {1, 1, 1}, {0.4, 0.38}, 22, 1);

        auto &animationTypeScreenL = entityManager.addComponent<AnimationMenuComponent>(entityTypeScreenL);
        animationTypeScreenL.animationElement = hudEngine->addImage(nullptr, {0.4, 0.395}, {0.03, 0.03}, -1, true);
        animationTypeScreenL.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animationTypeScreenL.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);
        animationTypeScreenL.HoverAnimation = [renderTypeScreenL](AnimationMenuComponent &anim, bool inverse)
        {
            renderTypeScreenL.hudElement->addAnimationScale(glm::vec2(1.4, 1.4), 3);
            anim.animationElement->playAnimation(0, inverse);
        };
        animationTypeScreenL.ClickAnimation = [](AnimationMenuComponent &anim)
        {
            anim.animationElement->playAnimation(1);
        };

        auto &entityTypeScreenR = entityManager.createEntity();
        auto &renderTypeScreenR = entityManager.addComponent<RenderMenuComponent>(entityTypeScreenR);
        const char *TextToRenderR = "->";
        renderTypeScreenR.hudElement = hudEngine->addText(TextToRenderR, {1, 1, 1}, {0.6, 0.38}, 22, 1);

        auto &animationTypeScreenR = entityManager.addComponent<AnimationMenuComponent>(entityTypeScreenR);
        animationTypeScreenR.animationElement = hudEngine->addImage(nullptr, {0.6, 0.395}, {0.03, 0.03}, -1, true);
        animationTypeScreenR.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animationTypeScreenR.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);
        animationTypeScreenR.HoverAnimation = [renderTypeScreenR](AnimationMenuComponent &anim, bool inverse)
        {
            renderTypeScreenR.hudElement->addAnimationScale(glm::vec2(1.4, 1.4), 3);
            anim.animationElement->playAnimation(0, inverse);
        };
        animationTypeScreenR.ClickAnimation = [](AnimationMenuComponent &anim)
        {
            anim.animationElement->playAnimation(1);
        };

        auto &entityTypeScreenRect = entityManager.createEntity();
        auto &renderTypeScreenRect = entityManager.addComponent<RenderMenuComponent>(entityTypeScreenRect);
        renderTypeScreenRect.hudElement = hudEngine->addRect({0.42, 0.37}, {0.16, 0.05}, {1, 1, 1, 0.7}, 0, false);

        const char *TextToRenderText = typesOfScreen[actualType];

        textType = hudEngine->addText(TextToRenderText, {1, 0, 0}, {0.5, 0.38}, 22, 1);

        auto &inputTypeScreenL = entityManager.addComponent<InputMenuComponent>(entityTypeScreenL);

        inputTypeScreenL.actionFunc = [&]()
        {
            auto currentTime = std::chrono::steady_clock::now();
            if (currentTime - lastChangeTime >= changeDelay)
            {

                if (actualType > 0)
                {
                    actualType--;
                }
                else
                {
                    actualType = 1;
                }
                const char *TextToRenderText = typesOfScreen[actualType];

                textType->text = TextToRenderText;
                lastChangeTime = currentTime;
            }
        };

        auto &inputTypeScreenR = entityManager.addComponent<InputMenuComponent>(entityTypeScreenR);

        inputTypeScreenR.actionFunc = [&]()
        {
            auto currentTime = std::chrono::steady_clock::now();
            if (currentTime - lastChangeTime >= changeDelay)
            {

                if (actualType < 1)
                {
                    actualType++;
                }
                else
                {
                    actualType = 0;
                }
                const char *TextToRenderText = typesOfScreen[actualType];

                textType->text = TextToRenderText;
                lastChangeTime = currentTime;
            }
        };

        // SIZE SCREEN ELEMENTS
        auto &entityTypeSizeScreenL = entityManager.createEntity();
        auto &renderTypeSizeScreenL = entityManager.addComponent<RenderMenuComponent>(entityTypeSizeScreenL);
        const char *TextToRenderSizeL = "<-";
        renderTypeSizeScreenL.hudElement = hudEngine->addText(TextToRenderSizeL, {1, 1, 1}, {0.4, 0.45}, 22, 1);

        auto &animationTypeSizeScreenL = entityManager.addComponent<AnimationMenuComponent>(entityTypeSizeScreenL);
        animationTypeSizeScreenL.animationElement = hudEngine->addImage(nullptr, {0.4, 0.465}, {0.03, 0.03}, -1, true);
        animationTypeSizeScreenL.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animationTypeSizeScreenL.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);
        animationTypeSizeScreenL.HoverAnimation = [renderTypeSizeScreenL](AnimationMenuComponent &anim, bool inverse)
        {
            renderTypeSizeScreenL.hudElement->addAnimationScale(glm::vec2(1.4, 1.4), 3);
            anim.animationElement->playAnimation(0, inverse);
        };
        animationTypeSizeScreenL.ClickAnimation = [](AnimationMenuComponent &anim)
        {
            anim.animationElement->playAnimation(1);
        };

        auto &entityTypeSizeScreenR = entityManager.createEntity();
        auto &renderTypeSizeScreenR = entityManager.addComponent<RenderMenuComponent>(entityTypeSizeScreenR);
        const char *TextToRenderSizeR = "->";
        renderTypeSizeScreenR.hudElement = hudEngine->addText(TextToRenderSizeR, {1, 1, 1}, {0.6, 0.45}, 22, 1);

        auto &animationTypeSizeScreenR = entityManager.addComponent<AnimationMenuComponent>(entityTypeSizeScreenR);
        animationTypeSizeScreenR.animationElement = hudEngine->addImage(nullptr, {0.6, 0.465}, {0.03, 0.03}, -1, true);
        animationTypeSizeScreenR.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animationTypeSizeScreenR.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);
        animationTypeSizeScreenR.HoverAnimation = [renderTypeSizeScreenR](AnimationMenuComponent &anim, bool inverse)
        {
            renderTypeSizeScreenR.hudElement->addAnimationScale(glm::vec2(1.4, 1.4), 3);
            anim.animationElement->playAnimation(0, inverse);
        };
        animationTypeSizeScreenR.ClickAnimation = [](AnimationMenuComponent &anim)
        {
            anim.animationElement->playAnimation(1);
        };

        auto &entityTypeSizeScreenRect = entityManager.createEntity();
        auto &renderTypeSizeScreenRect = entityManager.addComponent<RenderMenuComponent>(entityTypeSizeScreenRect);
        renderTypeSizeScreenRect.hudElement = hudEngine->addRect({0.42, 0.44}, {0.16, 0.05}, {1, 1, 1, 0.7}, 0, false);

        maxSizes = device.getInputReceiver()->getMaxScreenSizes();

        actualSize = maxSizes;

        std::string cadenaCompleta = std::to_string(int(maxSizes.x)) + "x" + std::to_string(int(maxSizes.y));

        textSize = hudEngine->addText(cadenaCompleta.c_str(), {1, 0, 0}, {0.5, 0.45}, 22, 1);

        auto &inputTypeSizeScreenL = entityManager.addComponent<InputMenuComponent>(entityTypeSizeScreenL);

        inputTypeSizeScreenL.actionFunc = [&]()
        {
            auto currentTime = std::chrono::steady_clock::now();
            if (currentTime - lastChangeTime >= changeDelay)
            {
                glm::vec2 aux = glm::vec2{actualSize.x / 1.5f, actualSize.y / 1.5f};

                if (aux.x > 450 && aux.y > 450)
                {
                    actualSize = glm::vec2{int(round(aux.x)), int(round(aux.y))};

                    textSize->text = std::to_string(int(actualSize.x)) + "x" + std::to_string(int(actualSize.y));
                }
                lastChangeTime = currentTime;
            }
        };

        auto &inputTypeSizeScreenR = entityManager.addComponent<InputMenuComponent>(entityTypeSizeScreenR);

        inputTypeSizeScreenR.actionFunc = [&]()
        {
            auto currentTime = std::chrono::steady_clock::now();
            if (currentTime - lastChangeTime >= changeDelay)
            {

                glm::vec2 aux = glm::vec2{actualSize.x * 1.5, actualSize.y * 1.5};
                if (aux.x <= maxSizes.x)
                {

                    actualSize = glm::vec2{int(round(aux.x)), int(round(aux.y))};

                    textSize->text = std::to_string(int(actualSize.x)) + "x" + std::to_string(int(actualSize.y));
                }
                else
                {
                    actualSize = maxSizes;
                }
                lastChangeTime = currentTime;
            }
        };

        // Boton aplicar cambios
        auto &entityApplyChanges = entityManager.createEntity();

        auto &animationApplyChanges = entityManager.addComponent<AnimationMenuComponent>(entityApplyChanges);
        animationApplyChanges.animationElement = hudEngine->addImage(nullptr, {0.55, 0.53}, {0.15, 0.05}, -1, true);
        animationApplyChanges.animationElement->setAnimationSprite(0, spritesHover, 0.03f, false);
        animationApplyChanges.animationElement->setAnimationSprite(1, spritesClick, 0.03f, false);

        animationApplyChanges.HoverAnimation = [&](AnimationMenuComponent &anim, bool inverse)
        {
            if (move)
                move->start();

            anim.animationElement->playAnimation(0, inverse);
        };

        animationApplyChanges.ClickAnimation = [&](AnimationMenuComponent &anim)
        {
            if (select)
                select->start();

            anim.animationElement->playAnimation(1);
        };

        auto &renderApplyChanges = entityManager.addComponent<RenderMenuComponent>(entityApplyChanges);
        const char *TextToApplyChanges = "Apply";
        renderApplyChanges.hudElement = hudEngine->addText(TextToApplyChanges, {1, 1, 1}, {0.55, 0.52}, 22, 1);

        auto &inputApplyChanges = entityManager.addComponent<InputMenuComponent>(entityApplyChanges);

        inputApplyChanges.actionFunc = [&]()
        {
            auto currentTime = std::chrono::steady_clock::now();
            if (currentTime - lastChangeTimeApply >= changeDelayApply)
            {
                device.getInputReceiver()->changeFullScreen(actualType);

                device.getInputReceiver()->setWindowSize(actualSize);

                lastChangeTimeApply = currentTime;
            }
        };
    }

    void setVolume(float valor, int type)
    {

        if (valor < 0.2f)
            valor = 0;
        else if (valor > 10.0f)
            valor = 10;

        std::string VCAType{""};

        switch (static_cast<AudioType>(type))
        {
        case AudioType::AUDIO_MUSIC:
            VCAType = "Music";
            break;
        case AudioType::AUDIO_AMBIENT:
            VCAType = "Ambient";
            break;
        case AudioType::AUDIO_EFFECTS:
            VCAType = "Effects";
            break;
        default:
            return;
        };

        volumeBars[std::size_t(type)]->size.x = 0.011f * valor;

        float valorvolumen = valor / 5.0f;

        audioEngine->setVCAvolume(valorvolumen, VCAType.c_str());
    }
};