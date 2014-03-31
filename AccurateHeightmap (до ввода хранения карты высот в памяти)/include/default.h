#pragma once

#include <boost/assign/list_inserter.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/numeric/interval.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <Ogre.h>


/*
* ����������� ��� ����� ������� ���������, ������, ������.
*/


// ��������� OpenCL
/* - �� ������������. ��. ����. � Port.h
const std::string PATH_OPENCL = "D:/Projects/workspace/ogre3d/AccurateHeightmap/resource/OpenCL";
const std::string PLATFORM_OPENCL = "NVIDIA";
*/



/**
* �������� ��������� ����� � ��������� ������.
*/
const float PRECISION = 0.01f;


/**
* ������������� ��������.
*/
typedef std::string uid_t;



/**
* ��������� ����������.
* �������, ����� ���������� �������� ������ �������. ��� � ����. ��
* ����� ����������� � �������� ����������� (��������, ������ ������
* ����� �������� � ������), ����� ��������� �� �������.
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
* ������� �� ���������. ������ ���������� - ���� � ������������ ������������.
*/
typedef std::pair< coord2d_t, coord2d_t >  box2d_t;
typedef std::pair< coordInt2d_t, coordInt2d_t >  boxInt2d_t;

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
* �������� �������������.
* ��������, �������������� ����� ���� ����������� ����. �������:
*   .---------.
*   | \     / |
*   |  \ 0 /  |
*   |  3 * 1  |
*   |  / 2 \  |
*   | /     \ |
*   .---------.
*/
typedef boost::array< triangle3d_t, 4 >  quadroTriangle3d_t;




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



/* * 2D operations * */

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






inline coordInt2d_t operator-( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return coordInt2d_t(
        a.get<0>() - b.get<0>(),
        a.get<1>() - b.get<1>()
    );
}


inline coordInt2d_t operator-=( coordInt2d_t& a, const coordInt2d_t& b ) {
    a.get<0>() -= b.get<0>();
    a.get<1>() -= b.get<1>();
    return a;
}


inline coordInt2d_t operator+( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return coordInt2d_t(
        a.get<0>() + b.get<0>(),
        a.get<1>() + b.get<1>()
    );
}


inline coordInt2d_t operator+=( coordInt2d_t& a, const coordInt2d_t& b ) {
    a.get<0>() += b.get<0>();
    a.get<1>() += b.get<1>();
    return a;
}


inline coordInt2d_t operator*( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return coordInt2d_t(
        a.get<0>() * b.get<0>(),
        a.get<1>() * b.get<1>()
    );
}


inline coordInt2d_t operator*=( coordInt2d_t& a, const coordInt2d_t& b ) {
    a.get<0>() *= b.get<0>();
    a.get<1>() *= b.get<1>();
    return a;
}


inline coordInt2d_t operator*( const coordInt2d_t& a, int k ) {
    return coordInt2d_t(
        a.get<0>() * k,
        a.get<1>() * k
    );
}


inline coordInt2d_t operator/( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return coordInt2d_t(
        (int)(a.get<0>() / b.get<0>()),
        (int)(a.get<1>() / b.get<1>())
    );
}


inline coordInt2d_t operator/=( coordInt2d_t& a, int k ) {
    a.get<0>() = (int)(a.get<0>() / k);
    a.get<1>() = (int)(a.get<1>() / k);
    return a;
}


inline coordInt2d_t operator/( const coordInt2d_t& a, int k ) {
    return coordInt2d_t(
        (int)(a.get<0>() / k),
        (int)(a.get<1>() / k)
    );
}


inline bool operator==( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return (a.get<0>() == b.get<0>())
        && (a.get<1>() == b.get<1>());
}

inline bool operator!=( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return !(a == b);
}


inline bool operator>( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return (a.get<0>() > b.get<0>()) && (a.get<1>() > b.get<1>());
}


inline bool operator<( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return (a.get<0>() < b.get<0>()) && (a.get<1>() < b.get<1>());
}


inline bool operator>=( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return (a.get<0>() >= b.get<0>()) && (a.get<1>() >= b.get<1>());
}


inline bool operator<=( const coordInt2d_t& a, const coordInt2d_t& b ) {
    return (a.get<0>() <= b.get<0>()) && (a.get<1>() <= b.get<1>());
}






/* * 3D operations * */

