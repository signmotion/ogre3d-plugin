#pragma once

#include "PhysicsPort.h"
#include "default.h"


/**
* ���� ��� ������������� ������ � �������� (��� ������) �������.
*/
class RealtimePhysicsPort :
    public PhysicsPort
{
public:
    inline RealtimePhysicsPort(
        Ogre::SceneManager* sm,
        const d::size3d_t& size,
        float timestep
    ) : PhysicsPort( sm, size, timestep )
    {
    }



    virtual inline ~RealtimePhysicsPort() {
        std::cout << "~RealtimePhysicsPort::RealtimePhysicsPort()" << std::endl;
    }

};

