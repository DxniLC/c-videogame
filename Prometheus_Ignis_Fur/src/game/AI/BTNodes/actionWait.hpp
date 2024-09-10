#pragma once

#include <util/BehaviourTree/node.hpp>
#include <chrono>
#include <iostream>


struct BTActionWait_t : BTNode_t{
    BTActionWait_t(float t) : time{t} {};

    BTNodeStatus_t run(ECS::Entity &e, ECS::EntityManager &entityManager) noexcept final{      //override para asegurar que se esta implementando la funcion correcta -> final igual pero si derivamos de este nodo no la dejaria sobreescribir
        if(inittime==false)
        {
            mStartTime = std::chrono::system_clock::now();
            inittime = true;

        }

        mDeltaTime = std::chrono::system_clock::now() - mStartTime;

        if (mDeltaTime.count() >= time)
        {
            inittime = false;
            return BTNodeStatus_t::success;
        }

        return BTNodeStatus_t::running;



        
    }

    private:
    
    std::chrono::system_clock::time_point mStartTime;
    std::chrono::duration<float> mDeltaTime;
    bool inittime{false};
    float time{};

};