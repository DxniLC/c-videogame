
#include "cinematic.hpp"

#include <game/component/cinematic/cinematic.hpp>

// Event Data

#include <game/events/eventsData/InitCinematicData.hpp>

template <typename GameCTX>
CinematicSystem<GameCTX>::CinematicSystem(EventManager &eventManager, SGEngine *engine)
{
    this->eventManager = &eventManager;
    this->engine = engine;
    eventManager.EventQueueInstance.Subscribe(EventIds::InitCinematic, this);
}

template <typename GameCTX>
void CinematicSystem<GameCTX>::update(GameCTX &g, float const &deltaTime)
{
    if (OnCinematic)
    {
        // Prepare to init values
        if (InitValues && engine->IsFadeOut())
        {
            for (auto &cinematicCmp : g.template getComponents<CinematicComponent>())
            {
                auto *entity = g.getEntityByKey(cinematicCmp.getEntityKey());
                auto *render = entity->template getComponent<RenderComponent>();
                auto *input = entity->template getComponent<InputComponent>();
                if (input)
                {
                    auto *cameraNode = render->getNodeChild(RenderComponent::RenderType::CAMERA);
                    if (cameraNode)
                        PlayerCamera = cameraNode->getID();
                }
                if (render)
                {
                    render->node->setGlobalPosition(cinematicCmp.initSpawnCinematic[CinematicID]);
                    auto *phy = entity->template getComponent<PhysicsComponent>();
                    if (phy)
                    {
                        phy->force = glm::vec3(0.0f);
                        phy->ownForce = glm::vec3(0.0f);
                        phy->externForces = glm::vec3(0.0f);
                    }
                }
                else
                {
                    auto *camera = entity->template getComponent<CameraComponent>();
                    if (camera)
                    {
                        camera->cam->setGlobalPosition(cinematicCmp.initSpawnCinematic[CinematicID]);
                    }
                }
            }
            engine->enableSGOptions(SG_CINEMATIC_BARS);
            engine->setFade({0, 0, 0}, 2.0f, true);
            InitValues = false;
        }

        // Processing cinematics
        if (not InitValues && not IsEnded)
        {
            ElapsedTime += deltaTime;
            unsigned int numCinematicsLeft{0};

            for (auto &cinematicCmp : g.template getComponents<CinematicComponent>())
            {
                auto *entity = g.getEntityByKey(cinematicCmp.getEntityKey());
                auto &CinematicList = cinematicCmp.cinematicMap[CinematicID];
                if (not CinematicList.empty())
                {
                    auto *currentCinematic = CinematicList.front().get();
                    if (currentCinematic->InitTime <= ElapsedTime)
                    {
                        bool IsFinished = currentCinematic->update(ElapsedTime, entity, engine, deltaTime);
                        if (IsFinished)
                        {
                            CinematicList.front().reset(nullptr);
                            CinematicList.pop_front();
                        }
                        else
                        {
                            numCinematicsLeft++;
                        }
                    }
                }
            }

            // if (ElapsedTime < EndTimeCinematic)

            if (numCinematicsLeft == 0 && ElapsedTime >= EndTimeCinematic)
            {
                engine->setFade({0, 0, 0}, 1.0f, false);
                IsEnded = true;
            }
        }

        if (IsEnded)
        {
            if (engine->IsFadeOut())
            {
                engine->setCameraActive(PlayerCamera);

                engine->disableSGOptions(SG_CINEMATIC_BARS);
                engine->setFade({0, 0, 0}, 1.5f, true);
                OnCinematic = false;
                eventManager->EventQueueInstance.EnqueueLateEvent(LockSystemData{false});
            }
        }
    }
}
template <typename GameCTX>
void CinematicSystem<GameCTX>::Process(EventData *eventData)
{
    if (eventData->EventId == EventIds::InitCinematic)
    {
        InitCinematicData *data{dynamic_cast<InitCinematicData *>(eventData)};
        if (data)
        {
            CinematicID = data->CinematicID;
            EndTimeCinematic = data->MaxDuration;
            InitValues = true;
            OnCinematic = true;
            IsEnded = false;
            engine->setFade({0, 0, 0}, 1.0f, false);
        }
        else
            std::cout << "\n***** ERROR EventInitCinematic *****\n";
    }
}
