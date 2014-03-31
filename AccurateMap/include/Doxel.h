#pragma once

#include "configure.h"
#include "default.h"
#include "DStore.h"
#include "ContentDoxel.h"
#include <boost/array.hpp>
#include <unordered_set>


/**
* Доксель - объект с равными измерениями. Доксель хранит некое содержание (контент)
* и может детализировать это содержание, *удваивая* точность *равномерно по
* всем* измерениям. Точность - вовсе не обязательно что-то наглядное. Это может
* быть и коротким / подробным рассказом о чём-то, что представлено докселем.
* Доксель сам определяет, что значит для него "удвоить точность".
*
* Каждый доксель характеризуется размером. Например, для докселя,
* представляющего некую часть 3D-пространства, размером может служить длина,
* ширина и высота этой области пространства. Но т.к. доксель - куб (по всем
* измерениям), можно ограничиться одной, максимальной, стороной.
*
* Потомки (уточняющие доксели) имеют свою координату в д-родителе. Кол-во
* областей, в которых может находиться потомок - ограничено кол-вом потомков
* (один потомок в одной области). Эта область называется дантом. Нумерация
* дантов - фиксирована и строится по след. принципу:
*
*   1D доксель, X
*     + (с правой стороны д.)
*     - (с левой стороны д.)
*
*   2D доксель, XY
*     ++
*     +-
*     --
*     -+
*
*   3D доксель, XYZ
*     (верх)
*     +++
*     +-+
*     --+
*     -++
*     (низ)
*     ++-
*     +--
*     ---
*     -+-
*
* и т.д.
*
* Уровни точности докселя:
*   0-й  1 еденица содержания (см. template::ContentT).
*   1-й  От 0 до 2^D докселей и соотв. столько же едениц содержания.
*   2-й  От 0 до 2^(D*2) докселей и соотв. столько же едениц содержания.
*   3-й  От 0 до 2^(D*3) докселей и соотв. столько же едениц содержания.
*   4-й  ...
* На количество уровней точности нет ограничения.
* @see DoxelTest.h
*
* Уточнение - это создание потомков для д.. Доксель, который содержит потомков
* называется родителем. Каждый потомок может иметь только одного родителя.
* Кол-во потомков у д. - см. выше "уровни точности".
*
* Доксель, который не имеет потомков, называется корневым докселем.
*
* Чтобы не хранить все уровни потомков, доксель обобщает хранимую информацию
* с помощью процедур. Благодаря им, д. может генерировать любой уровень
* точности. Примеры процедур - см. ContentDoxel.
*
* В этом проекте исп. три измерения: по осям OX / OY / OZ. Удвоение точности -
* создание 2^3 = 8 потомков, детализирующих содержимое. Содержимое - некая
* ячейка с данными (см. AboutCellCD).
*
* @template D  Количество измерений докселя.
* @template ContentT Чем представлено содержание докселя. Каждый доксель может
*           владеть данными определённого и только одного класса.
* @template SizeT Чем представлен размер докселя.
*/
template< size_t D, typename SizeT, typename ContentT >
class Doxel {
public:
    /**
    * Дант докселя. См. опред. выше.
    *   0 - минус
    *   1 - плюс
    *//* - Не удаётся красиво подключить в другие классы. Вынесено в 'd::'.
    typedef std::bitset< D >  dant_t;
    */

    /**
    * Потомки д., хранимые в оперативной памяти.
    */
    typedef boost::array<
        std::shared_ptr< Doxel >,
        // Количество потомков = 2 ^ D. Для трёх измерений: 2 ^ 3 = 8 потомков.
        d::pow2< 2, D >::value
    > child_t;
    //typedef std::vector< std::shared_ptr< Doxel > >  child_t;

    /**
    * Потомки д., хранимые во внешнем хранилище.
    */
    typedef boost::array<
        d::uid_t,
        // @see child_t
        d::pow2< 2, D >::value
    > externChild_t;
    //typedef std::vector< d::externKey_t >  externChild_t;


