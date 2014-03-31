#pragma once

#include <Ogre.h>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
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

    // @info Содержимое мира оформлено в виде структур, чтобы акцентировать
    //       внимание: максимально *эффективно* обрабатываем *данные*.
    //       Класс Vizi3DWorld поставляет необходимые для этого методы.

    /**
    * Каждый элемент области на карте обозначается уникальной (в этой области)
    * меткой.
    * В то же время, битовые образы хранятся с привязкой к названию элемента,
    * т.к. одного байта (символа) на идентификацию элемента в действительно
    * большом мире может не хватать.
    */
    typedef char sign_t;

    /**
    * Содержимое области.
    * Записывается как название элемента (почему - см. комм. к 'sign_t') и
    * битовый 3D-образ области, где елемент встречается.
    * Т.о. каждый *уникальный* элемент *области* занимает в памяти места как
    * объём области в ячейках / 8 бит. Для области 20 x 20 x 5 это будет =
    * = 2000 / 8 = 250 байт.
    *
    * @todo Как быть с вещами? Их, разных, может быть *много*. И они могут
    *       располагаться в одной ячейке.
    */
    typedef std::unordered_map<
        std::string,
        // Обёрнут в указатель, чтобы не определять для BitContent3D
        // конструктор по умолчанию
        std::shared_ptr< BitContent3D >
    > element_t;

    typedef std::shared_ptr< element_t >  elementPtr_t;


    /**
    * Область, подготовленная для показа.
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
    } visualContentElement_t;

    typedef std::shared_ptr< visualContentElement_t >  visualContentElementPtr_t;


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
    * Общее описание элемента мира.
    */
    struct noteCommon_t {
        friend class note_t;

        /**
        * Метка элемента.
        * Служит для визуального обозначения элемента на карте.
        * Не является ключом, т.к. область составляется из неск.
        * json-деклараций и может содержать более 256 элементов.
        */
        sign_t sign;

        /**
        * Название элемента.
        * Служит идентификатором для битовой карты элемента в области.
        * Записывается в качестве ключа 'map'.
        */
        std::string name;

        /**
        * Описание элемента.
        */
        std::string description;

        inline bool operator==( const noteCommon_t& b ) {
            return (sign == b.sign)
                && (name == b.name)
                && (description == b.description);
        }
    };

    typedef std::shared_ptr< noteCommon_t >  noteCommonPtr_t;




    /**
    * Описание физических характеристик элемента мира.
    */
    struct notePhysics_t {
        /**
        * Масса элемента.
        */
        float mass;

        /**
        * Материал, из которого сделан элемент.
        */
        std::string material;

        /**
        * Агрегатное состояние элемента.
        *//* - @todo
        enum physicsState_e state;
        */

        /**
        * Уточняющие координаты элемента.
        * Позволяют позиционировать э. не точно по центру, а сместив
        * его на указанный %.
        *//* - @todo
        d::coord3d_t accurateCoord;
        */

        inline bool operator==( const notePhysics_t& b ) {
            return (
                (abs(mass - b.mass) < d::PRECISION)
             && (material == b.material)
            );
        }
    };

    typedef std::shared_ptr< notePhysics_t >  notePhysicsPtr_t;


    /**
    * Описание визуального образа элемента мира.
    */
    struct noteVisual_t {
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


        inline noteVisual_t(
        ) :
            type( TYPE_ELEMENT_UNDEFINED )
            // остальные хар-ки имеют свой конструктор по умолчанию
        {
        }

        inline bool operator==( const noteVisual_t& b ) {
            return (
                (form == b.form)
             && (material == b.material)
             && (type == b.type)
            );
        }
    };

    typedef std::shared_ptr< noteVisual_t >  noteVisualPtr_t;



    /**
    * Общая информация о мире.
    */
    struct infoWorld_t {
        /**
        * Название мира.
        */
        std::string name;

        /**
        * Описание мира.
        */
        std::string description;

        /**
        * Пути к необходимым миру ресурсам.
        */
        //std::unordered_map< std::string, std::string >  pathToImage;
        std::vector< std::string >  pathToResource;

        inline bool operator==( const infoWorld_t& b ) {
            return (
                (name == b.name)
             && (description == b.description)
             && (pathToResource == b.pathToResource)
            );
        }
    };

    typedef std::shared_ptr< infoWorld_t >  infoWorldPtr_t;


    /**
    * Содержимое области. Неделимое.
    *
    * Всегда декларируется центральная ячейка, но каждое содержание
    * хранит развёрнутый перпендикулярно направлению взгляда элемент.
    * Сделано для того, чтобы можно было нарисовать стену / потолок,
    * заросшие травой, а не только поместить блок аля "трава на земле".
    *
    * Всегда смотрим сверху-вниз, слева-направо. Обход по часовой стрелке,
    * начинаем сверху:
    *   0 - потолок
    *   1 - справа
    *   2 - пол (самый частый гость)
    *   3 - слева
    *   4 - ближе
    *   5 - дальше
    */
    typedef boost::array< elementPtr_t, 6 >  content_t;


    /**
    * Об элементах, расположенных в области.
    * Каждый атомарный элемент области должен быть декларирован здесь.
    * Разные области мира могут содержать элементы с одинаковой меткой,
    * но разным названием - и это *разные* элементы.
    */
    struct note_t {
        std::unordered_map< std::string, noteCommon_t  >  common;
        std::unordered_map< std::string, notePhysics_t >  physics;
        std::unordered_map< std::string, noteVisual_t  >  visual;

        inline bool operator==( const note_t& b ) {
            return (
                (common == b.common)
             && (physics == b.physics)
             && (visual == b.visual)
            );
        }

    };

    typedef std::shared_ptr< note_t >  notePtr_t;



    /**
    * Структура с информацией о мире.
    * Оптимизирована для быстрой работы.
    * Мир - набор областей фиксированного размера с заданными координатами
    * (области разбиты на ячейки). По сути, области - тот же мир, но:
    *   1. Всегда меньшего размера.
    *   2. Могут содержать "атомы информации" - то, что не детализируется.
    */
    struct aboutArea_t {
        /**
        * Название области.
        * Представлено в виде списка, т.к. область может складываться из неск.
        * файлов (разделённых для удобства). Каждый файл дополняет название.
        */
        std::vector< std::string >  name;

        /**
        * Описание области.
        * @see Прим. к 'name'.
        */
        std::vector< std::string >  description;

        /**
        * Размер областей, которые включены в эту область. Задаётся в ячейках.
        * Все входящие в мир (== в область) меньшие области должны быть
        * одинакового размера.
        * Для области, которая не содержит других областей, значение размера
        * не имеет смысла.
        *//* - См. @todo к 'tinyArea'.
        d::sizeInt3d_t sizeTinyArea;
        */

        /**
        * Об элементах, расположенных в области.
        * Каждый атомарный элемент области должен быть декларирован здесь.
        * Разные области мира могут содержать элементы с одинаковой меткой,
        * но разным назначением.
        *//* - Заменено на структуру.
        std::unordered_map< sign_t, noteCommon_t >   noteCommonMap;
        std::unordered_map< sign_t, noteVisual_t >   noteVisualMap;
        std::unordered_map< sign_t, notePhysics_t >  notePhysicsMap;
        */
        note_t note;

        /**
        * Координаты области относительно родительского мира или - @todo -
        * другой области. Для корневого мира значение не имеет смысла.
        * Область позиционируется относительно своего центра.
        */
        d::coordInt3d_t worldCoord;

        /**
        * Меньшая область мира, перпендикулярная оси OZ: всегда смотрим
        * сверху-вниз, слева-направо.
        * Может отсутствовать, если у области есть содержимое (см. ниже).
        *//* - @todo extend Сейчас всегда отсутствует: работаем с миром,
                     у которого одно содержание.
        std::shared_ptr< aboutArea_t >  tinyArea;
        */

        /**
        * Содержимое области.
        * Может отсутствовать, если у области есть меньшая область 'tinyArea'.
        */
        content_t content;

        /**
        * Содержимое области, подготовленное для показа.
        * Используется для оптимизации отрисовки.
        * Имеет смысл только при наличии содержимого.
        */
        visualContentElementPtr_t visualContentElement;


        inline aboutArea_t(
        ) :
            //sizeTinyArea( d::ZEROINT3D ),
            worldCoord( d::ZERO3D )
            // остальные хар-ки имеют свой конструктор по умолчанию
        {
        }


        /**
        * @return Размер области в ячейках. Все области мира - одинакового
        *         размера. Все содержания мира - одинакового размера.
        */
        inline d::sizeInt3d_t size() const {
            // Размер области определим по первому не пустому содержанию
            for (auto itr = content.cbegin(); itr != content.cend(); ++itr) {
                const auto element = *itr;
                if ( element ) {
                    const auto elementItr = element->cbegin();
                    return (elementItr == element->cend())
                        ? (d::sizeInt3d_t)d::ZEROINT3D
                        : elementItr->second->size();
                }
            }
            return (d::sizeInt3d_t)d::ZEROINT3D;
        }


        inline bool operator==( const aboutArea_t& b ) {
            return (
                (name == b.name)
             && (description == b.description)
             && (note == b.note)
             && ( (d::coordInt3d_t)worldCoord == (d::coordInt3d_t)b.worldCoord )
             && (content == b.content)
             && (visualContentElement = b.visualContentElement)
            );
        }
    };

    typedef std::shared_ptr< aboutArea_t >  aboutAreaPtr_t;


    /**
    * Загруженные части мира (рабочая область).
    * См. принятую нумерацию ячеек в helper::isc().
    */
    typedef boost::array< aboutAreaPtr_t, 27 >  workArea_t;


    /**
    * Информация о мире:
    *   - ячейка в которой находится обозреватель
    *   - прилегающие к ней ячейки
    * 3D, всего - 27 ячеек.
    */
    struct about_t {
        /**
        * Общая информация.
        */
        infoWorld_t infoWorld;

        /**
        * Рабочие области (загруженные части мира).
        * 
        * @see load()
        */
        workArea_t workArea;


        /**
        * @return Размер мира в ячейках, с которым в данный момент
        *         можем работать. Складывается как три размера центральной
        *         области (все области - одинакового размера).
        */
        inline d::sizeInt3d_t size() const {
            if ( !workArea[0] ) {
                return d::ZEROINT3D;
            }
            return workArea[0]->worldCoord * 3;
        }

        inline bool operator==( const about_t& b ) {
            return (
                (infoWorld == b.infoWorld)
             && (workArea == b.workArea)
            );
        }
    };




    /**
    * Структура для получения информации об элементе для его визуализации.
    * Реализовано ссылками для быстрого доступа (ака оглавление для конкретного
    * элемента карты).
    */
    struct vce_t {
        /**
        * Метка элемента.
        * Быстрее передать копию (тип char).
        */
        const sign_t sign;

        /**
        * Что собой представляет элемент.
        */
        const noteVisual_t& note;

        /**
        * Позиции в области мира, где располагается элемент.
        */
        const BitContent3D& bitContent;

        /**
        * Мировые координаты и размер области мира, которая
        * содержит этот элемент.
        */
        const d::coordInt3d_t& worldCoord;
        const d::sizeInt3d_t& size;

        inline vce_t(
            sign_t sign,
            const noteVisual_t& note,
            const BitContent3D& bitContent,
            const d::coordInt3d_t& worldCoord,
            const d::sizeInt3d_t& size
        ) :
            sign( sign ),
            note( note ),
            bitContent( bitContent ),
            worldCoord( worldCoord ),
            size( size )
        {
        }
    };

    typedef std::shared_ptr< vce_t >  vcePtr_t;



    /**
    * Итератор по элементам мира (мир загружен в 'about_t').
    * Проходит последовательно:
    *   - по 27 областям 'about_t'
    *   - по 6 единицам содержания 'content_t'
    *   - по каждому элементу содержания 'element_t'
    */
    class WorldElementIterator :
        public std::iterator< std::forward_iterator_tag, Vizi3DWorld >
    {
    public:
        inline WorldElementIterator(
            about_t* world = nullptr,
            // Область мира, 27 единиц
            workArea_t::const_iterator  workArea = workArea_t::const_iterator(),
            // Содержание области, 6 единиц
            // Не имеет смысла, если workArea никуда не указывает
            content_t::const_iterator  content = content_t::const_iterator(),
            // Текущий элемент содержания, сколько угодно единиц
            // Не имеет смысла, если workArea никуда не указывает
            element_t::const_iterator  element = element_t::const_iterator()
        ) :
            world( world ),
            workArea( workArea ),
            content( content ),
            element( element )
        {
            if ( world && (workArea != world->workArea.cend()) ) {
                // Надо позиционировать на первый элемент первого содержания
                // указанной рабочей области?
                if (content != workArea->get()->content.cend()) {
                    content = workArea->get()->content.cbegin();
                    if (element != content->get()->cend()) {
                        element = content->get()->cbegin();
                    }
                }
            }
        }


        /**
        * @return Мир не содержит ни одной области (все ячейки пустые) или
        *         итератор не проинициализирован (world == nullptr).
        */
        inline bool empty() const {
            if ( !world ) {
                return true;
            }
            // Проверяем 27-мь областей
            for (auto itr = world->workArea.cbegin(); itr != world->workArea.cend(); ++itr) {
                if ( *itr ) {
                    return false;
                }
            }
            return true;
        }


        /**
        * @return Итератор указывает на последнюю область мира. Т.е.,
        *         больше непройденных элементов нет.
        */
        inline bool onEnd() const {
            return world && (workArea == world->workArea.cend());
        }


        inline WorldElementIterator& operator=( const WorldElementIterator& itr ) {
            world = itr.world;
            workArea = itr.workArea;
            content = itr.content;
            element = itr.element;
            return *this;
        }


        inline bool operator==( const WorldElementIterator& itr ) const {
            const bool ea = empty();
            const bool eb = itr.empty();
            if ( ea && !eb ) {
                return false;
            }
            if ( !ea && eb ) {
                return false;
            }
            if ( ea && eb ) {
                return true;
            }
            return (workArea == itr.workArea)
                && (content == itr.content)
                && (element == itr.element);
        }


        /**
        * @see nextContentElement()
        */
        inline WorldElementIterator& operator++() {
            nextContentElement();
            return *this;
        }


        /**
        * Переводит итератор на след. позицию.
        *
        * @alias operator++
        */
        // Двигаемся по элементам (самый глубокий уровень)
        void nextContentElement() {
            if ( empty() ) {
                // остаёмся на последнем элементе
                return;
            }
            /* - Проверяем ниже.
            // Остались непросмотренные области?  27 ед.
            if (workArea == world->workArea.cend()) {
                // Нет
                return;
            }
            // Осталось непросмотренное содержание?  6 ед.
            if (content == workArea->get()->content.cend()) {
                // Нет
                return;
            }
            // Остались непросмотренные элементы?  ~ ед.
            if (element == content->get()->cend()) {
                // Нет
                return;
            }
            */

            // Это последний элемент?
            if (element == content->get()->cend()) {
                if (content == workArea->get()->content.cend()) {
                    // Всё содержание просмотрено. Как на счёт областей?
                    if (workArea == world->workArea.cend()) {
                        // Все области просмотрены
                        // больше делать нечего
                    } else {
                        // Ещё есть непройденные области
                        ++workArea;
                        content = workArea->get()->content.cbegin();
                    }
                    return;
                } else {
                    // Ещё есть непройденное содержание
                    ++content;
                    element = content->get()->cbegin();
                    return;
                }
            } else {
                // перемещаемся на след. элемент мира
                ++element;
            }
        }


    public:
        about_t* world;

        /**
        * Итератор по 27-ми *областям* мира.
        */
        workArea_t::const_iterator  workArea;

        /**
        * Итератор по 6 *содержаниям* областей.
        * Каждая область карты сохранена 6-тью наборами - от потолка (0)
        * до ближнего (5).
        */
        content_t::const_iterator  content;

        /**
        * Итератор по *элементам* набора области карта.
        * Елемент хранится в виде UID и битового образа к этому UID
        * (эффективное хранение / доступ).
        */
        element_t::const_iterator  element;

        /**
        * Текущая позиция итератора по содержимому элемента (по битовому образу элемента).
        * Представляет собой одномерный указатель на элемент.
        * @see BitContent3D::ic()
        *//* - Достаточно двигаться по элементам области карты.
        size_t elementContent;
        */
    };





