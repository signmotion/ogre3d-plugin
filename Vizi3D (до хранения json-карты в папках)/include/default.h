#pragma once

// �������� ������ � WinDef.h
#define NOMINMAX

#include "configure.h"

#include <iostream>
#include <bitset>
#include <limits.h>
#include <boost/any.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>


/**
* �������������� ����������� assert().
*//*
inline void fnAssert( _In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line ) {
    std::cerr << "Assertion failed at " << *_File << ":" << _Line;
    //std::cerr << " inside " << __FUNCTION__ << std::endl;
    std::cerr << "Condition: " << _Message;
    std::cin.ignore();
    abort();
};
#undef assert
#define assert(_Expression) (void)( (!!(_Expression)) || (fnAssert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) )
*/
/*
#ifndef _FN_REDEFINED_ASSERT
    #define _FN_REDEFINED_ASSERT
    #undef assert
    inline void fnAssert( char* msg, char* file, unsigned int line ) {
        std::cout << "Assertion failed at " << *file << ":" << line;
        //std::cout << " inside " << __FUNCTION__ << std::endl;
        std::cout << "Condition: " << msg;
        std::cin.ignore();
        abort();
    };
    #define assert(x) ( fnAssert( #x, __FILE__, __LINE__ ) )
#endif
*/



