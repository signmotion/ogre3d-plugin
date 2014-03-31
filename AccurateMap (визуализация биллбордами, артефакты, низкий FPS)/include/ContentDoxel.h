#pragma once

//#include <unordered_map>
#include <boost/any.hpp>


/**
* ���������� �������.
* ��������� ������������� ��� ������������.
*
* @see Doxel
*/
struct ContentDoxel {
    /**
    * ������ �������.
    */
    //std::unordered_map< std::string, boost::any >  data;



    inline ContentDoxel() {
    }


    virtual inline ~ContentDoxel() {
    }

};





/**
* ���������� � ������ �����.
*/
struct CellCD : public ContentDoxel {

    /**
    * ��� ��������, ������������ � ���� ������.
    * �������� ������� �����������, ����� ��������� �������� ���
    * ����������� � ������. @todo optimize ������������������?
    */
    const int element;


    inline CellCD(
        int element = 0
    ) :
        element( element )
    {
    }


    virtual inline ~CellCD() {
    }

};