    struct Auxiliary;
    struct Externality;
    struct Statistics;


public:
    inline Doxel(
        const SizeT&                 size,
        std::shared_ptr< ContentT >  content,
        std::shared_ptr< Doxel >     parent,
        const d::dant_t&                dant
    ) :
        s( size ),
        cnt( content ),
        p( parent ),
        dt( dant )
        // Потомков докселя создадим по первому требованию
    {
        static_assert( (D <= 3), "Размерность докселя не может превышать заданной для d::dant_t" );

        aux.ext.key = d::uid_t();
        //aux.ext.childKey = externChild_t(); - Создадим по первому требованию
        aux.ext.parentKey = d::uid_t();

        clearAuxStat();
    }



    /**
    * Загружает доксель из внешнего хранилища.
    */
    inline Doxel(
        const d::uid_t& externKey
    ) {
        const auto ld = load( externKey );
        aux = ld.auxiliary();
        s = ld.size();
        cnt = ld.content();
        p = ld.parent();

        // Потомков докселя создадим по первому требованию

        clearAuxStat();
    }



    virtual inline ~Doxel() {
    }



    /**
    * @return Размерность докселя.
    */
    inline size_t dimension() const {
        return D;
    }



    /**
    * @return Размер докселя.
    */
    inline SizeT size() const {
        return s;
    }



    /**
    * @return Содержание докселя.
    */
    inline const std::shared_ptr< ContentT >  content() const {
        return cnt;
    }


    inline void content( const ContentT& cnt ) {
        // Храним ссылки на одно и то же содержание, не дублируем
        this->cnt = CDManager::singleton()->content( cnt );
    }



    /**
    * @return Родитель докселя.
    */
    inline std::shared_ptr< Doxel >  parent() {
        return p;
    }


    inline const std::shared_ptr< Doxel >  parent() const {
        return p;
    }



    /**
    * @return Доксель является корневым.
    */
    inline bool root() const {
        return (p.get() == nullptr);
    }



    /**
    * @return В каком данте (относительно родителя) находится доксель.
    */
    inline const d::dant_t& dant() const {
        return dt;
    }




    /**
    * @return Потомки докселя.
    *
    * @param i Номер потомка для инициализации.
    */
    inline const std::shared_ptr< child_t >  child() const {
        return chl;
    }


    inline void child( size_t i,  std::shared_ptr< Doxel > dox ) {
        // Потомков может ещё не быть
        if ( !this->chl ) {
            this->chl = std::shared_ptr< child_t >( new child_t() );
        }
        this->chl->at( i ) = dox;
    }



    /**
    * @return Количество потомков у докселя.
    */
    inline size_t countChild( size_t level ) const {
        return countChild( level, *this );
    }




    /**
    * Выгружает всех потомков докселя во внешнее хранилище.
    * Сам доксель попадает в хранилище только если он не имеет родителя.
    * Доксель именно *выгружается*:
    *   + сохраняется / обновляется в хранилище
    *   + удаляется из оперативной памяти
    *
    * @return Ключ для доступа к д. во внешнем хранилище.
    *//* - Объединено с сохранением.
    inline void unloadChild() {
        // Структура выгружаемого докселя - см. save()

        // Сохраняем доксель, если он без родителя
        // @see Нужные assert'ы сделаны в save().
        if ( !parent() ) {
            const auto key = save();
        } else {
            assert( ... );
        }
        
        // Выгружаем всех потомков докселя (которые - тоже доксели),
        // предварительно сохранив их в д-хранилище
        if ( chl ) {
            if ( !aux.ext.childKey ) {
                aux.ext.childKey = std::shared_ptr< externChild_t >( new externChild_t() );
            }
            for (auto itr = chl->cbegin(); itr != chl->cend(); ++itr) {
                auto ch = *itr;
                if ( ch ) {
                    // Сохраняем / обновляем потомка перед выгрузкой из о-памяти
                    const auto savedChildKey = ch->save();
                    const auto i = std::distance( chl->cbegin(), itr );
                    // Удаляем потомка из о-памяти, оставив лишь ссылку на потомка в д-хранилище
                    const auto unloadedChildKey = ch->unload();
                    assert( (savedChildKey == unloadedChildKey)
                        && "Ключи сохранённого и выгруженного из о-памяти потомка должны совпадать." );
                    aux.ext.childKey->at( i ) = unloadedChildKey;
                    ch.reset(); 
                }
            }
        }

        // Доксель полностью очищен от потомков. Благодаря рекурсии.
        // Сам он остаётся в памяти... пока родитель его не выгонит
        // (см. выше ch.reset() ).
    }
    */



