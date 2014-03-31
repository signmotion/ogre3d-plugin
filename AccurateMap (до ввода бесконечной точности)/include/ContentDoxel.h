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
* @see Doxel
*/
struct ContentDoxel {
    inline ContentDoxel() {
    }



    virtual inline ~ContentDoxel() {
    }



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
* Управляет контентом докселя.
* Не хранит дублирующийся контент, возвращает константную ссылку на него.
*
* Сокр. от Content Doxel Manager.
*
* @todo Добавить удаление редко исп. контента.
*/
class CDManager {
protected:
    inline CDManager() {
    }


public:
    static inline CDManager* singleton() {
        if ( instance ) {
            return instance;
        }
        instance = new CDManager();
        return instance;
    }



    virtual inline ~CDManager() {
        std::cout << "~CDManager()" << std::endl;
    }



    /**
    * @return Количество хранимого контента.
    */
    inline size_t count() const {
        return c.size();
    }



    /**
    * @return Очистка сохранённого в памяти контента.
    */
    inline void clear() {
        c.clear();
    }


    
    /**
    * @return Ссылка на полученный контент внутри менеджера. Если такого
    *         содержания ещё нет, добавляет содержание в хранилище.
    */
    inline const std::shared_ptr< ContentDoxel >  content( const ContentDoxel& cnt ) {
        // @todo optimize Избавиться от постоянного вызова clone().
        auto nc = std::shared_ptr< ContentDoxel >( cnt.clone() );
        auto itr = c.find( nc );
        if (itr != c.end()) {
            return *itr;
        }
        itr = c.insert( nc ).first;
        return *itr;
    }





protected:
    /**
    * Функторы для определения уникального содержания.
    */
    struct FHash {
        size_t operator()( const std::shared_ptr< ContentDoxel > a ) const { 
            assert( a );
            return a->hash();
        }
    };

    struct FEquals {
        bool operator()( const std::shared_ptr< ContentDoxel > a, const std::shared_ptr< ContentDoxel > b ) const {
            assert( a );
            assert( b );
            return a->equals( b );
        }
    };

    /*
    * Набор уникального содержания, когда-либо полученного менеджером.
    */
    std::unordered_set<
        std::shared_ptr< ContentDoxel >,
        FHash,
        FEquals
    > c;



    /**
    * Синглетон.
    */
    static CDManager* instance;

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
