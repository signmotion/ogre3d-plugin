#pragma once

#include "Port.h"
#include "default.h"
#include "struct.h"
#include "CollisionManager.h"
#include <Newton.h>


/**
* ����, �������������� ����� ��� � �������� ������.
* �� ���� ����� ����� ����� ���� ������� ������ �����. ��������, ���
* ������������� ������� ������ �� ����������� ������� � �������������
* �������� ������ � ������� �������. �.�. ����� ��������� ���������
* ������ ����: ��� ������� ������� �������������� ����� ��������������
* ������� ����, ��� ��������� �� ����������� �������.
*/
template< typename C >
class PhysicsPort :
    public Port
{
public:
    /**
    * ��������� � �������������� ����������� � ��������. �������� �����������
    * � ������� ����������� ��� ������ ��������� ����� �����.
    *
    * @see InitEntity
    */
    struct EntityAddon {
        // ��� ��������
        InitEntity::type_e type;
        // UID ��������
        uid_t uid;
       
        // ���������� ����� ��������
        typedef union {
            // 2D-�����
            Ogre::Billboard* b;
            // 3D-�����
            Ogre::SceneNode* sn;
        } visual_t;
        visual_t visual;
        
        // ���������� ����� �������� � NewtonDynamics
        NewtonBody* body;
        // ����, ������� ����� ��������� � �������� �� ����. ������
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
    * @param size ������ ����� � ��. @todo �������� �����?
    */
    PhysicsPort(
        Ogre::SceneManager* sm,
        const size2d_t& size
    );



    virtual inline ~PhysicsPort() {
        // ����� ����� ���������: �� ������������� ���� ���� ����� �������
        waitFreeSetLock();

        std::cout << "~PhysicsPort::PhysicsPort()" << std::endl;

        // (#) � debug-������ NewtonDynamics ����������� ����������.
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
    * @return true, ���� �������� ��� ���� � �����.
    */
    inline bool has( const uid_t& uid ) const {
        return (search( uid ) != nullptr);
    }




    /**
    * ��������� ��������.
    * �������� � ����������� UID �����������.
    */
    void set( InitEntity* ie );



    /**
    * ������� �������� �� ����� �����. �������� �������� ��� �� �����������,
    * ��� � � ����������� ������.
    *
    * @throw �������� � �������� UID �� ����������.
    */
    inline void destroy( const uid_t& uid ) {
        const auto ea = search( uid );
        bodyDestructorCallback( ea->body );
    }




    /**
    * @return ������ �� ����������� ��������� ��� nullptr, ���� �� �������.
    */
    inline EntityAddon* search( const uid_t& uid ) const {
        // �������� �� ���������� ����� (�.�. �� ������ �������� ���� UID)
        // @todo optimize ������� ��������� ������ �� UID ��� �������� ������.
        auto body = NewtonWorldGetFirstBody( nw );
        while ( body ) {
            auto ea = search( body );
            assert( ea && "���������� ������������ �� �������� ������. ��������� �����������." );
            assert( !ea->uid.empty() && "UID �������� �� ������ ���� ������." );
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
        assert( dataFromBody && "���������, ��� ���������� ������������ �������� ������. ��������� �����������." );
        return static_cast< EntityAddon* >( dataFromBody );
    }




    /**
    * @return ���������� XZ ����.
    */
    inline coord2d_t coord( const NewtonBody* const body ) const {
        float tm[16];
        NewtonBodyGetMatrix( body, tm );
        return coord2d_t( tm[12], tm[14] );
    }




    /**
    * ������ �����.
    *
    * @param qtyCell ���������� ����� �� ������������ ����.
    */
    void grid( size_t qtyCell );





protected:
    /**
    * @return �������� �� UID. ��������, ��� ���� �����, ��������.
    */
    static inline std::string sceneNodeName( const uid_t& uid ) {
        return uid + "::SceneNode::PhysicsPort";
    }

    static inline std::string billboardSetName( const std::string& model ) {
        return model + "::BillboardSet::PhysicsPort";
    }



    /**
    * ��������� � ���� ���������� �����.
    * ���������� ������ �� ��������� ����� (������� ��� �������� �������).
    *
    * @see set()
    */
    Ogre::Billboard* addVisual( const FlatInitEntity& ie );
    Ogre::SceneNode* addVisual( const VolumeInitEntity& ie );



    /**
    * ������� �� NewtonDynamics.
    * ������������ ��� �������� ���.
    *
    * @see set()
    * @see http://newtondynamics.com/wiki/index.php5?title=API_Database#Callbacks
    */


    // ���������� ���
    static void bodyForceAndTorqueCallback(
        const NewtonBody* const body,
        dFloat timestep,
        int threadIndex
    );


    // ���� ����������
    static void bodyDestructorCallback(
        const NewtonBody* body
    );


    // ���� �������� �� ������� ����������� ����
    static void bodyLeaveWorldCallback(
        const NewtonBody* body,
        int threadIndex
    );


    // ���� �������� ��� ���������
    static void bodyTransformCallback(
        const NewtonBody* body,
        // @see http://www.songho.ca/opengl/gl_transform.html
        const dFloat* tm,
        int threadIndex
    );






protected:
    /**
    * ������ ����� � ��.
    */
    const size2d_t sizeWorld;

    /**
    * ����� �����.
    * ������������� ������������� ��� �������� ������� � ���������.
    * ���������� � �������� ������������ �� ����������� � �����������
    * �������. ��� ������������ ������� �������� �� ����������� �����������.
    *
    * mp - ����. �� Map Port.
    * imp - ����. �� Index Map Port.
    *//* - ���������.
    std::unordered_map< uid_t, EntityAddon >  mp;
    std::unordered_map< EntityAddon::visual_t*, EntityAddon* > impVisual;
    std::unordered_map< NewtonBody*, EntityAddon* > impPhysics;
    */

    /**
    * ��� � �������.
    */
    const std::string sceneEntityName;
    Ogre::SceneNode* snEntity;

    const std::string sceneBillboardName;
    Ogre::SceneNode* snBillboard;

    const std::string sceneGridName;
    Ogre::SceneNode* snGrid;

    /**
    * ��� � ���������� ��������.
    */
    NewtonWorld* nw;

    /**
    * ���������� ����� ��������.
    */
    CollisionManager*  cm;

};
