#pragma once

#include <string>
#include "default.h"
#include <Ogre.h>


/**
* ����� - ����������, ������������� ������� ���������... ����-������.
*
* (!) ������ ������� ��� ������ ����� ��������.
* ����� �����:
*   - ���� PNG, �� �������� ������ ������ �������� R-������
*   - ������� - RGB(A)8
*   - ����������� - �������
*   - �������� R0 - ������������ �������
*   - �������� R127 - ������� ���� (��. Aheightmap::hm_t)
*   - �������� R255 - ������������ ������
*
*/
class Trend {
public:
    /**
    * @param source ������ ����� �������� ���������� �� ���������.
    */
    inline Trend(
        const std::string& source
    ) :
        source( source )
    {
    }


    virtual inline ~Trend() {
    }



    /**
    * @return ������, �������������� ����� (���������� �����).
    *         ������� ���������� ���� ������� ���� ������ ������������ OXY.
    */
    virtual Ogre::MeshPtr mesh(
        Ogre::SceneManager* sm
    ) const = 0;





protected:
    /**
    * �������� ���������� ��� ������.
    */
    const std::string source;

};
