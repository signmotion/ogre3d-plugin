#include "Port.h"
#include "struct.h"


Port::Port(
    Ogre::SceneManager* sm,
    float visualScale
) :
    sm( sm ),
    sn( nullptr ),
    visualScale( visualScale )
{
    assert( (visualScale > 0.001f) && "������� ������ ����� ���������� �������." );

    // �������������� �����
    if ( sm ) {
        sn = sm->getRootSceneNode()->createChildSceneNode();
        //sn->setScale( Ogre::Vector3( visualScale ) );
    }


    // �������������� �������

    // ������ ������������
    /* - @see ����. � ��������� � Port.h
    drawFlicker = std::shared_ptr< Theron::Framework >( new Theron::Framework( 1 ) );
    drawActor = drawFlicker->CreateActor< CellDrawActor< GasFD > >();
    */

}
