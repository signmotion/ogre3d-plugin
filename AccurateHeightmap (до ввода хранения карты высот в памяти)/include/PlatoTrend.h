#pragma once

#include "Trend.h"
#include "default.h"
#include "AHeightmap.h"

#define PNG_DEBUG 3
#include <png.h>


/**
* Плато - прямоугольная поверхность, расположенная на определённой
* высоте под определёным углом к плоскости XY.
* Данный тренд позволяет детализировать эту поверхность.
*/
template< size_t N >
class PlatoTrend :
    public Trend
{
public:
    /**
    * @param source Путь к файлу с картой высот тренда.
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
        // (!) Изображение должно быть размером N x N пикселей
        /* - В полной мере используем AHeightmap.
        auto r = loadImage( source );
        image_ = r.second;
        */
        // Для формирования поверхности воспользуемся возможностями AHeightmap.
        // (!) Формат карты высот для тренда чётко определён. См. комм. к Trend().
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




    /**
    * @return Ссылка на карту высот тренда в формате LibPNG.
    *
    * @see AHeightmap::
    */
    inline png_bytep const* image() const {
        return image_;
    }



protected:
    /**
    * Читает изображение карты высоты в 'image'.
    *
    * @return Размеры прочитанной карты в пикселях.
    */
    //static std::pair< sizeInt2d_t, png_bytep* >  loadImage( const std::string& source );



protected:
    /**
    * Реальный размер тренда, м.
    */
    const size3d_t& realSize;

    /**
    * Нормаль. Определяет угол между плоскостью OXY и трендом.
    */
    const normal3d_t norm;

    /**
    * Используем для построения поверхности.
    */
    std::shared_ptr< AHeightmap >  ahm;


    /**
    * Закешированная карта высот.
    */
    //mutable png_bytep* image_;
};

