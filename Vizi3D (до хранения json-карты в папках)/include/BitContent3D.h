#pragma once

#include <boost/dynamic_bitset.hpp>
#include "default.h"
#include "helper.h"


/**
* Содержание 3D, хранимое в виде битового образа.
*
* @see http://www.boost.org/doc/libs/1_48_0/libs/dynamic_bitset/dynamic_bitset.html
*/
class BitContent3D {
public:
    /**
    * Сырой (одномерный) набор бит.
    */
    typedef boost::dynamic_bitset< unsigned long >  raw_t;


public:
    explicit
    inline BitContent3D(
        const d::sizeInt3d_t& size
    ) :
        s( size )
    {
        const size_t N = (size_t)( size.get<0>() * size.get<1>() * size.get<2>() );
        assert( N != 0 );
        d = raw_t( N );
    }



    virtual inline ~BitContent3D() {
    }



    /**
    * @return Размер содержания.
    */
    inline d::sizeInt3d_t size() const {
        return s;
    }



    /**
    * @return Сырой (одномерный) набор бит.
    */
    inline raw_t& raw() {
        return d;
    }

    inline const raw_t& raw() const {
        return d;
    }



    /**
    * @return Бит с указанной стороны ячейки установлен.
    *
    * Если соседняя ячейка выходит за пределы 3D-матрицы (ячейка лежит на
    * границе), возвращает 'false'.
    *
    * @param n Рассматриваемая ячейка.
    * @param k Номер соседней ячейки. Нумерация сторон - см. helper::isc3D().
    */
    inline bool has( const d::coordInt3d_t& n, size_t k ) const {
        const d::coordInt3d_t coordCell = n + helper::isc3D( k );
        return helper::insideMatrix( coordCell, s ) && test( coordCell );
    }






    inline BitContent3D& operator=( bool x ) {
        x ? d.set() : d.reset();
        return *this;
    }



    inline BitContent3D& operator=( const BitContent3D& b ) {
        d = b.raw();
        return *this;
    }



    // bitset operations
    inline BitContent3D& operator&=(const BitContent3D& b) {
        d &= b.raw();
        return *this;
    }


    inline BitContent3D& operator|=(const BitContent3D& b) {
        d |= b.raw();
        return *this;
    }


    inline BitContent3D& operator^=(const BitContent3D& b) {
        d ^= b.raw();
        return *this;
    }



    inline bool operator==( const BitContent3D& b ) {
        return (d == b.raw());
    }



    inline bool operator!=( const BitContent3D& b ) {
        return (d != b.raw());
    }



    // basic bit operations
    inline BitContent3D& set( const d::coordInt3d_t& n, bool val = true) {
        d.set( ic(n), val );
        return *this;
    }


    inline BitContent3D& set() {
        d.set();
        return *this;
    }


    inline BitContent3D& reset( const d::coordInt3d_t& n ) {
        d.reset( ic(n) );
        return *this;
    }


    inline BitContent3D& reset() {
        d.reset();
        return *this;
    }


    inline BitContent3D& flip( const d::coordInt3d_t& n ) {
        d.flip( ic(n) );
        return *this;
    }


    inline BitContent3D& flip() {
        d.flip();
        return *this;
    }



    inline bool test( const d::coordInt3d_t& n ) const {
        return d.test( ic(n) );
    }


    inline bool any() const {
        return d.any();
    }


    inline bool none() const {
        return d.none();
    }


    inline BitContent3D operator~() const {
        BitContent3D b( *this );
        b.raw() = ~d;
        return b;
    }


    inline size_t count() const {
        return d.count();
    }



    // subscript
    inline BitContent3D& operator[]( const d::coordInt3d_t& c ) {
        const auto i = ic( c );
        return ( *this )[i];
    }


    inline bool empty() const {
        return d.empty();
    }



    /**
    * @return 3D-координата, переведёная в 1D.
    */
    inline size_t ic( const d::coordInt3d_t& c ) const {
        assert( (size_t)c.get<0>() < s.get<0>() );
        assert( (size_t)c.get<1>() < s.get<1>() );
        assert( (size_t)c.get<2>() < s.get<2>() );
        return (size_t)c.get<0>()
            + (size_t)c.get<1>() * s.get<0>()
            + (size_t)c.get<2>() * s.get<0>() * s.get<1>();
    }



private:
    /**
    * Размер содержания.
    */
    const d::sizeInt3d_t s;


    raw_t d;

};

