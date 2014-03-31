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


inline bool outsideMatrix( const coordInt2d_t& c, const sizeInt2d_t& size ) {
    return (c < ZEROINT2D) || (c >= size);
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
inline coordInt2d_t isc( size_t cell ) {
    coordInt2d_t c;
    switch ( cell ) {
        case 1:  c = coordInt2d_t(  0, +1 );  break;
        case 2:  c = coordInt2d_t( +1,  0 );  break;
        case 3:  c = coordInt2d_t(  0, -1 );  break;
        case 4:  c = coordInt2d_t( -1,  0 );  break;
        case 5:  c = coordInt2d_t( +1, +1 );  break;
        case 6:  c = coordInt2d_t( +1, -1 );  break;
        case 7:  c = coordInt2d_t( -1, -1 );  break;
        case 8:  c = coordInt2d_t( -1, +1 );  break;
        case 0:  c = coordInt2d_t(  0,  0 );  break;
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