    /**
    * Сохраняет / обновляет доксель во внешнем хранилище.
    *
    * Используем накопление данных и их последующий сброс в хранилище.
    * Корневой д. *всегда сразу* сбрасывается в хранилище, его потомки -
    * по мере накопления.
    * (!) Для гарантированного сохранения докселя, который не является
    * корневым, следует вызвать метод flush().
    *
    * @param unloadFromFastMemory После сохранения в д-хранилище, выгрузить
    *        доксель из оперативной памяти. Выгружаются потомки докселя.
    *        Доксель именно *выгружается*:
    *          + сохраняется / обновляется в хранилище
    *          + *потомки* удаляются из оперативной памяти
    *
    * @return Ключ для доступа к д. во внешнем хранилище. Этот же ключ
    *         записывается этим же методом в сам доксель.
    */
    inline d::uid_t lazySave( bool unloadFromFastMemory ) {
        // Структура сохраняемого докселя - см. DStore::save()

        // Сохранить или обновить?
        //const bool savedDoxel = aux.externKey.first.empty();
        // @todo optimize Не обновлять, если доксель не был модифицирован.

        // Если у д. есть родитель, проверяем, сохранён ли родитель
        if ( parent() && aux.ext.parentKey.empty() ) {
            // Не сохранён
            // Может, сейчас как раз формируется д., а значит родитель уже был
            // сохранён для первых потомков, но новые потомки ещё об этом
            // не узнали?
            // Сохранение снизу-вверх
            // Не передаём 'unloadFromFastMemory', т.к. родитель докселя всё
            // равно не будет выгружен (выгружаются только потомки, см. ниже)
            aux.ext.parentKey = parent()->lazySave( false );
        }

        // Сохранение сверху-вниз

        // Чтобы не оставалось сомнений
        assert( ( !parent() || ( parent() && !aux.ext.parentKey.empty() ) )
            && "Родительский доксель должен быть сохранён раньше потомка." );

        
        // Доксель мог быть сохранён в хранилище раньше
        const d::uid_t prevKey = aux.ext.key;
        /* - Обрабатываем ниже.
        if ( !prevKey.empty() ) {
            // Можем ли просто возвращать ключ? А потомки? Мы потеряем их
            // в unload(), если этот метод не позаботитится об их сохранении!
            // @todo Обновлять сам доксель!
            //return aux.ext.key;
        }
        */

        // @test
        auto t = *content();
        const auto d3d = static_cast< Doxel3D* >( this );
        //std::cout << *d3d << std::endl;

        // 1
        // Доксель получает уникальный ключ 'externKey'
        // Не сохраняем доксель, если он был сохранён раньше
        if ( prevKey.empty() ) {
            aux.ext.key = DStore::singleton()->lazySave< D, SizeT, ContentT >(
                size(), *content(), aux.ext.parentKey, dant()
            );
            // Для корневого д. вызываем немедленное сохранение
            if ( root() ) {
                flush();
            }
        }

        
        // 2
        // Важно позаботится о потомках
        if ( chl ) {
            if ( !aux.ext.childKey ) {
                aux.ext.childKey = std::shared_ptr< externChild_t >( new externChild_t() );
            }
            for (auto itr = chl->begin(); itr != chl->end(); ++itr) {
                auto ch = *itr;
                if ( ch ) {
                    // Каждый потомок получает 'externKey' родителя
                    ch->auxiliaryExternality().parentKey = aux.ext.key;
                    assert( ch->parent() && "Каждый потомок должен иметь родителя." );
                    const auto i = std::distance( chl->begin(), itr );
                    // Рекурсивно сохраняем потомка в д-хранилище, понимая, что он
                    // мог быть сохранён ранее
                    if ( aux.ext.childKey->at( i ).empty() ) {
                        const auto childKey = ch->lazySave( unloadFromFastMemory );
                        aux.ext.childKey->at( i ) = childKey;
                    }
                    // Удаляем потомка из о-памяти, оставив лишь ссылку на него в д-хранилище
                    if ( unloadFromFastMemory ) {
                        ch.reset();
                    }
                }
            }
        }

        return aux.ext.key;
    }



    /**
    * Сбрасывает накопленые доксели в хранилище.
    *
    * @see lazySave()
    */
    inline void flush() const {
        DStore::singleton()->flush();
    }




