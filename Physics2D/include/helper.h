#pragma once

#include <Ogre.h>
#include <SdkSample.h>


/**
* Маленькие полезные методы и классы, задействованные в этом проекте.
*/
namespace helper {



/**
* Для отладки.
*
* (!) Перед вызовом большей части методов Tray всегда должен быть
*     хотя бы один вызов init().
*/
class Tray {

protected:
    inline Tray() {
    }


public:

    /**
    * Инициализация.
    */
    static inline void init(
        OgreBites::SdkTrayManager* t
    ) {
        Tray::tray = t;
    }


    /**
    * @return true, если Tray был инициализирован.
    *         Может использоваться методами перед вызовом статических
    *         методов класса Tray.
    */
    static bool initiated() {
        return  tray ? true : false;
    }



    virtual inline ~Tray() {
    };




    /**
    * Печатает имя и значение на панель.
    * @require Вызов init().
    */
    static void p( const std::string& name, const std::string& value );




private:
    /**
    * Интерфейс для работы с визуальным представлением данных.
    * Например, динамически наполняемая панель.
    * Должен быть проинициализирован перед вызовов методов класса.
    */
    static OgreBites::SdkTrayManager* tray;

};


}