/*
* ����������� ��� ����� ������� ����, ���������, ������, ������.
*/
namespace d {


// ��������� OpenCL
/* - �� ������������. ��. ����. � Port.h
const std::string PATH_OPENCL = "D:/Projects/workspace/ogre3d/AccurateHeightmap/resource/OpenCL";
const std::string PLATFORM_OPENCL = "NVIDIA";
*/



/**
* �������� ��������� ����� � ��������� ������.
*/
const float PRECISION = 0.001f;


/**
* ���������� ������ ����� � ������� ������.
* ������� ��� �������� ��������, ������� ���� �����������������, ��������
* ���������� � �������.
*/
template <long num, size_t n, size_t y = 1>
struct pow2 {
    enum { value = pow2< num * num, (n >> 1), n & 1 ? num * y : y >::value };
};
template <long num, size_t y>
struct pow2< num, 0, y > {
    enum { value = y };
};





/**
* ������������� ��������.
* ������� CouchDB.
*/
typedef std::string uid_t;
typedef std::string rev_t;
typedef std::pair< uid_t, rev_t >  externKey_t;



/**
* ��������� ����������.
* ���� �������, ����� ���������� �������� ������ �������. ��
* ����� ����������� � �������� ����������� (��������, ������
* ������ ����� �������� � ������), ����� ��������� �� �������.
*/
typedef boost::tuple< float, float >         coord2d_t;
typedef boost::tuple< int, int >             coordInt2d_t;
typedef boost::tuple< float, float, float >  coord3d_t;
typedef boost::tuple< int, int, int >        coordInt3d_t;

/**
* �������� �� �����������.
* �� ����� ���. ����� boost::numeric::interval<>, �.�. �� �� �������� �
* ���������� ������.
*/
typedef boost::tuple< coord2d_t, coord2d_t >        interval2d_t;
typedef boost::tuple< coordInt2d_t, coordInt2d_t >  intervalInt2d_t;

/**
* �������. ������ ���������� - ���� � ������������ ������������.
*/
typedef std::pair< coord2d_t, coord2d_t >        box2d_t;
typedef std::pair< coordInt2d_t, coordInt2d_t >  boxInt2d_t;
typedef std::pair< coord3d_t, coord3d_t >        box3d_t;
typedef std::pair< coordInt3d_t, coordInt3d_t >  boxInt3d_t;


/**
* ������: ����� (��� X), ������ (��� Z).
*/
typedef boost::tuple< float, float >            size2d_t;
typedef boost::tuple< size_t, size_t >          sizeInt2d_t;
typedef boost::tuple< float, float, float >     size3d_t;
typedef boost::tuple< size_t, size_t, size_t >  sizeInt3d_t;

/**
* ������.
* ����� ��� �������� ���������� �������, �������� �����������.
*/
typedef boost::tuple< float, float >         vector2d_t;
typedef boost::tuple< float, float, float >  vector3d_t;

/**
* ������� �� ������������ ����.
*/
typedef boost::tuple< float, float >  scale2d_t;
typedef boost::tuple< float, float >  scale3d_t;


/**
* ������������� ����� ����� (�����������). ����� ����������� ������ -
* ��� ������, ������� �� ����� � � ����� �. � ����� ������� ��� ���������
* "�������" � vertex() ��� ������� Gas.
*/
typedef std::vector< coord2d_t >  surface2d_t;

/**
* ����� ����������� ���������� �� ������, �� ����� �������� ������� ��� �����.
* ���� ��� ��������� ������� "�������" �������. ���������� ��������
* ������������ ��������� �����������.
*/
typedef std::vector< box2d_t >  trend2d_t;

/**
* ������ �������.
*/
typedef vector2d_t  normal2d_t;
typedef vector3d_t  normal3d_t;

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
* �����������.
* ������� ������ ������ �� ��������.
*/
typedef boost::array< coord3d_t, 3 >  triangle3d_t;



/**
* ���� �������������.
* ��������, �������������� ����� ���� ����������� �.�.:
*   .---------.
*   |       / |
*   |  0   /  |
*   |    *    |
*   |  /   1  |
*   | /       |
*   .---------.
*/
typedef boost::array< triangle3d_t, 2 >  twoTriangle3d_t;


/**
* �������� �������������.
* ��������, �������������� ����� ���� ����������� ����. �������:
*   .---------.
*   | \  0  / |
*   |  \   /  |
*   |3   *   1|
*   |  /   \  |
*   | /  2  \ |
*   .---------.
*
* @see normal5Plane3d_t
*/
typedef boost::array< triangle3d_t, 4 >  fourTriangle3d_t;


/**
* ������� ��� �����������, �������������� 4 ��������������.
* 0 - ���������� �������,
* 1-4 - ������� �������������, �� ������� ������� �����������.
* 
*   .---------.
*   | \  1  / |
*   |  \   /  |
*   |4   0   2|
*   |  /   \  |
*   | /  3  \ |
*   .---------.
*
* @see fourTriangle3d_t
*/
typedef boost::array< normal3d_t, 5 >  normal5Plane3d_t;


/**
* ��������� � ���� ���������������.
* ������� ������ �����������:
*   3         0
*   .---------.
*   |         |
*   |         |
*   |         |
*   .---------.
*   2         1
*/
typedef boost::array< coord3d_t, 4 > plane3d_t;


/**
* ������� ��������� (�����������), �������������� 4 ��������������.
* ��������� ����� ���� ������� ��� ������.
* ������� ������ ������ - �� ������ �� ������� �������.
* 0 - ����������� �������,
* 1-4 - ������� �������, ������������� ���������.
* 
*   4---------1
*   | \  d  / |
*   |  \   /  |
*   | c  0   a|
*   |  /   \  |
*   | /  b  \ |
*   3---------2
*
* @see fourTriangle3d_t
*/
typedef boost::array< coord3d_t, 5 >  plane4Triangle3d_t;







/**
* ��������� ��������������� ���� �������.
* (!) ����� ������������ �� ����� ��������, ��� ��������
* ������� ��������������.
*/
const coord2d_t ZERO2D( 0.0f, 0.0f );
const coordInt2d_t ZEROINT2D( 0, 0 );
const coord3d_t ZERO3D( 0.0f, 0.0f, 1.0f );
const coordInt3d_t ZEROINT3D( 0, 0, 0 );
const coord2d_t ONE2D ( 1.0f, 1.0f );
const coordInt2d_t ONEINT2D ( 1, 1 );
const coord3d_t ONE3D ( 1.0f, 1.0f, 1.0f );
const coordInt3d_t ONEINT3D ( 1, 1, 1 );



} // namespace d











