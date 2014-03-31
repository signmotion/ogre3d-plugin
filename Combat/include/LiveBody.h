#pragma once

#include "default.h"
#include "CollisionManager.h"
#include "ManualObjectLB.h"
#include <Newton.h>
#include <Ogre.h>
#include <boost/array.hpp>



/**
* ���� - ������� ����� ��� ���� �������.
*
* ����� ������������� ��� ������ ������ PhysicsPort.
*/
/* - ���-�� ��������� ����� � �������� ������� � �������� ���������:
     ������ ��������� ���������� � ������ � PhysicsPort.
template< size_t N >
*/
class LiveBody {
public:
    /**
    * ����������� ��������� ��������� ����� ����.
    * ���������� �������� � ��.
    */
    typedef d::coord3d_t movablePoint_t;
    typedef std::vector< movablePoint_t >  allMovablePoint_t;


public:
    /**
    * UID ��������.
    */
    const d::uid_t uid;

    /**
    * ��������� ��������� ��������� ����� ����, � ��.
    * ��������� ��� ������� (��� ������������). ������������ 'currentMP'.
    */
    const allMovablePoint_t startMP;

    /**
    * ������� ��������� ��������� ����� ����.
    * ������� ������������ ������� �. � ����� ������. �� ������ �-�����
    * � ������ ������������� ����������� ���������, ���� ����� ���������
    * �������. ������� ��������� ����� �� ����������, � �������� � ��������
    * ������� ������������.
    */
    allMovablePoint_t currentMP;

    /**
    * �������� ��������� ��������� ����� ����.
    * � ����� ��������� ����� ��������� ������.
    */
    allMovablePoint_t desireMP;

    /**
    * ���������� ����� ��������.
    */
    NewtonBody* body;

    /**
    * ���������� ����� ��������.
    */
    Ogre::SceneNode* sceneImage;
    std::shared_ptr< ManualObjectLB >  moImage;

    /**
    * �������� ����� �������� ����� ����, ��� ������ ������� ���������� ������
    * ������ � ���������� ������ ��������.
    */
    Ogre::SceneManager* sm;

    /**
    * ����, ����������� � ������ ������ �� ����.
    * �������������� ��� ��������� ��������� ���� � ������������.
    * �������� � �������, ������� NewtonDynamics: ( x, y, z, K ).
    */
    float force[4];


    /**
    * ��������� ��������� ����.
    * ������ ��������� - ������. ��������, ��� ���������� "��������� �������"
    * � �������� �����������? ��� "����� �� �����", ����� �� ���� ���������
    * ���� ���������?
    *
    * @see position()
    */
    // @todo ? ����������� ����� ������ ���������? http://www.boost.org/doc/libs/1_48_0/libs/statechart/doc/tutorial.html
    /*
    struct Position {
        typedef size_t state;

        // ��������� ������
        state free : 1;

        // ������ ��� �����
        state forwardStep : 1;
        // ������ ��� �����
        state backwardStep : 1;
        // ������ ��� � �������
        state sidewayStep : 1;

        // �����
        state down : 1;
        // ����� �� �����
        state tergumDown : 1;
        // ����� �� ������
        state stomachDown : 1;
        // ����� �� ����
        state sideDown : 1;

        // ����� �� ���������
        state squat : 1;
    };
    */




public:
    inline LiveBody( const d::uid_t& uid, const allMovablePoint_t& startMP ) :
        uid ( uid ),
        startMP( startMP ),
        currentMP( startMP ),
        desireMP( startMP ),
        body( nullptr ),
        sceneImage( nullptr ),
        moImage( nullptr )
    {
        std::cout << "LiveBody()" << std::endl;

        // @todo optimize? �� ���������������� ��������� �������
        //       (��� ��������): ����� ����������� �������.

        force[0] = force[1] = force[2] = 0.0f;
        force[3] = 1.0f;

        // ������� ����� ������ �������������: ���. ��� ������������ UID
        ++LiveBody::count;
    }



    inline virtual ~LiveBody() {
        std::cout << "~LiveBody()" << std::endl;
    }



    /**
    * @return ���������� ��������� ����� � ��������.
    */
    inline size_t countMovablePoint() const {
        return currentMP.size();
    }



