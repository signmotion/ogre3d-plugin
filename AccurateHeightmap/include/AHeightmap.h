#pragma once

#include "default.h"
#include "helper.h"
#include <Ogre.h>
#include <boost/array.hpp>

#define PNG_DEBUG 3
#include <png.h>


/**
* ����������� ����������� ����.
* �������� ����� ������� ������� ����. �� ������ ��������� Trend �,
* ���� ������������, ����� ���������� ��� ��� ������ � ����. �� ��������
* ���� - AHeightmap - �� ��������� �� ������ ���������������, �������
* ������������ Trend. ������� ���� ��� ������.
*
* ����� - ������� ������ �� ��������������������� ����� �����.
* @see Plato
*/
class AHeightmap {
public:
    /**
    * ����� �����, �. ������������:
    *    1 - ����� ������� �������
    *    0 - ������� ����
    *   -1 - ����� �������� ��������
    * ����������� �������� ����������, �������� � ������������.
    * ���������� ����� �� ������ - ���� �� ���� ������������� �� �����.
    */
    typedef struct {
        // ��������, ������ ����� ������� ���������� � �������
        std::string source;
        // ������������ ������� (��� �������� � ��� ������� ����� ���������� � -1)
        size_t maxDepth;
        // ������� ���� �� ��������������� ����� ����� (��� �������� ����� ���������� � 0)
        size_t seaLevel;
        // ������������ ������ (��� �������� � ��� ������� ����� ���������� � 1)
        size_t maxHeight;
        // ������ ����� � ��������� XY, ���
        sizeInt2d_t mapSize;
        // ���������� ������ �����, ���
        coordInt2d_t mapCenter;
        // ������ ����, �
        size3d_t worldSize;
        // ������ ������� ������ ������ (��� ������ - ��������), �
        float sizeCell;
    } hm_t;



public:
    /**
    * ����� ����� �������� �� �����.
    *
    * @param n �� ������� ������ ������� ������� ������ ������.
    * @param size ������ ����, �.
    */
    AHeightmap(
        const std::string& file,
        size_t maxDepth,
        size_t seaLevel,
        size_t maxHeight,
        const size3d_t& worldSize
    );



    virtual inline ~AHeightmap() {
    }




    /**
    * @return ���-������ �� ��������� ��������������������� ����� ����� (��-�����).
    */
    Ogre::MeshPtr lowMesh( Ogre::SceneManager* sm ) const;



    /**
    * @return ��������������� ���������������������� ����� ����� ����� (��-�����).
    */
    template< size_t N >
    Ogre::MeshPtr hightMesh(
        Ogre::SceneManager* sm,
        const coordInt2d_t& aim
    ) const;




    /**
    * @return ����� ��������� 'hm_t' ��� ��-�����.
    */
    inline hm_t about() const {
        return lowHM;
    }



    /**
    * @return ������ ������� ������ ������ � ������.
    * 
    * (!) ����������� �� ����� ���������� (X). ��. �������� � ������������.
    */
    inline float sizeCell() const {
        return lowHM.sizeCell;
    }




    /**
    * @return ����� ������ ����������� ��������� ������ ������.
    *
    * @param aim ���������� ����������� *������������* hm_t::mapCenter (�����
    *        ���� �������������).
    *
    * ���� ���������� ����� ���������� ������ (������� ��������� ���
    * �����) - ��. avgHeightCell().
    */
    inline float maxHeightCell( const coordInt2d_t& aim ) const {
        // ������ ����� ������ �� �������� � ������������ (mapSize). �����
        // ���������, ��� �������� ���������� ������� ������������ hm_t::mapCenter.
        const coordInt2d_t hmCoord = aim + lowHM.mapCenter;
        if ( helper::outsideMatrix( hmCoord, lowHM.mapSize ) ) {
            throw "Coord is out of bounds.";
        }

        // �������� ������ �� ������������� � ������������ �������
        const size_t i = hmCoord.get<0>() + hmCoord.get<1>() * lowHM.mapSize.get<0>();
        return hmImage_.get()[i] * lowHM.worldSize.get<2>() / 2.0f;
    }




    /**
    * @return ����� ������ ����������� ��������� ������ ������. ������
    *         ����������� ��� ������� ����� ������ ��������������
    *         ����� (��. ).
    *
    * @param aim ���������� ����������� *������������* hm_t::mapCenter (�����
    *        ���� �������������).
    *
    * ���� ���������� ����� ������������ ������ (������� ����� � �����
    * �����) - ��. avgHeightCell().
    */
    inline float avgHeightCell( const coordInt2d_t& aim ) const {
        const auto vdc = vertexDeltaCell( aim );
        const float average = (vdc[0] + vdc[1] + vdc[2] + vdc[3]) / 4.0f;
        const float maxHeight = maxHeightCell( aim );
        return (maxHeight + average);
    }




    /**
    * @return ������� ���������� ��������� ������. ������������ ������.
    */
    inline coord3d_t coordCellMaxHeight( const coordInt2d_t& aim ) const {
        const float size = sizeCell();
        return coord3d_t(
            aim.get<0>() * size,
            aim.get<1>() * size,
            maxHeightCell( aim )
        );
    }