/* * 2D operations * */

inline d::coord2d_t operator-( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return d::coord2d_t(
        a.get<0>() - b.get<0>(),
        a.get<1>() - b.get<1>()
    );
}


inline d::coord2d_t operator-=( d::coord2d_t& a, const d::coord2d_t& b ) {
    a.get<0>() -= b.get<0>();
    a.get<1>() -= b.get<1>();
    return a;
}


inline d::coord2d_t operator+( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return d::coord2d_t(
        a.get<0>() + b.get<0>(),
        a.get<1>() + b.get<1>()
    );
}


inline d::coord2d_t operator+=( d::coord2d_t& a, const d::coord2d_t& b ) {
    a.get<0>() += b.get<0>();
    a.get<1>() += b.get<1>();
    return a;
}


inline d::coord2d_t operator*( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return d::coord2d_t(
        a.get<0>() * b.get<0>(),
        a.get<1>() * b.get<1>()
    );
}


inline d::coord2d_t operator*=( d::coord2d_t& a, const d::coord2d_t& b ) {
    a.get<0>() *= b.get<0>();
    a.get<1>() *= b.get<1>();
    return a;
}


inline d::coord2d_t operator*( const d::coord2d_t& a, float k ) {
    return d::coord2d_t(
        a.get<0>() * k,
        a.get<1>() * k
    );
}


inline d::coord2d_t operator/( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return d::coord2d_t(
        a.get<0>() / b.get<0>(),
        a.get<1>() / b.get<1>()
    );
}


inline d::coord2d_t operator/=( d::coord2d_t& a, float k ) {
    a.get<0>() /= k;
    a.get<1>() /= k;
    return a;
}


inline d::coord2d_t operator/( const d::coord2d_t& a, float k ) {
    return d::coord2d_t(
        a.get<0>() / k,
        a.get<1>() / k
    );
}


// @see d::PRECISION
inline bool operator==( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return (abs(a.get<0>() - b.get<0>()) < d::PRECISION)
        && (abs(a.get<1>() - b.get<1>()) < d::PRECISION);
}

inline bool operator!=( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return !(a == b);
}


inline bool operator>( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return (a.get<0>() > b.get<0>()) && (a.get<1>() > b.get<1>());
}


inline bool operator<( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return (a.get<0>() < b.get<0>()) && (a.get<1>() < b.get<1>());
}


inline bool operator>=( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return (a.get<0>() >= b.get<0>()) && (a.get<1>() >= b.get<1>());
}


inline bool operator<=( const d::coord2d_t& a, const d::coord2d_t& b ) {
    return (a.get<0>() <= b.get<0>()) && (a.get<1>() <= b.get<1>());
}






inline d::coordInt2d_t operator-( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return d::coordInt2d_t(
        a.get<0>() - b.get<0>(),
        a.get<1>() - b.get<1>()
    );
}


inline d::coordInt2d_t operator-=( d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    a.get<0>() -= b.get<0>();
    a.get<1>() -= b.get<1>();
    return a;
}


inline d::coordInt2d_t operator+( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return d::coordInt2d_t(
        a.get<0>() + b.get<0>(),
        a.get<1>() + b.get<1>()
    );
}


inline d::coordInt2d_t operator+=( d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    a.get<0>() += b.get<0>();
    a.get<1>() += b.get<1>();
    return a;
}


inline d::coordInt2d_t operator*( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return d::coordInt2d_t(
        a.get<0>() * b.get<0>(),
        a.get<1>() * b.get<1>()
    );
}


inline d::coordInt2d_t operator*=( d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    a.get<0>() *= b.get<0>();
    a.get<1>() *= b.get<1>();
    return a;
}


inline d::coordInt2d_t operator*( const d::coordInt2d_t& a, int k ) {
    return d::coordInt2d_t(
        a.get<0>() * k,
        a.get<1>() * k
    );
}


