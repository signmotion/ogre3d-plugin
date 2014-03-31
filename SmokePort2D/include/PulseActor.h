#pragma once

#include "Actor.h"
#include "Port.h"


/**
* Сообщение актора.
* Информация о порте.
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
* Для перевода системы в другое состояние используем модель акторов.
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
    * В бесконечном цикле переводит порт в след. состояние.
    */
    inline void portHandler( const PortMessage& m, const Theron::Address from ) {
        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " PulseActor::portHandler() start" << std::endl;
        
        // Меняем состояние порта.
        // Для первого запуска - только визуализируем порт без изменения
        // его состояния.
        const int tact = (from == Theron::Address::Null()) ? 1 : 1;
        m.port->pulse( tact );
        
        // Информируем о выполнении
        Send( true, from );

        // Немого повременив, отправляем сообщение "Повторить!"
        // (!) Без паузы - непредсказуемо вылетает.
        Sleep( 1000 );
        Send( m, GetAddress() );

        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " PulseActor::portHandler() stop" << std::endl;
    }

};

