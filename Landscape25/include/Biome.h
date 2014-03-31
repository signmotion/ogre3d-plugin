#pragma once

#include "FeatureBiome.h"
#include "Layer.h"

struct Layer;


/**
* Характеризует часть мира.
* Является характеристикой над слоями (Layer прислушивается к Biome).
*/
struct Biome {

    /**
    * Характеристика биома.
    */
    FeatureBiome feature;



    /**
    * @param См. прим. в свойствах Biome.
    *
    * @todo optimize ? Передавать размерность в template<>.
    */
    inline Biome(
        size_t length,
        size_t depth,
        size_t surfaceComplexity,
        size_t countLayer,
        //const boost::numeric::interval< int >&  fatLayer
        size_t countGradient,
        int seed = static_cast< int >( std::time( nullptr ) )
        // Другие характеристики биома иниц. знач. по умолчанию (см. тело)
    ) {
        assert( (length > 0) && "Протяжённость биома должна быть указана." );
        assert( (length <= INT_MAX) && "Протяжённость биома не может превышать INT_MAX." );
        assert( (countLayer > 0) && "Не указано количество слоёв." );
        //assert( (fatLayer.lower() < fatLayer.upper()) && "Проверьте диапазон [глубина; высота]." );
        assert( (countGradient > 0) && "Не указано количество точек перепада." );
        assert( (countGradient >= 2) && "Для биома должно быть указано как минимум 2 точки перепада." );

        feature.length = length;
        feature.depth = depth;
        feature.countLayer = countLayer;
        feature.elevation = 0;
        feature.altitude = 0;
        feature.surfaceComplexity = surfaceComplexity;
        feature.countGradient = countGradient;
        //feature.natural = by default
        feature.seed = seed;
    }



    virtual inline ~Biome() {
    }



    /**
    * @return Расстояние между слоями.
    */
    inline size_t distanceLayer() const {
        return (size_t)( feature.depth / feature.countLayer );
    }



    /**
    * @return Набор слоёв, созданных для этого биома.
    *
    * (!) Каждый вызов метода даёт конфигурацию слоёв согласно 'seed' биома.
    */
    virtual std::vector< std::shared_ptr< Layer > >  layer() = 0;




    /**
    * Инициирует генераторы случ. чисел для создания одинаковой
    * конфигурации биома.
    */
    void initGenerator();




protected:
    /**
    * Известные биому слои.
    * Определяется названием потомков Layer.
    */
    std::vector< std::string >  lr;
    
};








/**
* Пустыня. Песок, дюны.
*/
struct SandDesertBiome :
    public Biome
{
    SandDesertBiome(
        size_t length,
        size_t depth,
        size_t countLayer,
        size_t surfaceComplexity,
        size_t countGradient,
        int seed
    );



    virtual inline ~SandDesertBiome() {
    }



    /**
    * @see Layer
    */
    virtual std::vector< std::shared_ptr< Layer > >  layer();

};
