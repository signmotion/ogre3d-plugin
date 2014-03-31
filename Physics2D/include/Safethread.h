#pragma once

#include <assert.h>


/**
* Интерфейс для реализации потокобезопасных классов.
*
* (!) Каждый метод класса, который изменяет состояние внешних ресурсов,
* должен обеспечить вызов lock() или waitFreeSetLock() в начале работы
* и unlock() по завершении.
*/
class Safethread {
public:
    inline Safethread() :
        busy( 0 )
    {
    }



    inline ~Safethread() {
    }



    /**
    * @return Класс занят? == !isFree()
    */
    virtual inline bool isBusy() const {
        return (busy > 0);
    }



    /**
    * @return Класс свободен? == !isBusy()
    */
    virtual inline bool isFree() const {
        return (busy == 0);
    }



    /**
    * Ждёт, пока объект не скажет, что он "свободен" (пока
    * не будет вызван метод unlock() ).
    */
    virtual inline void waitFree() const {
        while (busy > 0)
        { };
        // @todo Добавить контроль ожидания, чтобы отлавливать ошибки.
    }



    /**
    * Ждёт, пока объект не скажет, что он "свободен", устанавливает
    * для него статус "занят".
    */
    virtual inline void waitFreeSetLock() const {
        waitFree();
        lock();
    }



// Контролировать занятость могут внешние объекты
public:

    /**
    * Устанавливает объекту статус "занят".
    *
    * @see assert() в теле метода
    */
    virtual inline void lock() const {
        ++busy;
        assert( (busy <= 1)
          && "Попытка организовать многоуровневую блокировку. Сложно. Небезопасно. Работайте проще." );
    }



    virtual inline void unlock() const {
        --busy;
        assert( (busy >= 0) && "Метод вызван без открывающего lock()." );
    }



private:
    /**
    * Декларируем признак "занят".
    * Представлен как счётчик, чтобы обеспечит контроль при попытках
    * организовать многуровневые блокировоки.
    *   0 - класс "свободен".
    *   1 - класс "занят".
    *   Прочие значения - ошибка.
    */
    mutable int busy;
};

