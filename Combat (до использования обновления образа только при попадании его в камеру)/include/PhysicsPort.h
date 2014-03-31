#pragma once

#include "Port.h"
#include "LiveBody.h"
#include "default.h"
#include "CollisionManager.h"
#include <Newton.h>


/**
* ����, �������������� ����� ��� � �������� ������.
* �� ���� ����� ����� ����� ���� ������� ������ �����. ��������, ���
* ������������� ������� ������ �� ����������� ������� � �������������
* �������� ������ � ������� �������. �.�. ����� ���������� ���������
* ���������: ��� ������� ������� �������������� ����� ���������
* ������� ����, ��� ��������� �� ����������� �������.
*/
class PhysicsPort :
    public Port
{
public:
    /**
    * @param size ������ ����� � ��.
    * @param timestep ������� �������� ������� ������ �� ���� ����� �����.
    */
    PhysicsPort(
        Ogre::SceneManager* sm,
        const d::size3d_t& size,
        float timestep
    );



    virtual inline ~PhysicsPort() {
        std::cout << "~PhysicsPort::PhysicsPort()" << std::endl;

        // (#) � debug-������ NewtonDynamics ����������� ����������.
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
    * @return true, ���� �������� ��� ���� � �����.
    */
    inline bool has( const d::uid_t& uid ) const {
        return (search( uid ) != nullptr);
    }




    /**
    * ��������� ��������.
    * �������� � ����������� UID �����������.
    *
    * @param coord ������� ���������� ��������, �
    * @param mass ����� ��������, ��
    *
    * ����� ������, ������ set() ������������ ������ �.�., ������� �������,
    * ��� ���� �������� � �������� � ����������� ��� ���� �������, ���������
    * ����������� ������������ �������. � �����, ��������, NewtonDynamics
    * ��������� �������� ������� �� ������������� ���. � ����. �������
    * �������� ������ �����, ����� ��������� � ���. ������ �������������
    * ���������. �� �� ����� ���������� ����� ����!
    */
    void set(
        //std::shared_ptr< LiveBody >  e,
        const LiveBody& entityForInclude,
        const d::coord3d_t& coord,
        float mass
    );



    /**
    * @return ���������� � ��������.
    *
    * @todo �������� get-������ ��� �����. ��������, ����. ���, �������� � �.�..
    *
    * @alias search()
    */
    inline LiveBody* get( const d::uid_t& uid ) const {
        return search( uid );
    }



    /**
    * ������� �������� �� ����� �����. �������� �������� ��� �� �����������,
    * ��� � � ����������� ������.
    *
    * @throw �������� � �������� UID �� ����������.
    */
    inline void destroy( const d::uid_t& uid ) {
        const auto ea = search( uid );
        bodyDestructorCallback( ea->body );
    }




    /**
    * @return ������ �� ����������� ��������� ��� nullptr, ���� �� �������.
    *
    * @alias get()
    */
    inline LiveBody* search( const d::uid_t& uid ) const {
        // �������� �� ���������� ����� (�.�. �� ������ �������� ���� UID)
        // @todo optimize ������� ��������� ������ �� UID ��� �������� ������.
        auto nb = NewtonWorldGetFirstBody( nw );
        while ( nb ) {
            auto ea = search( nb );
            assert( ea && "���������� ������������ �� �������� ������. ��������� �����������." );
            assert( (ea->uid != 0) && "UID �������� �� ������ ���� ������." );
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
        assert( dataFromBody && "���������, ��� ���������� ������������ �������� ������. ��������� �����������." );
        return static_cast< LiveBody* >( dataFromBody );
    }




    /**
    * @return ���������� XZ ����.
    */
    inline d::coord3d_t coord( const NewtonBody* const nb ) const {
        float tm[16];
        NewtonBodyGetMatrix( nb, tm );
        return d::coord3d_t( tm[12], tm[13], tm[14] );
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
    static inline std::string sceneNodeName( const d::uid_t& uid ) {
        return uid + "::SceneNode::PhysicsPort";
    }



    /**
    * ������� �� NewtonDynamics.
    * ������������ ��� �������� ���.
    *
    * @see set()
    * @see http://newtondynamics.com/wiki/index.php5?title=API_Database#Callbacks
    */


    // ���������� ���
    static void bodyForceAndTorqueCallback(
        const NewtonBody* const nb,
        dFloat timestep,
        int threadIndex
    );


    // ���� ����������
    static void bodyDestructorCallback(
        const NewtonBody* nb
    );


    // ���� �������� �� ������� ����������� ����
    static void bodyLeaveWorldCallback(
        const NewtonBody* nb,
        int threadIndex
    );


    // ���� �������� ��� ���������
    static void bodyTransformCallback(
        const NewtonBody* nb,
        // @see http://www.songho.ca/opengl/gl_transform.html
        const dFloat* tm,
        int threadIndex
    );






protected:
    /**
    * ������ ����� � ��.
    */
    const d::size3d_t sizeWorld;

    /**
    * ������� �����, ������� ������ �� 1 ����� �����.
    * 
    * @see pulse()
    */
    float timestep;


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

    const std::string sceneGridName;
    Ogre::SceneNode* snGrid;

    /**
    * ��� � ���������� ��������.
    */
    NewtonWorld* nw;

};
