#pragma once

#include "Port.h"


/**
* Порт, представляющий собой мир, разбитый на квадратные ячейки.
* Ячейка - неделимая единица со своими характеристиками, декларированных
* типом шаблона.
*/
template< typename C >
class CellPort :
    public Port
{
public:
    /**
    * @param size Размер порта в ячейках.
    *        (!) Т.к. расчёты ведутся через OpenCL, размеры должны соблюдать
    *        некторорые условия (см. тело конструктора). Введено, чтобы
    *        ускорить моделирование.
    * @param scale Масштаб порта в метрах. @todo Добавить время?
    */
    CellPort(
        Ogre::SceneManager* sm,
        float visualScale,
        const sizeInt2d_t& size,
        float scale
    );



    inline virtual ~CellPort() {
        delete contentT;
        delete content;
    }




    /**
    * @return Количество ячеек в порту.
    */
    inline size_t count() const {
        return NM;
    }



    /**
    * Заполняет ячейку состояния.
    */
    inline void set( const coordInt2d_t& coord, const C& c ) {
        const size_t i = helper::ic( coord.get<0>(), coord.get<1>(), N, M );
        content[i] = c;
    }






protected:
    /**
    * Размер порта в ячейках.
    * Уходим от структуры, чтобы уменьшить кол-во рассчётов в pulse().
    */
    const size_t N;
    const size_t M;
    const size_t NM;

    /**
    * Масштаб порта в метрах.
    * Сколько метров содержится в 1 ячейке.
    */
    const float scale;

    /**
    * Содержимое порта (мир).
    */
    C* content;

    /**
    * Вспомогательная матрица для моделирования.
    * Размером соотв. 'content'. Не инициализирована. Может использоваться
    * методами по своему усмотрению.
    */
    C* contentT;


    /**
    * Взаимодействие с OpenCL.
    * См. инициализацию в конструкторе.
    *//* - Не используется. См. прим. в Port.h
    cl_device_id* devices;
    cl_context context;
    cl_command_queue commandQueue;
    cl_kernel kernel;
    cl_program program;
    */

};

