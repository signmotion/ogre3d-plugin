#pragma once

#include "default.h"
#include "ContentDoxel.h"
#include <boost/array.hpp>


/**
* Доксель - объект с равными измерениями. Доксель хранит некое содержание (контент)
* и может детализировать это содержание, *удваивая* точность *равномерно по
* всем* своим измерениям. Точность - вовсе не обязательно что-то наглядное.
* Это может быть и коротким / подробным рассказом о чём-то, что представлено
* докселем. Доксель сам определяет, что значит для него "удвоить точность".
*
* Каждый доксель характеризуется размером. Например, для докселя,
* представляющего некую часть 3D-пространства, размером может служить длина,
* ширина и высота этой области пространства. Но т.к. доксель - куб (по всем
* измерениям), можно ограничиться одной, максимальной, стороной.
*
* Уровни точности докселя:
*   0-й  1 еденица содержания (см. template::CT).
*   1-й  От 0 до 2^D докселей и соотв. столько же едениц содержания.
*   2-й  От 0 до 2^(D*2) докселей и соотв. столько же едениц содержания.
*   3-й  От 0 до 2^(D*3) докселей и соотв. столько же едениц содержания.
*   4-й  ...
* На количество уровней точности нет ограничения.
* @see DoxelTest.h
*
* В этом проекте исп. три измерения: по осям OX / OY / OZ. Удвоение точности -
* создание 2^3 = 8 потомков, детализирующих содержимое. Содержимое - некая
* ячейка с данными (см. CellCD).
*
* @template D  Количество измерений докселя.
* @template CT Чем представлено содержание докселя.
* @template ST Чем представлен размер докселя.
*/
template< size_t D, typename CT, typename ST >
class Doxel {
public:
    /**
    * Тип для потомков докселя.
    */
    typedef boost::array<
        std::shared_ptr< Doxel >,
        // Количество потомков = 2 ^ D. Для трёх измерений: 2 ^ 3 = 8 потомков.
        d::pow2< 2, D >::value
    > child_t;



public:
    inline Doxel(
        const ST& size,
        std::shared_ptr< CT >  content = nullptr,
        std::shared_ptr< Doxel >  parent = nullptr
    ) :
        s( size ),
        cnt( content ),
        p( parent )
    {
    }



    virtual inline ~Doxel() {
    }



    /**
    * @return Содержание докселя.
    */
    inline CT const& content() const {
        return *c;
    }



    /**
    * Устанавливает для докселя новое содержание.
    */
    inline void content( std::shared_ptr< CT >  content  ) {
        cnt = content;
    }



    /**
    * @return Размер докселя.
    */
    inline ST size() const {
        return s;
    }


    /**
    * @return Ссылка на докселя-родителя. Доступно для изменения.
    */
    inline Doxel* parent() {
        return p.get();
    }

    inline Doxel const* parent() const {
        return p.get();
    }


    /**
    * @return Ссылка на потомков докселя.
    */
    inline child_t* child() {
        return &chl;
    }

    inline child_t const* child() const {
        return &chl;
    }



    /**
    * @return Количество потомков у докселя.
    */
    inline size_t countChild( size_t level ) const {
        return countChild( level, *this );
    }





private:
    /**
    * @see countChild( size_t )
    */
    static inline size_t countChild( size_t level, const Doxel& d ) {
        size_t n = 0;
        // Потомки докселя
        const auto childDox = d.child();
        for (auto itr = childDox->cbegin(); itr != childDox->cend(); ++itr) {
            const auto ch = *itr;
            if ( ch ) {
                assert( ch->parent() && "Каждый потомок должен иметь родителя." );
                /* - Заменено. См. ниже.
                if (level > 0) {
                    n += countChild( level - 1, *ch );
                } else {
                    ++n;
                }
                */
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
        return n;
    }






private:
    /**
    * Каждый доксель может владеть данными определённого (см. template) класса.
    */
    std::shared_ptr< CT >  cnt;

    /**
    * Размер докселя.
    * Обязательная характеристика.
    */
    const ST s;

    /**
    * Родительский доксель. Чтобы доксель можно было позиционировать
    * относительно родителя.
    */
    std::shared_ptr< Doxel >  p;

    /**
    * Доксель делит каждое известное ему измерение пополам. Т.о. получаем
    * потомков докселя.
    */
    child_t chl;

};









/**
* Доксель, детализирующий 3D-пространство.
* Позволяет строить карты с разной степенью детализации.
*
* Повышая точность, делит 3D-пространство тремя перпендикулярными плоскостями,
* образуя октант. Нумерация получившихся пространств - начиная с правого
* верхнего угла, по часовой стрелке, верхняя плоскость - нижняя плоскость.
*/
class Doxel3D : public Doxel<
    // Количество измерений
    3,
    // Содержание докселя представим такой вот структурой
    CellCD,
    // В качестве размера докселя берём длину / ширину / высоту. А т.к.
    // доксель - равносторонний, можем хранить размер как одно значение
    // с плавающей точкой.
    // @todo Позволить в шаблоне определять, в чём выражать размер докселя.
    float
> {
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
    * @see Комм. к свойствам класса.
    */
    inline Doxel3D(
        float size,
        std::shared_ptr< CellCD >   content,
        std::shared_ptr< Doxel3D >  parent = nullptr
    ) :
        Doxel( size, content, parent )
    {
    }


    virtual inline ~Doxel3D() {
    }



    /**
    * Создаёт 3D-доксель из карты высот (КВ).
    *
    * @param parent Родитель докселя.
    * @param hmImage Карта высот (КВ).
    * @param worldSize Размер мира, м. Размером докселя послужит максимальный
    *        размер по оси, т.к. доксель - всегда куб.
    */
    static std::shared_ptr< Doxel3D >  valueOf(
        std::shared_ptr< Doxel3D > parent,
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
        const std::string& source,
        const d::size3d_t& worldSize,
        size_t maxDepth = 0,
        size_t seaLevel = 127,
        size_t maxHeight = 255,
        int chasm = -1
    ) {
        const auto hmImage = loadHMImage(
            source,
            // Загружаем всю карту
            d::boxInt2d_t( d::ZEROINT2D, d::ZEROINT2D ),
            maxDepth, seaLevel, maxHeight, chasm
        );
        return Doxel3D::valueOf( parent, hmImage, worldSize );
    }





protected:
    /**
    * Загружает карту высот.
    *
    * @param maxDepth Значение, декларирующее максимальную глубину для КВ.
    *        Все меньшие значения получают значение maxDepth.
    * @param seaLevel Значение, декларирующее уровень моря для КВ.
    * @param maxHeight Значение, декларирующее максимальную высоту для КВ.
    *        Все большие значения получают значение maxHeight.
    * @param chasm Значения этой величины или ниже интерпретируются как
    *        "отсутствие высоты" и отмечается как
    *        std::numeric_limits< float >::infinity().
    *        -1 - бездонных пропастей на карте высот нет.
    */
    static hmImage_t loadHMImage(
        const std::string& source,
        const d::boxInt2d_t& box,
        size_t maxDepth,
        size_t seaLevel,
        size_t maxHeight,
        int chasm
    );





    /**
    * Строит доксель с максимальным уровнем детализации, который обеспечивает
    * заданная карта высот.
    */
    // @todo ...





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
                std::cout << std::endl << indent << "Октант " << k << ", потомков " << ch->countChild( 0 ) << std::endl;
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
