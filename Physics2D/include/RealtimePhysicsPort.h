#pragma once

#include "PhysicsPort.h"
#include "default.h"
#include "struct.h"


/**
* ���� ��� ������������� ������ � �������� (��� ������) �������.
*/
class RealtimePhysicsPort :
    public PhysicsPort< EntityRTPortFD >
{
public:
    inline RealtimePhysicsPort(
        Ogre::SceneManager* sm,
        const size2d_t& size
    ) : PhysicsPort( sm, size )
    {
    }



    virtual inline ~RealtimePhysicsPort() {
        std::cout << "~RealtimePhysicsPort::RealtimePhysicsPort()" << std::endl;
    }




    /**
    * ��������� ������ �� ���������� ����� (����-�������).
    */
    void pushDroneMesh( const uid_t& fromThrone );
    void pushDroneBillboard( const uid_t& fromThrone );

};

