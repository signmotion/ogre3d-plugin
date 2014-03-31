#pragma once

#include "default.h"
#include "FeatureTrend.h"


/**
* ����� - ��, ��� ��������� ������ ���������� ������, �����, ������������
* �����. ����� ������ - ��� ����������� ��� ����.
* ������ ������? ���������� ���� � �������, �� ����� �������� ��� ��,
* ��� �������, ����. �����������: �����, ������, ���� � ���. ��� �����������
* �������� "������".
*
* @see DuneLayer
*/
class Trend {

public:
    /**
    * �������������� ������.
    */
    FeatureTrend feature;



// ����� ������ ��� ������������
protected:
    /**
    * ������� ������ ������ ����������� �� ��� OX.
    * 
    * @param size ������������ ������� ������ �� ���� ���������.
    * @param scale ������� ������. ���� ������ 1, ������������� � 1.
    * @param ��. FeatureTrend.
    */
    Trend(
        const std::vector< form2d_t >&  all,
        const FeatureTrend& ft
    );


    
    virtual inline ~Trend() {
    }




public:
    /**
    * @return ������� ����� ������ (����� �����������).
    *
    * ������ ����� ���� �� ���� �������������:
    *     - ������������� ���� � �� �� ����� ����������� ��� ������ ����������
    *       ����������
    *     - ������� ���� ������ ������ �� ������ ����������� ����������
    *
    * ��� ���������� ������� (seed) - ���� � �� �� �������.
    *
    * @param natural true - � �������� ������������� ���������, ��� ���������
    *        ������������� ������������ ���������� (�������� �.�. �����������
    *        �������).
    */
    virtual form2d_t form( bool natural ) const;



    /**
    * @return ������� ����������� �������, � ������� ����� ��������� �����.
    */
    inline box2d_t box( bool natural ) const {
        const auto f = form( natural );
        return calcBox( f.first );
    }



    /**
    * @return ���������� ������ (a) � ��������� (b) ����� �����.
    *
    * ��������� ����������, �.�. ����� ������ �� vertex().
    */
    inline std::pair< coord2d_t, coord2d_t >  path( bool natural ) const {
        const auto v = form( natural ).first;
        return std::make_pair( v.front(), v.back() );
    }



    /**
    * @return �������� ����� ������. �.�. ����� �������� �� ����� � � �,
    *         �������� ����������� ��� ������� ��������� A � �. ������
    *         �����, ��������� ����������� / ��������� �� �������� �����
    *         ��� ��������� ����� ������ � ����������� (����������� �������
    *         ��������� ���������� � ���������).
    *
    * ��������� ����������, �.�. ����� ������ �� vertex().
    *
    * @see path()
    */
    inline vector2d_t grad( bool natural ) const {
        const auto p = path( natural );
        return (p.second - p.first);
    }




protected:
    /**
    * ������� ���������� ��������� �������, �� � �������� ��������� ���������.
    */
    static inline void distort( form2d_t* r, int seed, const size2d_t& amplitude ) {
        const auto ax = amplitude.get<0>();
        assert( (ax >= 0) && "��������� 1 ������� ������������� ������." );
        const auto az = amplitude.get<1>();
        assert( (az >= 0) && "��������� 2 ������� ������������� ������." );

        // ������ ��������� ��� ���������� ����������� ����
        // (!) mt19937 ��� ������������� ������������ �������������.
        // @see http://www.boost.org/doc/libs/1_47_0/doc/html/boost_random/reference.html
        boost::mt19937 rg( seed );
        boost::variate_generator<
            boost::mt19937,
            boost::uniform_int<>
        > axDistortGenerator( rg,  boost::uniform_int<>( -(int)ax, (int)ax ) );
        boost::variate_generator<
            boost::mt19937,
            boost::uniform_int<>
        > azDistortGenerator( rg,  boost::uniform_int<>( -(int)az, (int)az ) );

        // ������ � ��������� ���������� �� �������
        if (r->first.size() < 3) {
            return;
        }
        for (size_t i = 1; i < (r->first.size() - 1); ++i) {
            // ������ ����������
            // @todo ���������� �������, �.�. ������������ �����������
            //       �����, ������� ����� ��������� ����� ������ �� ��� OX.
            const float dx = (float)axDistortGenerator();
            if ((r->first[i].get<0>() + dx) > 0) {
                r->first[i].get<0>() += dx;
            }
            const float dz = (float)azDistortGenerator();
            r->first[i].get<1>() += dz;
        }

        // ������������ �������
        /* - ��� ����������: ������� ���������� ���. ��� ���.
        r->second = calcNormal( r->first.front(),  r->first.back() );
        */

    }





protected:
    /**
    * ��������� *������������* ����� ��� ����� ������. �� ������ ���� ����
    * �������� �� ������������ *�����������* ����.
    * @todo ���� ����� ���� ���������. ����������� (������������) ���������.
    */
    std::vector< form2d_t >  all;

};










// ������ ��������������� �������, ������� ����� �������������� Layer.
// ������ ����, ��� ������ ���������� ������, ��� ����� ���������
// ���������, ����������� �������� �� �����.
// (!) ������� ���� ������� �������� � ������.





/**
* ������ (������ ������).
*/
class AscendTrend :
    public Trend
{
public:
    inline AscendTrend(
        const FeatureTrend& ft
    ) :
        Trend( surface(), ft )
    {
        // ��������� ��������� ��� ������������. ������� � ��������� [0.0f; 1.0f]
        // � ����������� ��� ������� ������������ box() ������.
        // ��� ���� ��������� ������ ���� ���������, ����� ��������� ���������.
        // x, z
        feature.natural.get<0>() = 0.2f;
        feature.natural.get<1>() = 0.2f;
    }


    virtual inline ~AscendTrend() {
    }


    /**
    * @return ����� ������ ��� �������������.
    *
    * ����� ����� ��� ���������� �������� ���������� ��������.
    */
    static std::vector< form2d_t >  surface();

};





/**
* ����� (����� �������).
*/
class DescendTrend :
    public Trend
{
public:
    inline DescendTrend(
        const FeatureTrend& ft
    ) :
        Trend( surface(), ft )
    {
        // @see AscendTrend
        feature.natural.get<0>() = 0.2f;
        feature.natural.get<1>() = 0.2f;
    }


    virtual inline ~DescendTrend() {
    }


    /**
    * @see AscendTrend
    */
    static std::vector< form2d_t >  surface();

};






/**
* �������, ���, ������.
*/
class HollowTrend :
    public Trend
{
public:
    inline HollowTrend(
        const FeatureTrend& ft
    ) :
        Trend( surface(), ft )
    {
        // @see AscendTrend
        feature.natural.get<0>() = 0.2f;
        feature.natural.get<1>() = 0.2f;
    }


    virtual inline ~HollowTrend() {
    }


    /**
    * @see AscendTrend
    */
    static std::vector< form2d_t >  surface();

};
