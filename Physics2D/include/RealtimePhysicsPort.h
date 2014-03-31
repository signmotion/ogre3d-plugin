#pragma once

#include "PhysicsPort.h"
#include "default.h"
#include "struct.h"


/**
* ѕорт дл€ моделировани€ физики в реальном (дл€ игрока) времени.
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
    * ¬ыпускает дронов из указанного трона (мини-корабл€).
    */
    void pushDroneMesh( const uid_t& fromThrone );
    void pushDroneBillboard( const uid_t& fromThrone );

};

