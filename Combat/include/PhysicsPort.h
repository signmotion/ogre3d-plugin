#pragma once

#include "Port.h"
#include "LiveBody.h"
#include "default.h"
#include "CollisionManager.h"
#include <Newton.h>


/**
* Порт, представляющий собой мир с законами физики.
* На базе этого порта могут быть созданы разные порты. Например, для
* моделирования законов физики на поверхности планеты и моделирования
* движения планет в звёздной системе. Т.о. можем эффективно управлять
* расчётами: для звёздной системы взаимодействия могут считаться
* намного реже, чем изменения на поверхности планеты.
*/
class PhysicsPort :
    public Port
{
public:
    /**
    * @param size Размер порта в см.
    * @param timestep Сколько мирового времени пройдёт за один пульс порта.
    */
    PhysicsPort(
        Ogre::SceneManager* sm,
        const d::size3d_t& size,
        float timestep
    );



    virtual inline ~PhysicsPort() {
        std::cout << "~PhysicsPort::PhysicsPort()" << std::endl;

        // (#) В debug-релизе NewtonDynamics выбрасывает исключение.
        NewtonDestroyAllBodies( nw );
	    NewtonDestroy( nw );

        snEntity->removeAndDestroyAllChildren();
        sm->destroySceneNode( sceneEntityName );

        snGrid->removeAndDestroyAllChildren();
        sm->destroySceneNode( sceneGridName );
    }





    /**
    * @see Port
    */
    virtual void pulse( int tact );



    /**
    * @see Port
    */
    virtual inline size_t count() {
        return (size_t)NewtonWorldGetBodyCount( nw );
    }



    /**
    * @return true, если сущность уже есть в порту.
    */
    inline bool has( const d::uid_t& uid ) const {
        return (search( uid ) != nullptr);
    }




    /**
    * Добавляет сущность.
    * Сущности с совпадающим UID обновляются.
    *
    * @param coord Мировые координаты сущности, м
    * @param mass Масса сущности, кг
    *
    * Чтобы понять, почему set() декларирован именно т.о., следует помнить,
    * что порт работает с сущностю в оптимальном для него формате, используя
    * возможности подключённых движков. И здесь, например, NewtonDynamics
    * позволяет вызывать события по синхронизации физ. и граф. образов
    * сущности только тогда, когда изменения в физ. модели действительно
    * произошли. Мы не можем пропустить такое чудо!
    */
    void set(
        //std::shared_ptr< LiveBody >  e,
        const LiveBody& entityForInclude,
        const d::coord3d_t& coord,
        float mass
    );



    /**
    * @return Информацию о сущности.
    *
    * @todo Добавить get-методы для возвр. скорости, прил. сил, вращения и т.д..
    *
    * @alias search()
    */
    inline LiveBody* get( const d::uid_t& uid ) const {
        return search( uid );
    }



    /**
    * Удаляет сущность из этого порта. Сущность исчезает как из физического,
    * так и с визуального планов.
    *
    * @throw Сущность с заданным UID не обнаружена.
    */
    inline void destroy( const d::uid_t& uid ) {
        const auto ea = search( uid );
        bodyDestructorCallback( ea->body );
    }




    /**
    * @return Ссылка на связывающую структуру или nullptr, если не найдена.
    *
    * @alias get()
    */
    inline LiveBody* search( const d::uid_t& uid ) const {
        // Проходим по физическим телам (т.к. не храним отдельно ключ UID)
        // @todo optimize Создать отдельный индекс по UID для быстрого поиска.
        auto nb = NewtonWorldGetFirstBody( nw );
        while ( nb ) {
            auto ea = search( nb );
            assert( ea && "Физическая составляющая не содержит данных. Нарушение целостности." );
            assert( (ea->uid != 0) && "UID сущности не должен быть пустым." );
            if (ea->uid == uid) {
                return ea;
            }
            nb = NewtonWorldGetNextBody( nw, nb );
        };
        return nullptr;
    }


    static inline LiveBody* search( const NewtonBody* const nb ) {
        assert( nb );
        void* dataFromBody = NewtonBodyGetUserData( nb );
        assert( dataFromBody && "Ожидалось, что физическая составляющая содержит данные. Нарушение целостности." );
        return static_cast< LiveBody* >( dataFromBody );
    }




    /**
    * @return Координаты XZ тела.
    */
    inline d::coord3d_t coord( const NewtonBody* const nb ) const {
        float tm[16];
        NewtonBodyGetMatrix( nb, tm );
        return d::coord3d_t( tm[12], tm[13], tm[14] );
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
    static inline std::string sceneNodeName( const d::uid_t& uid ) {
        return uid + "::SceneNode::PhysicsPort";
    }



    /**
    * События от NewtonDynamics.
    * Подключаются при создании тел.
    *
    * @see set()
    * @see http://newtondynamics.com/wiki/index.php5?title=API_Database#Callbacks
    */


    // Добавление сил
    static void bodyForceAndTorqueCallback(
        const NewtonBody* const nb,
        dFloat timestep,
        int threadIndex
    );


    // Тело уничтожено
    static void bodyDestructorCallback(
        const NewtonBody* nb
    );


    // Тело вылетело за границы физического мира
    static void bodyLeaveWorldCallback(
        const NewtonBody* nb,
        int threadIndex
    );


    // Тело изменило своё положение
    static void bodyTransformCallback(
        const NewtonBody* nb,
        // @see http://www.songho.ca/opengl/gl_transform.html
        const dFloat* tm,
        int threadIndex
    );






protected:
    /**
    * Размер порта в см.
    */
    const d::size3d_t sizeWorld;

    /**
    * Мировое время, которое пройдёт за 1 пульс порта.
    * 
    * @see pulse()
    */
    float timestep;


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

    const std::string sceneGridName;
    Ogre::SceneNode* snGrid;

    /**
    * Мир в физических объектах.
    */
    NewtonWorld* nw;

};
