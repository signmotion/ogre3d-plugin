#pragma once

#include "Trend.h"
#include "Plato.h"
#include "default.h"
#include "AHeightmap.h"
#include <Ogre.h>


/**
* @see Plato.
*
* ������ ����� ����� �������������� �����.
*/
template< size_t N >
class PlatoTrend :
    public Trend
{
public:
    /**
    * @param source ���� � ����� � ������ ����� ������.
    */
    inline PlatoTrend(
        const std::string& source,
        const Plato< N >& plato
    ) :
        Trend( source ),
        p( plato )
    {
        // ��������� ����������� ����� �����
        // (!) ������ ����� ����� ��� ������ ����� ��������. ��. ����. � Trend().
        const size_t maxDepth = 0;
        const size_t seaLevel = 127;
        const size_t maxHeight = 255;
        // @info �������� ����� AHeightmap �� �����. ��. ����. � AHeightmap.
        const auto r = AHeightmap::loadHMImage(
            source,
            boxInt2d_t( ZEROINT2D, ZEROINT2D ),
            maxDepth,
            seaLevel,
            maxHeight
        );
        hmSize = r.first;
        hmHeight = r.second;
    }



    virtual inline ~PlatoTrend() {
        //std::cout << "~PlatoTrend()" << std::endl;
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





protected:
    /**
    * ���������� � ����� � ����.
    */
    const Plato< N > p;

    /**
    * ����� �����.
    */
    sizeInt2d_t hmSize;
    std::shared_ptr< float >  hmHeight;

    /**
    * �������� ��� ������ � ��� �������� ����.
    *//* - �� �������� � AHeightmap. ��. ����. � AHeightmap.
    std::shared_ptr< AHeightmap>  ahm;
    */
};

