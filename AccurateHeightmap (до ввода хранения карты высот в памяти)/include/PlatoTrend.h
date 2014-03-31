#pragma once

#include "Trend.h"
#include "default.h"
#include "AHeightmap.h"

#define PNG_DEBUG 3
#include <png.h>


/**
* ����� - ������������� �����������, ������������� �� �����������
* ������ ��� ���������� ����� � ��������� XY.
* ������ ����� ��������� �������������� ��� �����������.
*/
template< size_t N >
class PlatoTrend :
    public Trend
{
public:
    /**
    * @param source ���� � ����� � ������ ����� ������.
    */
    PlatoTrend(
        const std::string& source,
        const size3d_t& realSize,
        const normal3d_t& norm
    ) :
        Trend( source ),
        realSize( realSize ),
        norm( norm )
    {
        // (!) ����������� ������ ���� �������� N x N ��������
        /* - � ������ ���� ���������� AHeightmap.
        auto r = loadImage( source );
        image_ = r.second;
        */
        // ��� ������������ ����������� ������������� ������������� AHeightmap.
        // (!) ������ ����� ����� ��� ������ ����� ��������. ��. ����. � Trend().
        const size_t maxDepth = 0;
        const size_t seaLevel = 127;
        const size_t maxHeight = 255;
        ahm = std::shared_ptr< AHeightmap >( new AHeightmap(
            source,
            maxDepth,
            seaLevel,
            maxHeight,
            realSize
        ) );
    }



    virtual inline ~PlatoTrend() {
        std::cout << "~Trend()" << std::endl;
        /* - �������� ����� AHeightmap.
        std::cout << "~PlatoTrend()" << std::endl;
        for (size_t j = 0; j < N; ++j) {
            free( image_[j] );
        }
        free( image_ );
        */
    }




    /**
    * @return ������� �����. ���������� ���� ����� ���������� OXY � ������������
    *         ������ � ����.
    */
    inline normal3d_t normal() const {
        return norm;
    }



    /**
    * @see Trend
    */
    virtual Ogre::MeshPtr mesh(
        Ogre::SceneManager* sm
    ) const;




    /**
    * @return ������ �� ����� ����� ������ � ������� LibPNG.
    *
    * @see AHeightmap::
    */
    inline png_bytep const* image() const {
        return image_;
    }



protected:
    /**
    * ������ ����������� ����� ������ � 'image'.
    *
    * @return ������� ����������� ����� � ��������.
    */
    //static std::pair< sizeInt2d_t, png_bytep* >  loadImage( const std::string& source );



protected:
    /**
    * �������� ������ ������, �.
    */
    const size3d_t& realSize;

    /**
    * �������. ���������� ���� ����� ���������� OXY � �������.
    */
    const normal3d_t norm;

    /**
    * ���������� ��� ���������� �����������.
    */
    std::shared_ptr< AHeightmap >  ahm;


    /**
    * �������������� ����� �����.
    */
    //mutable png_bytep* image_;
};

