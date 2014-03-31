#pragma once

#include <boost/assign/list_inserter.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>


/*
* ����������� ��� ����� ������� ���������, ������, ������.
*/


/**
* ��������� ������������.
*/
typedef struct {
    bool gradient;
    bool surface;
    bool trend;
} show_t;




/**
* �������� ��������� ����� � ��������� ������.
*/
const float PRECISION = 0.01f;


/**
* ��������� ����������.
* �������, ����� ���������� �������� ������ �������. ��� � ����. ��
* ����� ����������� � �������� ����������� (��������, ������ ������
* ����� �������� � ������), ����� ��������� �� �������.
*/
typedef boost::tuple< float, float >         coord2d_t;
typedef boost::tuple< float, float, float >  coord3d_t;

/**
* �������� �� �����������.
* �� ����� ���. ����� boost::numeric::interval<>, �.�. �� �� �������� �
* ���������� ������.
*/
typedef boost::tuple< coord2d_t, coord2d_t >  interval2d_t;

/**
* ������� �� ���������. ������ ���������� - ������ ������� ����, �.�.
* ��� ��� ����������� ���������� ������ (X) � ����� (Z).
*/
typedef std::pair< coord2d_t, coord2d_t >  box2d_t;

/**
* ������: ����� (��� X), ������ (��� Z).
*/
typedef boost::tuple< float, float >  size2d_t;

/**
* ������.
* ����� ��� �������� ���������� �������, �������� �����������.
*/
typedef boost::tuple< float, float >  vector2d_t;

/**
* ������� �� ������������ ����.
*/
typedef boost::tuple< float, float >  scale2d_t;

/**
* ������������� ����� ����� (�����������). ����� ����������� ������ -
* ��� ������, ������� �� ����� � � ����� �. � ����� ������� ��� ���������
* "�������" � vertex() ��� ������� Trend.
*/
typedef std::vector< coord2d_t >  surface2d_t;

/**
* ����� ����������� ���������� �� ������, �� ����� �������� ������� ��� �����.
* ���� ��� ��������� ������� "�������" �������. ���������� �������� �
* ������������ ��������� �����������.
*/
typedef std::vector< box2d_t >  trend2d_t;

/**
* ������� ����������, � ����� ������ ����������� (�����) *��������*.
* ��������, ������� (1, 1) - ���������� � ��� ������ - �������,
* ��� ������ � ������ - ��������. (0, -1) - �������� ������, �����
* �������� ������ ����.
*/
//typedef boost::tuple< char, char >  normal2d_t; - �����, �����.
typedef coord2d_t  normal2d_t;

/**
* ����� ������ - ��� ����������� � �������, ������� �� ����.
*/
typedef std::pair< surface2d_t, normal2d_t >  form2d_t;



/**
* �������������.
* ��������� ����� �� ��������� �����������.
* ��������� ������ ��� ������������� ������������ ����.
*/
typedef boost::tuple< float, float >  natural_t;



/**
* ��������� ��������������� ���� �������.
* (!) ����� ������������ �� ����� ��������, ��� ��������
* ������� ��������������.
*/
const coord2d_t ZERO2D( 0.0f, 0.0f );
const coord2d_t ONE2D ( 1.0f, 1.0f );



// (!) ������ ���� �������� �������� � ����������������� ����� ������ �
// ����� �������������� � �������� ������� ������� ��� ������ �����.
// ��������: coord2d_t, vector2d_t, size2d_t.


/**
* @return ������� � �������� �� ������ ���������.
*
* @template indexCoord ���������� (0 = x, 1 = y).
*
* @ min<>(), max<>()
*/
template< size_t indexCoord >
inline boost::numeric::interval< float >  minmax(
    const surface2d_t&  v
) {
    assert( (v.size() > 0) && "����������� �� ������ ������." );
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
* @return ������� �� ������ ���������.
*
* @ minmax<>()
*/
template< size_t indexCoord >
inline float min( const surface2d_t&  v ) {
    assert( (v.size() > 0) && "����������� �� ������ ������." );
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
* @return �������� �� ������ ���������.
*
* @ minmax<>()
*/
template< size_t indexCoord >
inline float max( const surface2d_t&  v ) {
    assert( (v.size() > 0) && "����������� �� ������ ������." );
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
* ����� �������� ��� �������.
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
* @return �������� �������.
*/
inline bool empty( const coord2d_t& c ) {
    return abs(c.get<0>() < PRECISION) && abs(c.get<1>() < PRECISION);
}

inline bool empty( const box2d_t& b ) {
    return (b.first == b.second);
}




/**
* @return ������, ���������������� ������, ����� ����� ��������� �������.
*/
inline normal2d_t calcNormal( const coord2d_t& a, const coord2d_t& b ) {
    const auto d = b - a;
    return normal2d_t( -d.get<0>(), d.get<1>() );
}



/**
* @return ���������� ����� ����� �������.
*/
inline float calcDistance( const coord2d_t& a, const coord2d_t& b ) {
    const auto d = b - a;
    return sqrt( d.get<0>() * d.get<0>() + d.get<1>() * d.get<1>() );
}




/**
* @return ����������� ������ �������, � ������� ����� ��������� ������.
*/
inline box2d_t calcBox( const surface2d_t& f ) {
    assert( (f.size() > 0) && "����������� �� ������ �����������." );
    const auto x = minmax<0>( f );
    const auto z = minmax<1>( f );
    const coord2d_t p1( x.upper(), z.upper() );
    const coord2d_t p2( x.lower(), z.lower() );
    return box2d_t( p1, p2 );
}




/**
* @return ������.
*/
inline size2d_t calcSize( const box2d_t& b ) {
    const size2d_t s(
        abs( b.first.get<0>() - b.second.get<0>() ),
        abs( b.first.get<1>() - b.second.get<1>() )
    );
    /* - ��������� ���������� ������� ��� ����� �������� �����.
    assert( ( (s.get<0>() > 0) && (s.get<1>() > 0) )
        && "���������� ������� ������ ���� ������ ������ ������� � ����� ������ ������." );
    */
    return s;
}
