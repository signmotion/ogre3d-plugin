#pragma once

#include <Ogre.h>
#include <Theron/Actor.h>
#include <Theron/Framework.h>


/**
* Общий актор.
*/
class Actor : public Theron::Actor {
public:
    /**
    * Признаки, какие обработчик в данный момент выполняются.
    * Может использоваться обработчиками, чтобы перезапускать себя,
    * и/или не создавать бесполезных очередей.
    * Ответственность за поддержания карты в актуальном состоянии
    * несут сами обработчики.
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
    * Принимает сообщение о результате выполнении операции.
    */
    inline void resultHandler( const bool& m, const Theron::Address from ) {
        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " PulseActor::resultHandler() " << (m ? "true" : "false") << std::endl;
        // Сообщение принято, ничего не отвечаем, чтобы не засорять эфир
        //TailSend( true, from );
    }




    /**
    * Обработчик неизвестных актору сообщений.
    */
    inline void defaultHandler( const Theron::Address from ) {
        std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
            " (!) Message is not recognized." << std::endl;

        // Говорим адресату, что получили нечто непонятное
        TailSend( false, from );
    }

};
