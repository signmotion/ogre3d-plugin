#pragma once

#include "Trend.h"
#include "Plato.h"
#include "default.h"
#include "AHeightmap.h"
#include <Ogre.h>


/**
* @see Plato.
*
* Данный тренд умеет детализировать плато.
*/
template< size_t N >
class PlatoTrend :
    public Trend
{
public:
    /**
    * @param source Путь к файлу с картой высот тренда.
    */
    inline PlatoTrend(
        const std::string& source,
        const Plato< N >& plato
    ) :
        Trend( source ),
        p( plato )
    {
        // Загружаем изображение карты высот
        // (!) Формат карты высот для тренда чётко определён. См. комм. к Trend().
        const size_t maxDepth = 0;
        const size_t seaLevel = 127;
        const size_t maxHeight = 255;
        // @info Работать через AHeightmap не можем. См. комм. к AHeightmap.
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
        /* - Работаем через AHeightmap.
        std::cout << "~PlatoTrend()" << std::endl;
        for (size_t j = 0; j < N; ++j) {
            free( image_[j] );
        }
        free( image_ );
        */
    }




    /**
    * @return Нормаль плато. Определяет угол между плоскостью OXY и поверхностью
    *         тренда в мире.
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
    * Информация о плато в мире.
    */
    const Plato< N > p;

    /**
    * Карта высот.
    */
    sizeInt2d_t hmSize;
    std::shared_ptr< float >  hmHeight;

    /**
    * Механизм для работы с эти участком мира.
    *//* - Не работаем с AHeightmap. См. комм. к AHeightmap.
    std::shared_ptr< AHeightmap>  ahm;
    */
};