    /**
    * @return ���������� � ������� ��������� ��������� ����� ����.
    *
    * @param i ����� �-�����.
    *
    * (!) �� ���������� *���� � �������* ������������.
    */
    inline const movablePoint_t& operator[]( size_t i ) const {
        return currentMP[i];
    }



    /**
    * ���������� ������� ��������� �-�����.
    */
    inline void setCurrent( size_t i, const d::coord3d_t& coord ) {
        currentMP[i] = coord;
    }


    inline void setCurrent( const allMovablePoint_t& mp ) {
        assert( (currentMP.size() == mp.size()) && "���������� ����� �� �����. ��������." );
        currentMP = mp;
    }



    /**
    * ���������� ��� �������� �-����� ���� ����� �������� ���������.
    */
    inline void setDesire( size_t i, const d::coord3d_t& coord ) {
        desireMP[i] = coord;
    }


    inline void setDesire( const allMovablePoint_t& mp ) {
        assert( (currentMP.size() == mp.size()) && "���������� ����� �� �����. ��������." );
        desireMP = mp;
    }



    /**
    * �� ��, ��� setDesire(), �� ��������� ��������������
    * ������������ ��������.
    */
    inline void setShiftDesire( size_t i, const d::coord3d_t& coord ) {
        desireMP[i] = currentMP[i] + coord;
    }


    inline void setShiftDesire( const allMovablePoint_t& mp ) {
        assert( (currentMP.size() == mp.size()) && "���������� ����� �� �����. ��������." );
        for (size_t i = 0; i < mp.size(); ++i) {
            desireMP[i] = currentMP[i] + mp[i];
        }
    }




    /**
    * @return ����� ��� ���������� �������� � ���������� ���.
    */
    virtual NewtonCollision* collision( NewtonWorld* nw ) const = 0;



    /**
    * @return ����� ��� ������������ ��������.
    *         ����������� �� ����� 'sn'.
    */
    inline Ogre::SceneNode* visual( Ogre::SceneManager* sm, Ogre::SceneNode* sn ) {
        assert( sm );
        assert( sn );

        // ���������� ���
        if ( this->moImage ) {
            assert( this->sm );
            assert( this->sceneImage );
            assert( (this->sm == sm) && "������� ������������ ������ �������� ����� ��� ������������ ����� �������." );
            assert( (this->sceneImage == sn) && "������� ������������ ������ ����� ��� ������������ ����� �������." );
            return this->sceneImage;
        }

        // ������ ����� ������������������� ��������� �����...
        this->sm = sm;

        // ...� ���������� ���� �� ��������� ������
        this->moImage = manualObject( sm );
        auto mo = static_cast< Ogre::ManualObject* >( moImage.get() );
        sn->attachObject( mo );
        this->sceneImage = sn;

        return this->sceneImage;
    }




    /**
    * @return ����� ��������. �� ���� � std::shared_ptr, �.�. ���� �����
    *         "������" �������������� � �� ��������.
    */
    virtual LiveBody* clone() const = 0;




    /**
    * @return ��� ������ ������� ���������� (������� �������) ���������� ��
    *         ��������. �������� ����� � ��������� [0.0; 1.0] ==
    *         == ["����� ���������"; "����� ����������"].
    *
    * @param precissionGrid �������� �����: ��� ������ ����� ����������
    *        ����������, ����� ��� ��������� ��� "����������� � ��������".
    */
    virtual inline float distinguishCoord( float precissionGrid ) {
        /* - @todo ...
        // ������� ���������� ������ ����� �� �������� ���������.
        // �������� ���������� ��� ������ ����� � ������ �������
        // ��������������.
        float s = 0.0f;
        for (size_t i = 0; i < MOVABLE_POINT_N * 3; i += 3) {
            // �������
            const float cx = currentCoord[i + 0];
            const float cy = currentCoord[i + 1];
            const float cz = currentCoord[i + 2];
            // ��������
            const float ex = estimateCoord[i + 0];
            const float ey = estimateCoord[i + 1];
            const float ez = estimateCoord[i + 2];
            // ������� ����������
            const float sx = ex - cx;
            const float sy = ey - cy;
            const float sz = ez - cz;
            //s = std::max( sx, std::max( sy, sz ) );
            // @todo optimize ���������� �� �����.
            s += std::sqrt( sx * sx + sy * sy + sz * sz );
        }
        s /= (float)MOVABLE_POINT_N;

        return (s > precissionGrid) ? 1.0 : (s / precissionGrid);
        */

        return 0.0f;
    }



