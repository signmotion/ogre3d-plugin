#pragma once

#include "Port.h"
#include "default.h"
#include "struct.h"
#include "CollisionManager.h"
#include <Newton.h>


/**
* Порт, представляющий собой мир с законами физики.
* На базе этого порта могут быть созданы разные порты. Например, для
* моделирования законов физики на поверхности планеты и моделирования
* движения планет в звёздной системе. Т.о. можем управлять расчётами
* модели мира: для звёздной системы взаимодействия могут рассчитываться
* намного реже, чем изменения на поверхности планеты.
*/
template< typename C >
class PhysicsPort :
    public Port
{
public:
    /**
    * Структура с дополнительной информацией о сущности. Сущность сохраняется
    * в формате оптимальном для работы конкретно этого порта.
    *
    * @see InitEntity
    */
    struct EntityAddon {
        // Тип сущности
        InitEntity::type_e type;
        // UID сущности
        uid_t uid;
       
        // Визуальный образ сущности
        typedef union {
            // 2D-образ
            Ogre::Billboard* b;
            // 3D-образ
            Ogre::SceneNode* sn;
        } visual_t;
        visual_t visual;
        
        // Физический образ сущности в NewtonDynamics
        NewtonBody* body;
        // Сила, которая будет приложена к сущности на след. пульсе
        Ogre::Vector3 force;

        inline EntityAddon( InitEntity::type_e type, const uid_t& uid ) :
            type( type ),
            uid( uid ),
            body( nullptr ),
            force( Ogre::Vector3::ZERO )
        {
            visual.b = nullptr;
            visual.sn = nullptr;
        }
    };






public:
    /**
    * @param size Размер порта в см. @todo Добавить время?
    */
    PhysicsPort(
        Ogre::SceneManager* sm,
        const size2d_t& size
    );



    virtual inline ~PhysicsPort() {
        // Будем очень аккуратны: не ликвидируемся пока порт занят работой
        waitFreeSetLock();

        std::cout << "~PhysicsPort::PhysicsPort()" << std::endl;

        // (#) В debug-релизе NewtonDynamics выбрасывает исключение.
        NewtonDestroyAllBodies( nw );
	    NewtonDestroy( nw );

        snEntity->removeAndDestroyAllChildren();
        sm->destroySceneNode( sceneEntityName );

        snGrid->removeAndDestroyAllChildren();
        sm->destroySceneNode( sceneGridName );

        sm->destroyAllBillboardSets();
        snBillboard->removeAndDestroyAllChildren();
        sm->destroySceneNode( sceneBillboardName );

        unlock();
    }





    /**
    * @see Port
    */
    virtual void pulse( int tact, float timestep );



    /**
    * @see Port
    */
    virtual inline size_t count() {
        return (size_t)NewtonWorldGetBodyCount( nw );
    }



    /**
    * @return true, если сущность уже есть в порту.
    */
    inline bool has( const uid_t& uid ) const {
        return (search( uid ) != nullptr);
    }




    /**
    * Добавляет сущность.
    * Сущности с совпадающим UID обновляются.
    */
    void set( InitEntity* ie );



    /**
    * Удаляет сущность из этого порта. Сущность исчезает как из физического,
    * так и с визуального планов.
    *
    * @throw Сущность с заданным UID не обнаружена.
    */
    inline void destroy( const uid_t& uid ) {
        const auto ea = search( uid );
        bodyDestructorCallback( ea->body );
    }




    /**
    * @return Ссылка на связывающую структуру или nullptr, если не найдена.
    */
    inline EntityAddon* search( const uid_t& uid ) const {
        // Проходим по физическим телам (т.к. не храним отдельно ключ UID)
        // @todo optimize Создать отдельный индекс по UID для быстрого поиска.
        auto body = NewtonWorldGetFirstBody( nw );
        while ( body ) {
            auto ea = search( body );
            assert( ea && "Физическая составляющая не содержит данных. Нарушение целостности." );
            assert( !ea->uid.empty() && "UID сущности не должен быть пустым." );
            if (ea->uid == uid) {
                return ea;
            }
            body = NewtonWorldGetNextBody( nw, body );
        };
        return nullptr;
    }


    static inline EntityAddon* search( const NewtonBody* const body ) {
        assert( body );
        void* dataFromBody = NewtonBodyGetUserData( body );
        assert( dataFromBody && "Ожидалось, что физическая составляющая содержит данные. Нарушение целостности." );
        return static_cast< EntityAddon* >( dataFromBody );
    }




    /**
    * @return Координаты XZ тела.
    */
    inline coord2d_t coord( const NewtonBody* const body ) const {
        float tm[16];
        NewtonBodyGetMatrix( body, tm );
        return coord2d_t( tm[12], tm[14] );
    }




    /**
    * Рисует сетку.
    *
    * @param qtyCell Количество ячеек по координатным осям.
    */
    void grid( size_t qtyCell );





protected:
    /**
    * @return Название по UID. Например, для узла сцены, билборда.
    */
    static inline std::string sceneNodeName( const uid_t& uid ) {
        return uid + "::SceneNode::PhysicsPort";
    }

    static inline std::string billboardSetName( const std::string& model ) {
        return model + "::BillboardSet::PhysicsPort";
    }



    /**
    * Добавляет в порт визуальный образ.
    * Возвращает ссылку на созданный образ (полезно для быстрого доступа).
    *
    * @see set()
    */
    Ogre::Billboard* addVisual( const FlatInitEntity& ie );
    Ogre::SceneNode* addVisual( const VolumeInitEntity& ie );



    /**
    * События от NewtonDynamics.
    * Подключаются при создании тел.
    *
    * @see set()
    * @see http://newtondynamics.com/wiki/index.php5?title=API_Database#Callbacks
    */


    // Добавление сил
    static void bodyForceAndTorqueCallback(
        const NewtonBody* const body,
        dFloat timestep,
        int threadIndex
    );


    // Тело уничтожено
    static void bodyDestructorCallback(
        const NewtonBody* body
    );


    // Тело вылетело за границы физического мира
    static void bodyLeaveWorldCallback(
        const NewtonBody* body,
        int threadIndex
    );


    // Тело изменило своё положение
    static void bodyTransformCallback(
        const NewtonBody* body,
        // @see http://www.songho.ca/opengl/gl_transform.html
        const dFloat* tm,
        int threadIndex
    );






protected:
    /**
    * Размер порта в см.
    */
    const size2d_t sizeWorld;

    /**
    * Карта порта.
    * Предназначена исключительно для быстрого доступа к сущностям.
    * Информация о сущности распределена по визуальному и физическому
    * образах. Это минимизирует затраты ресурсов на поддержание целостности.
    *
    * mp - сокр. от Map Port.
    * imp - сокр. от Index Map Port.
    *//* - Обходимся.
    std::unordered_map< uid_t, EntityAddon >  mp;
    std::unordered_map< EntityAddon::visual_t*, EntityAddon* > impVisual;
    std::unordered_map< NewtonBody*, EntityAddon* > impPhysics;
    */

    /**
    * Мир в образах.
    */
    const std::string sceneEntityName;
    Ogre::SceneNode* snEntity;

    const std::string sceneBillboardName;
    Ogre::SceneNode* snBillboard;

    const std::string sceneGridName;
    Ogre::SceneNode* snGrid;

    /**
    * Мир в физических объектах.
    */
    NewtonWorld* nw;

    /**
    * Физические формы объектов.
    */
    CollisionManager*  cm;

};
