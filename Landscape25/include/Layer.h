#pragma once

#include "Ogre.h"
#include "FeatureLayer.h"
#include "Trend.h"

struct Biome;


/**
* Слой - то, что отображается в мире. Форма слоя определяется трендами.
*
* @param biome Свойства биома определяют свойства слоя. См.
*        конструкторы Layer() и Biome().
*
* @see Trend
*/
struct Layer {

    /**
    * Характеристика слоя.
    */
    FeatureLayer feature;



    inline Layer(
        size_t length,
        size_t surfaceComplexity,
        size_t countGradient,
        int seed = static_cast< int >( std::time( nullptr ) )
        // Другие характеристики биома иниц. знач. по умолчанию (см. тело)
    ) {
        // Каждый слой должен сам определять свой характер. Например, дюна должна
        // так менять поверхности в своих трендах, чтобы не создавать острых
        // углов. И при формировании характера слоя мы должны руководствоваться
        // настройками из биома, но (!) в самом слое необходимо предусмотреть
        // вариант, когда настройки биома выходят за допустимые пределы.
        assert( (length > 0) && "Протяжённость слоя должна быть указана." );
        assert( (length <= INT_MAX) && "Протяжённость слоя не может превышать INT_MAX." );
        feature.length = length;

        feature.surfaceComplexity = surfaceComplexity;

        assert( (countGradient > 0) && "Не указано количество точек перепада слоя." );
        assert( (countGradient >= 2) && "Для слоя должно быть указано как минимум 2 точки перепада." );
        feature.countGradient = countGradient;

        //feature.natural = by default
        
        feature.seed = seed;
    }



    /**
    * @todo (!) Не проверяется корректность получаемых данных.
    */
    /* - Не используем.
    inline Layer( const FeatureLayer& feature ) :
        feature( feature )
    {}
    */



    virtual inline ~Layer() {
    }




    /**
    * Рисует образ слоя.
    */
    inline void draw(
        Ogre::SceneManager* sm,
        Ogre::SceneNode* sn,
        const show_t& show
    ) const {
        const auto gp = gradientPoint();
        if ( show.gradient ) {
            gradientPointDraw( sm, sn, gp );
        }

        const auto sl = surfaceLine( gp );
        if ( show.surface ) {
            surfaceLineDraw( sm, sn, sl.second );
        }
        if ( show.trend ) {
            trendDraw( sm, sn, sl.first );
        }
    }



    virtual void gradientPointDraw(
        Ogre::SceneManager* sm,
        Ogre::SceneNode* sn,
        const surface2d_t& gp
    ) const;


    virtual void surfaceLineDraw(
        Ogre::SceneManager* sm,
        Ogre::SceneNode* sn,
        const surface2d_t& sl
    ) const;


    virtual void trendDraw(
        Ogre::SceneManager* sm,
        Ogre::SceneNode* sn,
        const trend2d_t&  tr
    ) const;




    /**
    * @return Сгенерированные точки перепада для этого слоя. Используется
    *         для грубой примерки поверхности.
    *
    * @see Значения параметров в Biome.
    * @see surfaceLine()
    */
    virtual surface2d_t  gradientPoint() const;



    /**
    * @return Детализированная линия поверхности для этого слоя.
    *
    * @param gp Точки перепада.
    *
    * Координаты коробки указываются в той же системе координат,
    * что и линия поверхности.
    *
    * @see Значения параметров в Biome.
    * @see gradientPoint()
    */
    virtual std::pair< trend2d_t, surface2d_t >  surfaceLine(
        const surface2d_t&  gp
    ) const = 0;

    



protected:
    /**
    * @return Тренд, который наилучшим образом
    *         приближает 'current' к 'aim'.
    *
    * Смотрим именно натуральные тренды: т.к. могут существовать тренды,
    * которые кардинально меняют свою форму (свой градиент) в зависимости
    * от 'seed'.
    */
    virtual std::shared_ptr< Trend >  bestNaturalTrend(
        const coord2d_t& current,
        const coord2d_t& aim
    ) const;





protected:
    /**
    * Известные слою тренды.
    * Инициируется потомками. Характеристики тренда используются при
    * построении новых трендов.
    */
    std::vector< std::pair< std::string, FeatureTrend > >  tr;

};









/**
* Слой, имеющий форму дюны.
* Дюна ступает мягко.
* Она знает свои тренды:
*   - Яма
*   - Спуск
*   - Подъём
*   - Зыбь
* Другими словами, дюна знает, как *может* выглядеть, чтобы в ней узнавали дюну.
*/
struct DunaLayer :
    public Layer
{
    /**
    * @see Layer / прим. к конструктору.
    */
    DunaLayer(
        size_t length,
        size_t surfaceComplexity,
        size_t countGradient,
        int seed = static_cast< int >( std::time( nullptr ) )
    );


    /* - Не используем.
    inline DunaLayer( const FeatureLayer& feature ) :
        Layer( feature )
    {
        init( this );
    }
    */

        
    virtual inline ~DunaLayer() {
    }



    /**
    * @see Layer
    */
    virtual std::pair< trend2d_t, surface2d_t >  surfaceLine(
        const surface2d_t&  gp
    ) const;


};
