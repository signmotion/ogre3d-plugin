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
        return m.size();
    }




    /**
    * ������������ ������, �.�. ��� ������� �������� � std::shared_ptr()
    * ����������� ����������� ����������.
    *
    * (!) ��� ������ ������� ����� ������� ���� ���������� 'seed'.
    */
    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonSphereParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        const auto kf = search( 100, p.m_r0, p.m_r1, p.m_r2 );
        if ( kf.second) {
            return kf.second;
        }
        m[ kf.first ] = NewtonCreateSphere(
            nw,  p.m_r0, p.m_r1, p.m_r2,  shapeID, offsetMatrix
        );
        return m[ kf.first ];
    }



    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonBoxParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        const auto kf = search( 101, p.m_x, p.m_y, p.m_z );
        if ( kf.second) {
            return kf.second;
        }
        m[ kf.first ] = NewtonCreateBox(
            nw,  p.m_x, p.m_y, p.m_z,  shapeID, offsetMatrix
        );
        return m[ kf.first ];
    }




private:
    /**
    * @return ����������� ���� � ��������� �����. ���� ����� ��� - nullptr.
    *
    * (!) ��� ������ ������� ����� ������ ���������� ���� ���������� 'seed'.
    */
    inline std::pair< size_t,  NewtonCollision* >  search(
        size_t seed, dFloat p1, dFloat p2, dFloat p3
    ) {
        // ��������� ������������ ����� ������� ����, ��������� ��
        assert(p1 > 0.0f);
        assert(p2 > 0.0f);
        assert(p3 > 0.0f);

        size_t key = seed;
        boost::hash_combine( key, p1 );
        boost::hash_combine( key, p2 );
        boost::hash_combine( key, p3 );
        const auto sf = m.find( key );
        const auto form = (sf == m.cend()) ? nullptr : sf->second;
        return std::make_pair( key, form ) ;
    }




protected:
    /*
    * �����, ��� �������� ������ �����.
    * ��� ���������� ���-����� �������� �
    * http://www.boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    std::unordered_map< size_t,  NewtonCollision* >  m;




    /**
    * ���������.
    */
    static CollisionManager* instance;

};