    /**
    * ������ ����� ����� ������ ��������� �������� ��������� ����� ���� �
    * ��������� 'desireMP'.
    */
    virtual inline void desirePulse() {
        /* - @todo ...
        // ������� ���������� ������ ����� �� �������� ��������� �
        // �������� ����� � ���� �����������
        const float k = 1.0f / 100.0f;
        for (size_t i = 0; i < MOVABLE_POINT_N * 3; i += 3) {
            // �������
            const float cx = currentCoord[i + 0];
            const float cy = currentCoord[i + 1];
            const float cz = currentCoord[i + 2];
            // ��������
            const float ex = estimateCoord[i + 0];
            const float ey = estimateCoord[i + 1];
            const float ez = estimateCoord[i + 2];
            // ������ ����������
            const float sx = (ex - cx) * k;
            const float sy = (ey - cy) * k;
            const float sz = (ez - cz) * k;
            // ����� ����������
            currentCoord[i + 0] += sx;
            currentCoord[i + 1] += sy;
            currentCoord[i + 2] += sz;
        }

        // �������������� ���������� �����
        //synchronizeVisual();
        */
    }






protected:
    /**
    * ������ ManualObject ��� ����� ����.
    */
    virtual std::shared_ptr< ManualObjectLB >  manualObject( Ogre::SceneManager* sm ) = 0;





private:
    /**
    * ������������� ����� ���������� ��� �-����� ���� �������� ��� ����������
    * ������������.
    *
    * @param oc ���� ����������.
    * @param nc ����� ����������.
    *
    * @see set()
    */
    inline void updateWithConstraint(
        const std::vector< float* >&  oc,
        const std::vector< Ogre::Vector3 >&  nc
    ) const {
        // ���������, ����� �������� ���������� ���� ��������� � ��������
        // ��� ���������� �������������

        // ������� �����������

        // @todo ...

    }






protected:
    /**
    * �.�. ManualObject ����� ���������� ��������� ������ � ����� �����,
    * ������ ������ ��������� ����� � ��������������� � ���� ������ �
    * ManualObject, ����� ����������� ��� �������, ��������� �� �-�����
    * �������.
    */
    std::unordered_map<
        // ����� ��������� ����� � �������
        size_t,
        // � ������ ��������� (���������) � 'moImage' ������� ��� �����
        std::unordered_set< size_t >
    > associatePoint;


    /**
    * ���������� ��������� ���. ����� �������������� ��� ������ ���
    * ������������ UID ������ ����.
    */
    static size_t count;

};








/**
* ���� ��������.
* ������ ����� - �������. ����� ������ - ����� ����������, ����� ��������� �
* ��������� ������� ��� ������� � ���������� � ����������� ������.
* ����� ����� ������ � ��.

 9       /---\
 8      |  0  |
 7       \---/
 6         |
 5       --1--
 4      /  |  \
 3     /   |   \
 2   7/    |    \8
 1   |     |     |
 0   |    -2-    |
-1   3   /   \   4
-2      /     \
-3     /       \
-4   9/         \10
-5   |           |
-6   |           |
-7   |           |
-8   |           |
-9   5           6
.
ZX. -6543210123456+
*/
class HumanLB: public LiveBody {
public:
    inline HumanLB(
        const d::uid_t& uid = (d::uid_t)(LiveBody::count + 1)
    ) :
        LiveBody(
            uid,
            boost::assign::list_of
                (d::coord3d_t( 0.0f, 0.0f, 80.0f * 2.0f ))
                (d::coord3d_t( 0.0f, 0.0f, 50.0f * 2.0f ))
                (d::coord3d_t( 0.0f, 0.0f, 0.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, -10.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, -10.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, -90.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, -90.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, 20.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, 20.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, -40.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, -40.0f * 2.0f ))
        )
    {
        // ������������ ������
        /* - �������� � ����������� ����.
        const auto S = d::coord3d_t( 1.0f, 1.0f, 2.0f );
        startCoord = boost::assign::list_of
            (S * d::coord3d_t( 0.0f, 0.0f, 80.0f ))
            (S * d::coord3d_t( 0.0f, 0.0f, 50.0f ))
            (S * d::coord3d_t( 0.0f, 0.0f, 0.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, -10.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, -10.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, -90.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, -90.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, 20.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, 20.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, -40.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, -40.0f ))
        ;
        */
    }



