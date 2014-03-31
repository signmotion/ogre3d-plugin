#pragma once

#include "configure.h"
#include <assert.h>
#include <iostream>
#include <unordered_set>
#include <couchdb/CouchDB.h>
#include <unordered_map>


/**
* ���������� �������.
* ��������� ������������� ��� ������������.
*
* ���������� - ��� ��������� ������ ����� ������ ������� �..
*
* @see Doxel
*/
struct ContentDoxel {
    /**
    * ���� ����������.
    *
    * @see ���������� ������.
    */
    enum type_e {
        TYPE_CONCRETE = 1,
        TYPE_SOLID
    };



    inline ContentDoxel() {
    }



    virtual inline ~ContentDoxel() {
    }



    /**
    * @return ��� ����������. ������ ���������� ����������� ����
    *         ���������� ���.
    */
    virtual type_e type() const = 0;




    /**
    * @return ���� ����������.
    */
    virtual std::shared_ptr< ContentDoxel >  clone() const = 0;



    /**
    * @return ������������� ���������� ��� ���������� � �-���������.
    *         ������������ CouchDB.
    *
    * @see DStore
    */
    virtual CouchDB::Object json() const = 0;



    /**
    * ���-������� � ������� ��������� ����������.
    * ���������� ��� ����������� ������������ ���������� � ��� ���������.
    *
    * ��� ���������� ���-����� ����� �������� �
    * http://boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    virtual size_t hash() const = 0;
    virtual bool equals( const std::shared_ptr< ContentDoxel > a ) const = 0;


};




    




/**
* ���������� 3D-�������.
* ��������� ������������� ��� ������������.
*
* @see Doxel
*/
struct ContentDoxel3D : public ContentDoxel {

    inline ContentDoxel3D() {
    }



    virtual inline ~ContentDoxel3D() = 0;

};




    




/**
* ���������� ���������� 3D-�������.
*
* ��� ���������� ����� �� ����� ���� ��������. ������, �������������
* �� ����� ������ ������� ��������.
* ��������, � �������� ������ ���� (������� ����������� �.) �����
* ���������� �������. ������� ����� - "���������� ����������". ���� -
* "�������".
*
* @see Doxel
*/
struct ConcreteContentDoxel3D : public ContentDoxel3D {

    inline ConcreteContentDoxel3D() {
    }



    virtual inline ~ConcreteContentDoxel3D() {
    }



    /**
    * @see ContentDoxel
    */
    virtual inline type_e type() const {
        return TYPE_CONCRETE;
    }



    /**
    * @see ContentDoxel
    */
    virtual std::shared_ptr< ContentDoxel >  clone() const = 0;



    /**
    * @see ContentDoxel
    */
    virtual CouchDB::Object json() const = 0;



    /**
    * ���-������� � ������� ��������� ����������.
    * ���������� ��� ����������� ������������ ���������� � ��� ���������.
    *
    * ��� ���������� ���-����� ����� �������� �
    * http://boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    virtual size_t hash() const = 0;
    virtual bool equals( const std::shared_ptr< ContentDoxel > a ) const = 0;


};




    



/**
* ������� ���������� 3D-�������.
*
* ����� ���������� ��������� ������� � ������������ ������ ����������
* ��������.
* ��� ��������� �., ��� �������� ����� ������� ������ �����
* ������������ ���� �����������. �� ��� �� ��������� �����������
* ������� ���������� - ����������� ��� �������� - �� ������ �������.
*
* @see Doxel
*/
struct SolidContentDoxel3D : public ContentDoxel3D {

    inline SolidContentDoxel3D() {
    }



    virtual inline ~SolidContentDoxel3D() {
    }



    /**
    * @return ������� �� �������� �����������.
    *
    * ���� ��� ���������� ������ ���������� ���� � ��� �� �������.
    */
    virtual inline Doxel3D get( const d::coord3d_t& c ) const {
        return ...
    }



    /**
    * @see ContentDoxel
    */
    virtual inline type_e type() const {
        return TYPE_SOLID;
    }



    /**
    * @return ���� ����������.
    */
    virtual std::shared_ptr< ContentDoxel >  clone() const = 0;



    /**
    * @see ContentDoxel
    */
    virtual CouchDB::Object json() const = 0;



    /**
    * ���-������� � ������� ��������� ����������.
    * ���������� ��� ����������� ������������ ���������� � ��� ���������.
    *
    * ��� ���������� ���-����� ����� �������� �
    * http://boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    virtual size_t hash() const = 0;
    virtual bool equals( const std::shared_ptr< ContentDoxel > a ) const = 0;


};




    





/**
* ���������� � ������ �����.
*/
struct AboutCellCD : public ContentDoxel {

    /**
    * ��� ��������, ������������ � ���� ������.
    * �������� ������� �����������, ����� ��������� �������� ���
    * ����������� � ������. @todo optimize ������������������?
    */
    const int element;

#ifdef INCLUDE_DEBUG_INFO_IN_CONTENT
    /**
    * ������� ��� ������������.
    */
    std::unordered_map< std::string, boost::any >  test;
#endif



    inline AboutCellCD(
        int element = 0
    ) :
        element( element )
    {
    }


    virtual inline ~AboutCellCD() {
    }




    virtual inline std::shared_ptr< ContentDoxel >  clone() const {
        return std::shared_ptr< ContentDoxel >( new AboutCellCD(
            element
        ) );
    }


    
    virtual inline CouchDB::Object json() const {
        CouchDB::Object o;

        o["element"] = CouchDB::cjv( element + 0 );

#ifdef INCLUDE_DEBUG_INFO_IN_CONTENT
        // ������ �������������� ����������, ����� ���������������� ��� �������
        for (auto itr = test.cbegin(); itr != test.cend(); ++itr) {
            const auto field = *itr;
            o[ field.first ] = CouchDB::cjv( field.second );
        }
#endif

        return o;
    }
    



    virtual inline size_t hash() const {
        return element;
    }



    virtual inline bool equals( const std::shared_ptr< ContentDoxel > a ) const {
        assert( a );
        const auto pa = static_cast< AboutCellCD* >( a.get() );
        return (pa->element == element);
    }

};
