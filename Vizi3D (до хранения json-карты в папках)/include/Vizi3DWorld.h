#pragma once

#include <Ogre.h>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include "configure.h"
#include "default.h"
#include "BitContent3D.h"
#include "../external/vjson/json.h"


/**
* Визуализатор мира.
*/
class Vizi3DWorld {
public:
    /**
    * Каждый элемент карты обозначается односимвольным элементом - меткой.
    */
    typedef char signElement_t;

    /**
    * Содержимое карты.
    * Записывается как метка элемента и битовый 3D-образ области, где
    * метка встречается.
    */
    typedef std::unordered_map<
        signElement_t,
        // Обёрнут в указатель, чтобы не определять для BitContent3D
        // конструктор по умолчанию
        std::shared_ptr< BitContent3D >
    > content_t;

    /**
    * Область карты, подготовленная для показа.
    * Для *области* формируется битовый образ, который помогает
    * оптимизировать визуализацию. Например, не рисовать невидимые
    * элементы.
    *
    * @see prepareVisualBitImage()
    * @see drawNext()
    */
    typedef struct {
        std::shared_ptr< BitContent3D >  alphaTypeBitImage;
        std::shared_ptr< BitContent3D >  solidTypeBitImage;
    } visualContent_t;


    /**
    * Декларация *области* карты.
    * Сейчас можно указать одну область - 'mapZ'.
    */
    typedef struct {
        /**
        * Мировые координаты области.
        * Область позиционируется относительно своего центра.
        */
        d::coord3d_t coord;

        /**
        * Размер области в ячейках.
        * Хранится здесь, т.к. общая карта может иметь несколько областей.
        */
        d::sizeInt3d_t size;

        /**
        * Масштаб области или, др. словами, размер одной ячейки.
        * Это значение переопределяет масштаб, указанный для мира.
        */
        float scale;

        /**
        * Содержимое области.
        */
        content_t content;

        /**
        * Содержимое области, подготовленное для показа.
        */
        visualContent_t visualContent;

    } tinyMap_t;


    /**
    * Типы элементов.
    * Помогают оптимизировать визуализацию карты.
    * Поддерживаемые типы:
    *   - alpha За элементом могут быть видны другие элементы.
    *   - solid Непрозрачный элемент, занимающий всю ячейку.
    */
    enum typeElement_e {
        TYPE_ELEMENT_UNDEFINED = 0,
        TYPE_ELEMENT_ALPHA,
        TYPE_ELEMENT_SOLID
    };


    /**
    * Описание элемента мира, размещённого на карте.
    * Не указываем здесь метку элемента, т.к. она используется как ключ
    * в 'content_t' - и этого достаточно для работы с картой.
    */
    struct noteElement_t {
        /**
        * Название элемента.
        */
        std::string name;

        /**
        * Форма.
        */
        std::string form;

        /**
        * Материал.
        */
        std::string material;

        /**
        * Тип.
        */
        typeElement_e type;


        inline noteElement_t(
        ) :
            type( TYPE_ELEMENT_UNDEFINED )
            // остальные хар-ки имеют свой конструктор по умолчанию
        {
        }

    };


    /**
    * Структура с информацией о мире.
    */
    struct about_t {
        /**
        * Название карты.
        */
        std::string name;

        /**
        * Масштаб карты.
        */
        float scale;

        /**
        * Путь к образам карты.
        */
        std::unordered_map< std::string, std::string >  pathToImage;

        /**
        * Карта cостоит из областей со своими координатами. Каждая область
        * имеет название.
        *
        * @todo Расположение карты может быть не только перпендик. плоскости Z.
        */
        std::unordered_map< std::string, tinyMap_t >  tinyMap;

        /**
        * Об элементах карты.
        * Каждый элемент карты tinyMap_t должен быть декларирован здесь.
        */
        std::unordered_map< signElement_t, noteElement_t >  noteMap;


        inline about_t(
        ) :
            scale( 0.0f )
            // остальные хар-ки имеют свой конструктор по умолчанию
        {
        }

    };


    /**
    * Структура для получения информации об элементе для его визуализации.
    * Реализовано ссылками для быстрого доступа (ака оглавление для конкретного
    * элемента карты).
    */
    struct visualElement_t {
        /**
        * Масштаб мира (для позиционирования элемента).
        * В общем случае, может отличаться от масштаба области карты (scale).
        */
        const float scaleWorld;

        /**
        * Метка элемента.
        * Быстрее передать копию (тип char).
        */
        const signElement_t signElement;

