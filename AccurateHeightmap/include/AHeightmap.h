#pragma once

#include "default.h"
#include "helper.h"
#include <Ogre.h>
#include <boost/array.hpp>

#define PNG_DEBUG 3
#include <png.h>


/**
* Увеличивает детализацию мира.
* Является самым верхним уровнем мира. Во многом повторяет Trend и,
* если поднапрячься, можно объединить эти два класса в один. Но верхушка
* мира - AHeightmap - не нуждается во многих характеристиках, которые
* представляет Trend. Поэтому живы оба класса.
*
* Плато - область высоты из низкодетализированной карты высот.
* @see Plato
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
        // Размер стороны ячейки высоты (все ячейки - квадраты), м
        float sizeCell;
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
    }




    /**
    * @return Меш-модель по имеющейся низкодетализированной карте высот (нд-карта).
    */
    Ogre::MeshPtr lowMesh( Ogre::SceneManager* sm ) const;



    /**
    * @return Сгенерированная высокодетализированная часть карты высот (вд-карта).
    */
    template< size_t N >
    Ogre::MeshPtr hightMesh(
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
        return lowHM.sizeCell;
    }




    /**
    * @return Какую высоту декларирует указанная ячейка высоты.
    *
    * @param aim Координата указывается *относительно* hm_t::mapCenter (может
    *        быть отрицательной).
    *
    * Если необходимо найти усреднённую высоту (которая считается для
    * плато) - см. avgHeightCell().
    */
    inline float maxHeightCell( const coordInt2d_t& aim ) const {
        // Размер файла образа мы выяснили в конструкторе (mapSize). Здесь
        // учитываем, что входящая координата указана относительно hm_t::mapCenter.
        const coordInt2d_t hmCoord = aim + lowHM.mapCenter;
        if ( helper::outsideMatrix( hmCoord, lowHM.mapSize ) ) {
            throw "Coord is out of bounds.";
        }

        // Получаем высоту из подготовлённой в конструкторе матрицы
        const size_t i = hmCoord.get<0>() + hmCoord.get<1>() * lowHM.mapSize.get<0>();
        return hmImage_.get()[i] * lowHM.worldSize.get<2>() / 2.0f;
    }




    /**
    * @return Какую высоту декларирует указанная ячейка высоты. Высота
    *         вычисляется как среднее между углами прямоугольного
    *         плато (см. ).
    *
    * @param aim Координата указывается *относительно* hm_t::mapCenter (может
    *        быть отрицательной).
    *
    * Если необходимо найти максимальную высоту (которая лежит в карте
    * высот) - см. avgHeightCell().
    */
    inline float avgHeightCell( const coordInt2d_t& aim ) const {
        const auto vdc = vertexDeltaCell( aim );
        const float average = (vdc[0] + vdc[1] + vdc[2] + vdc[3]) / 4.0f;
        const float maxHeight = maxHeightCell( aim );
        return (maxHeight + average);
    }




    /**
    * @return Мировые координаты указанной ячейки. Максимальная высота.
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
    * @return Мировые координаты указанной ячейки. Усреднённая по соседям высота.
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
    * @return Четырёхугольная область поверхности плато с позицией в реальном
    *         мире (true) или с центром в точке (0; 0) (false).
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
    * @return 4 значения, декларирующие насколько углы плато указанной высоты
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
    * @return 5 координат: 1-я - *максимальная* высота самой ячейки, 4 остальных -
    *         вершины плоскости для построения вд-карты.
    *
    * @see plane4Triangle3d_t
    * @see vertexDeltaCell()
    */
    plane4Triangle3d_t vertexCoordCell( const coordInt2d_t& aim ) const;



    /**
    * @return Результат из vertexCoordCell() плюс 5 нормалей:
    *         1-я - *усреднённая* нормаль плоскости, 4 остальных -
    *         нормали треугольников, на которые разбита эта плоскость.
    *         Нормали нормализованы.
    *
    * @see normal5Plane3d_t
    */
    std::pair< plane4Triangle3d_t, normal5Plane3d_t > vertexCoordNormalCell(
        const coordInt2d_t& aim
    ) const;





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
    *//* - Не используется.
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
    */




    /**
    * @return Нормализованная карта высот.
    *
    * х, у - лежат в диапазоне, заданном изображением 'source'.
    * z (высота) - лежит в диапазоне [-1.0; 1.0].
    *
    * @param box Определят квадрат изображения (первая координата - левый нижний
    *        угол), который попадёт в результат. Если == ZEROINT2D, читается всё
    *        изображение.
    *
    * @see Конструктор AHeightmap.
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
    mutable std::shared_ptr< float >  hmImage_;
};