inline d::coordInt2d_t operator/( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return d::coordInt2d_t(
        (int)(a.get<0>() / b.get<0>()),
        (int)(a.get<1>() / b.get<1>())
    );
}


inline d::coordInt2d_t operator/=( d::coordInt2d_t& a, int k ) {
    a.get<0>() = (int)(a.get<0>() / k);
    a.get<1>() = (int)(a.get<1>() / k);
    return a;
}


inline d::coordInt2d_t operator/( const d::coordInt2d_t& a, int k ) {
    return d::coordInt2d_t(
        (int)(a.get<0>() / k),
        (int)(a.get<1>() / k)
    );
}


inline bool operator==( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return (a.get<0>() == b.get<0>())
        && (a.get<1>() == b.get<1>());
}

inline bool operator!=( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return !(a == b);
}


inline bool operator>( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return (a.get<0>() > b.get<0>()) && (a.get<1>() > b.get<1>());
}


inline bool operator<( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return (a.get<0>() < b.get<0>()) && (a.get<1>() < b.get<1>());
}


inline bool operator>=( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return (a.get<0>() >= b.get<0>()) && (a.get<1>() >= b.get<1>());
}


inline bool operator<=( const d::coordInt2d_t& a, const d::coordInt2d_t& b ) {
    return (a.get<0>() <= b.get<0>()) && (a.get<1>() <= b.get<1>());
}






/* * 3D operations * */

// d::coord3d_t

inline d::coord3d_t operator-( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return d::coord3d_t(
        a.get<0>() - b.get<0>(),
        a.get<1>() - b.get<1>(),
        a.get<2>() - b.get<2>()
    );
}


inline d::coord3d_t operator-=( d::coord3d_t& a, const d::coord3d_t& b ) {
    a.get<0>() -= b.get<0>();
    a.get<1>() -= b.get<1>();
    a.get<2>() -= b.get<2>();
    return a;
}


inline d::coord3d_t operator+( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return d::coord3d_t(
        a.get<0>() + b.get<0>(),
        a.get<1>() + b.get<1>(),
        a.get<2>() + b.get<2>()
    );
}


inline d::coord3d_t operator+=( d::coord3d_t& a, const d::coord3d_t& b ) {
    a.get<0>() += b.get<0>();
    a.get<1>() += b.get<1>();
    a.get<2>() += b.get<2>();
    return a;
}


inline d::coord3d_t operator*( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return d::coord3d_t(
        a.get<0>() * b.get<0>(),
        a.get<1>() * b.get<1>(),
        a.get<2>() * b.get<2>()
    );
}


inline d::coord3d_t operator*=( d::coord3d_t& a, const d::coord3d_t& b ) {
    a.get<0>() *= b.get<0>();
    a.get<1>() *= b.get<1>();
    a.get<2>() *= b.get<2>();
    return a;
}


inline d::coord3d_t operator*( const d::coord3d_t& a, float k ) {
    return d::coord3d_t(
        a.get<0>() * k,
        a.get<1>() * k,
        a.get<2>() * k
    );
}


inline d::coord3d_t operator/( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return d::coord3d_t(
        a.get<0>() / b.get<0>(),
        a.get<1>() / b.get<1>(),
        a.get<2>() / b.get<2>()
    );
}


inline d::coord3d_t operator/=( d::coord3d_t& a, float k ) {
    a.get<0>() /= k;
    a.get<1>() /= k;
    a.get<2>() /= k;
    return a;
}


inline d::coord3d_t operator/( const d::coord3d_t& a, float k ) {
    return d::coord3d_t(
        a.get<0>() / k,
        a.get<1>() / k,
        a.get<2>() / k
    );
}


// @see d::PRECISION
inline bool operator==( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return (abs(a.get<0>() - b.get<0>()) < d::PRECISION)
        && (abs(a.get<1>() - b.get<1>()) < d::PRECISION)
        && (abs(a.get<2>() - b.get<2>()) < d::PRECISION);
}

