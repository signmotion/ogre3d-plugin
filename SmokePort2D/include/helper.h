#pragma once

#include <Ogre.h>
#include <SdkSample.h>
#include "struct.h"
#include "default.h"


/**
* ��������� �������� ������ � ������.
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





/**
* @return ���������� ���������� �� �������� ���������.
*          -1, ���� ���� �� ��������� ������� �� ������� ������.
*
* ic - ����. 'i' cell
*/
inline int ic(
    int x, int z,
    size_t N, size_t M
) {
    return (
        outsideMatrix( x, z, N, M )
          ? -1
          : (x + z * N)
    );
}







/**
* @return ���������� ������ ��������� (������������ �������) ������.
*         ���� ������ ������� �� ������� ����, ���������� -1.
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
inline int isc(
    int x, int z,
    int dx, int dz,
    size_t N, size_t M
) {
    const int tx = x + dx;
    const int tz = z + dz;
    return (
        ( outsideMatrix( tx, tz, N, M ) || outsideMatrix( x, z, N, M ) )
          ? -1
          : (tx + tz * N)
    );
}




/**
* @see near()
*/
inline int nearCell(
    int x, int z,
    size_t cell,
    size_t N, size_t M
) {
    switch ( cell ) {
        case 1:  return isc( x, z,  0, +1, N, M );
        case 2:  return isc( x, z, +1,  0, N, M );
        case 3:  return isc( x, z,  0, -1, N, M );
        case 4:  return isc( x, z, -1,  0, N, M );
        case 5:  return isc( x, z, +1, +1, N, M );
        case 6:  return isc( x, z, +1, -1, N, M );
        case 7:  return isc( x, z, -1, -1, N, M );
        case 8:  return isc( x, z, -1, +1, N, M );
        case 0:  return ic( x, z, N, M );
    }

    return -1;
}






/**
* ��������� 'uid' � 'pressure' ��� ������ ���� � ������ �������.
* �������� ����, ����� ��������� ������� �� OpenCL.
*/
inline void calc(
    GasFD* content,
    int x, int z,
    size_t N, size_t M,
    size_t* i, size_t* uid, float* pressure
) {
    // �������� �� �������, ������� ����� ��������
    *i = nearCell( x, z, 0, N, M );
    assert(*i != -1);
    float allPressure = content[*i].pressure;
    for (size_t k = 1; k <= 8; ++k) {
        const int q = nearCell( x, z, k, N, M );
        if (q != -1) {
            allPressure += content[q].pressure;
        }
    }
    // �������������� �������� � ������
    *pressure = allPressure / 8.0f;

    // UID � ������ ����� ����������
    *uid = (*pressure < 0.01f) ? 0 : content[*i].uid;
}






/**
* ������� ������ ���������, ������� �������������� �� ���������
* ����������. ���������� ��������� ����������� �� ������ � ������ ��������
* ���� ���������� � (int).
* -1 ���� �������� �� ���������� �� ������.
*/
inline int index( const Ogre::BillboardSet& bs, const coordInt2d_t& coord, float scale ) {

    for (int index = 0; index < bs.getNumBillboards(); ++index) {
        const auto b = bs.getBillboard( index );
        const Ogre::Vector3 c = b->getPosition() / scale;
        if ( (coord.get<0>() == (int)c.x) && (coord.get<1>() == (int)c.z) ) {
            return index;
        }
    }

    return -1;
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
