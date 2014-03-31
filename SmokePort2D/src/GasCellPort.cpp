#include "GasCellPort.h"
#include "helper.h"
#include "CellDrawActor.h"


void GasCellPort::pulse( int tact ) {

    // ��������� ��������� ������������� �����
    // ---------------------------------------

    // �������� �� ������ ������ ����� � ������� ��������� � �����������
    // �� ��������� �������.
    // @todo optimize ���������� ��� OpenCL.
    if (tact != 0) {

        // ������ I
        for (size_t z = 0; z < M; ++z) {
            for (size_t x = 0; x < N; ++x) {
                // �������� � ������ �������������.
                // �������� � helper, ����� ��������� ������� �� OpenCL.
                size_t i;
                size_t uid;
                float pressure;
                helper::calc( content, x, z, N, M, &i, &uid, &pressure );
                contentT[i].uid = uid;
                contentT[i].pressure = pressure;
                // ������������������� ���������� ��� ���������
                contentT[i].deltaPressure = content[i].deltaPressure;

                // @test
                //contentT[i].uid = (rand() % 2 == 0) ? 0 : 101;
                //contentT[i].pressure = (rand() % 2 == 0) ? 0 : 1000;
            }
        }

        // ������ II
        const size_t n = NM * sizeof( GasFD );
        memcpy( content, contentT, n );

    } // if (tact != 0)


    // �������� ��� ���������� �������
    // -------------------------------
    // ��� ��������� ����� ������������ � ������� �������. ��������� �����
    // ����� ����������� ���������� ���������� ���������� ��� ������, ��������
    // ������ ������� �� ���������.

    // (!) ������ � ��������� XZ. ����������� ����: OX - ������, OZ - �����.

    // ���������� ������������� �������
    const float S = scale * visualScale;

    // ������������� ������
    /* - ��������. ��. ����.
    // ������������� ������ ������
    for (size_t z = 0; z < M; ++z) {
        for (size_t x = 0; x < N; ++x) {
            const coordInt2d_t coord( (int)x, (int)z );
            cellDraw( coord );
            // @test
            //break;
        }
    }
    */
    /* - ��������������� ����. ��������. ��. ����.
    drawActor.Push(
        AllDrawMessage< GasFD >( sm, S, content, N, M, "smokeport2d/smoke-", true ),
        Theron::Address::Null()
    );
    */
    const AllDrawMessage< GasFD > m(
        sm, S, content, N, M, "smokeport2d/smoke-", true
    );
    CellDrawActor< GasFD >::allDraw( m );

}








#if 0
void GasCellPort::cellDraw( const coordInt2d_t& coord ) const {

    // ������ �����

    // ���������� ������ ���������� � ���� ����

    const int i = helper::ic( coord.get<0>(), coord.get<1>(), N, M );
    assert( (i != -1) && "�������� ���������� �� ��������� ������� �����." );
    const GasFD c = content[i];
    if (c.uid == 0) {
        // ������ �� ����������
        return;
    }

    
#if 0
    // - ��������. ���������� ����� ���������.
    /*
    const std::string name =
        "smoke-pressure-"
      + Ogre::StringConverter::toString( (int)c.pressure );
    */
    const std::string meshName = "sphere.mesh";
    /* - @todo ��� �� ������ ��� ����� ����� ��������?
    const std::string entityName = meshName + ".entity";
    const bool has = sm->hasEntity( entityName );
    Ogre::Entity* image = has ?
        sm->getEntity( entityName ) :
        sm->createEntity( entityName, meshName );
    */
    Ogre::Entity* image = sm->createEntity( meshName );
    auto childScene = sn->createChildSceneNode();
    childScene->attachObject( image );
    // ���������� ������ ������ ��������
    childScene->setPosition(
        (float)coord.get<0>() * scale,
        0.0f,
        (float)coord.get<1>() * scale
    );
    // ����� ��������� ������ �������
    //const Ogre::Vector3 fillScale( scale / image->getBoundingRadius() );
    //childScene->setScale( fillScale );
#endif

    // �������� ����� ���������
    // @source http://89.151.96.106/forums/viewtopic.php?f=10&t=60455

    const float S = scale * visualScale;

    // ������� ��������� ��� ������ ��������� � ���
    // @todo ��������������� � ����������� �� �������������� ������ � �������.
    std::string bsName = "GasCellPort-bs";
    Ogre::BillboardSet* bs = nullptr;
    if ( sm->hasBillboardSet( bsName ) ) {
        bs = sm->getBillboardSet( bsName );

    } else {
        // ��� - ���� ����� �� ��� ������
        bs = sm->createBillboardSet( bsName, count() );
        bs->setAutoextend( false );
        const std::string prefixMaterial = "smokeport2d/smoke-";
        const std::string nameMaterial = prefixMaterial + "0";
        bs->setMaterialName( nameMaterial );
        bs->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
        bs->setCommonUpVector( Ogre::Vector3::UNIT_Z );
        bs->setCommonDirection( -Ogre::Vector3::UNIT_Y );
        bs->setBillboardsInWorldSpace( true );
        bs->setBillboardOrigin( Ogre::BBO_CENTER );
        bs->setDefaultDimensions( S, S );
        sn->attachObject( bs );
    }

    // ������������� ��������
    Ogre::Billboard* b = bs->createBillboard(
        (float)coord.get<0>() * S,
        0.0f,
        (float)coord.get<1>() * S
    );
    //b->setRotation( Ogre::Degree( (float)rand() ) );

}
#endif
