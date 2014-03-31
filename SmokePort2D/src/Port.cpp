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
    assert( (visualScale > 0.001f) && "Масштаб образа порта необходимо указать." );

    // Подготавливаем сцены
    if ( sm ) {
        sn = sm->getRootSceneNode()->createChildSceneNode();
        //sn->setScale( Ogre::Vector3( visualScale ) );
    }


    // Инициализируем акторов

    // Акторы визуализации
    /* - @see Прим. к свойствам в Port.h
    drawFlicker = std::shared_ptr< Theron::Framework >( new Theron::Framework( 1 ) );
    drawActor = drawFlicker->CreateActor< CellDrawActor< GasFD > >();
    */

}
