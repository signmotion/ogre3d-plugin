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


inline bool insideMatrix( int x, int y, int z, size_t N, size_t M, size_t L ) {
    return (
        (x >= 0) && (x < (int)N)
     && (y >= 0) && (y < (int)M)
     && (z >= 0) && (z < (int)L)
    );
}


inline bool insideMatrix( const d::coordInt2d_t& c, const d::sizeInt2d_t& size ) {
    return insideMatrix(
        c.get<0>(),    c.get<1>(),
        size.get<0>(), size.get<1>()
    );
}


inline bool insideMatrix( const d::coordInt3d_t& c, const d::sizeInt3d_t& size ) {
    return insideMatrix(
        c.get<0>(),    c.get<1>(),    c.get<2>(),
        size.get<0>(), size.get<1>(), size.get<2>()
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


inline bool outsideMatrix( const d::coordInt2d_t& c, const d::sizeInt2d_t& size ) {
    return outsideMatrix(
        c.get<0>(),    c.get<1>(),
        size.get<0>(), size.get<1>()
    );
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
inline d::coordInt2d_t isc2D( size_t cell ) {
    d::coordInt2d_t c;
    switch ( cell ) {
        case 1:  c = d::coordInt2d_t(  0, +1 );  break;
        case 2:  c = d::coordInt2d_t( +1,  0 );  break;
        case 3:  c = d::coordInt2d_t(  0, -1 );  break;
        case 4:  c = d::coordInt2d_t( -1,  0 );  break;
        case 5:  c = d::coordInt2d_t( +1, +1 );  break;
        case 6:  c = d::coordInt2d_t( +1, -1 );  break;
        case 7:  c = d::coordInt2d_t( -1, -1 );  break;
        case 8:  c = d::coordInt2d_t( -1, +1 );  break;
        case 0:  c = d::coordInt2d_t(  0,  0 );  break;
        default: throw "Not correct index of cell. See description for this method.";
    }

    return c;
}



/**
* @see isc2D
*
* Для 3D-пространства.
* Принцип нумерациии ячеек. Смотрим по оси OY на куб 3x3x3. Ось Z направлена
* вверх. Снимаем средний (центральный) 2D слой - номеруем его согласно isc2D().
* Снимаем ближний - номеруем также. Снимаем дальний слой - аналогично.
*
  Нумерация:
    центральный 2D слой
    8   1   5
    4   0   2
    7   3   6

    ближний 2D слой
    17   10   14
    13    9   11
    16   12   15

    дальний 2D слой
    26   19   23
    22   18   20
    25   21   24
*
*/
inline d::coordInt3d_t isc3D( size_t cell ) {
    d::coordInt3d_t c;
    switch ( cell ) {
        // центральный 2D слой
        case 0:  c = d::coordInt3d_t(  0,  0,  0 );  break;
        case 1:  c = d::coordInt3d_t(  0,  0, +1 );  break;
        case 2:  c = d::coordInt3d_t( +1,  0,  0 );  break;
        case 3:  c = d::coordInt3d_t(  0,  0, -1 );  break;
        case 4:  c = d::coordInt3d_t( -1,  0,  0 );  break;
        case 5:  c = d::coordInt3d_t( +1,  0, +1 );  break;
        case 6:  c = d::coordInt3d_t( +1,  0, -1 );  break;
        case 7:  c = d::coordInt3d_t( -1,  0, -1 );  break;
        case 8:  c = d::coordInt3d_t( -1,  0, +1 );  break;

        // ближний 2D слой
        case  9:  c = d::coordInt3d_t(  0, -1,  0 );  break;
        case 10:  c = d::coordInt3d_t(  0, -1, +1 );  break;
        case 11:  c = d::coordInt3d_t( +1, -1,  0 );  break;
        case 12:  c = d::coordInt3d_t(  0, -1, -1 );  break;
        case 13:  c = d::coordInt3d_t( -1, -1,  0 );  break;
        case 14:  c = d::coordInt3d_t( +1, -1, +1 );  break;
        case 15:  c = d::coordInt3d_t( +1, -1, -1 );  break;
        case 16:  c = d::coordInt3d_t( -1, -1, -1 );  break;
        case 17:  c = d::coordInt3d_t( -1, -1, +1 );  break;

        // дальний 2D слой
        case 18:  c = d::coordInt3d_t(  0, +1,  0 );  break;
        case 19:  c = d::coordInt3d_t(  0, +1, +1 );  break;
        case 20:  c = d::coordInt3d_t( +1, +1,  0 );  break;
        case 21:  c = d::coordInt3d_t(  0, +1, -1 );  break;
        case 22:  c = d::coordInt3d_t( -1, +1,  0 );  break;
        case 23:  c = d::coordInt3d_t( +1, +1, +1 );  break;
        case 24:  c = d::coordInt3d_t( +1, +1, -1 );  break;
        case 25:  c = d::coordInt3d_t( -1, +1, -1 );  break;
        case 26:  c = d::coordInt3d_t( -1, +1, +1 );  break;

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
