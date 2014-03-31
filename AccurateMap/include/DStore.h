#pragma once

#include "configure.h"
#include "default.h"
#include "Doxel.h"
#include <couchdb/CouchDB.h>
#include <boost/algorithm/string.hpp>



/**
* Долговременное хранилище для докселя (д-хранилище).
* Используется CouchDB.
* Название всегда переводится в нижний регистр (требование CouchDB).
*/
class DStore {
protected:
    inline DStore() {
        std::cout << "DStore()" << std::endl;
        // @see newSingleton()
    }



public:
    /**
    * Подключается к новому хранилищу.
    * @todo Сделать поддержку набора хранилищ?
    *
    * Первый вызов DStore должен быть обязательно с параметрами подключения.
    * Дальше можно пользоваться ::singleton() без параметров.
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
        assert( instance && "Перед использованием DStore его следует инициализировать. См. newSingleton()." );
        return instance;
    }



    virtual inline ~DStore() {
        std::cout << "~DStore()" << std::endl;
        delete instance;
    }




    /**
    * Сохраняет части докселя в д-хранилище.
    *
    * Используем накопление данных и их последующий сброс в хранилище.
    * Корневой д. *всегда сразу* сбрасывается в хранилище, его потомки -
    * по мере накопления.
    * (!) Для гарантированного сохранения докселя, который не является
    * корневым, следует вызвать метод flush().
    *
    * Родитель д. должен быть сохранён до вызова этого метода.
    * Структура сохраняемого докселя:
    *   - размерность
    *   - размер
    *   - &содержание
    *   - &родитель
    * Потомков не храним: воспользуемся замечательной возможностью CouchDB
    * строить индексы по набору данных.
    *
    * @return Ключ для доступа к этому докселю.
    *
    * @see Комм. в коде Doxel::lazySave()
    */
    template< size_t D, typename SizeT, typename ContentT >
    inline d::uid_t lazySave(
        const SizeT& size,
        const ContentT& cnt,
        const d::uid_t& extParentKey,
        const d::dant_t& dant
    ) {
        // Содержание и доксель храним как один документ:
        //   - размер д-хранилища не накладывает серьёзного ограничения
        //   - более быстрое получение / изменение докселя
        // Хранение в оперативной памяти - другое дело (см. CDManager).

        CouchDB::Object o;

        o["D"] = CouchDB::cjv( D );
        o["size"] = CouchDB::cjv( size );
        o["content"] = CouchDB::cjv( cnt.json() );

        if ( !extParentKey.empty() ) {
            o["parent"] = CouchDB::cjv( extParentKey );
        }

        /* - Накапливаем результат и сбрасываем bulk'ом в потоке. См. конструктор и ниже.
        const CouchDB::Document doc = store->createDocument( CouchDB::cjv( o ) );
        const d::uid_t uid = doc.getID();
        const d::rev_t rev = doc.getRevision();

        return d::externKey_t( uid, rev );
        */

        const d::uid_t uid = store->createBulk( o );

        return uid;
    }



    /**
    * Сбрасывает накопленные доксели в хранилище.
    *
    * @see lazySave()
    */
    inline void flush() const {
        store->flush();
    }




    /**
    * @return Количество записей в хранилище.
    */
    inline size_t count() const {
        const size_t countDoc = store->countDoc();
        const size_t countDesign = store->countDesign();
        return (countDoc - countDesign);
    }




    /**
    * @return Очистка хранилища.
    *
    * @param includeDesign true, если необходимо очистить и design-документы.
    */
    inline void clear( bool includeDesign ) {
        const std::string name = store->getName();
        conn->clearDatabase( name, false );
    }




protected:
    /**
    * Подключение к CouchDB.
    */
    std::string name;
    std::string user;
    std::string password;
    std::string url;

    std::shared_ptr< CouchDB::Connection >  conn;
    std::shared_ptr< CouchDB::Database >    store;


    /**
    * Синглетон.
    */
    static DStore* instance;

};
