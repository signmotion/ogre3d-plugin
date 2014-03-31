#pragma once

#include "ContentDoxel.h"


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
