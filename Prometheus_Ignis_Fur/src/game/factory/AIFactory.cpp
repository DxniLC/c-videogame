
#include "AIFactory.hpp"

// NODES
#include <util/BehaviourTree/nodeselector.hpp>
#include <util/BehaviourTree/nodegroup.hpp>

// ACTION NODES
#include <game/AI/BTNodes/actionWait.hpp>
#include <game/AI/BTNodes/actionLookAt.hpp>
#include <game/AI/BTNodes/actionSee.hpp>
#include <game/AI/BTNodes/actionHaveBall.hpp>
#include <game/AI/BTNodes/actionRecollectBall.hpp>
#include <game/AI/BTNodes/actionMoveLastSeen.hpp>
#include <game/AI/BTNodes/actionChargeAndShoot.hpp>
#include <game/AI/BTNodes/actionLookAtTarget.hpp>
#include <game/AI/BTNodes/actionAdvise.hpp>
#include <game/AI/BTNodes/actionPatroll.hpp>
#include <game/AI/BTNodes/actionFollowPathfinding.hpp>
#include <game/AI/BTNodes/actionCalculatePath.hpp>
#include <game/AI/BTNodes/actionHear.hpp>
#include <game/AI/BTNodes/actionAdviseHearing.hpp>
#include <game/AI/BTNodes/actionScape.hpp>
#include <game/AI/BTNodes/actionJump.hpp>
#include <game/AI/BTNodes/actionReturnPatroll.hpp>

void AIFactory::createBirdTree(BehaviourTree_t &BehaviourTree) const
{
    BehaviourTree.createNode<BTNodeSelector>(

        &BehaviourTree.createNode<BTNodeGroup_t>(

            &BehaviourTree.createNode<BTNodeSequence_t>(

                &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(1, 0, 0)) 
                ,
                &BehaviourTree.createNode<BTActionWait_t>(2.0f)
                    ,
                &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(-1, 0, 0)) 
                ,
                &BehaviourTree.createNode<BTActionWait_t>(2.0f)
                    ,
                &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(0, 0, 1))
                ,
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),
                &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(0, 0, -1)) 
                ,
                &BehaviourTree.createNode<BTActionWait_t>(2.0f)

                    ),
            &BehaviourTree.createNode<BTNodeSequence_t>(
                &BehaviourTree.createNode<BTActionSee_t>())

                ),
        &BehaviourTree.createNode<BTNodeSelector>(
            &BehaviourTree.createNode<BTNodeSequence_t>(
                &BehaviourTree.createNode<BTActionHaveBall>(),
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),
                &BehaviourTree.createNode<BTActionRecollectBall>(true),
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),

                &BehaviourTree.createNode<BTActionMoveLastSeen>()

                    ),
            &BehaviourTree.createNode<BTNodeSequence_t>(
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),
                &BehaviourTree.createNode<BTActionMoveLastSeen>(),
                &BehaviourTree.createNode<BTActionChargeAndShoot>()

                    )

                )

    );
}

void AIFactory::seeAndFollow(BehaviourTree_t &BehaviourTree) const
{
    BehaviourTree.createNode<BTNodeSelector>(

        &BehaviourTree.createNode<BTNodeSequence_t>(
            &BehaviourTree.createNode<BTNodeGroup_t>(
                &BehaviourTree.createNode<BTActionSee_t>(),
                &BehaviourTree.createNode<BTActionAdvise_t>()),
            &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(1, 0, 0)),
            &BehaviourTree.createNode<BTNodeGroup_t>(
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),
                &BehaviourTree.createNode<BTNodeGroup_t>(
                    &BehaviourTree.createNode<BTActionSee_t>(),
                    &BehaviourTree.createNode<BTActionAdvise_t>())),

            &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(-1, 0, 0)),
            &BehaviourTree.createNode<BTNodeGroup_t>(
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),
                &BehaviourTree.createNode<BTNodeGroup_t>(
                    &BehaviourTree.createNode<BTActionSee_t>(),
                    &BehaviourTree.createNode<BTActionAdvise_t>())),

            &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(0, 0, 1)),
            &BehaviourTree.createNode<BTNodeGroup_t>(
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),
                &BehaviourTree.createNode<BTNodeGroup_t>(
                    &BehaviourTree.createNode<BTActionSee_t>(),
                    &BehaviourTree.createNode<BTActionAdvise_t>())),

            &BehaviourTree.createNode<BTActionLookAt_t>(glm::vec3(0, 0, -1)),
            &BehaviourTree.createNode<BTNodeGroup_t>(
                &BehaviourTree.createNode<BTActionWait_t>(2.0f),
                &BehaviourTree.createNode<BTNodeGroup_t>(
                    &BehaviourTree.createNode<BTActionSee_t>(),
                    &BehaviourTree.createNode<BTActionAdvise_t>()))

                ),
        &BehaviourTree.createNode<BTNodeSelector>(
            &BehaviourTree.createNode<BTNodeGroup_t>(
                &BehaviourTree.createNode<BTActionLookAtTarget>(),
                &BehaviourTree.createNode<BTActionMoveLastSeen>())

                )

    );
}

void AIFactory::patroll(BehaviourTree_t &BehaviourTree) const
{
    BehaviourTree.createNode<BTNodeSelector>(

        &BehaviourTree.createNode<BTNodeGroup_t>(
            &BehaviourTree.createNode<BTActionPatroll_t>(),
            &BehaviourTree.createNode<BTActionSee_t>(),
            &BehaviourTree.createNode<BTActionAdvise_t>()),
        &BehaviourTree.createNode<BTNodeSequence_t>(
            &BehaviourTree.createNode<BTNodeGroup_t>(
                &BehaviourTree.createNode<BTActionCalculatePath_t>(),
                &BehaviourTree.createNode<BTActionFollowPathfinding_t>())),
        &BehaviourTree.createNode<BTNodeSequence_t>(
            &BehaviourTree.createNode<BTactionJump>(1),
            &BehaviourTree.createNode<BTactionreturnPatroll>())

    );
}

void AIFactory::hearAndFly(BehaviourTree_t &BehaviourTree, glm::vec3 position) const
{
    BehaviourTree.createNode<BTNodeSelector>(

        &BehaviourTree.createNode<BTNodeGroup_t>(
            &BehaviourTree.createNode<BTActionHear_t>(),
            &BehaviourTree.createNode<BTActionAdviseHearing_t>()),

        &BehaviourTree.createNode<BTActionScape_t>(position)

    );
}