    /**
    * Загружает доксель из внешнего хранилища.
    *
    * @param all true, когда надо загрузить в память и всех потомков докселя.
    *            (!) Осторожно, размеры!
    */
    static inline Doxel load( const d::uid_t& key, bool all = false ) {

        // @todo

        return Doxel( SizeT(), nullptr, nullptr, d::dant_t() );
    }




    /**
    * @return Дополнительную информацию о докселе.
    */
    inline const Auxiliary& auxiliary() const {
        return aux;
    }


    inline Externality& auxiliaryExternality() {
        return aux.ext;
    }



    /**
    * Обнуляет статистику докселя.
    */
    inline void clearAuxStat() {
        aux.stat.countRequest = 0;
        aux.stat.lastRequest = 0;
    }








private:
    /**
    * @see countChild( size_t )
    */
    static inline size_t countChild( size_t level, const Doxel& d ) {
        size_t n = 0;
        // Потомки докселя
        const auto childDox = d.child();
        if ( childDox ) {
            for (auto itr = childDox->cbegin(); itr != childDox->cend(); ++itr) {
                const auto ch = *itr;
                if ( ch ) {
                    assert( ch->parent() && "Каждый потомок должен иметь родителя." );
                    /* - Не считает доксель потомком, если у д. не оказалось
                         своих потомков. Расширено. См. ниже.
                    n += (level == 0) ? 1 : countChild( level - 1, *ch );
                    */
                    size_t w = 1;
                    if (level > 0) {
                        w = countChild( level - 1, *ch );
                    }
                    n += (w == 0) ? 1 : w;
                }
            }
        } // if ( chl )

        return n;
    }





    /**
    * Сохраняет / обновляет доксель во внешнем хранилище, двигаясь вверх,
    * от потомков к родителям.
    *
    * @return Ключ для доступа к д. во внешнем хранилище. Этот же ключ
    *         записывается этим же методом в сам доксель.
    *//*
    inline d::externKey_t saveTop() {
        // Структура сохраняемого докселя - см. DStore::save()

        // Сохранить или обновить?
        //const bool savedDoxel = aux.externKey.first.empty();
        // @todo optimize Не обновлять, если доксель не был модифицирован.

        auto store = DStore::singleton();

        if ( parent() ) {
            const bool test = true;
        }
        assert( ( !parent() || ( parent() && !aux.ext.parentKey.first.empty() ) )
            && "Родительский доксель должен быть сохранён раньше потомка." );

        // 1
        // Доксель получает уникальный ключ 'externKey'
        const d::uid_t prevKey = aux.ext.key.first;
        aux.ext.key = store->save< D, SizeT, ContentT >(
            size(), *content(), aux.ext.parentKey
        );
        assert( ( prevKey.empty() || (aux.ext.key.first == prevKey) )
            && "Новый ключ, полученный при сохранении докселя, отличается от установленного ранее." );

        // 2
        if ( chl ) {
            if ( !aux.ext.childKey ) {
                aux.ext.childKey = std::shared_ptr< externChild_t >( new externChild_t() );
            }
            for (auto itr = chl->cbegin(); itr != chl->cend(); ++itr) {
                const auto ch = *itr;
                if ( ch ) {
                    // Каждый потомок получает 'externKey' родителя
                    ch->auxiliaryExternality().parentKey = aux.ext.key;
                    assert( ch->parent() && "Каждый потомок должен иметь родителя." );
                    const auto i = std::distance( chl->cbegin(), itr );
                    // Рекурсивно сохраняем потомка в д-хранилище
                    const auto childKey = ch->save();
                    assert( ( aux.ext.childKey->at( i ).first.empty() || (childKey.first == aux.ext.childKey->at( i ).first) )
                        && "Новый ключ для потомка отличается от установленного ранее." );
                    aux.ext.childKey->at( i ) = childKey;
                }
            }
        }

        return aux.ext.key;
    }
    */





protected:
    /**
    * Каждый доксель может владеть данными определённого (см. template) класса.
    */
    std::shared_ptr< ContentT >  cnt;


    /**
    * Размер докселя.
    * Обязательная характеристика.
    */
    SizeT s;

    /**
    * Родительский доксель и дант докселя. Чтобы доксель можно было
    * позиционировать относительно родителя.
    */
    std::shared_ptr< Doxel >  p;
    d::dant_t dt;

