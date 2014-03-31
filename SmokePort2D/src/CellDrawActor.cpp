#include "CellDrawActor.h"
#include "default.h"
#include "struct.h"
#include "helper.h"


template< typename C >
void CellDrawActor< C >::allDraw(
    const AllDrawMessage< C >& m
) {
    // Рисуем только если ещё не было нарисовано
    // или по просьбе "обновлять постоянно"
    const std::string sceneName = "CellDrawActor::allDraw::Scene";
    const std::string bsName = "CellDrawActor::allDraw::BillboardSet";

    // @test
    /*
    try {
        m.sm->getRootSceneNode()->removeAndDestroyChild( sceneName );
    } catch ( ... ) {
    }
    try {
        m.sm->destroyBillboardSet( bsName );
    } catch ( ... ) {
    }
    */

    if ( m.update || !m.sm->hasSceneNode( sceneName ) ) {

        // Каждому образу - свой холст (общая сцена)
        auto scene = m.sm->hasSceneNode( sceneName ) ?
            m.sm->getSceneNode( sceneName ) :
            m.sm->getRootSceneNode()->createChildSceneNode( sceneName );

        // Определяем общее кол-во элементов
        const size_t NM = m.N * m.M;

        // Готовим биллборды

        // Обращаются за отрисовкой первый раз?
        const bool virgin = m.sm->hasBillboardSet( bsName );
        Ogre::BillboardSet* bs = nullptr;
        if ( virgin ) {
            bs = m.sm->getBillboardSet( bsName );

        } else {
            // Один образ для всех ячеек
            // @todo Расширить кол-во образов.
            bs = m.sm->createBillboardSet( bsName, NM );
            bs->setAutoextend( true );
            const std::string nameMaterial = m.prefixMaterial + "0";
            bs->setMaterialName( nameMaterial );
            bs->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
            bs->setCommonUpVector( Ogre::Vector3::UNIT_Z );
            bs->setCommonDirection( -Ogre::Vector3::UNIT_Y );
            bs->setBillboardsInWorldSpace( true );
            bs->setBillboardOrigin( Ogre::BBO_CENTER );
            bs->setDefaultDimensions( m.scale, m.scale );
            bs->setUseAccurateFacing( false );
            scene->attachObject( bs );
        }

        // Визуализируем каждую ячейку

        /* - @todo Можно ускорить отрисовку > http://www.ogre3d.org/forums/viewtopic.php?f=2&t=11134&start=0
        Ogre::Billboard b = Ogre::Billboard( Ogre::Vector3::ZERO, bs );
        b.setColour( Ogre::ColourValue::White );
        b.setRotation( Ogre::Degree( (float)rand() ) );
        */

        //bs->beginBillboards();
        for (size_t z = 0; z < m.M; ++z) {
            for (size_t x = 0; x < m.N; ++x) {
                const coordInt2d_t coord( (int)x, (int)z );

                // Создадим образ ячейки с указанной координатой

                const int i = helper::ic( coord.get<0>(), coord.get<1>(), m.N, m.M );
                assert( (i != -1) && "Получена координата за пределами матрицы порта." );
                const C c = m.content[i];

                // Биллборд уже мог быть создан ранее
                const auto bi = helper::index( *bs, coord, m.scale );
                if (bi != -1) {
                    bs->removeBillboard( bi );
                }

                if (c.uid == 0) {
                    // Вакуум - отсутствие образа
                    continue;
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

                // @todo Визаулизировать в зависимости от характеристики ячейки и соседей.

                // Позиционируем биллборд

                Ogre::Billboard* b = bs->createBillboard(
                    (float)coord.get<0>() * m.scale,
                    0.0f,
                    (float)coord.get<1>() * m.scale
                );
                b->setRotation( Ogre::Degree( (float)rand() ) );
                /*
                b.setPosition(
                    (float)coord.get<0>() * m.scale,
                    0.0f,
                    (float)coord.get<1>() * m.scale
                );
                bs->injectBillboard( b );
                */
                
                // @test
                //break;

            } // for (size_t x = 0; x < m.N; ++x)

        } // for (size_t z = 0; z < m.M; ++z)

        //bs->endBillboards();

    } // if ( m.update || !m.sm->hasSceneNode( sceneName ) )

}



template void CellDrawActor< GasFD >::allDraw(
    const AllDrawMessage< GasFD >& m
);








template< typename C >
void CellDrawActor< C >::allDrawHandler(
    const AllDrawMessage< C >& m,
    const Theron::Address from
) {
    std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
        " CellDrawActor::allDrawHandler() start" << std::endl;

    CellDrawActor< C >::allDraw( m );

    // Информируем о выполнении
    TailSend( true, from );

    // @test Спим, чтобы убедиться: мы в потоке и нам потоков хватает.
    //Sleep( 5000 );

    std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
        " CellDrawActor::allDrawHandler() stop" << std::endl;
}



template void CellDrawActor< GasFD >::allDrawHandler(
    const AllDrawMessage< GasFD >& m,
    const Theron::Address from
);








template< typename C >
void CellDrawActor< C >::gridDraw(
    const GridDrawMessage& m
) {
    // Левый нижний угол - центр координат (порт - матрица).
    // Нумерация строк / столбцов матрицы совпадает с напраслением
    // осей OX / OZ.

    // Не рисуем сетку, если она уже есть
    const std::string sceneName = "CellDrawActor::gridDraw::Scene";
    const bool virgin = m.sm->hasSceneNode( sceneName );
    if ( !virgin ) {

        // Каждому образу - свой холст (общая сцена)
        auto scene = m.sm->getRootSceneNode()->createChildSceneNode( sceneName );

        // Ячейки смещаем на полмасштаба влево-вниз
        const auto halfCell = m.scale / 2.0f;

        auto mo = m.sm->createManualObject();
        mo->begin( "smokeport2d/grid", Ogre::RenderOperation::OT_LINE_LIST );

        const float side = (float)m.size * m.scale;
        for (size_t i = 0; i <= m.size; ++i) {
            // OX
            const float z  = (float)i * m.scale - halfCell;
            mo->position( Ogre::Vector3( -halfCell, 0.0f, z ) );
            mo->position( Ogre::Vector3( side - halfCell, 0.0f, z ) );
            // OZ
            const float x  = (float)i * m.scale - halfCell;
            mo->position( Ogre::Vector3( x, 0.0f, -halfCell  ) );
            mo->position( Ogre::Vector3( x, 0.0f, side - halfCell ) );
        }

        mo->end();

        // Каждому образу - свой холст
        auto moScene = scene->createChildSceneNode( "CellDrawActor::gridDraw::MOScene::Scene" );
        moScene->attachObject( mo );
        //childScene->setPosition( Ogre::Vector3::ZERO );

        // Отмечаем первую ячейку (центр координат)
        auto centerScene = scene->createChildSceneNode( "CellDrawActor::gridDraw::CenterScene::Scene" );
        auto point = m.sm->createEntity( "sphere.mesh" );
        //centerScene->setPosition( Ogre::Vector3::ZERO );
        centerScene->attachObject( point );
        //centerScene->setScale( Ogre::Vector3( 1.0f / 20.0f ) );

    } // if ( !virgin )

}



template void CellDrawActor< GasFD >::gridDraw(
    const GridDrawMessage& m
);






template< typename C >
void CellDrawActor< C >::gridDrawHandler(
    const GridDrawMessage& m,
    const Theron::Address from
) {
    std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
        " CellDrawActor::gridDrawHandler() start" << std::endl;

    CellDrawActor< GasFD >::gridDraw( m );

    // Информируем о выполнении
    Send( true, from );

    // @test Спим, чтобы убедиться: мы в потоке и нам потоков хватает.
    // Если потоков на отрисовку не хватает, будет пауза.
    //Sleep( 5000 );

    std::cout << Ogre::StringConverter::toString( from.AsInteger() ) <<
        " CellDrawActor::gridDrawHandler() stop" << std::endl;
}



template void CellDrawActor< GasFD >::gridDrawHandler(
    const GridDrawMessage& m,
    const Theron::Address from
);







template< typename C >
void CellDrawActor< C >::drawHandler(
    const DrawMessage< C >& m,
    const Theron::Address from
) {
    // @todo ...
}



template void CellDrawActor< GasFD >::drawHandler(
    const DrawMessage< GasFD >& m,
    const Theron::Address from
);