inline bool operator!=( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return !(a == b);
}


inline bool operator>( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return (a.get<0>() > b.get<0>())
        && (a.get<1>() > b.get<1>())
        && (a.get<2>() > b.get<2>());
}


inline bool operator<( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return (a.get<0>() < b.get<0>())
        && (a.get<1>() < b.get<1>())
        && (a.get<2>() < b.get<2>());
}


inline bool operator>=( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return (a.get<0>() >= b.get<0>())
        && (a.get<1>() >= b.get<1>())
        && (a.get<2>() >= b.get<2>());
}


inline bool operator<=( const d::coord3d_t& a, const d::coord3d_t& b ) {
    return (a.get<0>() <= b.get<0>())
        && (a.get<1>() <= b.get<1>())
        && (a.get<2>() <= b.get<2>());
}





// d::coordInt3d_t

inline d::coordInt3d_t operator-( const d::coordInt3d_t& a, const d::coordInt3d_t& b ) {
    return d::coordInt3d_t(
        a.get<0>() - b.get<0>(),
        a.get<1>() - b.get<1>(),
        a.get<2>() - b.get<2>()
    );
}


inline d::coordInt3d_t operator-=( d::coordInt3d_t& a, const d::coordInt3d_t& b ) {
    a.get<0>() -= b.get<0>();
    a.get<1>() -= b.get<1>();
    a.get<2>() -= b.get<2>();
    return a;
}


inline d::coordInt3d_t operator+( const d::coordInt3d_t& a, const d::coordInt3d_t& b ) {
    return d::coordInt3d_t(
        a.get<0>() + b.get<0>(),
        a.get<1>() + b.get<1>(),
        a.get<2>() + b.get<2>()
    );
}


inline d::coordInt3d_t operator+=( d::coordInt3d_t& a, const d::coordInt3d_t& b ) {
    a.get<0>() += b.get<0>();
    a.get<1>() += b.get<1>();
    a.get<2>() += b.get<2>();
    return a;
}


inline d::coordInt3d_t operator*( const d::coordInt3d_t& a, const d::coordInt3d_t& b ) {
    return d::coordInt3d_t(
        a.get<0>() * b.get<0>(),
        a.get<1>() * b.get<1>(),
        a.get<2>() * b.get<2>()
    );
}


inline d::coordInt3d_t operator*=( d::coordInt3d_t& a, const d::coordInt3d_t& b ) {
    a.get<0>() *= b.get<0>();
    a.get<1>() *= b.get<1>();
    a.get<2>() *= b.get<2>();
    return a;
}


inline d::coordInt3d_t operator*( const d::coordInt3d_t& a, int k ) {
    return d::coordInt3d_t(
        a.get<0>() * k,
        a.get<1>() * k,
        a.get<2>() * k
    );
}


inline d::coordInt3d_t operator/( const d::coordInt3d_t& a, const d::coordInt3d_t& b ) {
    return d::coordInt3d_t(
        a.get<0>() / b.get<0>(),
        a.get<1>() / b.get<1>(),
        a.get<2>() / b.get<2>()
    );
}


inline d::coordInt3d_t operator/=( d::coordInt3d_t& a, int k ) {
    a.get<0>() /= k;
    a.get<1>() /= k;
    a.get<2>() /= k;
    return a;
}


inline d::coordInt3d_t operator/( const d::coordInt3d_t& a, int k ) {
    return d::coordInt3d_t(
        a.get<0>() / k,
        a.get<1>() / k,
        a.get<2>() / k
    );
}






/**
* ����� �������� ��� �������.
*/
inline d::box2d_t operator+( const d::box2d_t& box, const d::coord2d_t& c ) {
    const auto a = box.first  + c;
    const auto b = box.second + c;
    return d::box2d_t( a, b );
}

inline d::box2d_t operator-( const d::box2d_t& box, const d::coord2d_t& c ) {
    const auto a = box.first  - c;
    const auto b = box.second - c;
    return d::box2d_t( a, b );
}








