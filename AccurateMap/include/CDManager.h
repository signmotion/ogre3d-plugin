#pragma once

#include "ContentDoxel.h"


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
