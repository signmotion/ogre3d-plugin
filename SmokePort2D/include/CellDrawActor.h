#pragma once

#include "Actor.h"
#include <Ogre.h>


/**
* Сообщение актора.
* Визуализировать содержимое, представленное матрицей.
*/
template< typename C >
struct AllDrawMessage {

    // @see Пояснение > Port.h, CellPort.h
    Ogre::SceneManager* sm;

    /**
    * Коэффициент масштабирования.
    */
    const float scale;

    /**
    * Матрица и её размеры.
    */
    const C* content;
    const size_t N;
    const size_t M;

    /**
    * Префиксы для построения образов.
    * См. реализацию allDrawHandler().
    */
    const std::string prefixMaterial;

    /**
    * Обновлять образ при каждом обращении (true) или нарисовать лишь
    * один раз (false).
    */
    const bool update;


    inline AllDrawMessage(
        Ogre::SceneManager* sm,
        float scale,
        C* content,
        size_t N, size_t M,
        const std::string& prefixMaterial,
        bool update
    ) :
        sm( sm ),
        scale( scale ),
        content( content ),
        N( N ), M( M ),
        prefixMaterial( prefixMaterial ),
        update( update )
    {
        assert( sm );
        assert( (scale > 0.001f) && "Масштаб для образа сетки указать необходимо." );
        assert( content && "Содержимое должно быть указано." );
        assert( ((N > 0) && (M > 0)) && "Пропущен размер матрицы." );
        assert( !prefixMaterial.empty() && "Без префикса не сможем найти материал." );
    }
        
};







/**
* Сообщение актора.
* Необходимо нарисовать ячеистую сетку.
* Отрисовка сетки происходит один раз.
*/
struct GridDrawMessage {

    // @see Пояснение > Port.h, CellPort.h
    Ogre::SceneManager* sm;

    /**
    * Коэффициент масштабирования.
    */
    const float scale;

    /**
    * Количество ячеек в сетке.
    */
    const size_t size;


    inline GridDrawMessage(
        Ogre::SceneManager* sm,
        float scale,
        size_t size
    ) :
        sm( sm ),
        scale( scale ),
        size( size )
    {
        assert( sm );
        assert( (scale > 0.001f) && "Масштаб для образа сетки указать необходимо." );
        assert( (size >= 1) && "Сетки без ячеек не бывает." );
    }
        
};






/**
* Сообщение актора.
* Визуализировать ячейку с указанными координатами.
*/
template< typename C >
struct DrawMessage {

    // @see Пояснение > Port.h, CellPort.h
    Ogre::SceneManager* sm;

    /**
    * Коэффициент масштабирования.
    */
    const float scale;

    /**
    * Содержимое ячейки для показа.
    */
    const C cell;

    /**
    * Префиксы для построения образов.
    * См. реализацию drawHandler().
    */
    const std::string prefixMaterial;


    inline DrawMessage(
        Ogre::SceneManager* sm,
        float scale,
        const C& cell,
        const std::string& prefixMaterial,
        bool update
    ) :
        sm( sm ),
        scale( scale ),
        cell( cell),
        prefixMaterial( prefixMaterial )
    {
        assert( sm );
        assert( (scale > 0.001f) && "Масштаб для образа сетки указать необходимо." );
        assert( !prefixMaterial.empty() && "Без префикса не сможем найти материал." );
    }
        
};





/**
* Актор.
* Рисует на холсте. Холст разбит на ячейки.
*/
template< typename C >
class CellDrawActor : public Actor {
public:
    inline CellDrawActor() {
        RegisterHandler( this, &CellDrawActor::allDrawHandler );
        RegisterHandler( this, &CellDrawActor::gridDrawHandler );
        RegisterHandler( this, &CellDrawActor::drawHandler );

    }



    virtual inline ~CellDrawActor() {
    }



    /**
    * @see allDrawHandler()
    * Без потоков.
    */
    static void allDraw( const AllDrawMessage< C >& m );



    /**
    * @see gridDrawHandler()
    * Без потоков.
    */
    static void gridDraw( const GridDrawMessage& m );



protected:
    /**
    * Визуализирует указанное содержимое.
    * Годится только для очень небольших размеров матрицы.
    */
    void allDrawHandler(
        const AllDrawMessage< C >& m,
        const Theron::Address from
    );



    /**
    * Рисует ячеистую сетку.
    */
    void gridDrawHandler(
        const GridDrawMessage& m,
        const Theron::Address from
    );



    /**
    * Визуализирует ячейку с указанными координатами.
    */
    void drawHandler(
        const DrawMessage< C >& m,
        const Theron::Address from
    );

};
