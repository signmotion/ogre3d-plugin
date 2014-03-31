#pragma once

#include <boost/assign/list_inserter.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>


/*
* Необходимые для этого проекта константы, методы, классы.
*/


/**
* Параметры визуализации.
*/
typedef struct {
    bool gradient;
    bool surface;
    bool trend;
} show_t;




/**
* Точность сравнения чисел с плавающей точкой.
*/
const float PRECISION = 0.01f;


/**
* Декартовы координаты.
* Красиво, когда координаты задаются целыми числами. Так и было. Но
* когда привязались к реальным расстояниям (например, решили размер
* биома задавать в метрах), целых координат не хватило.
*/
typedef boost::tuple< float, float >         coord2d_t;
typedef boost::tuple< float, float, float >  coord3d_t;

/**
* Интервал по координатам.
* Не можем исп. здесь boost::numeric::interval<>, т.к. он не работает с
* составными типами.
*/
typedef boost::tuple< coord2d_t, coord2d_t >  interval2d_t;

/**
* Коробка на плоскости. Первая координата - правый верхний угол, т.к.
* оси для наблюдателя направлены вправо (X) и вверх (Z).
*/
typedef std::pair< coord2d_t, coord2d_t >  box2d_t;

/**
* Размер: длина (ось X), ширина (ось Z).
*/
typedef boost::tuple< float, float >  size2d_t;

/**
* Вектор.
* Введён для красивой декларации методов, строящих поверхность.
*/
typedef boost::tuple< float, float >  vector2d_t;

/**
* Масштаб по координатным осям.
*/
typedef boost::tuple< float, float >  scale2d_t;

/**
* Представление линии формы (поверхности). Любые поверхности тренда -
* это кривая, летящая из точки А в точку Б. С какой стороны она заполнена
* "прошито" в vertex() для каждого Trend.
*/
typedef std::vector< coord2d_t >  surface2d_t;

/**
* Когда поверхность собирается из частей, мы можем захотеть увидеть эти части.
* Этот тип позволяет указать "коробки" трендов. Координаты задаются в
* относительно собранной поверхности.
*/
typedef std::vector< box2d_t >  trend2d_t;

/**
* Нормаль показывает, с каких сторон поверхность (форма) *свободна*.
* Например, нормаль (1, 1) - представим её как вектор - говорит,
* что справа и сверху - свободно. (0, -1) - свободно сверху, линия
* является линией пола.
*/
//typedef boost::tuple< char, char >  normal2d_t; - Проще, проще.
typedef coord2d_t  normal2d_t;

/**
* Форма тренда - это поверхность и нормаль, бегущие за руку.
*/
typedef std::pair< surface2d_t, normal2d_t >  form2d_t;



/**
* Натурализация.
* Позволяет слоям не выглядеть однообразно.
* Безымяный кортеж для натурализации отображаемых форм.
*/
typedef boost::tuple< float, float >  natural_t;



/**
* Константы декларированных выше классов.
* (!) Молча используются со всеми методами, где возможно
* неявное преобразование.
*/
const coord2d_t ZERO2D( 0.0f, 0.0f );
const coord2d_t ONE2D ( 1.0f, 1.0f );



// (!) Методы ниже помогают работать с декларированнымии здесь типами и
// могут использоваться в качестве НЕЯВНЫХ методов для схожих типов.
// Например: coord2d_t, vector2d_t, size2d_t.


/**
* @return Минимум и максимум из набора координат.
*
* @template indexCoord Координата (0 = x, 1 = y).
*
* @ min<>(), max<>()
*/
template< size_t indexCoord >
inline boost::numeric::interval< float >  minmax(
    const surface2d_t&  v
) {
    assert( (v.size() > 0) && "Невыполнимо на пустом списке." );
    float minCoord = v.cbegin()->get< indexCoord >();
    float maxCoord = minCoord;
    for (auto itr = v.cbegin() + 1; itr != v.cend(); ++itr) {
        const coord2d_t c = *itr;
        const auto n = c.get< indexCoord >();
        if (n < minCoord) {
            minCoord = n;
        } else if (n > maxCoord) {
            maxCoord = n;
        }
    }
    return boost::numeric::interval< float >( minCoord, maxCoord );
}



/**
* @return Минимум из набора координат.
*
* @ minmax<>()
*/
template< size_t indexCoord >
inline float min( const surface2d_t&  v ) {
    assert( (v.size() > 0) && "Невыполнимо на пустом списке." );
    float minCoord = v.cbegin()->get< indexCoord >();
    for (auto itr = v.cbegin() + 1; itr != v.cend(); ++itr) {
        const coord2d_t c = *itr;
        const auto n = c.get< indexCoord >();
        if (n < minCoord) {
            minCoord = n;
        }
    }
    return minCoord;
}



/**
* @return Максимум из набора координат.
*
* @ minmax<>()
*/
template< size_t indexCoord >
inline float max( const surface2d_t&  v ) {
    assert( (v.size() > 0) && "Невыполнимо на пустом списке." );
    float maxCoord = v.cbegin()->get< indexCoord >();
    for (auto itr = v.cbegin() + 1; itr != v.cend(); ++itr) {
        const coord2d_t c = *itr;
        const auto n = c.get< indexCoord >();
        if (n > maxCoord) {
            maxCoord = n;
        }
    }
    return maxCoord;
}



