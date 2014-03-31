#pragma once

#include "Actor.h"
#include "Port.h"


/**
* ��������� ������.
* ���������� � �����.
*/
struct PortMessage {

    Port* port;


    inline PortMessage( Port*  port ) :
        port( port )
    {
        assert( port );
    }
        
};






/**
* ��� �������� ������� � ������ ��������� ���������� ������ �������.
*/
class PulseActor : public Actor {
public:
    inline PulseActor() {
        RegisterHandler( this, &PulseActor::portHandler );
    }



    virtual inline ~PulseActor() {
    }



public:
    /**
    * � ����������� ����� ��������� ���� � ����. ���������.
    */
    inline void portHandler( const PortMessage& m, const Theron::Address from ) {
        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " PulseActor::portHandler() start" << std::endl;
        
        // ������ ��������� �����.
        // ��� ������� ������� - ������ ������������� ���� ��� ���������
        // ��� ���������.
        const int tact = (from == Theron::Address::Null()) ? 1 : 1;
        m.port->pulse( tact );
        
        // ����������� � ����������
        Send( true, from );

        // ������ ����������, ���������� ��������� "���������!"
        // (!) ��� ����� - �������������� ��������.
        Sleep( 1000 );
        Send( m, GetAddress() );

        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " PulseActor::portHandler() stop" << std::endl;
    }

};

