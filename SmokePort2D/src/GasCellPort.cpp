#include "GasCellPort.h"
#include "helper.h"
#include "CellDrawActor.h"


void GasCellPort::pulse( int tact ) {

    // Вычисляем изменение характеристик порта
    // ---------------------------------------

    // Проходим по каждой ячейке порта и считаем изменение в зависимости
    // от состояния соседей.
    // @todo optimize Переписать под OpenCL.
    if (tact != 0) {

        // Проход I
        for (size_t z = 0; z < M; ++z) {
            for (size_t x = 0; x < N; ++x) {
                // Давление в ячейке выравнивается.
                // Вынесено в helper, чтобы облегчить переход на OpenCL.
                size_t i;
                size_t uid;
                float pressure;
                helper::calc( content, x, z, N, M, &i, &uid, &pressure );
                contentT[i].uid = uid;
                contentT[i].pressure = pressure;
                // Проинициализировать необходимо всю структуру
                contentT[i].deltaPressure = content[i].deltaPressure;

                // @test
                //contentT[i].uid = (rand() % 2 == 0) ? 0 : 101;
                //contentT[i].pressure = (rand() % 2 == 0) ? 0 : 1000;
            }
        }

        // Проход II
        const size_t n = NM * sizeof( GasFD );
        memcpy( content, contentT, n );

    } // if (tact != 0)


    // Работаем над визуальным образом
    // -------------------------------
    // Вся отрисовка сцены организована с помощью акторов. Благодаря этому
    // можем моментально возвращать управление вызвавшему нас методу, отправив
    // актору задание на отрисовку.

    // (!) Рисуем в плоскости XZ. Направление осей: OX - вправо, OZ - вверх.

    // Определяем окончательный масштаб
    const float S = scale * visualScale;

    // Визуализируем ячейки
    /* - Заменено. См. ниже.
    // Визуализируем каждую ячейку
    for (size_t z = 0; z < M; ++z) {
        for (size_t x = 0; x < N; ++x) {
            const coordInt2d_t coord( (int)x, (int)z );
            cellDraw( coord );
            // @test
            //break;
        }
    }
    */
    /* - Непредсказуемые сбои. Заменено. См. ниже.
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

    // Создаём образ

    // Содержимое ячейки представим в виде меша

    const int i = helper::ic( coord.get<0>(), coord.get<1>(), N, M );
    assert( (i != -1) && "Получена координата за пределами матрицы порта." );
    const GasFD c = content[i];
    if (c.uid == 0) {
        // Вакуум не показываем
        return;
    }

    
#if 0
    // - Медленно. Переписано через биллборды.
    /*
    const std::string name =
        "smoke-pressure-"
      + Ogre::StringConverter::toString( (int)c.pressure );
    */
    const std::string meshName = "sphere.mesh";
    /* - @todo Так ли каждый раз нужна новая сущность?
    const std::string entityName = meshName + ".entity";
    const bool has = sm->hasEntity( entityName );
    Ogre::Entity* image = has ?
        sm->getEntity( entityName ) :
        sm->createEntity( entityName, meshName );
    */
    Ogre::Entity* image = sm->createEntity( meshName );
    auto childScene = sn->createChildSceneNode();
    childScene->attachObject( image );
    // Координаты образа кратны масштабу
    childScene->setPosition(
        (float)coord.get<0>() * scale,
        0.0f,
        (float)coord.get<1>() * scale
    );
    // Образ заполняет ячейку целиком
    //const Ogre::Vector3 fillScale( scale / image->getBoundingRadius() );
    //childScene->setScale( fillScale );
#endif

    // Работаем через биллборды
    // @source http://89.151.96.106/forums/viewtopic.php?f=10&t=60455

    const float S = scale * visualScale;

    // Готовим биллборды при первом обращении к ним
    // @todo Визаулизировать в зависимости от характеристики ячейки и соседей.
    std::string bsName = "GasCellPort-bs";
    Ogre::BillboardSet* bs = nullptr;
    if ( sm->hasBillboardSet( bsName ) ) {
        bs = sm->getBillboardSet( bsName );

    } else {
        // Дым - один образ на всю ячейку
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

    // Позиционируем биллборд
    Ogre::Billboard* b = bs->createBillboard(
        (float)coord.get<0>() * S,
        0.0f,
        (float)coord.get<1>() * S
    );
    //b->setRotation( Ogre::Degree( (float)rand() ) );

}
#endif