inline coord2d_t operator-( const coord2d_t& a, const coord2d_t& b ) {
    return coord2d_t(
        a.get<0>() - b.get<0>(),
        a.get<1>() - b.get<1>()
    );
}


inline coord2d_t operator-=( coord2d_t& a, const coord2d_t& b ) {
    a.get<0>() -= b.get<0>();
    a.get<1>() -= b.get<1>();
    return a;
}


inline coord2d_t operator+( const coord2d_t& a, const coord2d_t& b ) {
    return coord2d_t(
        a.get<0>() + b.get<0>(),
        a.get<1>() + b.get<1>()
    );
}


inline coord2d_t operator+=( coord2d_t& a, const coord2d_t& b ) {
    a.get<0>() += b.get<0>();
    a.get<1>() += b.get<1>();
    return a;
}


inline coord2d_t operator*( const coord2d_t& a, const coord2d_t& b ) {
    return coord2d_t(
        a.get<0>() * b.get<0>(),
        a.get<1>() * b.get<1>()
    );
}


inline coord2d_t operator*=( coord2d_t& a, const coord2d_t& b ) {
    a.get<0>() *= b.get<0>();
    a.get<1>() *= b.get<1>();
    return a;
}


inline coord2d_t operator*( const coord2d_t& a, float k ) {
    return coord2d_t(
        a.get<0>() * k,
        a.get<1>() * k
    );
}


inline coord2d_t operator/( const coord2d_t& a, const coord2d_t& b ) {
    return coord2d_t(
        a.get<0>() / b.get<0>(),
        a.get<1>() / b.get<1>()
    );
}


inline coord2d_t operator/=( coord2d_t& a, float k ) {
    a.get<0>() /= k;
    a.get<1>() /= k;
    return a;
}


inline coord2d_t operator/( const coord2d_t& a, float k ) {
    return coord2d_t(
        a.get<0>() / k,
        a.get<1>() / k
    );
}


// @see PRECISION
inline bool operator==( const coord2d_t& a, const coord2d_t& b ) {
    return (abs(a.get<0>() - b.get<0>()) < PRECISION)
        && (abs(a.get<1>() - b.get<1>()) < PRECISION);
}

inline bool operator!=( const coord2d_t& a, const coord2d_t& b ) {
    return !(a == b);
}


inline bool operator>( const coord2d_t& a, const coord2d_t& b ) {
    return (a.get<0>() > b.get<0>()) && (a.get<1>() > b.get<1>());
}


inline bool operator<( const coord2d_t& a, const coord2d_t& b ) {
    return (a.get<0>() < b.get<0>()) && (a.get<1>() < b.get<1>());
}


inline bool operator>=( const coord2d_t& a, const coord2d_t& b ) {
    return (a.get<0>() >= b.get<0>()) && (a.get<1>() >= b.get<1>());
}


inline bool operator<=( const coord2d_t& a, const coord2d_t& b ) {
    return (a.get<0>() <= b.get<0>()) && (a.get<1>() <= b.get<1>());
}




/**
* Задаёт смещение для коробки.
*/
inline box2d_t operator+( const box2d_t& box, const coord2d_t& c ) {
    const auto a = box.first  + c;
    const auto b = box.second + c;
    return box2d_t( a, b );
}

inline box2d_t operator-( const box2d_t& box, const coord2d_t& c ) {
    const auto a = box.first  - c;
    const auto b = box.second - c;
    return box2d_t( a, b );
}




/**
* @return Признаки пустоты.
*/
inline bool empty( const coord2d_t& c ) {
    return abs(c.get<0>() < PRECISION) && abs(c.get<1>() < PRECISION);
}

inline bool empty( const box2d_t& b ) {
    return (b.first == b.second);
}




/**
* @return Вектор, перпендикулярный прямой, между двумя заданными точками.
*/
inline normal2d_t calcNormal( const coord2d_t& a, const coord2d_t& b ) {
    const auto d = b - a;
    return normal2d_t( -d.get<0>(), d.get<1>() );
}



/**
* @return Расстояние между двумя точками.
*/
inline float calcDistance( const coord2d_t& a, const coord2d_t& b ) {
    const auto d = b - a;
    return sqrt( d.get<0>() * d.get<0>() + d.get<1>() * d.get<1>() );
}




/**
* @return Минимальный размер коробки, в которую можно заключить кривую.
*/
inline box2d_t calcBox( const surface2d_t& f ) {
    assert( (f.size() > 0) && "Невыполнимо на пустой поверхности." );
    const auto x = minmax<0>( f );
    const auto z = minmax<1>( f );
    const coord2d_t p1( x.upper(), z.upper() );
    const coord2d_t p2( x.lower(), z.lower() );
    return box2d_t( p1, p2 );
}




/**
* @return Размер.
*/
inline size2d_t calcSize( const box2d_t& b ) {
    const size2d_t s(
        abs( b.first.get<0>() - b.second.get<0>() ),
        abs( b.first.get<1>() - b.second.get<1>() )
    );
    /* - Вычисляем координаты коробки при любом раскладе углов.
    assert( ( (s.get<0>() > 0) && (s.get<1>() > 0) )
        && "Координаты коробки должны быть заданы правым верхним и левым нижним углами." );
    */
    return s;
}
