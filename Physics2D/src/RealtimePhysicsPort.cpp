#include "RealtimePhysicsPort.h"


// Визуальные образы - меши.
// @todo ? Отображает через раз. Непонятное поведение.
void RealtimePhysicsPort::pushDroneMesh( const uid_t& fromThrone ) {

    // Т.к. метод вызывается в потоке, позаботимся о безопасности
    waitFreeSetLock();

    // Находим запрашиваемого трона
    auto throne = search( fromThrone );
    assert( throne && "Сущность по заданному UID не найдена." );

    // Определяем координаты трона
    const coord2d_t coordThrone = coord( throne->body );

    // "Выстреливаем" дронов
    /* - Заменено. См. ниже.
    InitEntity ie;
    const auto a = Ogre::StringConverter::toString( count() + 1 );
    const auto b = Ogre::StringConverter::toString( GetTickCount() );
    ie.uid = "D" + a + "-" + b;
    std::cout << "RealtimePhysicsPort::pushDrone() UID " << ie.uid << std::endl;
    ie.boundingRadius = 1.0f * 100.0f;
    ie.mass = 1.0f;
    const float rx = (float)(rand() % 100 - 50) * 100.0f;
    const float rz = (float)(rand() % 200) * 100.0f;
    ie.coord = coord2d_t( coordThrone.x + rx, coordThrone.z + rz );
    ie.model = "sphere.mesh";
    ie.force = vector2d_t( 0.0f, 1000.0f );
    set( ie );
    */

    for (size_t n = 0; n < 100; ++n ) {
        VolumeInitEntity ie( InitEntity::FORM_SPHERE );
        const auto a = Ogre::StringConverter::toString( n + 1 );
        const auto b = Ogre::StringConverter::toString( GetTickCount() );
        ie.uid = "D" + a + "-" + b;
        std::cout << "RealtimePhysicsPort::pushDroneMesh() UID " << ie.uid << std::endl;
        ie.boundingRadius = 1.0f * 100.0f;
        ie.mass = 1.0f;
        const float rx = ((float)n * 0.1f - 0.0f) * 100.0f;
        const float rz = ((float)n * 2.0f + 5.0f) * 100.0f;
        ie.coord = coordThrone + coord2d_t( rx, rz );
        ie.model = "sphere.mesh";
        ie.force = vector2d_t( 0.0f, 0.0f );
        assert( ie.correct() );
        set( &ie );
    }

    // Дальше всё сделает физический движок

    // Освобождаем порт
    unlock();
}






// Визуальные образы - билборды.
void RealtimePhysicsPort::pushDroneBillboard( const uid_t& fromThrone ) {

    // Т.к. метод вызывается в потоке, позаботимся о безопасности
    waitFreeSetLock();

    // Находим запрашиваемого трона
    auto throne = search( fromThrone );
    assert( throne && "Сущность по заданному UID не найдена." );

    // Определяем координаты трона
    const coord2d_t coordThrone = coord( throne->body );

    // "Выстреливаем" дронов
    const size_t N = 100;
    const size_t M = 5;
    for (size_t m = 0; m < M; ++m ) {
        for (size_t n = 0; n < N; ++n ) {
            FlatInitEntity ie( InitEntity::FORM_SPHERE );
            const auto a = Ogre::StringConverter::toString( n + 1 );
            const auto b = Ogre::StringConverter::toString( GetTickCount() );
            ie.uid = "D" + a + "-" + b;
            //std::cout << "RealtimePhysicsPort::pushDroneBillboard() UID " << ie.uid << std::endl;
            ie.boundingRadius = 1.0f * 100.0f;
            ie.mass = 1.0f;
            //const float rx = ((float)n * 0.1f - 0.0f) * 100.0f;
            //const float rz = ((float)n * 2.0f + 5.0f) * 100.0f;
            const float rx = (((float)n - (float)N / 2.0f) * 2.0f) * 100.0f;
            const float rz = (((float)m - (float)M / 2.0f) * 2.0f + (float)M * 5.0f) * 100.0f;
            ie.coord = coordThrone + coord2d_t( rx, rz );
            ie.model = "sphere.png";
            ie.force = vector2d_t( 0.0f, 10000.0f );
            set( &ie );
        }
    }

    // Дальше всё сделает физический движок

    // Освобождаем порт
    unlock();
}