    inline virtual ~HumanLB() {
    }




    /**
    * @see LiveBody
    */
    virtual inline NewtonCollision* collision( NewtonWorld* nw ) const {
        NewtonCollisionInfoRecord::NewtonCapsuleParam capsule;
        capsule.m_r0 = capsule.m_r1 = 120.0f / 2.0f;
        capsule.m_height = 180.0f;
        return CollisionManager::singleton()->collision( nw, capsule );
    }


    
    /**
    * @see LiveBody
    */
    virtual inline std::shared_ptr< ManualObjectLB >  manualObject( Ogre::SceneManager* sm ) {

        assert( sm );

        // ����� ���������� ���: ������ ������ ������������� ���� ���, ����
        // ���� ����� ������, ������ ������� ������������ ������� ������ �����
        // ������ �� ������� ���� ��� ���������, ��� UID ���������� � ���
        // ������� - ���������.
        const std::string imageName =
            Ogre::StringConverter::toString( uid ) + "::MO::HumanLB";

        // ��. ����� � ����. � ������
        //auto mo = sm->createManualObject( imageName );
        auto mo = std::shared_ptr< ManualObjectLB >( new ManualObjectLB( imageName ) );
        mo->begin( "combat/white", Ogre::RenderOperation::OT_LINE_LIST );

        const auto& ptrCurrentCoord = currentMP;
        auto& ptrAssociatePoint = associatePoint;
        size_t ind = 0;
        const auto addVertex = [ &mo, ptrCurrentCoord, &ind, &ptrAssociatePoint ] ( size_t ai, size_t bi ) {
            const d::coord3d_t a = ptrCurrentCoord.at( ai );
            mo->position( a.get<0>(), a.get<1>(), a.get<2>() );
            mo->index( ind );
            auto itr = ptrAssociatePoint.find( ai );
            if (itr == ptrAssociatePoint.end()) {
                itr = ptrAssociatePoint.insert( std::make_pair( ai,  std::unordered_set< size_t >() ) ).first;
            }
            itr->second.insert( ind );
            ++ind;

            const d::coord3d_t b = ptrCurrentCoord.at( bi );
            mo->position( b.get<0>(), b.get<1>(), b.get<2>() );
            mo->index( ind );
            itr = ptrAssociatePoint.find( bi );
            if (itr == ptrAssociatePoint.end()) {
                itr = ptrAssociatePoint.insert( std::make_pair( bi,  std::unordered_set< size_t >() ) ).first;
            }
            itr->second.insert( ind );
            ++ind;
        };

        addVertex( 0, 1 );
        addVertex( 1, 2 );
        addVertex( 1, 7 );
        addVertex( 1, 8 );
        addVertex( 7, 3 );
        addVertex( 8, 4 );
        addVertex( 2, 9 );
        addVertex( 2, 10 );
        addVertex( 9, 5 );
        addVertex( 10, 6 );

        mo->end();

        /* - �� ��������� � ���, ����� ����� ���� �������������� ��������� �������.
        auto mesh = mo->convertToMesh( imageName );
        auto e = sm->createEntity( mesh->getName() );
        */
        mo->setDebugDisplayEnabled( true );
        mo->setBoundingBox( Ogre::AxisAlignedBox(
            -60.0f, -1.0f, -160.0f,
             60.0f,  1.0f,  160.0f
        ) );

        return mo;
    }




    /**
    * @see LiveBody
    */
    virtual inline LiveBody* clone() const {
        return new HumanLB( this->uid );
    }




    /**
    * @see LiveBody
    */
    virtual inline void synchronizeVisual() {
        // 
        /* - @todo ������������� �����.
        for (size_t i = 0; i < MOVABLE_POINT_N * 3; i += 3) {
            // �������
            const float cx = currentCoord[i + 0];
            const float cy = currentCoord[i + 1];
            const float cz = currentCoord[i + 2];
            // ������������� �����
        }
        */
        // ���������� �����
        // @todo ...

    }


};