    /**
    * Доксель делит каждое известное ему измерение пополам. Т.о. получаем
    * потомков докселя.
    *
    * @see Auxiliary::Externality::childKey
    */
    std::shared_ptr< child_t >  chl;




private:
    /**
    * Вспомогательная информация о докселе.
    */
    struct Auxiliary {
        Externality ext;
        Statistics stat;
    };
    Auxiliary aux;


    /**
    * Ключи для доступа к д. во внешнем хранилище.
    */
    struct Externality {
        /**
        * Доксель получает UID, когда выгружается во внешнее (долговременное)
        * хранилище. Используется CouchDB.
        */
        d::uid_t key;

        /**
        * Ключ для доступа к родителю докселя.
        */
        d::uid_t parentKey;

        /**
        * Тут же храним ключи потомков докселя. Т.о. сможем вытянуть потомков
        * целиком по мере необходимости.
        */
        std::shared_ptr< externChild_t >  childKey;
    };


    /**
    * Статистика обращений к докселю.
    * На её основе методы могут производить очистку редко используемых докселей.
    */
    struct Statistics {
        /**
        * Количество обращений к докселю.
        */
        size_t countRequest;

        /**
        * Время последнего обращения к докселю.
        */
        long lastRequest;
    };


};













/**
* Доксель, детализирующий 3D-пространство.
* Позволяет строить карты с разной степенью точности.
*
* Повышая точность, делит 3D-пространство тремя перпендикулярными плоскостями,
* образуя октант. Нумерация получившихся пространств (дантов) - см. в Doxel
* "нумерация дантов" - начиная с правого верхнего угла, по часовой стрелке,
* верхняя плоскость + аналогично нижняя плоскость.
*/
class Doxel3D :
    public Doxel<
        // Количество измерений
        3,
        // В качестве размера докселя берём длину / ширину / высоту. А т.к.
        // доксель - равносторонний, можем хранить размер как одно значение
        // с плавающей точкой.
        // @todo ? Позволить в шаблоне определять, в чём выражать размер докселя.
        float,
        // Содержание докселя представим такой вот структурой
        AboutCellCD
    >
{
public:
    /**
    * Тип для карты высот.
    *
    * @see loadHMImage()
    */
    typedef std::pair<
        // Размер карты высот, пкс: длина (N) x ширина (M)
        d::sizeInt2d_t,
        // Матрица N x M с высотами в диапазоне [-1; 1], 0 - уровень моря.
        std::shared_ptr< float >
    >  hmImage_t;


public:
    /**
    * @see Комм. к свойствам класса Doxel.
    */
    inline Doxel3D(
        float size,
        std::shared_ptr< AboutCellCD >  content,
        std::shared_ptr< Doxel3D >  parent,
        const d::dant_t& dant
    ) :
        Doxel( size, content, parent, dant )
    {
    }


    inline Doxel3D(
        const d::uid_t& key
    ) :
        Doxel( key )
    {
    }


    virtual inline ~Doxel3D() {
    }



    /**
    * @return 3D-доксели, выбранные из этого докселя. Результат возвращается в
    *         виде набора, т.к. в общем случае может быть запрос на доксели из
    *         участка мира, который не совпадает с размером докселя: ведь у
    *         докселя равны все стороны (измерения), всегда равны).
    *
    * @param box Область выборки - 3D-пространство.
    * @param size Размеры докселей, которые попадут в выборку.
    */
    std::unordered_set<
        std::shared_ptr< Doxel3D >
    > sampling(
        const d::box3d_t& box,
        const boost::numeric::interval< float >& size
    );



    /**
    * Создаёт 3D-доксель из карты высот (КВ).
    *
    * @param parent Родитель докселя.
    * @param dant Дан докселя. Иммет смысл только когда доксель - не корневой.
    * @param hmImage Карта высот (КВ).
    * @param worldSize Размер мира, м. Размером докселя послужит максимальный
    *        размер по оси, т.к. доксель - всегда куб.
    */
    static std::shared_ptr< Doxel3D >  valueOf(
        std::shared_ptr< Doxel3D > parent,
        const d::dant_t& dant,
        const hmImage_t& hmImage,
        const d::size3d_t& worldSize
    );


    /*
    * Тоже, что выше, с указанием имени файла для чтения карты высот.
    *
    * @see Описание параметров в loadHMImage().
    */
    static inline std::shared_ptr< Doxel3D >  valueOf(
        std::shared_ptr< Doxel3D > parent,
        const d::dant_t& dant,
        const std::string& source,
        const d::size3d_t& worldSize,
        size_t maxDepth = 0,
        size_t seaLevel = 127,
        size_t maxHeight = 255,
        int chasm = -1
    ) {
        const float worldHeight = worldSize.get<2>();
        const float maxSide = std::max( worldSize.get<0>(), worldSize.get<1>() );
        const float kh = worldHeight / maxSide;
        const auto hmImage = loadHMImage(
            source,
            // Загружаем всю карту
            d::boxInt2d_t( d::ZEROINT2D, d::ZEROINT2D ),
            kh, maxDepth, seaLevel, maxHeight, chasm
        );
        return Doxel3D::valueOf( parent, dant, hmImage, worldSize );
    }





protected:
    /**
    * Загружает карту высот (КВ).
    *
    * @param kh Высота мира делённая на максимальную сторону карты высот.
    *        Будет использована, чтобы нормализировать КВ, т.к. доксель -
    *        всегда куб, а самая высокая точка на КВ не обязательно соотв.
    *        протяжённости мира. Благодаря этому коэф., мир расположится
    *        в докселе по центру (по всем осям) и уровень моря придётся в
    *        точности на плоскость XY.
    * @param maxDepth Значение, декларирующее максимальную глубину для КВ.
    *        Все меньшие значения получают значение maxDepth.
    * @param seaLevel Значение, декларирующее уровень моря для КВ.
    * @param maxHeight Значение, декларирующее максимальную высоту для КВ.
    *        Все большие значения получают значение maxHeight.
    * @param chasm Значения этой величины или ниже интерпретируются как
    *        "отсутствие высоты" и отмечается как
    *        std::numeric_limits< float >::infinity().
    *        Чтобы не учитывать, пишем '-1': бездонных пропастей на КВ нет.
    */
    static hmImage_t loadHMImage(
        const std::string& source,
        const d::boxInt2d_t& box,
        float kh,
        size_t maxDepth,
        size_t seaLevel,
        size_t maxHeight,
        int chasm
    );





public:
    /**
    * Счётчики работы методов.
    * (!) Для корректного результата счётчики должны быть обнулены перед
    * вызовом метода.
    * @test
    */
    static size_t countDoxel_ValueOf;

};