public:
    /**
    * Загружает область мира по координатам обозревателя.
    * Информация о мире хранится в файлах, которые распределены по папкам.
    * Папки хранят области и вложены друг в друга в порядке Z->Y->X (название
    * папки - это координата области в мире).
    * Размер мира определяется по наличию папок, где хранятся области.
    * Размеры всех областей - одинаковы.
    *
    * @param sourceWorld Папка, где хранится мир.
    * @param sm Для визуализации мира. Допустимо указывать nullptr, когда
    *        не планируется исп. визуализацию.
    */
    inline Vizi3DWorld(
        const std::string& sourceWorld,
        Ogre::SceneManager* sm
    ) :
        sourceWorld( sourceWorld ),
        sm( sm ),
        mAbout()
    {
        //assert( sm ); - Допустимо.

        // Получаем общую информацию о мире здесь, чтобы вызов load() проходил
        // максимально быстро
        mAbout.infoWorld = loadInfo( sourceWorld );

        /* - Ресурсы подключаются по требованию для каждой области.
        // Загружаем требуемые областями мира ресурсы
        loadResource( mAbout.infoWorld.pathToImageForm );
        loadResource( mAbout.infoWorld.pathToImageMaterial );
        */
    }



    virtual inline ~Vizi3DWorld() {
    }




    /**
    * Загружает области мира по координате обозревателя.
    * Файлы JSON с префиксом '-' в имени не участвуют в процессе.
    *
    * @see about_t
    */
    void load( d::coordInt3d_t observer );



    /**
    * @return Информация о карте.
    */
    inline const about_t& about() const {
        return mAbout;
    }


    inline about_t& about() {
        return mAbout;
    }



    /**
    * @return Количество загруженных рабочих областей.
    */
    inline const size_t countWorkArea() const {
        // Проверяем все 27-мь областей
        /*
        size_t n = 0;
        for (auto itr = mAbout.workArea.cbegin(); itr != mAbout.workArea.cend(); ++itr) {
            if ( *itr ) {
                ++n;
            }
        }
        */
        /*
        return ( mAbout.workArea.static_size - std::count(
            mAbout.workArea.cbegin(), mAbout.workArea.cend(), nullptr
        ) );
        */
        return std::count_if(
            mAbout.workArea.cbegin(),
            mAbout.workArea.cend(),
            [] ( const aboutAreaPtr_t& aa ) {
                return (aa != nullptr);
            }
        );
    }



    /**
    * @return Количество единиц содержания в рабочей области.
    */
    inline const size_t countContent( const aboutArea_t& aa ) const {
        // Проверяем все 6-ть единиц содержания
        /*
        return ( aa.content.static_size - std::count(
            aa.content.cbegin(), aa.content.cend(), nullptr
        ) );
        */
        return std::count_if(
            aa.content.cbegin(),
            aa.content.cend(),
            [] ( const elementPtr_t& e ) {
                return (e != nullptr);
            }
        );
    }



    /**
    * Отображает на холсте след. ещё не отрисованный элемент.
    */
    void drawNext();




    /**
    * Итераторы для последовательного обхода элементов мира.
    */
    inline WorldElementIterator cbegin() {
        // на первый элемент первого содержания первой рабочей области
        return WorldElementIterator( &mAbout, mAbout.workArea.cbegin() );
    }


    inline WorldElementIterator cend() {
        // за последним элементом последнего содержания последней рабочей области
        return WorldElementIterator( &mAbout, mAbout.workArea.cend() );
    }




    /**
    * @return Об элементе мира.
    *         nullptr, если итератор не указывает на элемент.
    */
    inline vcePtr_t get( WorldElementIterator& itr ) const {

        return vcePtr_t();

        /*
        if ( itr.empty() ) {
            // Итератор не указывает ни на один элемент карты
            return vePtr_t();
        }

        const auto tinyMapItr = itr.tinyMap();
        const auto mapElementItr = itr.mapElement();

        const float scaleWorld = mAbout.scale;
        const sign_t sign = mapElementItr->first;

  
        assert( (mAbout.noteVisualMap.find( sign ) != mAbout.noteVisualMap.cend())
            && "Каждый элемент должен быть декларирован в 'note'." );
        const noteVisual_t& note = mAbout.noteVisualMap.at( sign );

        const BitContent3D& bitContent = *mapElementItr->second;
        const d::coord3d_t& coord = tinyMapItr->second.coord;
        const d::sizeInt3d_t& size = tinyMapItr->second.size;
        const float scale = tinyMapItr->second.scale;

        return vePtr_t( new visualElement_t(
            scaleWorld,
            sign,
            note,
            bitContent,
            coord,
            size,
            scale
        ) );
        */
    }






    inline bool operator==( const Vizi3DWorld& b ) {
        return ( about() == b.about() );
    }






