#include "Biome.h"


SandDesertBiome::SandDesertBiome(
    size_t length,
    size_t depth,
    size_t countLayer,
    size_t surfaceComplexity,
    size_t countGradient,
    int seed
) :
    Biome( length, depth, countLayer, surfaceComplexity, countGradient, seed )
{
    // На базе полученных значений строим характеристики возможных слоёв биома.

    // Декларируем слои, известные биому. Позже эти слои используются для
    // формирования визуального образа биома.

    // Зерно для слоёв
    /* - Храним только названия слоёв.
    int layerSeed = seed + 1;

    // Дюна из песка
    ++layerSeed;
    auto duna = std::shared_ptr< Layer >(
        new DunaLayer(
            feature.length,
            feature.surfaceComplexity,
            feature.countGradient,
            layerSeed
        )
    );
    lr.push_back( duna );
    */

    // Храним только названия слоёв
    lr.push_back( "DunaLayer" );


    // @todo Добавить слои.

}





std::vector< std::shared_ptr< Layer > >  SandDesertBiome::layer() {

    assert( (lr.size() > 0) && "Для этого биома слои не определены." );

    std::vector< std::shared_ptr< Layer > >  v;

    // Инициируем генераторы для получения одинаковой конфигурации биома
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > indexLayerGenerator(
        boost::mt19937( feature.seed ),  
        boost::uniform_int<>( 0,  lr.size() - 1 )
    );

    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > seedLayerGenerator(
        boost::mt19937( feature.seed + 1 ),
        boost::uniform_int<>( 0, INT_MAX )
    );

    // Нам необходимо создать 'countLayer' слоёв
    for (size_t i = 0; i < feature.countLayer; ++i) {
        // Выбираем слой из тех, которые могут быть созданы в этом биоме
        const int n = indexLayerGenerator();
        const std::string name = lr[n];
        // Каждый раз создаём новый набор слоёв на базе доступных биому
        if (name == "DunaLayer") {
            auto ptr = std::shared_ptr< Layer >( new DunaLayer(
                feature.length,
                feature.surfaceComplexity,
                feature.countGradient,
                // Необходимо подавать разные SEED, иначе слои получим одинаковые
                seedLayerGenerator()
            ) );
            v.push_back( ptr );
            continue;
        }

        // @todo Другие слои.
    }

    return v;
}
