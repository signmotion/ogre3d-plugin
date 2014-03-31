#pragma once

#include "FeatureBiome.h"
#include "Layer.h"

struct Layer;


/**
* ������������� ����� ����.
* �������� ��������������� ��� ������ (Layer �������������� � Biome).
*/
struct Biome {

    /**
    * �������������� �����.
    */
    FeatureBiome feature;



    /**
    * @param ��. ����. � ��������� Biome.
    *
    * @todo optimize ? ���������� ����������� � template<>.
    */
    inline Biome(
        size_t length,
        size_t depth,
        size_t surfaceComplexity,
        size_t countLayer,
        //const boost::numeric::interval< int >&  fatLayer
        size_t countGradient,
        int seed = static_cast< int >( std::time( nullptr ) )
        // ������ �������������� ����� ����. ����. �� ��������� (��. ����)
    ) {
        assert( (length > 0) && "������������ ����� ������ ���� �������." );
        assert( (length <= INT_MAX) && "������������ ����� �� ����� ��������� INT_MAX." );
        assert( (countLayer > 0) && "�� ������� ���������� ����." );
        //assert( (fatLayer.lower() < fatLayer.upper()) && "��������� �������� [�������; ������]." );
        assert( (countGradient > 0) && "�� ������� ���������� ����� ��������." );
        assert( (countGradient >= 2) && "��� ����� ������ ���� ������� ��� ������� 2 ����� ��������." );

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
    * @return ���������� ����� ������.
    */
    inline size_t distanceLayer() const {
        return (size_t)( feature.depth / feature.countLayer );
    }



    /**
    * @return ����� ����, ��������� ��� ����� �����.
    *
    * (!) ������ ����� ������ ��� ������������ ���� �������� 'seed' �����.
    */
    virtual std::vector< std::shared_ptr< Layer > >  layer() = 0;




    /**
    * ���������� ���������� ����. ����� ��� �������� ����������
    * ������������ �����.
    */
    void initGenerator();




protected:
    /**
    * ��������� ����� ����.
    * ������������ ��������� �������� Layer.
    */
    std::vector< std::string >  lr;
    
};








/**
* �������. �����, ����.
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