        /**
        * Что собой представляет элемент.
        */
        const noteElement_t& noteElement;

        /**
        * Позиции в области карты, где располагается элемент.
        */
        const BitContent3D& bitContent;

        /**
        * Мировые координаты, размер и *масштаб области* карты, которая
        * содержит этот элемент.
        */
        const d::coord3d_t& coord;
        const d::sizeInt3d_t& size;
        // Быстрее передать копию (тип float).
        const float scale;

        inline visualElement_t(
            float scaleWorld,
            signElement_t signElement,
            const noteElement_t& noteElement,
            const BitContent3D& bitContent,
            const d::coord3d_t& coord,
            const d::sizeInt3d_t& size,
            float scale
        ) :
            scaleWorld( scaleWorld ),
            signElement( signElement ),
            noteElement( noteElement ),
            bitContent( bitContent ),
            coord( coord ),
            size( size ),
            scale( scale )
        {
        }

    };



    /**
    * Общий итератор по карте.
    */
    class iterator :
        public std::iterator< std::forward_iterator_tag, Vizi3DWorld >
    {
    public:
        inline iterator(
        ) :
            map( nullptr ),
            mTinyMap( std::unordered_map< std::string, tinyMap_t >::const_iterator() ),
            mMapElement( content_t::const_iterator() )
        {
        }

        inline iterator(
            Vizi3DWorld& map,
            std::unordered_map< std::string, tinyMap_t >::const_iterator  tinyMap
        ) :
            map( &map ),
            mTinyMap( tinyMap )
        {
            // инициируем первым элементом
            assert( (map.about().tinyMap.size() > 0) 
                && "Нет возможности инициировать указатель на пустой карте (отсутствуют областей)." );
            auto dest = (tinyMap == map.about().tinyMap.cend())
                ? map.about().tinyMap.cbegin()
                : tinyMap;
            assert( (dest->second.content.cbegin() != dest->second.content.cend()) 
                && "Попытка инициировать указатель на пустом содержании (отсутствуют элементы в области)." );
            mMapElement = dest->second.content.cbegin();
        }

        /**
        * @return Итератор не указывает на элемент (указывает на cend()).
        *
        * Работает быстрее (itr == cend()).
        */
        inline bool empty() const {
            // Признаком конца списка служит только указатель на области карты
            return !map || (mTinyMap == map->about().tinyMap.cend());
        }

        /**
        * @return Текущий указатель на область карты.
        */
        inline std::unordered_map< std::string, tinyMap_t >::const_iterator  tinyMap() {
            return mTinyMap;
        }

        /**
        * @return Текущий указатель на элемент области карты.
        *         nullptr, если указателя нет.
        */
        inline content_t::const_iterator  mapElement() {
            return mMapElement;
        }

        inline iterator& operator=( const iterator& itr ) {
            map = itr.map;
            mTinyMap = itr.mTinyMap;
            mMapElement = itr.mMapElement;
            return *this;
        }

        inline bool operator==( const iterator& itr ) const {
            if ( empty() && itr.empty() ) {
                return true;
            }
            return (mTinyMap == itr.mTinyMap)
                && (mMapElement == itr.mMapElement);
        }

        inline iterator& operator++() {
            assert( map && "Итератор не инициализирован." );
            if ( empty() ) {
                // остаёмся на последнем элементе
                return *this;
            }
            // Указатель на элементы инициируется в конструкторе
            ++mMapElement;
            if (mMapElement == mTinyMap->second.content.cend()) {
                // переход в след. область карты
                ++mTinyMap;
                const auto dest = (mTinyMap == map->about().tinyMap.cend())
                    // Берём первую область карты, чтобы инициировать указатель
                    ? map->about().tinyMap.cbegin()
                    : mTinyMap;
                mMapElement = dest->second.content.cbegin();
            }
            return *this;
        }


    private:
        Vizi3DWorld* map;

        /**
        * Текущая позиция итератора по областям карты.
        */
        std::unordered_map< std::string, tinyMap_t >::const_iterator  mTinyMap;

        /**
        * Текущая позиция итератора по элементам области карты.
        * Каждая область карты сохранена в виде знака элемента и битового образа
        * к этому знаку (эффективное хранение / доступ).
        */
        content_t::const_iterator  mMapElement;

        /**
        * Текущая позиция итератора по содержимому элемента (по битовому образу элемента).
        * Представляет собой одномерный указатель на элемент.
        * @see BitContent3D::ic()
        *//* - Достаточно двигаться по элементам области карты.
        size_t elementContent;
        */
    };





public:
    inline Vizi3DWorld(
        Ogre::SceneManager* sm
    ) :
        sm( sm ),
        mAbout()
    {
    }