inline coord3d_t operator-( const coord3d_t& a, const coord3d_t& b ) {
    return coord3d_t(
        a.get<0>() - b.get<0>(),
        a.get<1>() - b.get<1>(),
        a.get<2>() - b.get<2>()
    );
}


inline coord3d_t operator-=( coord3d_t& a, const coord3d_t& b ) {
    a.get<0>() -= b.get<0>();
    a.get<1>() -= b.get<1>();
    a.get<2>() -= b.get<2>();
    return a;
}


inline coord3d_t operator+( const coord3d_t& a, const coord3d_t& b ) {
    return coord3d_t(
        a.get<0>() + b.get<0>(),
        a.get<1>() + b.get<1>(),
        a.get<2>() + b.get<2>()
    );
}


inline coord3d_t operator+=( coord3d_t& a, const coord3d_t& b ) {
    a.get<0>() += b.get<0>();
    a.get<1>() += b.get<1>();
    a.get<2>() += b.get<2>();
    return a;
}


inline coord3d_t operator*( const coord3d_t& a, const coord3d_t& b ) {
    return coord3d_t(
        a.get<0>() * b.get<0>(),
        a.get<1>() * b.get<1>(),
        a.get<2>() * b.get<2>()
    );
}


inline coord3d_t operator*=( coord3d_t& a, const coord3d_t& b ) {
    a.get<0>() *= b.get<0>();
    a.get<1>() *= b.get<1>();
    a.get<2>() *= b.get<2>();
    return a;
}


inline coord3d_t operator*( const coord3d_t& a, float k ) {
    return coord3d_t(
        a.get<0>() * k,
        a.get<1>() * k,
        a.get<2>() * k
    );
}


inline coord3d_t operator/( const coord3d_t& a, const coord3d_t& b ) {
    return coord3d_t(
        a.get<0>() / b.get<0>(),
        a.get<1>() / b.get<1>(),
        a.get<2>() / b.get<2>()
    );
}


inline coord3d_t operator/=( coord3d_t& a, float k ) {
    a.get<0>() /= k;
    a.get<1>() /= k;
    a.get<2>() /= k;
    return a;
}


inline coord3d_t operator/( const coord3d_t& a, float k ) {
    return coord3d_t(
        a.get<0>() / k,
        a.get<1>() / k,
        a.get<2>() / k
    );
}


// @see PRECISION
inline bool operator==( const coord3d_t& a, const coord3d_t& b ) {
    return (abs(a.get<0>() - b.get<0>()) < PRECISION)
        && (abs(a.get<1>() - b.get<1>()) < PRECISION)
        && (abs(a.get<2>() - b.get<2>()) < PRECISION);
}

inline bool operator!=( const coord3d_t& a, const coord3d_t& b ) {
    return !(a == b);
}


inline bool operator>( const coord3d_t& a, const coord3d_t& b ) {
    return (a.get<0>() > b.get<0>())
        && (a.get<1>() > b.get<1>())
        && (a.get<2>() > b.get<2>());
}


inline bool operator<( const coord3d_t& a, const coord3d_t& b ) {
    return (a.get<0>() < b.get<0>())
        && (a.get<1>() < b.get<1>())
        && (a.get<2>() < b.get<2>());
}


inline bool operator>=( const coord3d_t& a, const coord3d_t& b ) {
    return (a.get<0>() >= b.get<0>())
        && (a.get<1>() >= b.get<1>())
        && (a.get<2>() >= b.get<2>());
}


inline bool operator<=( const coord3d_t& a, const coord3d_t& b ) {
    return (a.get<0>() <= b.get<0>())
        && (a.get<1>() <= b.get<1>())
        && (a.get<2>() <= b.get<2>());
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
    return s;
}

inline sizeInt2d_t calcSize( const boxInt2d_t& b ) {
    const sizeInt2d_t s(
        abs( b.first.get<0>() - b.second.get<0>() ),
        abs( b.first.get<1>() - b.second.get<1>() )
    );
    return s;
}





/**
* ����� � �����.
*/
inline std::ostream& operator<<( std::ostream &out, const coord3d_t& c ) {
    out << "(" << c.get<0>() << ", " << c.get<1>() << ", " << c.get<2>() << ")";
    return out;
}


inline std::ostream& operator<<( std::ostream &out, const triangle3d_t& t ) {
    out << "{ " << t[0] << ", " << t[1] << ", " << t[2] << " }";
    return out;
}
