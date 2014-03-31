#include "MapDoxel.h"
#include <boost/thread.hpp>


MapDoxel::MapDoxel(
    const std::string& source,
    const d::size3d_t& worldSizeInKm,
    Ogre::SceneManager* sm
) :
    s( source ),
    sm( sm ),
    sn( nullptr )
{
    // ��������� ������� �� ����� �����
    root = Doxel3D::valueOf(
        nullptr,
        source,
        // ��������� � �����
        worldSizeInKm * 1000.0f
    );


    // �������������� ������� ������ ��� ������������ ��������

    sn = sm->getRootSceneNode()->createChildSceneNode();

    // ������� ������� ��� ��������.
    // �������� - ��, ��� �������� ���������, �� ����� ������������ �
    // ������ ������ �����.
    // ������ ��������� ��� ������������ �������: �� �������� �� ������
    // ������� ���� (��� Z - ���������� �����, X - ������, Y - �����).

    const auto sn_ = sn;
    const auto createBillboardSet = [ sm, sn_ ] (
        const std::string& material,
        const Ogre::Vector3& upVector,
        const Ogre::Vector3& upDirection,
        const Ogre::BillboardOrigin origin
    ) -> Ogre::BillboardSet* {
        Ogre::BillboardSet* bs = sm->createBillboardSet( 500 );
        bs->setAutoextend( true );
        bs->setMaterialName( material );
        bs->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
        bs->setCommonUpVector( upVector );
        bs->setCommonDirection( upDirection );
        bs->setBillboardsInWorldSpace( true );
        bs->setBillboardOrigin( origin );
        bs->setSortingEnabled( true );
        //bs->setDefaultDimensions( 100.0f, 100.0f ); - ��������������� ��� ���������. ��. draw().
        sn_->attachObject( bs );
        return bs;
    };


    /* @copy MapDoxel::dbs_t
    *   0 - ����
    *   1 - ������
    *   2 - �����
    *   3 - �����
    *   4 - ������
    *   5 - �������
    */
    dBS[0] = createBillboardSet( "accuratemap/bs/doxel",  Ogre::Vector3::UNIT_Y,  Ogre::Vector3::UNIT_Z,  Ogre::BBO_CENTER );
    dBS[1] = createBillboardSet( "accuratemap/bs/doxel",  Ogre::Vector3::UNIT_Z,  Ogre::Vector3::UNIT_X,  Ogre::BBO_CENTER );
    dBS[2] = createBillboardSet( "accuratemap/bs/doxel",  Ogre::Vector3::UNIT_Y, -Ogre::Vector3::UNIT_Z,  Ogre::BBO_CENTER );
    dBS[3] = createBillboardSet( "accuratemap/bs/doxel",  Ogre::Vector3::UNIT_Z, -Ogre::Vector3::UNIT_X,  Ogre::BBO_CENTER );
    dBS[4] = createBillboardSet( "accuratemap/bs/doxel",  Ogre::Vector3::UNIT_Z,  Ogre::Vector3::UNIT_Y,  Ogre::BBO_CENTER );
    dBS[5] = createBillboardSet( "accuratemap/bs/doxel",  Ogre::Vector3::UNIT_Z, -Ogre::Vector3::UNIT_Y,  Ogre::BBO_CENTER );

    // �������� ��������
    testBS = createBillboardSet( "accuratemap/bs/test-doxel",  Ogre::Vector3::UNIT_Y,  Ogre::Vector3::UNIT_Z,  Ogre::BBO_CENTER );
    // ����� ��� �������� ������ (����� ������ ������ ���������)
    /* - ��������� ��������.
    const auto fn = [] ( Ogre::BillboardSet* testBS ) {
        while ( true ) {
            Sleep( 2000 );
            if ( !testBS ) {
                break;
            }
            std::cout << "clear test billboard" << std::endl;
            try {
                testBS->clear();
            } catch ( ... ) {
                // ��� ������ �� ����� ��������
            }
        }
    };
    boost::thread( fn, testBS );
    */
}





MapDoxel::~MapDoxel() {
    for (auto itr = dBS.begin(); itr != dBS.end(); ++itr) {
        if ( *itr ) {
            sm->destroyBillboardSet( *itr );
        }
    }
    sn->removeAndDestroyAllChildren();
    sm->destroySceneNode( sn );
}





#if 0
// - �� ������������.
void MapDoxel::draw(
    const d::box3d_t& box
) const {

    // ����� ��������� � ���� ���������� ��������

    /*
    // @test �������
    draw( *root,  d::coord3d_t( d->size(), 0.0f, 0.0f ) );
    */

    // @test ������� ������� �� ��������� ������
    drawChild( *root,  d::coord3d_t( 0.0f, 0.0f, 0.0f ),  0 );



    // @todo ...

}
#endif