private:
    /**
    * Загружает общую информацию о мире.
    * Информация должна быть декларирована в файле 'about.json'.
    */
    static infoWorld_t loadInfo( const std::string& folder );


    /**
    * Загружает в Ogre3D ресурсы.
    * Ресурсы могут находиться во вложенных папках.
    * Папки с префиксом '-' игнорируются.
    *
    * @param depthFolder true, если нужно подклюяить все вложенные папки
    */
    static void loadResource( const std::string& pathToResource, bool depthFolder );


    /**
    * Загружает область мира.
    * Заполняется 'area'.
    *
    * @param location Область мира однозначно идентифицируется координатой.
    *
    * @see about_t
    */
    void loadArea( aboutAreaPtr_t area, const d::coordInt3d_t& location );

    
    /**
    * Заполняет область мира данными из указанного файла.
    */
    static void loadArea( aboutAreaPtr_t area, const std::string& fileName );




    /**
    * Извлекает область карты из JSON-структуры и *дописывает* данные
    * в 'area'.
    *
    * @param ptr Указатель на первый элемент списка содержания.
    */
    static void fillContentArea( elementPtr_t element, const note_t& note, json_value* ptr );


    /**
    * Извлекает декларации символов карты из JSON-структуры.
    */
    static void fillNoteMap( std::unordered_map< std::string, noteCommon_t  >&  r,  json_value* ptr );
    static void fillNoteMap( std::unordered_map< std::string, notePhysics_t >&  r,  json_value* ptr );
    static void fillNoteMap( std::unordered_map< std::string, noteVisual_t  >&  r,  json_value* ptr );


    /**
    * @return Название элемента по его метке или пустую строку, если
    *         элемент не найден.
    */
    static inline std::string nameElement( sign_t sign, const note_t& note ) {
        const auto itr = std::find_if(
            note.common.cbegin(),
            note.common.cend(),
            [ sign ] ( const std::pair< std::string, noteCommon_t >&  el ) {
                return (el.second.sign == sign);
            }
        );
        return (itr == note.common.cend()) ? "" : itr->second.name;
    }



    /**
    * @return Строка символов, лишённая разделителей-пробелов.
    *
    * В символьной карте содержимого каждый элемент разделён пробелом, чтобы
    * содержимое легче воспринималось визуально.
    */
    static inline std::string prepareBitLine( const char* line ) {
        std::string r = "";
        std::string t = "_";
        for (size_t i = 0; line[i] != 0; i += 2) {
            t[0] = line[i];
            r += t;
            // Символ может быть один или строка может оказаться с лишним символом
            if (line[ i + 1 ] == 0) {
                break;
            }
        }
        return r;
    }



    /**
    * Подготавливает битовый образ для визуализации мира.
    */
    static void prepareVisualBitImage( aboutAreaPtr_t area );




private:
    /**
    * Источник с информацией о мире (папка с файлами).
    */
    std::string sourceWorld;


    /**
    * Оптимизированная информация о мире.
    */
    about_t mAbout;


    /**
    * Используется для визуализации.
    */
    Ogre::SceneManager* sm;

    /**
    * Указатель на след. неотрисованный элемент карты.
    * == cend(), когда все элементы отрисованы.
    */
    WorldElementIterator drawItr;

};
