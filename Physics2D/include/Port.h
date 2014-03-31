#pragma once

#include <Ogre.h>
#include "default.h"
#include "Safethread.h"


/**
* ���� - ����������, ����������� ������������ �������������� � ����, ������
* ��� �������� � �������� ����������� ��������� � ������������
* ���������������� � ��������� ��������������.
*
* ������ 1. ������� ������� ����� ���� ������������ ������� � ���������
* ������, ����������������� �������� ������ ���������� ���������. ����� ����,
* ������� ����� ��������� ������� ����� ���������� �� ��� �����, ��� ������
* ��������� ���������, ������, �� ����� ��������.
*
* ������ 2. ����� ������������� ��������������� ����, ��� ����� ���� ������
* �� ������, ������ �� ������� ����� ��������� ���������� � ���������� ����
* � ���, ������������ � ��������, ������������ ������� ��������������� ����.
*
* ������ 3. ���� ���������� ����������� �������� � ������� ����, �� ����������
* ������� � ������ ��� ������� � ������� ������ �� ����������� ��������������
* � �������� �������. �.�. ����� ���� ������ ���� ��� ������� ������� ��������,
* �� ����� ������ � �������� ����������.
*
* ��������� �������� ������ �����:
*   - ������ ������������� (���������) ��� ���������
*   - ������ ��������
*   - ������ �����
* => ����������� ������������� ��������.
*
* @see RealtimePort
* @see ������ Sample_SmokePort2D
*/
class Port : public Safethread {
public:
    /**
    * ���� ����� ����� ���� ���������������. ���� ����� ���������� ���,
    * ��������� ����� ��������.
    */
    Port( Ogre::SceneManager* sm );



    virtual inline ~Port() {
        std::cout << "~Port()" << std::endl;
        /* - @todo ? ������.
        if ( sn ) {
            try {
                sn->removeAndDestroyAllChildren();
            } catch ( ... ) {
            }
        }
        */
    }





    /**
    * ��������� ���� � ��������� ���������.
    * �������� �� ��� ��������� ����������� ����� ������� ��� ���������� �����.
    *
    * ���� �� ����������� ��������� ������ ��� ��������� ������ / �������������,
    * �.�. ����� ��������� ����������� ����������� ��� ��������� � ������ ������
    * ������. ��������, ����� ���������� ������ � ������ ��� �� ��������
    * ��������� �� �����.
    *
    * @param tact ���������� ������, ������� ������ ���� ���������� ��� �����.
    *        ����� ���� ������� (��������: "������ ��������������� ����������").
    *        ����� ���� ������������� ("�������� ������ �������").
    * @param timestep �������� ������� � �������� ��� ������� ��������������.
    *        ��� ������ - ��� �������. ��� tact == 0, timestep �� �����������.
    */
    virtual void pulse( int tact, float timestep ) = 0;




    /**
    * @return ���������� ���������, ����������� � �����.
    */
    virtual size_t count() = 0;






protected:
    /**
    * �� ��� ����� ��� ������������ ����������� �����.
    */
    Ogre::SceneManager* sm;

    /**
    * ������������ �������� � ������� �������.
    *//* - �������������� ��������. �������� �� ����� ����������� �������
           ��� ���������.
    std::shared_ptr< Theron::Framework >  drawFlicker;
    Theron::ActorRef drawActor;
    */

};
