#pragma once

#include <string>
#include "default.h"
#include <Ogre.h>


/**
* Тренд - абстракция, декларирующая правила поведения... чего-угодно.
*
* (!) Формат образов для тренда чётко определён.
* Карта высот:
*   - файл PNG, из которого берётся только значение R-канала
*   - палитра - RGB(A)8
*   - изображение - квадрат
*   - значение R0 - максимальная глубина
*   - значение R127 - уровень моря (см. Aheightmap::hm_t)
*   - значение R255 - максимальная высота
*
*/
class Trend {
public:
    /**
    * @param source Каждый тренд получает информацию из источника.
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
    * @return Модель, представляющая тренд (визуальный образ).
    *         Нормаль определяет угол наклона этой модели относительно OXY.
    */
    virtual Ogre::MeshPtr mesh(
        Ogre::SceneManager* sm
    ) const = 0;





protected:
    /**
    * Источник информации для тренда.
    */
    const std::string source;

};
