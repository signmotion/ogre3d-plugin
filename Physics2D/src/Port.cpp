#include "Port.h"
#include "struct.h"


Port::Port( Ogre::SceneManager* sm ) :
    sm( sm )
{
    // Подготовка сцен лежит на потомках

    // Инициализируем акторов

    // Акторы визуализации
    /* - @see Прим. к свойствам в Port.h
    drawFlicker = std::shared_ptr< Theron::Framework >( new Theron::Framework( 1 ) );
    drawActor = drawFlicker->CreateActor< CellDrawActor< GasFD > >();
    */

}
