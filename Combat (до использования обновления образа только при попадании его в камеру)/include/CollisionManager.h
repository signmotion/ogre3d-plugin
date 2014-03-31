#pragma once

#include <memory>
#include <unordered_map>
#include <Newton.h>
#include <boost/functional/hash.hpp>


/**
* ��������� ��������� ���������� ���� ���������. �� ������ ��������� ����.
* �����, �.�. NewtonDymnamics ��������� �������� � ������� ���������� ����
* ��� ������ ��������. �����������, ��.
*/
class CollisionManager {
public:
    enum shape_e {
        SHAPE_BOX = 101,
        SHAPE_CAPSULE,
        SHAPE_SPHERE
    };


    /**
    * ����������� ���. ��. �������� ��� �������� ����������.
    */
    union shape_u {
        NewtonCollisionInfoRecord::NewtonBoxParam box;
        NewtonCollisionInfoRecord::NewtonCapsuleParam capsule;
        NewtonCollisionInfoRecord::NewtonSphereParam sphere;
    };




protected:
    inline CollisionManager() {
    }


public:
    static inline CollisionManager* singleton() {
        if ( instance ) {
            return instance;
        }
        instance = new CollisionManager();
        return instance;
    }



    virtual inline ~CollisionManager() {
        std::cout << "~CollisionManager()" << std::endl;
    }



    /**
    * @return ���������� �������� ����.
    */
    inline size_t count() const {
        return nc.size();
    }




    /**
    * ������������ ������, �.�. ��� ������� �������� � std::shared_ptr()
    * ����������� ����������� ����������.
    *
    * (!) ��� ������ ������� ����� ������� ���� ���������� 'seed'.
    */
    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonBoxParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        const auto kf = search( SHAPE_BOX, p.m_x, p.m_y, p.m_z );
        if ( kf.second) {
            return kf.second;
        }
        nc[ kf.first ] = NewtonCreateBox(
            nw,  p.m_x, p.m_y, p.m_z,  shapeID, offsetMatrix
        );
        return nc[ kf.first ];
    }


    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonCapsuleParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        // ����������� ������ 'm_r0', ��. NewtonCreateCapsule()
        assert( (p.m_r0 == p.m_r1) && "������� �������� � ����� ��������, ������� �� �������, ����� ����� ����� ��������." );
        assert( (p.m_r0 > 0.0f) && "� ������� ������ ���� ������." );
        assert( (p.m_height > 0.0f) && "������� ��� ������, ���?" );
        const auto kf = search( SHAPE_CAPSULE, p.m_r0, p.m_r0, p.m_height );
        if ( kf.second) {
            return kf.second;
        }
        nc[ kf.first ] = NewtonCreateCapsule(
            nw,  p.m_r0, p.m_height,  shapeID, offsetMatrix
        );
        return nc[ kf.first ];
    }


    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonSphereParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        assert( ((p.m_r0 > 0.0f) && (p.m_r1 > 0.0f) && (p.m_r2 > 0.0f)) && "�����. ����� ��� ��� �������.." );
        const auto kf = search( SHAPE_SPHERE, p.m_r0, p.m_r1, p.m_r2 );
        if ( kf.second) {
            return kf.second;
        }
        nc[ kf.first ] = NewtonCreateSphere(
            nw,  p.m_r0, p.m_r1, p.m_r2,  shapeID, offsetMatrix
        );
        return nc[ kf.first ];
    }







private:
    /**
    * @return ����������� ���� � ��������� �����. ���� ����� ��� - nullptr.
    *
    * (!) ��� ������ ������� ����� ������ ���������� ���� ���������� 'seed'.
    */
    inline std::pair< size_t,  NewtonCollision* >  search(
        shape_e shape, dFloat p1, dFloat p2, dFloat p3
    ) {
        // ��������� ������������ ����� ������� ����, ��������� ��
        assert(p1 > 0.0f);
        assert(p2 > 0.0f);
        assert(p3 > 0.0f);

        size_t key = (size_t)shape;
        boost::hash_combine( key, p1 );
        boost::hash_combine( key, p2 );
        boost::hash_combine( key, p3 );
        const auto sf = nc.find( key );
        const auto form = (sf == nc.cend()) ? nullptr : sf->second;
        return std::make_pair( key, form ) ;
    }




protected:
    /*
    * �����, ��� �������� ������ �����.
    * ��� ���������� ���-����� �������� �
    * http://www.boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    std::unordered_map< size_t,  NewtonCollision* >  nc;



    static CollisionManager* instance;

};

