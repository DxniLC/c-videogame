#pragma once

template <typename GameCTX>
struct CinematicSystem : EventObserver
{
    explicit CinematicSystem(EventManager &eventManager, SGEngine *engine);
    void update(GameCTX &g, float const &deltaTime);

    void Process(EventData *eventData) override final;

private:
    EventManager *eventManager{nullptr};
    SGEngine *engine{nullptr};

    bool OnCinematic{false};
    bool InitValues{false};
    bool IsEnded{false};

    unsigned int CinematicID{0};
    float ElapsedTime{0.0f};
    float EndTimeCinematic{0.0f};

    unsigned int PlayerCamera;
};