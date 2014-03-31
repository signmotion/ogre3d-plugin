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
    // �� ���� ���������� �������� ������ �������������� ��������� ���� �����.

    // ����������� ����, ��������� �����. ����� ��� ���� ������������ ���
    // ������������ ����������� ������ �����.

    // ����� ��� ����
    /* - ������ ������ �������� ����.
    int layerSeed = seed + 1;

    // ���� �� �����
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

    // ������ ������ �������� ����
    lr.push_back( "DunaLayer" );


    // @todo �������� ����.

}





std::vector< std::shared_ptr< Layer > >  SandDesertBiome::layer() {

    assert( (lr.size() > 0) && "��� ����� ����� ���� �� ����������." );

    std::vector< std::shared_ptr< Layer > >  v;

    // ���������� ���������� ��� ��������� ���������� ������������ �����
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

    // ��� ���������� ������� 'countLayer' ����
    for (size_t i = 0; i < feature.countLayer; ++i) {
        // �������� ���� �� ���, ������� ����� ���� ������� � ���� �����
        const int n = indexLayerGenerator();
        const std::string name = lr[n];
        // ������ ��� ������ ����� ����� ���� �� ���� ��������� �����
        if (name == "DunaLayer") {
            auto ptr = std::shared_ptr< Layer >( new DunaLayer(
                feature.length,
                feature.surfaceComplexity,
                feature.countGradient,
                // ���������� �������� ������ SEED, ����� ���� ������� ����������
                seedLayerGenerator()
            ) );
            v.push_back( ptr );
            continue;
        }

        // @todo ������ ����.
    }

    return v;
}
