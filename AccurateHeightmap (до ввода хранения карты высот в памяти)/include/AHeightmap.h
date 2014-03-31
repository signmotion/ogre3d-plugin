#pragma once

#include "default.h"
#include <Ogre.h>
#include <boost/array.hpp>

#define PNG_DEBUG 3
#include <png.h>


/**
* ����������� ����������� ����� �����.
*
* ����� - ������� ������ �� ��������������������� ����� �����.
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
        for (size_t j = 0; j < lowHM.mapSize.get<1>(); ++j) {
            free( image_[j] );
        }
        free( image_ );
    }




    /**
    * @return ���-������ �� ��������� ��������������������� ����� ����� (��-�����).
    */
    Ogre::MeshPtr lowMesh( Ogre::SceneManager* sm ) const;



    /**
    * @return ��������������� ���������������������� ����� ����� ����� (��-�����).
    */
    template< size_t N >
    std::pair< Ogre::MeshPtr, normal3d_t > hightMesh(
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
        return lowHM.worldSize.get<0>() / (float)lowHM.mapSize.get<0>();
    }



    /**
    * @return ����� ������ ����������� ��������� ������ ������.
    *
    * @param aim ���������� ����������� *������������* hm_t::mapCenter (�����
    *        ���� �������������).
    */
    float heightCell( const coordInt2d_t& aim ) const;



    /**
    * @return ������� ���������� ��������� ������.
    */
    inline coord3d_t coordCell( const coordInt2d_t& aim ) const {
        const float size = sizeCell();
        return coord3d_t(
            aim.get<0>() * size,
            aim.get<1>() * size,
            heightCell( aim )
        );
    }




    /**
    * @return 4 ��������, ������������� ��������� ���� ������� ��������� ������
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
    * @return 5 ���������: 1-� - ������ ����� ������, 4 ��������� -
    *         ������� ��������� ��� ���������� ��-�����.
    *
    *   4         1
    *   .---------.
    *   |         |
    *   |    0    |
    *   |         |
    *   .---------.
    *   3         2
    *
    * @see vertexDeltaCell()
    */
    boost::array< coord3d_t, 5 >  vertexCoordCell( const coordInt2d_t& aim ) const;



    /**
    * @return 5 ���������: 1-� - ������� ���������, 4 ��������� - �������
    *         �������������, �� ������� ������� ��� ���������.
    *
    *   .---------.
    *   | \  1  / |
    *   |  \   /  |
    *   | 4  0  2 |
    *   |  /   \  |
    *   | /  3  \ |
    *   .---------.
    */
    boost::array< normal3d_t, 5 >  vertexNormalCell( const coordInt2d_t& aim ) const;





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
    */
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




private:
    /**
    * �������������� ��������� ���������� �� �����.
    */
    //static sizeInt2d_t sizeMapFromFile( const std::string& source );
    static std::pair< sizeInt2d_t, png_bytep* >  loadImage( const std::string& source );
    

    /**
    * ������ ����������� ��������������������� ����� ����� � ��� 'image_'.
    */
    //void loadLowHMImage() const;




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
    mutable png_bytep* image_;
};

