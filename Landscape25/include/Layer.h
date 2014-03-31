#pragma once

#include "Ogre.h"
#include "FeatureLayer.h"
#include "Trend.h"

struct Biome;


/**
* ���� - ��, ��� ������������ � ����. ����� ���� ������������ ��������.
*
* @param biome �������� ����� ���������� �������� ����. ��.
*        ������������ Layer() � Biome().
*
* @see Trend
*/
struct Layer {

    /**
    * �������������� ����.
    */
    FeatureLayer feature;



    inline Layer(
        size_t length,
        size_t surfaceComplexity,
        size_t countGradient,
        int seed = static_cast< int >( std::time( nullptr ) )
        // ������ �������������� ����� ����. ����. �� ��������� (��. ����)
    ) {
        // ������ ���� ������ ��� ���������� ���� ��������. ��������, ���� ������
        // ��� ������ ����������� � ����� �������, ����� �� ��������� ������
        // �����. � ��� ������������ ��������� ���� �� ������ �����������������
        // ����������� �� �����, �� (!) � ����� ���� ���������� �������������
        // �������, ����� ��������� ����� ������� �� ���������� �������.
        assert( (length > 0) && "������������ ���� ������ ���� �������." );
        assert( (length <= INT_MAX) && "������������ ���� �� ����� ��������� INT_MAX." );
        feature.length = length;

        feature.surfaceComplexity = surfaceComplexity;

        assert( (countGradient > 0) && "�� ������� ���������� ����� �������� ����." );
        assert( (countGradient >= 2) && "��� ���� ������ ���� ������� ��� ������� 2 ����� ��������." );
        feature.countGradient = countGradient;

        //feature.natural = by default
        
        feature.seed = seed;
    }



    /**
    * @todo (!) �� ����������� ������������ ���������� ������.
    */
    /* - �� ����������.
    inline Layer( const FeatureLayer& feature ) :
        feature( feature )
    {}
    */



    virtual inline ~Layer() {
    }




    /**
    * ������ ����� ����.
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
    * @return ��������������� ����� �������� ��� ����� ����. ������������
    *         ��� ������ �������� �����������.
    *
    * @see �������� ���������� � Biome.
    * @see surfaceLine()
    */
    virtual surface2d_t  gradientPoint() const;



    /**
    * @return ���������������� ����� ����������� ��� ����� ����.
    *
    * @param gp ����� ��������.
    *
    * ���������� ������� ����������� � ��� �� ������� ���������,
    * ��� � ����� �����������.
    *
    * @see �������� ���������� � Biome.
    * @see gradientPoint()
    */
    virtual std::pair< trend2d_t, surface2d_t >  surfaceLine(
        const surface2d_t&  gp
    ) const = 0;

    



protected:
    /**
    * @return �����, ������� ��������� �������
    *         ���������� 'current' � 'aim'.
    *
    * ������� ������ ����������� ������: �.�. ����� ������������ ������,
    * ������� ����������� ������ ���� ����� (���� ��������) � �����������
    * �� 'seed'.
    */
    virtual std::shared_ptr< Trend >  bestNaturalTrend(
        const coord2d_t& current,
        const coord2d_t& aim
    ) const;





protected:
    /**
    * ��������� ���� ������.
    * ������������ ���������. �������������� ������ ������������ ���
    * ���������� ����� �������.
    */
    std::vector< std::pair< std::string, FeatureTrend > >  tr;

};









/**
* ����, ������� ����� ����.
* ���� ������� �����.
* ��� ����� ���� ������:
*   - ���
*   - �����
*   - ������
*   - ����
* ������� �������, ���� �����, ��� *�����* ���������, ����� � ��� �������� ����.
*/
struct DunaLayer :
    public Layer
{
    /**
    * @see Layer / ����. � ������������.
    */
    DunaLayer(
        size_t length,
        size_t surfaceComplexity,
        size_t countGradient,
        int seed = static_cast< int >( std::time( nullptr ) )
    );


    /* - �� ����������.
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
