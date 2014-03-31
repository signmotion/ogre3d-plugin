#pragma once

#include <Ogre.h>
#include <Theron/Actor.h>
#include <Theron/Framework.h>


/**
* ����� �����.
*/
class Actor : public Theron::Actor {
public:
    /**
    * ��������, ����� ���������� � ������ ������ �����������.
    * ����� �������������� �������������, ����� ������������� ����,
    * �/��� �� ��������� ����������� ��������.
    * ��������������� �� ����������� ����� � ���������� ���������
    * ����� ���� �����������.
    */
    //std::unordered_map< std::string, bool >  running;


public:
    inline Actor() {
        RegisterHandler( this, &Actor::resultHandler );
        SetDefaultHandler( this, &Actor::defaultHandler );
    }



    virtual inline ~Actor() {
    }



public:
    /**
    * ��������� ��������� � ���������� ���������� ��������.
    */
    inline void resultHandler( const bool& m, const Theron::Address from ) {
        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " PulseActor::resultHandler() " << (m ? "true" : "false") << std::endl;
        // ��������� �������, ������ �� ��������, ����� �� �������� ����
        //TailSend( true, from );
    }




    /**
    * ���������� ����������� ������ ���������.
    */
    inline void defaultHandler( const Theron::Address from ) {
        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " (!) Message is not recognized." << std::endl;

        // ������� ��������, ��� �������� ����� ����������
        TailSend( false, from );
    }

};
