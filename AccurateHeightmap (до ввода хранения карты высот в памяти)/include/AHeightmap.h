#pragma once

#include "default.h"
#include <Ogre.h>
#include <boost/array.hpp>

#define PNG_DEBUG 3
#include <png.h>


/**
* Увеличивает детализацию карты высот.
*
* Плато - область высоты из низкодетализированной карты высот.
*/
class AHeightmap {
public:
    /**
    * Карта высот, м. Нормализация:
    *    1 - самая высокая вершина
    *    0 - уровень моря
    *   -1 - самая глубокая пропасть
    * Нормализуем согласно параметрам, заданным в конструкторе.
    * Содержимое карты не храним - берём по мере необходимости из файла.
    */
    typedef struct {
        // Источник, откуда будет браться информация о высотах
        std::string source;
        // Максимальная глубина (это значение и все меньшие будут приравнены к -1)
        size_t maxDepth;
        // Уровень моря на предоставляемой карте высот (это значение будет приравнено к 0)
        size_t seaLevel;
        // Максимальная высота (это значение и все меньшие будут приравнены к 1)
        size_t maxHeight;
        // Размер карты в плоскости XY, пкс
        sizeInt2d_t mapSize;
        // Координаты центра карты, пкс
        coordInt2d_t mapCenter;
        // Размер мира, м
        size3d_t worldSize;
    } hm_t;



public:
    /**
    * Карта высот строится из файла.
    *
    * @param n На сколько частей следует разбить ячейку высоты.
    * @param size Размер мира, м.
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
    * @return Меш-модель по имеющейся низкодетализированной карте высот (нд-карта).
    */
    Ogre::MeshPtr lowMesh( Ogre::SceneManager* sm ) const;



    /**
    * @return Сгенерированная высокодетализированная часть карты высот (вд-карта).
    */
    template< size_t N >
    std::pair< Ogre::MeshPtr, normal3d_t > hightMesh(
        Ogre::SceneManager* sm,
        const coordInt2d_t& aim
    ) const;




    /**
    * @return Копия структуры 'hm_t' для нд-карты.
    */
    inline hm_t about() const {
        return lowHM;
    }



    /**
    * @return Размер стороны ячейки высоты в метрах.
    * 
    * (!) Вычисляется по одной координате (X). См. проверку в конструкторе.
    */
    inline float sizeCell() const {
        return lowHM.worldSize.get<0>() / (float)lowHM.mapSize.get<0>();
    }



    /**
    * @return Какую высоту декларирует указанная ячейка высоты.
    *
    * @param aim Координата указывается *относительно* hm_t::mapCenter (может
    *        быть отрицательной).
    */
    float heightCell( const coordInt2d_t& aim ) const;



    /**
    * @return Мировые координаты указанной ячейки.
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
    * @return 4 значения, декларирующие насколько углы вершины указанной высоты
    *         отклонены от значения самой вершины. Вычисляются с учётом
    *         соседей. Возвращаются в порядке как у helper::isc() = 5, 6, 7, 8.
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
    * @return 5 координат: 1-я - высота самой ячейки, 4 остальных -
    *         вершины плоскости для построения вд-карты.
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
    * @return 5 координат: 1-я - нормаль плоскости, 4 остальных - нормали
    *         треугольников, на которые разбита эта плоскость.
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
    * @return Список вершин и их нормализованных координат, построенных из карты высот
    *         плюс размер в ячейках получившегося снимка (sizeInt2d_t) плюс коробка
    *         с учётом реальных размеров получившихся вершин (Ogre::AxisAlignedBox)
    *         плюс радиус, если будем строить меш. Если координаты 'box'
    *         оказываются вне пределов изображения, они усекаются до размера
    *         изображения.
    *
    * Результат готов для копирования в Ogre::VertexData().
    *
    * @param box Определят квадрат изображения (первая коорд. - левый нижний
    *        угол), который попадёт в результат.
    * @param maxDepth, seaLevel, maxHeight  См. комм. к 'hm_t'.
    * @param worldSize Размер мира, м. Коорд. вершин приводятся к этому размеру.
    */
    typedef boost::tuple<
        sizeInt2d_t,
        std::shared_ptr< float >,
        // Коробка
        Ogre::AxisAlignedBox,
        // Радиус
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
    * Инициализирует структуру значениями из файла.
    */
    //static sizeInt2d_t sizeMapFromFile( const std::string& source );
    static std::pair< sizeInt2d_t, png_bytep* >  loadImage( const std::string& source );
    

    /**
    * Читает изображение низкодетализированной карты высот в кеш 'image_'.
    */
    //void loadLowHMImage() const;




private:
    /**
    * Низкодетализированная карта высот.
    */
    hm_t lowHM;

    /**
    * Высокодетализированная карта высот.
    *//* - Вычисляется.
    hm_t hightHM;
    */

    /**
    * На сколько частей разбивать ячейку высоты, чтобы построить
    * высокодетализированную карту.
    *//* - В шаблоне.
    const size_t n;
    */


    /**
    * Кеш для методов.
    * Введён, чтобы "летало".
    */
    mutable Ogre::MeshPtr lowMesh_;
    mutable png_bytep* image_;
};

