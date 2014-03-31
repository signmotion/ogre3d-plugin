#pragma once

#include "default.h"


/**
* �������������� ������� � ������� ��� ����������� ���� ��� ������� "�������".
*
* @template N �� ������� ����� ������� ������� ����� (������� �����������).
*/
template< size_t N >
class Plato {
public:
    /**
    * @see plane4Triangle3d_t
    * @see normal5Plane3d_t
    *
    * @param realCoord ���������� ������� � �������� ����.
    * @param worldSize ������ ��������� ����.
    */
    inline Plato(
        const std::pair< plane4Triangle3d_t, normal5Plane3d_t >& cn,
        const coord3d_t& realCoord,
        const size3d_t& worldSize
    ) :
        pt( cn.first ),
        np( cn.second ),
        rc( realCoord ),
        ws( worldSize )
    {
        // @test
        for (size_t i = 0; i < pt.static_size; ++i) {
            std::cout << "������� " << Ogre::StringConverter::toString( i ) << " " << pt[i] << std::endl;
        }

        assert( (ws.get<0>() > PRECISION)
             && (ws.get<1>() > PRECISION)
             && (ws.get<2>() > PRECISION)
             && "������ ���� �� ������."
        );
        assert( (rc.get<0>() <= ws.get<0>())
             && (rc.get<1>() <= ws.get<1>())
             && (rc.get<2>() <= ws.get<2>())
             && "���������� ����� ����� �� ��������� ����."
        );

#if 0
// - ��� ����������.
        // ��������� ������ ������� �����
        /* - @copy plane4Triangle3d_t
        *   4---------1
        *   | \  d  / |
        *   |  \   /  |
        *   | c  0   a|
        *   |  /   \  |
        *   | /  b  \ |
        *   3---------2
        */
        // ������ � ��������� XY � ��� ��������� (����� �����)...
        const float x = std::abs( pt[1].get<0>() - pt[4].get<0>() );
        std::cout << x << std::endl;
        assert( x > 0.0f );
        const float y = std::abs( pt[1].get<1>() - pt[2].get<1>() );
        assert( y > 0.0f );
        // ...� ��� � Z �� �� ��� ������
        const boost::array< float, 4 > az = {
            pt[1].get<2>(),
            pt[2].get<2>(),
            pt[3].get<2>(),
            pt[4].get<2>()
        };
        const auto minmaxZ = std::minmax_element( az.cbegin(), az.cend() );
        const float z = std::abs( *minmaxZ.second - *minmaxZ.first );
        assert( z > 0.0f );
        bx = size3d_t( x, y, z );
#endif

        // ��������� ����� ������ �����
        se[0] = calcDistance( pt[4], pt[1] );
        se[1] = calcDistance( pt[1], pt[2] );
        se[2] = calcDistance( pt[2], pt[3] );
        se[3] = calcDistance( pt[3], pt[4] );

        // ��������� ��������� ����� (����� ����� ���� ������)
        const auto minmaxSizeEdgeX = std::minmax( se[0], se[2] );
        const auto minmaxSizeEdgeY = std::minmax( se[1], se[3] );
        minKXY = minmaxSizeEdgeX.first / minmaxSizeEdgeY.second;
        maxKXY = minmaxSizeEdgeX.second / minmaxSizeEdgeY.first;
        avgKXY = (minKXY + maxKXY) / 2.0;
    }




    virtual inline ~Plato() {
    }



    /**
    * @return ���������� �����.
    */
    inline plane4Triangle3d_t plane() const {
        return pt;
    }


    /**
    * @return ����������� �����, ���������� "������" � ��������� XY.
    */
    inline plane4Triangle3d_t planeXY() const {
        // ����� ����������� � ��������� XY, �������� �� ������� �������
        const Ogre::Vector3 oNormal = Ogre::Vector3(
            np[0].get<0>(), np[0].get<1>(), np[0].get<2>()
        );
        const Ogre::Quaternion rotation = oNormal.getRotationTo( Vector3::UNIT_Z );
        // ���������� ����������� �������
        plane4Triangle3d_t planeXY;
        for (auto itr = pt.cbegin(); itr != pt.cend(); ++itr) {
            const coord3d_t coord = *itr;
            const Ogre::Vector3 rv = rotation * Ogre::Vector3(
                coord.get<0>(), coord.get<1>(), coord.get<2>()
            );
            const size_t i = std::distance( pt.cbegin(), itr );
            planeXY[i] = coord3d_t( rv.x, rv.y, rv.z );
        }

        return planeXY;
    }




    /**
    * @return ����� ��������.
    */
    inline normal5Plane3d_t normal() const {
        return np;
    }



    /**
    * @return ���������� ����� �� ������� (��� �����) ����.
    */
    inline coord3d_t realCoord() const {
        return rc;
    }



    /**
    * @return ������� �������� ���� (� ������� ����� ��� �����).
    */
    inline size3d_t worldSize() const {
        return ws;
    }



    /**
    * @return ������� �����.
    *//*
    inline size3d_t box() const {
        return bx;
    }
    */



    /**
    * @return ��������� �����.
    */
    inline float minProportion() const {
        return minKXY;
    }

    inline float maxProportion() const {
        return maxKXY;
    }

    inline float avgProportion() const {
        return avgKXY;
    }




    /**
    * @return ���������� ����� �����.
    */
    inline size_t countCell() const {
        return N;
    }



    /**
    * @return ������ ������� �����, �.
    */
    inline float sizeEdge( size_t edge ) const {
        assert( (edge < se.static_size) && "� ����� ����� 4 �������." );
        return se[ edge ];
    }



    /**
    * @return ������ ������� ������ �����, �.
    */
    inline float sizeCell( size_t edge ) const {
        return sizeEdge / (float)N;
    }






private:
    // @see �����������.
    const plane4Triangle3d_t pt;
    const normal5Plane3d_t   np;
    const coord3d_t          rc;
    const size3d_t           ws;

    /**
    * ������ ������� �����.
    * � ����� ������ - �� ���.
    *//*
    size3d_t bx;
    */

    /**
    * ����� ������ �����.
    *
    *        0
    *   .---------.
    *   |         |
    *   |         |
    * 3 |         | 1
    *   |         |
    *   |         |
    *   |         |
    *   .---------.
    *        2
    */
    boost::array< float, 4 >  se;

    /**
    * ��������� �����.
    * ����� ����� ���� ������. ������� ���������� ����. ��������� ������.
    */
    float minKXY;
    float maxKXY;
    float avgKXY;
};

