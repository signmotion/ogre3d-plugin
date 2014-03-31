#pragma once

#include <Ogre.h>
#include <SdkSample.h>
#include "default.h"


/**
* ��������� �������� ������ � ������, ��������������� � ���� �������.
*/
namespace helper {


inline bool borderRight( int x, int z, size_t N, size_t M ) {
    return (x == ((int)N - 1));
}

inline bool borderBottom( int x, int z, size_t N, size_t M ) {
    return (z == 0);
}

inline bool borderLeft( int x, int z, size_t N, size_t M ) {
    return (x == 0);
}

inline bool borderTop( int x, int z, size_t N, size_t M ) {
    return (z == ((int)M - 1));
}


/**
* @return �������, ��� ��������� ����� ����� ������ ������� N x M.
* == !outsideMatrix()
*/
inline bool insideMatrix( int x, int z, size_t N, size_t M ) {
    return (
        (x >= 0) && (x < (int)N)
     && (z >= 0) && (z < (int)M)
    );
}


inline bool insideMatrix( int x, int y, int z, size_t N, size_t M, size_t L ) {
    return (
        (x >= 0) && (x < (int)N)
     && (y >= 0) && (y < (int)M)
     && (z >= 0) && (z < (int)L)
    );
}


inline bool insideMatrix( const d::coordInt2d_t& c, const d::sizeInt2d_t& size ) {
    return insideMatrix(
        c.get<0>(),    c.get<1>(),
        size.get<0>(), size.get<1>()
    );
}


inline bool insideMatrix( const d::coordInt3d_t& c, const d::sizeInt3d_t& size ) {
    return insideMatrix(
        c.get<0>(),    c.get<1>(),    c.get<2>(),
        size.get<0>(), size.get<1>(), size.get<2>()
    );
}



/**
* @return �������, ��� ��������� ����� ����� ��� ������� N x M.
* == !insideMatrix()
*/
inline bool outsideMatrix( int x, int z, size_t N, size_t M ) {
    return (
        (z < 0) || (z >= (int)M)
     || (x < 0) || (x >= (int)N)
    );
}


inline bool outsideMatrix( const d::coordInt2d_t& c, const d::sizeInt2d_t& size ) {
    return outsideMatrix(
        c.get<0>(),    c.get<1>(),
        size.get<0>(), size.get<1>()
    );
}



/**
* @return �������� �������� � ������� ��������� ������.
*         ���� ��������� ������� ����, ���������� (0; 0).
*
* ������������ �����:
*   M
*   .
*   .
*   8   1   5
*   4   0   2
*   7   3   6  .  .  N
*
* ����� ������������ ������� (�� �������, �� ������, �� �����), ����� ����
* ������ ��������� �� �������� � ����� ������, � ����������� �� ������������.
* ��� ������� ��������� � ����� ���������.
*
* isc - ����. 'i' shift cell
*/
inline d::coordInt2d_t isc2D( size_t cell ) {
    d::coordInt2d_t c;
    switch ( cell ) {
        case 1:  c = d::coordInt2d_t(  0, +1 );  break;
        case 2:  c = d::coordInt2d_t( +1,  0 );  break;
        case 3:  c = d::coordInt2d_t(  0, -1 );  break;
        case 4:  c = d::coordInt2d_t( -1,  0 );  break;
        case 5:  c = d::coordInt2d_t( +1, +1 );  break;
        case 6:  c = d::coordInt2d_t( +1, -1 );  break;
        case 7:  c = d::coordInt2d_t( -1, -1 );  break;
        case 8:  c = d::coordInt2d_t( -1, +1 );  break;
        case 0:  c = d::coordInt2d_t(  0,  0 );  break;
        default: throw "Not correct index of cell. See description for this method.";
    }

    return c;
}



/**
* @see isc2D
*
* ��� 3D-������������.
* ������� ���������� �����. ������� �� ��� OY �� ��� 3x3x3. ��� Z ����������
* �����. ������� ������� (�����������) 2D ���� - �������� ��� �������� isc2D().
* ������� ������� - �������� �����. ������� ������� ���� - ����������.
*
  ���������:
    ����������� 2D ����
    8   1   5
    4   0   2
    7   3   6

    ������� 2D ����
    17   10   14
    13    9   11
    16   12   15

    ������� 2D ����
    26   19   23
    22   18   20
    25   21   24
*
*/
inline d::coordInt3d_t isc3D( size_t cell ) {
    d::coordInt3d_t c;
    switch ( cell ) {
        // ����������� 2D ����
        case 0:  c = d::coordInt3d_t(  0,  0,  0 );  break;
        case 1:  c = d::coordInt3d_t(  0,  0, +1 );  break;
        case 2:  c = d::coordInt3d_t( +1,  0,  0 );  break;
        case 3:  c = d::coordInt3d_t(  0,  0, -1 );  break;
        case 4:  c = d::coordInt3d_t( -1,  0,  0 );  break;
        case 5:  c = d::coordInt3d_t( +1,  0, +1 );  break;
        case 6:  c = d::coordInt3d_t( +1,  0, -1 );  break;
        case 7:  c = d::coordInt3d_t( -1,  0, -1 );  break;
        case 8:  c = d::coordInt3d_t( -1,  0, +1 );  break;

        // ������� 2D ����
        case  9:  c = d::coordInt3d_t(  0, -1,  0 );  break;
        case 10:  c = d::coordInt3d_t(  0, -1, +1 );  break;
        case 11:  c = d::coordInt3d_t( +1, -1,  0 );  break;
        case 12:  c = d::coordInt3d_t(  0, -1, -1 );  break;
        case 13:  c = d::coordInt3d_t( -1, -1,  0 );  break;
        case 14:  c = d::coordInt3d_t( +1, -1, +1 );  break;
        case 15:  c = d::coordInt3d_t( +1, -1, -1 );  break;
        case 16:  c = d::coordInt3d_t( -1, -1, -1 );  break;
        case 17:  c = d::coordInt3d_t( -1, -1, +1 );  break;

        // ������� 2D ����
        case 18:  c = d::coordInt3d_t(  0, +1,  0 );  break;
        case 19:  c = d::coordInt3d_t(  0, +1, +1 );  break;
        case 20:  c = d::coordInt3d_t( +1, +1,  0 );  break;
        case 21:  c = d::coordInt3d_t(  0, +1, -1 );  break;
        case 22:  c = d::coordInt3d_t( -1, +1,  0 );  break;
        case 23:  c = d::coordInt3d_t( +1, +1, +1 );  break;
        case 24:  c = d::coordInt3d_t( +1, +1, -1 );  break;
        case 25:  c = d::coordInt3d_t( -1, +1, -1 );  break;
        case 26:  c = d::coordInt3d_t( -1, +1, +1 );  break;

        default: throw "Not correct index of cell. See description for this method.";
    }

    return c;
}






/**
* ��� �������.
*
* (!) ����� ������� ������� ����� ������� Tray ������ ������ ����
*     ���� �� ���� ����� init().
*/
class Tray {

protected:
    inline Tray() {
    }


public:

    /**
    * �������������.
    */
    static inline void init(
        OgreBites::SdkTrayManager* t
    ) {
        Tray::tray = t;
    }


    /**
    * @return true, ���� Tray ��� ���������������.
    *         ����� �������������� �������� ����� ������� �����������
    *         ������� ������ Tray.
    */
    static bool initiated() {
        return  tray ? true : false;
    }



    virtual inline ~Tray() {
    };




    /**
    * �������� ��� � �������� �� ������.
    * @require ����� init().
    */
    static void p( const std::string& name, const std::string& value );




private:
    /**
    * ��������� ��� ������ � ���������� �������������� ������.
    * ��������, ����������� ����������� ������.
    * ������ ���� ������������������ ����� ������� ������� ������.
    */
    static OgreBites::SdkTrayManager* tray;

};


}
