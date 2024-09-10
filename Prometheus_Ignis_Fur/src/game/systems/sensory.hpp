
#pragma once

struct CollisionResult;
template <typename, typename>
struct RulesManager;

struct SensoryComponent;

template <typename GameCTX>
struct SensorySystem : EventObserver
{
    explicit SensorySystem(EventManager &eventManager);

    void update(GameCTX &g);

    void Process(EventData *eventData0) override final;

private:
    bool IsLocked{false};
    bool LockSystem(bool status);

    enum SenseType : uint8_t
    {
        NONE_SENSE,
        SIGHT_SENSE,
        HEARING_SENSE
    };

    void CheckVisionRays(SensoryComponent &senseCMP);

    void CheckHearingSense(SensoryComponent &sense_A, TNode &node_A, SensoryComponent &sense_B, TNode &node_B);

    void CheckSightSense(SensoryComponent &sense_A, TNode &node_A, SensoryComponent &sense_B, TNode &node_B);
    
    static void Player_Npc_Interaction(CollisionResult &pts);

    inline static SenseType senseType{NONE_SENSE};

    inline static GameCTX *game_manager{nullptr};

    static void standardCollision([[maybe_unused]] CollisionResult &pts){};
    RulesManager<CollisionResult, uint32_t> rules_manager;
};
