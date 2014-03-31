#pragma once

#include <Ogre.h>
#include <Newton.h>
#include "default.h"


// Структуры для быстрых расчётов.

// Значения, отмеченные префиксом '=', рассчитываются на основе других значений и
// предназначены, в первую очередь для ускорения расчётов.


#if 0
// - Все сущности хранятся в планах порта (физическом, визуальном). Нет
//   надобности использовать дополнительные структуры.
/**
* "Быстрая структура данных" для использования в портах.
*/
struct FastData {
};



/**
* Сущность для RealTimePort.
* Информация о самой сущности распределена по образам порта. Эта структура
* служит лишь картой для быстрого поиска сущностей по UID.
*/
struct EntityRTPortFD : public FastData {

    inline EntityRTPortFD() :
        //uid( "" ),
        visualImage( nullptr ),
        physicsImage( nullptr )
    {
    }

    /**
    * UID сущности.
    *//* - Храним как ключ в карте порта.
    uid_t uid;
    */

    /**
    * Визуальный образ сущности.
    */
    Ogre::SceneNode* visualImage;

    /**
    * Физический образ сущности.
    */
    NewtonBody* physicsImage;

};
#endif

// @todo Структура оставлена, чтобы не переписывать шаблонизированные классы. Убрать.
struct EntityRTPortFD {
};




/**
* Базовая структура для инициации портов сущностями.
* Размеры и координаты задаются в см, т.к. масштабирование и камера Ogre
* показали себя не лучшим образом при значениях меньших 1.0.
*/
struct InitEntity {

    enum type_e {
        TYPE_UNDEFINED = 0,
        TYPE_FLAT,
        TYPE_VOLUME
    };

    enum form_e {
        FORM_UNDEFINED = 0,
        // Прямоугольник или параллелипипед
        FORM_BOX,
        // Круг или сфера
        FORM_SPHERE
    };

    // Тип сущности
    const type_e type;
    // Форма сущности
    const form_e form;

    //  UID сущности
    uid_t uid;
    // Размеры сущности, cм
    // Размеры для объёмных и плоских форм - общие
    float boundingRadius;
    size3d_t size;
    // Масса сущности, кг
    float mass;
    // Координаты сущности, cм
    coord2d_t coord;
    // Визуальный образ сущности
    std::string model;
    // Сила, прилагаемая в данный момент к сущности, Н
    vector2d_t force;


    /**
    * @return true, если параметры инициализации заданы корректно.
    */
    virtual bool correct() const = 0;


protected:
    inline InitEntity(
        type_e type, form_e form
    ) :
        type( type ),
        form( form ),
        uid( "" ),
        boundingRadius( 0.0f ),
        size( ZERO3D ),
        mass( 0.0f ),
        coord( ZERO2D ),
        model( "" ),
        force( ZERO2D )
    {
    }

};




/**
* Образ представлен плоским изображением.
*/
struct FlatInitEntity : public InitEntity {
    inline FlatInitEntity( form_e form ) : InitEntity( TYPE_FLAT, form ) {
    }

    virtual inline bool correct() const {
        return ( ((form == InitEntity::FORM_BOX)  && !emptyAny( size ))
            || ((form == InitEntity::FORM_SPHERE) && !empty( boundingRadius ))
        ) && !uid.empty()
          && !model.empty();
    }
};



/**
* Образ представлен 3D-изображением.
*/
struct VolumeInitEntity : public InitEntity {
    inline VolumeInitEntity( form_e form ) : InitEntity( TYPE_VOLUME, form ) {
    }

    virtual inline bool correct() const {
        return ( ((form == InitEntity::FORM_BOX)  && !emptyAny( size ))
            || ((form == InitEntity::FORM_SPHERE) && !empty( boundingRadius ))
        ) && !uid.empty()
          && !model.empty();
    }
};
