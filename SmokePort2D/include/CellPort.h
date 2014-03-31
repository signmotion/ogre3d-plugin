#pragma once

#include "Port.h"


/**
* ����, �������������� ����� ���, �������� �� ���������� ������.
* ������ - ��������� ������� �� ������ ����������������, ���������������
* ����� �������.
*/
template< typename C >
class CellPort :
    public Port
{
public:
    /**
    * @param size ������ ����� � �������.
    *        (!) �.�. ������� ������� ����� OpenCL, ������� ������ ���������
    *        ���������� ������� (��. ���� ������������). �������, �����
    *        �������� �������������.
    * @param scale ������� ����� � ������. @todo �������� �����?
    */
    CellPort(
        Ogre::SceneManager* sm,
        float visualScale,
        const sizeInt2d_t& size,
        float scale
    );



    inline virtual ~CellPort() {
        delete contentT;
        delete content;
    }




    /**
    * @return ���������� ����� � �����.
    */
    inline size_t count() const {
        return NM;
    }



    /**
    * ��������� ������ ���������.
    */
    inline void set( const coordInt2d_t& coord, const C& c ) {
        const size_t i = helper::ic( coord.get<0>(), coord.get<1>(), N, M );
        content[i] = c;
    }






protected:
    /**
    * ������ ����� � �������.
    * ������ �� ���������, ����� ��������� ���-�� ��������� � pulse().
    */
    const size_t N;
    const size_t M;
    const size_t NM;

    /**
    * ������� ����� � ������.
    * ������� ������ ���������� � 1 ������.
    */
    const float scale;

    /**
    * ���������� ����� (���).
    */
    C* content;

    /**
    * ��������������� ������� ��� �������������.
    * �������� �����. 'content'. �� ����������������. ����� ��������������
    * �������� �� ������ ����������.
    */
    C* contentT;


    /**
    * �������������� � OpenCL.
    * ��. ������������� � ������������.
    *//* - �� ������������. ��. ����. � Port.h
    cl_device_id* devices;
    cl_context context;
    cl_command_queue commandQueue;
    cl_kernel kernel;
    cl_program program;
    */

};