/**
* Печать в поток.
*/
inline ::std::ostream& operator<<( ::std::ostream &out, const Doxel3D& d ) {
    const size_t n = d.countChild( 0 );
    out << "Doxel3D #" << &d << "  Размер: " << d.size();
    if ( d.parent() ) {
         out << "  Родитель: #" << d.parent();
    }
    if (n > 0) {
        out << "  Окт / Пот (" << n << "): ";
        for (auto itr = d.child()->cbegin(); itr != d.child()->cend(); ++itr) {
            const auto ch = *itr;
            if ( ch ) {
                const size_t k = std::distance( d.child()->cbegin(), itr );
                out << k << "/" << ch->countChild( 0 ) << " ";
            }
        }
    }
    return out;
}



inline void print( const Doxel3D& d, const std::string& title = "", bool pause = false, const std::string& indent = "" ) {
    if ( !title.empty() ) {
        std::cout << indent << title << std::endl;
    }
    std::cout << indent << d << std::endl;
    if (d.countChild( 0 ) > 0) {
        std::cout << indent << "<Потомки>";
        for (auto itr = d.child()->cbegin(); itr != d.child()->cend(); ++itr) {
            const auto ch = *itr;
            if ( ch ) {
                const size_t k = std::distance( d.child()->cbegin(), itr );
                std::cout << std::endl << indent << "Дант " << k << ", потомков " << ch->countChild( 0 ) << std::endl;
                const auto d3d = static_cast< Doxel3D* >( ch.get() );
                //std::cout << *d3d << std::endl;
                print( *d3d, title, pause, indent + "  " );
                if ( pause && (((k + 1) % 4) == 0) ) {
                    std::cout << "pause..." << std::endl;
                    std::cin.ignore();
                }
            }
        }
        std::cout << indent << "</Потомки>" << std::endl << std::endl;
    }
}
