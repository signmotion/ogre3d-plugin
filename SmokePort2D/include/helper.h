#pragma once

#include <Ogre.h>
#include <SdkSample.h>
#include "struct.h"
#include "default.h"


/**
* Маленькие полезные методы и классы.
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





/**
* @return Одномерная координата по заданным двумерным.
*          -1, если одна из координат выходит за границы списка.
*
* ic - сокр. 'i' cell
*/
inline int ic(
    int x, int z,
    size_t N, size_t M
) {
    return (
        outsideMatrix( x, z, N, M )
          ? -1
          : (x + z * N)
    );
}







/**
* @return Одномерный индекс указанной (относительно текущей) ячейки.
*         Если ячейка выходит за границы мира, возвращает -1.
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
inline int isc(
    int x, int z,
    int dx, int dz,
    size_t N, size_t M
) {
    const int tx = x + dx;
    const int tz = z + dz;
    return (
        ( outsideMatrix( tx, tz, N, M ) || outsideMatrix( x, z, N, M ) )
          ? -1
          : (tx + tz * N)
    );
}




/**
* @see near()
*/
inline int nearCell(
    int x, int z,
    size_t cell,
    size_t N, size_t M
) {
    switch ( cell ) {
        case 1:  return isc( x, z,  0, +1, N, M );
        case 2:  return isc( x, z, +1,  0, N, M );
        case 3:  return isc( x, z,  0, -1, N, M );
        case 4:  return isc( x, z, -1,  0, N, M );
        case 5:  return isc( x, z, +1, +1, N, M );
        case 6:  return isc( x, z, +1, -1, N, M );
        case 7:  return isc( x, z, -1, -1, N, M );
        case 8:  return isc( x, z, -1, +1, N, M );
        case 0:  return ic( x, z, N, M );
    }

    return -1;
}






/**
* Вычисляет 'uid' и 'pressure' для ячейки газа с учётом соседей.
* Вынесено сюда, чтобы облегчить переход на OpenCL.
*/
inline void calc(
    GasFD* content,
    int x, int z,
    size_t N, size_t M,
    size_t* i, size_t* uid, float* pressure
) {
    // Проходим по ячейкам, считаем общее давление
    *i = nearCell( x, z, 0, N, M );
    assert(*i != -1);
    float allPressure = content[*i].pressure;
    for (size_t k = 1; k <= 8; ++k) {
        const int q = nearCell( x, z, k, N, M );
        if (q != -1) {
            allPressure += content[q].pressure;
        }
    }
    // Результирующее давление в ячейке
    *pressure = allPressure / 8.0f;

    // UID в ячейке может измениться
    *uid = (*pressure < 0.01f) ? 0 : content[*i].uid;
}






/**
* Находит индекс биллборда, который позиционирован по указанной
* координате. Координаты биллборда округляются до целого с учётом масштаба
* путём приведения к (int).
* -1 если биллборд по координате не найден.
*/
inline int index( const Ogre::BillboardSet& bs, const coordInt2d_t& coord, float scale ) {

    for (int index = 0; index < bs.getNumBillboards(); ++index) {
        const auto b = bs.getBillboard( index );
        const Ogre::Vector3 c = b->getPosition() / scale;
        if ( (coord.get<0>() == (int)c.x) && (coord.get<1>() == (int)c.z) ) {
            return index;
        }
    }

    return -1;
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