void MapDoxel::draw(
    const Doxel3D& doxel,
    const d::coord3d_t& coord,
    size_t maxCount,
    bool testMode
) const {

    // ������� ������� ��� ��������. ������ ���������������� � ������������.

    // ������ ���������� ����� �������: �� �������� �� ������ �������
    // (��� Z - ���������� �����, X - ������, Y - �����)

    const size_t numBB = (size_t)dBS[0]->getNumBillboards();
    Ogre::Billboard* b = nullptr;
    const Ogre::Vector3 c( coord.get<0>(), coord.get<1>(), coord.get<2>() );
    const float halfSize = doxel.size() / 2.0f;

    // @test
    /*
    testBS->clear();
    if (numBB == 64) {
        b = testBS->createBillboard( c );
        testBS->setDefaultDimensions( doxel.size() / 1.5f, doxel.size() / 1.5f );
        return;
    } else {
        b = dBS[0]->createBillboard( Ogre::Vector3::ZERO );
        return;
    }
    */

    // ����������� �� ����������
    if ( (maxCount != 0)
      && ( (numBB >= maxCount) || ((size_t)testBS->getNumBillboards() >= maxCount) )
    ) {
        return;
    }

    if ( testMode ) {
        // ���������� �������������� ��������, ����� ������� ������ �������
        b = testBS->createBillboard( c );
        std::srand( (unsigned int)( &doxel ) );
        b->setRotation( Ogre::Degree( (float)rand() ) );
        //testBS->setCommonDirection( Ogre::Vector3( (float)rand(), (float)rand(), (float)rand() ) );
        //auto direction = testBS->getCommonDirection();
        //direction.y = (float)(rand() % 100) / 100.0f;
        //testBS->setCommonDirection( direction );
        testBS->setDefaultDimensions( doxel.size() / 1.5f, doxel.size() / 1.5f );

        // ������������ �������� ������ ������ �����
        /* - �� ���������. �������� � �����������.
        const auto fn = [] ( Ogre::BillboardSet* testBS ) {
            Sleep( 1000 );
            if ( testBS ) {
                try {
                    testBS->clear();
                } catch ( ... ) {
                }
            }
        };
        boost::thread( fn, testBS );
        */

    } else {

        // ������
        b = dBS[0]->createBillboard(
            c.x,
            c.y,
            c.z + halfSize
        );

        // ������
        b = dBS[1]->createBillboard(
            c.x + halfSize,
            c.y,
            c.z
        );

        // �����
        b = dBS[2]->createBillboard(
            c.x,
            c.y,
            c.z - halfSize
        );

        // �����
        b = dBS[3]->createBillboard(
            c.x - halfSize,
            c.y,
            c.z
        );

        // ������
        b = dBS[4]->createBillboard(
            c.x,
            c.y + halfSize,
            c.z
        );

        // �������
        b = dBS[5]->createBillboard(
            c.x,
            c.y - halfSize,
            c.z
        );

        // ������������� ����� ������ ���� ���������� �������
        // (�������, ������� �����������, ��� ���������� ��������� ������ ��������
        // �� ������ ������)
        for (auto itr = dBS.begin(); itr != dBS.end(); ++itr) {
            auto bs = *itr;
            assert( bs && "������ ���� ������������ � ������������." );
            bs->setDefaultDimensions( doxel.size(), doxel.size() );
        }

    } // else if ( testMode )

}






void MapDoxel::drawChild(
    const Doxel3D& doxel,
    const d::coord3d_t& coord,
    size_t level,
    size_t maxCount,
    bool testMode
) const {

    // @test
    const auto n = doxel.countChild( 0 );
    const float size = doxel.size();

    // �������� ������ �������, ���� �� ��������� ��������� ������. ��������
    // �� ���� ��������.
    for (auto itr = doxel.child()->cbegin(); itr != doxel.child()->cend(); ++itr) {
        const auto child = *itr;
        if ( !child ) {
            continue;
        }

        const auto d3d = static_cast< Doxel3D* >( child.get() );
        // �������� �������� ��� �������� ����� �������
        const float shift = d3d->size() / 2.0f;

        const size_t k = std::distance( doxel.child()->cbegin(), itr );
        d::coord3d_t c = coord;
        // @see ��������� �������� � ���� Doxel3D::valueOf()
        switch ( k ) {
            // ������� ��������� (z >= 0)
            case 0 :
                c += d::coord3d_t( shift, shift, shift );
                break;
            case 1 :
                c += d::coord3d_t( shift, -shift, shift );
                break;
            case 2 :
                c += d::coord3d_t( -shift, -shift, shift );
                break;
            case 3 :
                c += d::coord3d_t( -shift, shift, shift );
                break;
            // ������ ��������� (z < 0)
            case 4 :
                c += d::coord3d_t( shift, shift, -shift );
                break;
            case 5 :
                c += d::coord3d_t( shift, -shift, -shift );
                break;
            case 6 :
                c += d::coord3d_t( -shift, -shift, -shift );
                break;
            case 7 :
                c += d::coord3d_t( -shift, shift, -shift );
                break;
            default:
                throw "��� ����� �������� � Doxel3D => 3 (���) ���������, 8 �������� ��������.";
        }

        // �������� ���������� - ������������ ����� ������ �������
        // � ������� 0 (��. ����)
        if (level > 0) {
            drawChild( *d3d, c, level - 1, maxCount, testMode );

        } else {
            // ������ ������� ������ 0
            draw( *d3d, c, maxCount, testMode );
        }

    } // for (auto itr = d->child()->cbegin() ...

}