    virtual inline ~Vizi3DWorld() {
    }




    /**
    * Загружает файл с картой.
    */
    void load( const std::string& name );



    /**
    * @return Информация о карте.
    */
    inline const about_t& about() const {
        return mAbout;
    }



    /**
    * @return Область карты или nullptr, если отсутствует.
    */
    inline const tinyMap_t* tinyMap( const std::string& name ) const {
        auto fi = mAbout.tinyMap.find( name );
        return (fi == mAbout.tinyMap.end()) ? nullptr : &fi->second;
    }


    /**
    * @return Области карты.
    *
    * @alias about().tinyMap
    */
    inline const std::unordered_map< std::string, tinyMap_t >&  tinyMap() const {
        return about().tinyMap;
    }



    /**
    * @return Знак карты или nullptr, если отсутствует.
    */
    inline const noteElement_t* noteMap( signElement_t name ) const {
        auto fi = mAbout.noteMap.find( name );
        return (fi == mAbout.noteMap.end()) ? nullptr : &fi->second;
    }


    /**
    * @return Знаки карты.
    *
    * @alias about().noteMap
    */
    inline const std::unordered_map< signElement_t, noteElement_t >&  noteMap() const {
        return about().noteMap;
    }



    /**
    * Отображает на холсте след. ещё не отрисованный элемент.
    */
    void drawNext();




    /**
    * Итераторы для обхода карты.
    */
    inline iterator cbegin() {
        // на первую область карты
        return iterator( *this, mAbout.tinyMap.cbegin() );
    }


    inline iterator cend() {
        // за последней областью карты
        return iterator( *this, mAbout.tinyMap.cend() );
    }




    /**
    * @return Об элементе карты, расположенном по итератору.
    *         nullptr, если итератор не указывает на элемент.
    */
    inline std::shared_ptr< visualElement_t >  get( iterator& itr ) const {

        if ( itr.empty() ) {
            // Итератор не указывает ни на один элемент карты
            return std::shared_ptr< visualElement_t >();
        }

        const auto tinyMapItr = itr.tinyMap();
        const auto mapElementItr = itr.mapElement();

        const float scaleWorld = mAbout.scale;
        const signElement_t signElement = mapElementItr->first;

  
        assert( (mAbout.noteMap.find( signElement ) != mAbout.noteMap.cend())
            && "Каждый элемент должен быть декларирован в 'note'." );
        const noteElement_t& noteElement = mAbout.noteMap.at( signElement );

        const BitContent3D& bitContent = *mapElementItr->second;
        const d::coord3d_t& coord = tinyMapItr->second.coord;
        const d::sizeInt3d_t& size = tinyMapItr->second.size;
        const float scale = tinyMapItr->second.scale;

        return std::shared_ptr< visualElement_t >( new visualElement_t(
            scaleWorld,
            signElement,
            noteElement,
            bitContent,
            coord,
            size,
            scale
        ) );
    }






private:
    /**
    * @return Название области карты. Для внутр. исп..
    */
    inline std::string parseTinyMapName( const std::string& keyName ) {
        std::vector< std::string >  r;
        boost::split( r, keyName, boost::is_any_of( "-" ), boost::token_compress_off );
        assert( (r.size() == 3) && "Ключ должен состоять из трёх частей." );
        const std::string name = r.back();
        assert( !name.empty() && "Название области карты не может быть пустым." );

        return name;
    }






private:
    /**
    * Извлекает область карты из JSON-структуры.
    *
    * @param defaultScale Масштаб по умолчанию. Будет установлен для области,
    *        если она не переопределяет масштаб, указанный для мира.
    */
    static tinyMap_t parseTinyMap( json_value* ptr, float defaultScale );


    /**
    * Извлекает декларации символов карты из JSON-структуры.
    */
    static std::unordered_map< signElement_t, noteElement_t >  parseNoteMap( json_value* ptr );



    /**
    * Подготавливает битовый образ для визуализации мира.
    */
    void prepareVisualBitImage();




private:
    Ogre::SceneManager* sm;


    /**
    * Указатель на след. неотрисованный элемент карты.
    * == cend(), когда все элементы отрисованы.
    */
    iterator drawItr;


    /**
    * Информация о мире.
    */
    about_t mAbout;

};