namespace d {



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
inline boost::numeric::interval< float >  calcMinMax(
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
inline float calcMin( const surface2d_t&  v ) {
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
inline float calcMax( const surface2d_t&  v ) {
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




/**
* @return �������� �������.
*/
inline bool empty( float v ) {
    return (abs( v ) < PRECISION);
}

inline bool empty( int v ) {
    return (v == 0);
}

inline bool empty( const coord2d_t& c ) {
    return empty( c.get<0>() )
        && empty( c.get<1>() );
}

inline bool empty( const coord3d_t& c ) {
    return empty( c.get<0>() )
        && empty( c.get<1>() )
        && empty( c.get<2>() );
}

inline bool emptyAny( const coord2d_t& c ) {
    return empty( c.get<0>() )
        || empty( c.get<1>() );
}


inline bool emptyAny( const coord3d_t& c ) {
    return empty( c.get<0>() )
        || empty( c.get<1>() )
        || empty( c.get<2>() );
}


inline bool empty( const box2d_t& b ) {
    return (b.first == b.second);
}



/**
* @return ������ ���������.
*/
inline bool correct( const box2d_t& b ) {
    return (b.second.get<0>() >= b.first.get<0>())
        && (b.second.get<1>() >= b.first.get<1>());
}

inline bool correct( const boxInt2d_t& b ) {
    return (b.second.get<0>() >= b.first.get<0>())
        && (b.second.get<1>() >= b.first.get<1>());
}




/**
* @return ������, ���������������� ������ ����� ����� ��������� �������.
*/
inline normal2d_t calcNormal( const coord2d_t& a, const coord2d_t& b ) {
    const auto d = b - a;
    return normal2d_t( -d.get<0>(), d.get<1>() );
}



/**
* @return ��������������� ������.
*/
inline vector3d_t calcNormalised( const vector3d_t& v ) {
    const float l = std::sqrt(
        v.get<0>() * v.get<0>()
      + v.get<1>() * v.get<1>()
      + v.get<2>() * v.get<2>()
    );
    assert( (l > (float)1e-08) && "������ �� ������." );
    const float l1 = 1.0f / l;
    return vector3d_t(
        v.get<0>() * l1,
        v.get<1>() * l1,
        v.get<2>() * l1
    );
}



/**
* @return ������� � ������������.
*/
inline normal3d_t calcNormal( const triangle3d_t& t ) {
    const float x1 = t[0].get<0>();
    const float y1 = t[0].get<1>();
    const float z1 = t[0].get<2>();
    const float x2 = t[1].get<0>();
    const float y2 = t[1].get<1>();
    const float z2 = t[1].get<2>();
    const float x3 = t[2].get<0>();
    const float y3 = t[2].get<1>();
    const float z3 = t[2].get<2>();
    // @todo optimize ���������� �� �������� ������.
    return calcNormalised( normal3d_t(
        -( y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2) ),
        -( z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2) ),
        -( x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2) )
    ) );
}



/**
* @return ����� ������������, �������� �������� ����������� �����.
*
* �������� ����� � ������������ ��� �� ��������� XY.
* ����� ������������ ��� �������� ����� �� �������������� ������.
*
* @see fourTriangle3d_t
*
* @copy fourTriangle3d_t
*   .---------.
*   | \  0  / |
*   |  \   /  |
*   |3   *   1|
*   |  /   \  |
*   | /  2  \ |
*   .---------.
*
*//* - ���� �� ����.
inline int calcInTriangle(
    const fourTriangle3d_t& t ...
) {


    return -1;
}
*/




/**
* @return ���������� ����� ����� �������.
*/
inline float calcDistance( const coord2d_t& a, const coord2d_t& b ) {
    const auto d = b - a;
    return std::sqrt(
        d.get<0>() * d.get<0>()
      + d.get<1>() * d.get<1>()
    );
}


inline float calcDistance( const coord3d_t& a, const coord3d_t& b ) {
    const auto d = b - a;
    return std::sqrt(
        d.get<0>() * d.get<0>()
      + d.get<1>() * d.get<1>()
      + d.get<2>() * d.get<2>()
    );
}



/**
* @return ����������� ������ �������, � ������� ����� ��������� ������.
*/
inline box2d_t calcBox( const surface2d_t& f ) {
    assert( (f.size() > 0) && "����������� �� ������ �����������." );
    const auto x = calcMinMax<0>( f );
    const auto z = calcMinMax<1>( f );
    const coord2d_t p1( x.upper(), z.upper() );
    const coord2d_t p2( x.lower(), z.lower() );
    return box2d_t( p1, p2 );
}




/**
* @return ������.
*
* (!) ������ boxInt2d_t ������ ������ 1. box2d_t - ����� ���� ����� 0.
*/
inline size2d_t calcSize( const box2d_t& b ) {
    const size2d_t s(
        abs( b.first.get<0>() - b.second.get<0>() ),
        abs( b.first.get<1>() - b.second.get<1>() )
    );
    return s;
}

inline sizeInt2d_t calcSize( const boxInt2d_t& b ) {
    const sizeInt2d_t s(
        abs( b.first.get<0>() - b.second.get<0>() ) + 1,
        abs( b.first.get<1>() - b.second.get<1>() ) + 1
    );
    return s;
}





/**
* @return ������������ ����� ��� numeric_limits< double >::infinity().
*/
inline double numberCast( const std::string& s ) {
    try {
        return boost::lexical_cast< double >( s );
    }
    catch ( boost::bad_lexical_cast& ) {
        return std::numeric_limits< double >::infinity();
    }
}







/**
* ���������� �������� boost::any.
*
* @return true1, ���� ���� �����. true2, ���� �������� �����.
*
* (!) ����� ���������� ���� ( false, true ), �.�. ����� ������ �����
* ������������ ��� �����.
*
* @param precision �������� ��� ��������� �������� � ��������� ������.
*
* @see strongEquals()
*/
inline std::pair< bool /* ���� ��������� */,  bool /* ���������� ��������� */ >
lazyEquals( const boost::any& a, const boost::any& b, double precision = 1e-8 ) {
    using namespace std;
    using namespace boost;

    // ������� ������ �� ���������
    const type_info& at = a.type();
    const type_info& bt = b.type();
    std::pair< bool, bool >  r = std::make_pair(
        (at == bt),
        false
    );
    
    // double
    if (at == typeid( double )) {
        const auto ca = any_cast< double >( a );
        double cb = numeric_limits< double >::infinity();
        if (bt == typeid( double )) {
            cb = any_cast< double >( b );
        } else if (bt == typeid( float )) {
            cb = (double)any_cast< float >( b );
        } else if (bt == typeid( int )) {
            cb = (double)any_cast< int >( b );
        } else if (bt == typeid( size_t )) {
            cb = (double)any_cast< size_t >( b );
        } else if (bt == typeid( string )) {
            cb = numberCast( any_cast< string >( b ) );
            // ������� infinity(), ���� �� �����
        }
        if (cb != numeric_limits< double >::infinity()) {
            r.second = (precision > std::abs( ca - cb ));
        }

    // float
    } else if (at == typeid( float )) {
        const auto ca = any_cast< float >( a );
        float cb = numeric_limits< float >::infinity();
        if (bt == typeid( double )) {
            cb = (float)any_cast< double >( b );
        } else if (bt == typeid( float )) {
            cb = any_cast< float >( b );
        } else if (bt == typeid( int )) {
            cb = (float)any_cast< int >( b );
        } else if (bt == typeid( size_t )) {
            cb = (float)any_cast< size_t >( b );
        } else if (bt == typeid( string )) {
            cb = (float)numberCast( any_cast< string >( b ) );
            // ������� infinity(), ���� �� �����
        }
        if (cb != numeric_limits< float >::infinity()) {
            r.second = (precision > std::abs( ca - cb ));
        }

    // int
    } else if (at == typeid( int )) {
        const auto ca = any_cast< int >( a );
        if (bt == typeid( double )) {
            r.second = (ca == (int)any_cast< double >( b ));
        } else if (bt == typeid( float )) {
            r.second = (ca == any_cast< float >( b ));
        } else if (bt == typeid( int )) {
            r.second = (ca == any_cast< int >( b ));
        } else if (bt == typeid( size_t )) {
            r.second = (ca == (int)any_cast< size_t >( b ));
        } else if (bt == typeid( string )) {
            const double temp = numberCast( any_cast< string >( b ) );
            // ������� infinity(), ���� �� �����
            if (temp != numeric_limits< double >::infinity()) {
                r.second = (ca == (int)temp);
            }
        }

    // size_t
    } else if (at == typeid( size_t )) {
        const auto ca = any_cast< size_t >( a );
        if (bt == typeid( double )) {
            r.second = (ca == (size_t)any_cast< double >( b ));
        } else if (bt == typeid( float )) {
            r.second = (ca == (size_t)any_cast< float >( b ));
        } else if (bt == typeid( int )) {
            r.second = (ca == (size_t)any_cast< int >( b ));
        } else if (bt == typeid( size_t )) {
            r.second = (ca == any_cast< size_t >( b ));
        } else if (bt == typeid( string )) {
            const double temp = numberCast( any_cast< string >( b ) );
            // ������� infinity(), ���� �� �����
            if (temp != numeric_limits< double >::infinity()) {
                r.second = (ca == (size_t)temp);
            }
        }

    // string
    } else if (at == typeid( string )) {
        const string ca = any_cast< string >( a );
        if (bt == typeid( string )) {
            r.second = (ca == any_cast< string >( b ));
        }
        // �����, ����� ������������ �������? ������ ����� ���������� �
        // ����� � ���������� ����� �������� �� ����
        const double nca = numberCast( ca );
        if (nca != numeric_limits< double >::infinity()) {
            if (bt == typeid( double )) {
                const auto ncb = any_cast< double >( b );
                r.second = (precision > std::abs( nca - ncb ));
            } else if (bt == typeid( float )) {
                const auto ncb = any_cast< float >( b );
                r.second = (precision > std::abs( (float)nca - ncb ));
            } else if (bt == typeid( int )) {
                const auto ncb = any_cast< int >( b );
                r.second = ((int)nca == ncb);
            } else if (bt == typeid( size_t )) {
                const auto ncb = any_cast< size_t >( b );
                r.second = ((size_t)nca == ncb);
            }
            // string ���������� ����
        }

    } // if-else ...

    return r;
}





} // namespace d








/**
* ����� � �����.
*/
inline ::std::ostream& operator<<( ::std::ostream &out, const d::coord3d_t& c ) {
    out << "(" << c.get<0>() << ", " << c.get<1>() << ", " << c.get<2>() << ")";
    return out;
}


inline ::std::ostream& operator<<( ::std::ostream &out, const d::coordInt3d_t& c ) {
    out << "(" << c.get<0>() << ", " << c.get<1>() << ", " << c.get<2>() << ")";
    return out;
}


inline ::std::ostream& operator<<( ::std::ostream &out, const d::plane3d_t& p ) {
    out << "[ " << p[0] << ", " << p[1] << ", " << p[2] << ", " << p[3] << " ]";
    return out;
}


inline ::std::ostream& operator<<( ::std::ostream &out, const d::plane4Triangle3d_t& p ) {
    out << "[ (" << p[0] << "), " << p[1] << ", " << p[2] << ", " << p[3] << ", " << p[4] << " ]";
    return out;
}


inline ::std::ostream& operator<<( ::std::ostream &out, const d::triangle3d_t& t ) {
    out << "{ " << t[0] << ", " << t[1] << ", " << t[2] << " }";
    return out;
}
