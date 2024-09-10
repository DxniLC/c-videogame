
#pragma once

struct CollisionResult;
template <typename, typename>
struct RulesManager;
template <typename GameCTX>
struct CollisionSystem
{
    explicit CollisionSystem();

    void update(GameCTX &g);

private:
    inline static GameCTX *game_manager{nullptr};
    inline static EventManager *event_manager{nullptr};
    static void standardCollision(CollisionResult &pts);
    RulesManager<CollisionResult, uint32_t> rules_manager;
    inline static uint32_t triggerElements{0};
    static void Player_Box(CollisionResult &pts);
    static void Player_Plate(CollisionResult &pts);
    static void Box_Plate(CollisionResult &pts);
    static void Pick_Consumable(CollisionResult &pts);
    static void SetCheckpoint(CollisionResult &pts);
    static void GoToCheckpoint(CollisionResult &pts);
    static void LoadNewLevel(CollisionResult &pts);
    static void NPCFLOOR(CollisionResult &pts);


    static void LostHP(CollisionResult &pts);
    static void LoadCinematic(CollisionResult &pts);

    static void projectileStuck(CollisionResult &pts);

    static void GetDivineObject(CollisionResult &pts);

    static void ShowBillboard(CollisionResult &pts);
    static void PushForce(CollisionResult &pts);
};
