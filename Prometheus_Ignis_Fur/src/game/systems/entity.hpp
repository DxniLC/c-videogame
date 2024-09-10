
#pragma once

template <typename GameCTX>
struct EntityManagerSystem
{
    explicit EntityManagerSystem() = default;
    
    void update(GameCTX &g);

};
