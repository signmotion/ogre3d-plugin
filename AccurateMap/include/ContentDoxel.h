#pragma once

#include "configure.h"
#include <assert.h>
#include <iostream>
#include <unordered_set>
#include <couchdb/CouchDB.h>
#include <unordered_map>


/**
* Содержание докселя.
* Структура предназначена для наследования.
*
* Содержание - это обобщение данных более низких уровней д..
*
* @see Doxel
*/
struct ContentDoxel {
    /**
    * Типы содержания.
    *
    * @see Наследники класса.
    */
    enum type_e {
        TYPE_CONCRETE = 1,
        TYPE_SOLID
    };



    inline ContentDoxel() {
    }



    virtual inline ~ContentDoxel() {
    }



    /**
    * @return Тип содержания. Каждое содержание декларирует свой
    *         уникальный тип.
    */
    virtual type_e type() const = 0;




    /**
    * @return Клон содержания.
    */
    virtual std::shared_ptr< ContentDoxel >  clone() const = 0;



    /**
    * @return Представление содержания для сохранения в д-хранилище.
    *         Используется CouchDB.
    *
    * @see DStore
    */
    virtual CouchDB::Object json() const = 0;



    /**
    * Хеш-функция и функция сравнения содержания.
    * Необходимы для определения уникальности содержания и его сравнения.
    *
    * Для вычисления хеш-ключа можно работать с
    * http://boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    virtual size_t hash() const = 0;
    virtual bool equals( const std::shared_ptr< ContentDoxel > a ) const = 0;


};




    




/**
* Содержание 3D-докселя.
* Структура предназначена для наследования.
*
* @see Doxel
*/
struct ContentDoxel3D : public ContentDoxel {

    inline ContentDoxel3D() {
    }



    virtual inline ~ContentDoxel3D() = 0;

};




    




/**
* Конкретное содержание 3D-докселя.
*
* Это содержание более не может быть уточнено. Обычно, располагается
* на самых низких уровнях точности.
* Например, в огромном объёме воды (который представлен д.) может
* находиться корабль. Корабль здесь - "конкретное содержание". Вода -
* "цельное".
*
* @see Doxel
*/
struct ConcreteContentDoxel3D : public ContentDoxel3D {

    inline ConcreteContentDoxel3D() {
    }



    virtual inline ~ConcreteContentDoxel3D() {
    }



    /**
    * @see ContentDoxel
    */
    virtual inline type_e type() const {
        return TYPE_CONCRETE;
    }



    /**
    * @see ContentDoxel
    */
    virtual std::shared_ptr< ContentDoxel >  clone() const = 0;



    /**
    * @see ContentDoxel
    */
    virtual CouchDB::Object json() const = 0;



    /**
    * Хеш-функция и функция сравнения содержания.
    * Необходимы для определения уникальности содержания и его сравнения.
    *
    * Для вычисления хеш-ключа можно работать с
    * http://boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    virtual size_t hash() const = 0;
    virtual bool equals( const std::shared_ptr< ContentDoxel > a ) const = 0;


};




    



/**
* Цельное содержание 3D-докселя.
*
* Любое дальнейшее уточнение приведёт к формированию объёма одинаковых
* докселей.
* При отрисовке д., все элементы более низкого уровня будут
* представлены этим содержанием. Но это не исключает присутствие
* другого содержания - конкретного или цельного - на других уровнях.
*
* @see Doxel
*/
struct SolidContentDoxel3D : public ContentDoxel3D {

    inline SolidContentDoxel3D() {
    }



    virtual inline ~SolidContentDoxel3D() {
    }



    /**
    * @return Доксель по заданным координатам.
    *
    * Этот тип содержания всегда возвращает один и тот же доксель.
    */
    virtual inline Doxel3D get( const d::coord3d_t& c ) const {
        return ...
    }



    /**
    * @see ContentDoxel
    */
    virtual inline type_e type() const {
        return TYPE_SOLID;
    }



    /**
    * @return Клон содержания.
    */
    virtual std::shared_ptr< ContentDoxel >  clone() const = 0;



    /**
    * @see ContentDoxel
    */
    virtual CouchDB::Object json() const = 0;



    /**
    * Хеш-функция и функция сравнения содержания.
    * Необходимы для определения уникальности содержания и его сравнения.
    *
    * Для вычисления хеш-ключа можно работать с
    * http://boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    virtual size_t hash() const = 0;
    virtual bool equals( const std::shared_ptr< ContentDoxel > a ) const = 0;


};




    





/**
* Информация о ячейке карты.
*/
struct AboutCellCD : public ContentDoxel {

    /**
    * Код элемента, находящегося в этой ячейке.
    * Попроуем сделать константным, чтобы уменьшить ентропию при
    * манипуляции с картой. @todo optimize Производительность?
    */
    const int element;

#ifdef INCLUDE_DEBUG_INFO_IN_CONTENT
    /**
    * Введено для тестирования.
    */
    std::unordered_map< std::string, boost::any >  test;
#endif



    inline AboutCellCD(
        int element = 0
    ) :
        element( element )
    {
    }


    virtual inline ~AboutCellCD() {
    }




    virtual inline std::shared_ptr< ContentDoxel >  clone() const {
        return std::shared_ptr< ContentDoxel >( new AboutCellCD(
            element
        ) );
    }


    
    virtual inline CouchDB::Object json() const {
        CouchDB::Object o;

        o["element"] = CouchDB::cjv( element + 0 );

#ifdef INCLUDE_DEBUG_INFO_IN_CONTENT
        // Вводим дополнительную информацию, чтобы идентифицировать это доксель
        for (auto itr = test.cbegin(); itr != test.cend(); ++itr) {
            const auto field = *itr;
            o[ field.first ] = CouchDB::cjv( field.second );
        }
#endif

        return o;
    }
    



    virtual inline size_t hash() const {
        return element;
    }



    virtual inline bool equals( const std::shared_ptr< ContentDoxel > a ) const {
        assert( a );
        const auto pa = static_cast< AboutCellCD* >( a.get() );
        return (pa->element == element);
    }

};
