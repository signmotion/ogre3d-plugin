#pragma once

#include <Ogre.h>
#include <SdkSample.h>
#include "default.h"


/**
* Маленькие полезные методы и классы, задействованные в этом проекте.
*/
namespace helper {


inline bool borderRight( int x, int z, size_t N, size_t M ) {
    return (x == ((int)N - 1));
}

inline bool borderBottom( int x, int z, size_t N, size_t M ) {
    return (z == 0);
}

inline bool borderLeft( int x, int z, size_t N, size_t M ) {
    return (x == 0);
}

inline bool borderTop( int x, int z, size_t N, size_t M ) {
    return (z == ((int)M - 1));
}


/**
* @return Признак, что указанная точка лежит внутри матрицы N x M.
* == !outsideMatrix()
*/
inline bool insideMatrix( int x, int z, size_t N, size_t M ) {
    return (
        (x >= 0) && (x < (int)N)
     && (z >= 0) && (z < (int)M)
    );
}



/**
* @return Признак, что указанная точка лежит вне матрицы N x M.
* == !insideMatrix()
*/
inline bool outsideMatrix( int x, int z, size_t N, size_t M ) {
    return (
        (z < 0) || (z >= (int)M)
     || (x < 0) || (x >= (int)N)
    );
}


inline bool outsideMatrix( const coordInt2d_t& c, const sizeInt2d_t& size ) {
    return (c < ZEROINT2D) || (c >= size);
}



/**
* @return Величина смещения в сторону указанной ячейки.
*         Если превышены границы мира, возвращает (0; 0).
*
* Расположение ячеек:
*   M
*   .
*   .
*   8   1   5
*   4   0   2
*   7   3   6  .  .  N
*
* Такое расположение выбрано (по часовой, по граням, по углам), чтобы было
* удобно пробегать по сторонам и углам ячейки, в зависимости от потребностей.
* Оси матрицы совпадают с осями координат.
*
* isc - сокр. 'i' shift cell
*/
inline coordInt2d_t isc( size_t cell ) {
    coordInt2d_t c;
    switch ( cell ) {
        case 1:  c = coordInt2d_t(  0, +1 );  break;
        case 2:  c = coordInt2d_t( +1,  0 );  break;
        case 3:  c = coordInt2d_t(  0, -1 );  break;
        case 4:  c = coordInt2d_t( -1,  0 );  break;
        case 5:  c = coordInt2d_t( +1, +1 );  break;
        case 6:  c = coordInt2d_t( +1, -1 );  break;
        case 7:  c = coordInt2d_t( -1, -1 );  break;
        case 8:  c = coordInt2d_t( -1, +1 );  break;
        case 0:  c = coordInt2d_t(  0,  0 );  break;
        default: throw "Not correct index of cell. See description for this method.";
    }

    return c;
}





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
