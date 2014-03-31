#pragma once

#include "Ogre.h"


/**
* Объект для визуализации LiveBody.
*/
class ManualObjectLB :
    public Ogre::ManualObject
{
public:
    inline ManualObjectLB(
        const std::string& name
    ) :
        Ogre::ManualObject( name )
    {
    }



    inline virtual ~ManualObjectLB() {
        std::cout << "~ManualObjectLB()" << std::endl;
    }



    /**
    * Обновлять координаты подвижных точек в визуальном образе будем
    * только когда образ виден.
    */
    inline void _notifyCurrentCamera( Ogre::Camera* cam ) {

        std::cout << "ManualObjectLB::_notifyCurrentCamera" << std::endl;

        Ogre::ManualObject::_notifyCurrentCamera( cam );
    }

};

