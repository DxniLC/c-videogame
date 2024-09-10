#pragma once

struct EventManager;

struct CollisionResult;
template <typename, typename>
struct RulesManager;
template <typename GameCTX>
struct InteractionSystem : EventObserver
{
    explicit InteractionSystem(EventManager &eventManager);
    void update(GameCTX &g);

    void Process(EventData *eventData) override final;

private:
    bool IsLocked{false};

    RulesManager<CollisionResult, uint32_t> hoverRules;
    static void ShowControl(CollisionResult &pts);

    RulesManager<CollisionResult, uint32_t> interactionRules;
    static void standardCollision(CollisionResult &pts);
    static void Player_Box(CollisionResult &pts);
    static void Player_Climb(CollisionResult &pts);
    static void Player_Chest(CollisionResult &pts);

    inline static EventManager *eventManager{nullptr};
};
