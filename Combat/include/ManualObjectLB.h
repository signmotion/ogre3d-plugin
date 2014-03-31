#pragma once

#include "Ogre.h"


/**
* ������ ��� ������������ LiveBody.
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
    * ��������� ���������� ��������� ����� � ���������� ������ �����
    * ������ ����� ����� �����.
    */
    inline void _notifyCurrentCamera( Ogre::Camera* cam ) {

        std::cout << "ManualObjectLB::_notifyCurrentCamera" << std::endl;

        Ogre::ManualObject::_notifyCurrentCamera( cam );
    }

};

