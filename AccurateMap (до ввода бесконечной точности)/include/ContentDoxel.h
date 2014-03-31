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
* @see Doxel
*/
struct ContentDoxel {
    inline ContentDoxel() {
    }



    virtual inline ~ContentDoxel() {
    }



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
* ��������� ��������� �������.
* �� ������ ������������� �������, ���������� ����������� ������ �� ����.
*
* ����. �� Content Doxel Manager.
*
* @todo �������� �������� ����� ���. ��������.
*/
class CDManager {
protected:
    inline CDManager() {
    }


public:
    static inline CDManager* singleton() {
        if ( instance ) {
            return instance;
        }
        instance = new CDManager();
        return instance;
    }



    virtual inline ~CDManager() {
        std::cout << "~CDManager()" << std::endl;
    }



    /**
    * @return ���������� ��������� ��������.
    */
    inline size_t count() const {
        return c.size();
    }



    /**
    * @return ������� ����������� � ������ ��������.
    */
    inline void clear() {
        c.clear();
    }


    
    /**
    * @return ������ �� ���������� ������� ������ ���������. ���� ������
    *         ���������� ��� ���, ��������� ���������� � ���������.
    */
    inline const std::shared_ptr< ContentDoxel >  content( const ContentDoxel& cnt ) {
        // @todo optimize ���������� �� ����������� ������ clone().
        auto nc = std::shared_ptr< ContentDoxel >( cnt.clone() );
        auto itr = c.find( nc );
        if (itr != c.end()) {
            return *itr;
        }
        itr = c.insert( nc ).first;
        return *itr;
    }





protected:
    /**
    * �������� ��� ����������� ����������� ����������.
    */
    struct FHash {
        size_t operator()( const std::shared_ptr< ContentDoxel > a ) const { 
            assert( a );
            return a->hash();
        }
    };

    struct FEquals {
        bool operator()( const std::shared_ptr< ContentDoxel > a, const std::shared_ptr< ContentDoxel > b ) const {
            assert( a );
            assert( b );
            return a->equals( b );
        }
    };

    /*
    * ����� ����������� ����������, �����-���� ����������� ����������.
    */
    std::unordered_set<
        std::shared_ptr< ContentDoxel >,
        FHash,
        FEquals
    > c;



    /**
    * ���������.
    */
    static CDManager* instance;

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
