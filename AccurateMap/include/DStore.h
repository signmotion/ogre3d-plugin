#pragma once

#include "configure.h"
#include "default.h"
#include "Doxel.h"
#include <couchdb/CouchDB.h>
#include <boost/algorithm/string.hpp>



/**
* �������������� ��������� ��� ������� (�-���������).
* ������������ CouchDB.
* �������� ������ ����������� � ������ ������� (���������� CouchDB).
*/
class DStore {
protected:
    inline DStore() {
        std::cout << "DStore()" << std::endl;
        // @see newSingleton()
    }



public:
    /**
    * ������������ � ������ ���������.
    * @todo ������� ��������� ������ ��������?
    *
    * ������ ����� DStore ������ ���� ����������� � ����������� �����������.
    * ������ ����� ������������ ::singleton() ��� ����������.
    */
    static inline DStore* newSingleton(
        const std::string& name,
        const std::string& user,
        const std::string& password,
        const std::string& url
    ) {
        if ( !instance ) {
            instance = new DStore();
        };

        std::string pName = name;
        boost::algorithm::to_lower( pName );
        instance->name = pName;

        instance->user = user;
        instance->password = password;
        instance->url = 
            user.empty()
              ? ("http://" + url)
              : ("http://" + user + ":" + password + "@" + url);

        instance->conn = std::shared_ptr< CouchDB::Connection >(
            new CouchDB::Connection( instance->url )
        );

        if ( !instance->conn->existsDatabase( instance->name ) ) {
            instance->conn->createDatabase( instance->name );
        }

        instance->store = std::shared_ptr< CouchDB::Database >(
            new CouchDB::Database( instance->conn->getDatabase( instance->name ) )
        );

        return instance;
    }



    static inline DStore* singleton() {
        assert( instance && "����� �������������� DStore ��� ������� ����������������. ��. newSingleton()." );
        return instance;
    }



    virtual inline ~DStore() {
        std::cout << "~DStore()" << std::endl;
        delete instance;
    }




    /**
    * ��������� ����� ������� � �-���������.
    *
    * ���������� ���������� ������ � �� ����������� ����� � ���������.
    * �������� �. *������ �����* ������������ � ���������, ��� ������� -
    * �� ���� ����������.
    * (!) ��� ���������������� ���������� �������, ������� �� ��������
    * ��������, ������� ������� ����� flush().
    *
    * �������� �. ������ ���� ������� �� ������ ����� ������.
    * ��������� ������������ �������:
    *   - �����������
    *   - ������
    *   - &����������
    *   - &��������
    * �������� �� ������: ������������� ������������� ������������ CouchDB
    * ������� ������� �� ������ ������.
    *
    * @return ���� ��� ������� � ����� �������.
    *
    * @see ����. � ���� Doxel::lazySave()
    */
    template< size_t D, typename SizeT, typename ContentT >
    inline d::uid_t lazySave(
        const SizeT& size,
        const ContentT& cnt,
        const d::uid_t& extParentKey,
        const d::dant_t& dant
    ) {
        // ���������� � ������� ������ ��� ���� ��������:
        //   - ������ �-��������� �� ����������� ���������� �����������
        //   - ����� ������� ��������� / ��������� �������
        // �������� � ����������� ������ - ������ ���� (��. CDManager).

        CouchDB::Object o;

        o["D"] = CouchDB::cjv( D );
        o["size"] = CouchDB::cjv( size );
        o["content"] = CouchDB::cjv( cnt.json() );

        if ( !extParentKey.empty() ) {
            o["parent"] = CouchDB::cjv( extParentKey );
        }

        /* - ����������� ��������� � ���������� bulk'�� � ������. ��. ����������� � ����.
        const CouchDB::Document doc = store->createDocument( CouchDB::cjv( o ) );
        const d::uid_t uid = doc.getID();
        const d::rev_t rev = doc.getRevision();

        return d::externKey_t( uid, rev );
        */

        const d::uid_t uid = store->createBulk( o );

        return uid;
    }



    /**
    * ���������� ����������� ������� � ���������.
    *
    * @see lazySave()
    */
    inline void flush() const {
        store->flush();
    }




    /**
    * @return ���������� ������� � ���������.
    */
    inline size_t count() const {
        const size_t countDoc = store->countDoc();
        const size_t countDesign = store->countDesign();
        return (countDoc - countDesign);
    }




    /**
    * @return ������� ���������.
    *
    * @param includeDesign true, ���� ���������� �������� � design-���������.
    */
    inline void clear( bool includeDesign ) {
        const std::string name = store->getName();
        conn->clearDatabase( name, false );
    }




protected:
    /**
    * ����������� � CouchDB.
    */
    std::string name;
    std::string user;
    std::string password;
    std::string url;

    std::shared_ptr< CouchDB::Connection >  conn;
    std::shared_ptr< CouchDB::Database >    store;


    /**
    * ���������.
    */
    static DStore* instance;

};