    /**
    * @return ������� ���������� ��������� ������. ���������� �� ������� ������.
    */
    inline coord3d_t coordCellAvgHeight( const coordInt2d_t& aim ) const {
        const float size = sizeCell();
        return coord3d_t(
            aim.get<0>() * size,
            aim.get<1>() * size,
            avgHeightCell( aim )
        );
    }




    /**
    * @return �������������� ������� ����������� ����� � �������� � ��������
    *         ���� (true) ��� � ������� � ����� (0; 0) (false).
    */
    inline plane3d_t plane( const coordInt2d_t& aim, bool realWorld ) const {
        const auto vdc = vertexDeltaCell( aim );
        const float halfSize = sizeCell() / 2.0f;
        const coord3d_t shift = realWorld
            ? coordCellAvgHeight( aim )
            : ZERO3D;
        const plane3d_t plane = {
            coord3d_t(  halfSize,   halfSize,  vdc[0] ) + shift,
            coord3d_t(  halfSize,  -halfSize,  vdc[1] ) + shift,
            coord3d_t( -halfSize,  -halfSize,  vdc[2] ) + shift,
            coord3d_t( -halfSize,   halfSize,  vdc[3] ) + shift
        };

        return plane;
    }





    /**
    * @return 4 ��������, ������������� ��������� ���� ����� ��������� ������
    *         ��������� �� �������� ����� �������. ����������� � ������
    *         �������. ������������ � ������� ��� � helper::isc() = 5, 6, 7, 8.
    *
    *   3         0
    *   .---------.
    *   |         |
    *   |         |
    *   |         |
    *   .---------.
    *   2         1
    *
    * @see heightCell()
    */
    boost::array< float, 4 >  vertexDeltaCell( const coordInt2d_t& aim ) const;




    /**
    * @return 5 ���������: 1-� - *������������* ������ ����� ������, 4 ��������� -
    *         ������� ��������� ��� ���������� ��-�����.
    *
    * @see plane4Triangle3d_t
    * @see vertexDeltaCell()
    */
    plane4Triangle3d_t vertexCoordCell( const coordInt2d_t& aim ) const;



    /**
    * @return ��������� �� vertexCoordCell() ���� 5 ��������:
    *         1-� - *����������* ������� ���������, 4 ��������� -
    *         ������� �������������, �� ������� ������� ��� ���������.
    *         ������� �������������.
    *
    * @see normal5Plane3d_t
    */
    std::pair< plane4Triangle3d_t, normal5Plane3d_t > vertexCoordNormalCell(
        const coordInt2d_t& aim
    ) const;





    /**
    * @return ������ ������ � �� ��������������� ���������, ����������� �� ����� �����
    *         ���� ������ � ������� ������������� ������ (sizeInt2d_t) ���� �������
    *         � ������ �������� �������� ������������ ������ (Ogre::AxisAlignedBox)
    *         ���� ������, ���� ����� ������� ���. ���� ���������� 'box'
    *         ����������� ��� �������� �����������, ��� ��������� �� �������
    *         �����������.
    *
    * ��������� ����� ��� ����������� � Ogre::VertexData().
    *
    * @param box ��������� ������� ����������� (������ �����. - ����� ������
    *        ����), ������� ������ � ���������.
    * @param maxDepth, seaLevel, maxHeight  ��. ����. � 'hm_t'.
    * @param worldSize ������ ����, �. �����. ������ ���������� � ����� �������.
    *//* - �� ������������.
    typedef boost::tuple<
        sizeInt2d_t,
        std::shared_ptr< float >,
        // �������
        Ogre::AxisAlignedBox,
        // ������
        float
    > vertexData_t;
    static vertexData_t AHeightmap::vertexData(
        const std::string& file,
        const boxInt2d_t& box,
        size_t maxDepth,
        size_t seaLevel,
        size_t maxHeight,
        const size3d_t& worldSize
    );
    */




    /**
    * @return ��������������� ����� �����.
    *
    * �, � - ����� � ���������, �������� ������������ 'source'.
    * z (������) - ����� � ��������� [-1.0; 1.0].
    *
    * @param box ��������� ������� ����������� (������ ���������� - ����� ������
    *        ����), ������� ������ � ���������. ���� == ZEROINT2D, �������� ��
    *        �����������.
    *
    * @see ����������� AHeightmap.
    */
    static std::pair< sizeInt2d_t, std::shared_ptr< float > >  loadHMImage(
        const std::string& source,
        const boxInt2d_t& box,
        size_t maxDepth,
        size_t seaLevel,
        size_t maxHeight
    );
    




private:
    /**
    * ��������������������� ����� �����.
    */
    hm_t lowHM;

    /**
    * ���������������������� ����� �����.
    *//* - �����������.
    hm_t hightHM;
    */

    /**
    * �� ������� ������ ��������� ������ ������, ����� ���������
    * ���������������������� �����.
    *//* - � �������.
    const size_t n;
    */


    /**
    * ��� ��� �������.
    * �����, ����� "������".
    */
    mutable Ogre::MeshPtr lowMesh_;
    mutable std::shared_ptr< float >  hmImage_;
};

