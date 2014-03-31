#pragma once

#include "Doxel.h"
#include <Ogre.h>
#include <boost/array.hpp>


/**
* Доксельная карта.
* Определяет, как рисовать доксели в зависимости от их размера. Размер взят как
* ключевой признак: ведь содержание докселей может отличаться в зависимости от
* глубины вложенности. Глубина вложенности докселя определяет точность
* содержания.
*
* @see Doxel
*/
class MapDoxel {
public:
    /**
    * Строит карту из предоставленного файла высот.
    *
    * @param source Путь к файлу, который содержит карту высот для докселя.
    * @param size Размер карты, км. Карта - куб.
    */
    MapDoxel(
        const std::string& source,
        const d::size3d_t& worldSizeInKm,
        Ogre::SceneManager* sm
    );




    virtual ~MapDoxel();




    /**
    * Рисует карту в указанной координатной коробке.
    *
    * @see clearCanvas() для очистки.
    *//* - Не используется.
    void draw(
        const d::box3d_t& box
    ) const;
    */




    /**
    * Отображает доксель по указанной координате.
    *
    * @param maxCount Если не 0, больше этого кол-ва докселей не визуализируется.
    * @param testMode Режим тестирования позволяет разглядеть лишние доксели.
    *
    * (!) Одновременно могут рисоваться доксели только одно размера.
    * См. примечание в коде draw() к setDefaultDimensions().
    */
    void draw(
        const Doxel3D& doxel,
        const d::coord3d_t& coord,
        size_t maxCount = 0,
        bool testMode = false
    ) const;




    /**
    * Отображает потомков докселя по указанной координате на
    * указанном уровне.
    *
    * @todo Что делать если потомки есть не у всех? Сейчас - не рисуем ничего.
    */
    inline void drawChild(
        const d::coord3d_t& coord,
        size_t level,
        size_t maxCount = 0,
        bool testMode = false
    ) const {
        drawChild( *root, coord, level, maxCount, testMode );
    }


    void drawChild(
        const Doxel3D& doxel,
        const d::coord3d_t& coord,
        size_t level,
        size_t maxCount = 0,
        bool testMode = false
    ) const;




    /**
    * Очищает холст.
    *
    * @see draw() для рисования.
    */
    inline void clearCanvas() {
        for (auto itr = dBS.begin(); itr != dBS.end(); ++itr) {
            auto bs = *itr;
            bs->clear();
        }
        testBS->clear();
        sn->removeAllChildren();
    }






private:
    /**
    * Доксельная карта.
    */
    std::shared_ptr< Doxel3D >  root;

    /**
    * Источник - файл с картой высот.
    */
    const std::string s;


    /**
    * Всё что нужно для визуализации докселя.
    *
    * Визуальный образ докселя. Нумерация:
    *   0 - верх
    *   1 - справа
    *   2 - снизу
    *   3 - слева
    *   4 - позади
    *   5 - впереди
    */
    Ogre::SceneManager* sm;
    Ogre::SceneNode* sn;
    typedef boost::array< Ogre::BillboardSet*, 6 >  dBS_t;
    dBS_t dBS;

    /**
    * Используется для тестового показа образа.
    */
    Ogre::BillboardSet* testBS;

};

