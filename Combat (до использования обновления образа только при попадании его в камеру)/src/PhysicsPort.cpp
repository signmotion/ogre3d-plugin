#include "PhysicsPort.h"


PhysicsPort::PhysicsPort(
    Ogre::SceneManager* sm,
    const d::size3d_t& sizeWorld,
    float timestep
) :
    Port( sm ),
    sizeWorld( sizeWorld ),
    timestep( timestep ),
    sceneEntityName(
        Ogre::StringConverter::toString( timestep ) + "::Entity::PhysicsPort"
          + "::" + Ogre::StringConverter::toString( sizeWorld.get<0>() )
          + "x"  + Ogre::StringConverter::toString( sizeWorld.get<1>() )
          + "x"  + Ogre::StringConverter::toString( sizeWorld.get<2>() )
    ),
    sceneGridName(
        Ogre::StringConverter::toString( timestep )+ "::Grid::PhysicsPort"
          + "::" + Ogre::StringConverter::toString( sizeWorld.get<0>() )
          + "x"  + Ogre::StringConverter::toString( sizeWorld.get<1>() )
          + "x"  + Ogre::StringConverter::toString( sizeWorld.get<2>() )
    )
{
    assert( sm );
    assert( ( (sizeWorld.get<0>() > 0) && (sizeWorld.get<1>() > 0) && (sizeWorld.get<2>() > 0) )
        && "Размер порта не задан." );

    // Визуальный мир
    snEntity = sm->getRootSceneNode()->createChildSceneNode( sceneEntityName );

    // Сцена для сетки
    snGrid = sm->getRootSceneNode()->createChildSceneNode( sceneGridName );

    // Физический мир
    nw = NewtonCreate();
    const d::size3d_t halfSizeWorld = sizeWorld / 2.0f;
    float minCoord[3];
    minCoord[0] = -halfSizeWorld.get<0>();
    minCoord[1] = -halfSizeWorld.get<1>();
    minCoord[2] = -halfSizeWorld.get<2>();
    float maxCoord[3];
    maxCoord[0] = halfSizeWorld.get<0>();
    maxCoord[1] = halfSizeWorld.get<1>();
    maxCoord[2] = halfSizeWorld.get<2>();
    NewtonSetWorldSize( nw, minCoord, maxCoord );

    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonSetSolverModel
    NewtonSetSolverModel( nw, 0 );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonSetFrictionModel
    NewtonSetFrictionModel( nw, 1 );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonSetPlatformArchitecture
    NewtonSetPlatformArchitecture( nw, 2 );

    // События из физического мира
    NewtonSetBodyLeaveWorldEvent( nw, PhysicsPort::bodyLeaveWorldCallback );
}







void PhysicsPort::pulse( int tact ) {

    // Вычисляем изменение физических характеристик порта
    // --------------------------------------------------
    NewtonUpdate( nw, timestep );

    // Синхронизируем изменения с визуальным образом
    // ---------------------------------------------
    // @see bodyTransformCallback()

}







void PhysicsPort::set(
    //std::shared_ptr< LiveBody >  e,
    const LiveBody& entityForInclude,
    const d::coord3d_t& coord,
    float mass
) {
    //assert( e );

    // Сразу делаем копию сущности. Память освободим при удалении сущности.
    LiveBody* e = entityForInclude.clone();

    const bool present = has( e->uid );
    if ( present ) {
        destroy( e->uid );
    }

    // Сохраняем сущность в порту

    // Визуальный образ
    e->sceneImage = e->visual( sm, snEntity->createChildSceneNode() );
    e->sceneImage->setPosition( coord.get<0>(), coord.get<1>(), coord.get<2>() );


    // Физический образ
    const auto x = coord.get<0>();
    const auto y = coord.get<1>();
    const auto z = coord.get<2>();
    const float tm[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x,    y,    z,    1.0f
    };
    NewtonCollision* collision = e->collision( nw );
    assert( collision );

    e->body = NewtonCreateBody( nw, collision, tm );
    NewtonBodySetMassMatrix( e->body, mass, 1.0f, 1.0f, 1.0f );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonBodySetAutoSleep
    NewtonBodySetAutoSleep( e->body, 1 );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonBodySetAngularDamping
    const float angularDamp[3] = { 0.001f, 0.001f, 0.001f };
    NewtonBodySetAngularDamping( e->body, angularDamp );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonBodySetLinearDamping
    NewtonBodySetLinearDamping( e->body, 0.001f );

    // События из физического мира
    NewtonBodySetForceAndTorqueCallback( e->body, PhysicsPort::bodyForceAndTorqueCallback );
    NewtonBodySetDestructorCallback( e->body, PhysicsPort::bodyDestructorCallback );
    NewtonBodySetTransformCallback( e->body, PhysicsPort::bodyTransformCallback );


    // Связываем физическую и визуальную части структурой
    NewtonBodySetUserData( e->body, e );
    /* - Обходимся.
    ea->visual.sn->getUserObjectBindings().setUserAny( "ea", Ogre::Any( ea ) );
    */
    

    // @test
    std::cout << "Визуальных 3D-образов " << snEntity->numChildren() << std::endl;
    std::cout << "Физических образов " << NewtonWorldGetBodyCount( nw )  << std::endl;
    std::cout << "Уникальных физических форм " << CollisionManager::singleton()->count() << std::endl;

}








void PhysicsPort::grid( size_t qtyCell ) {

    // Левый нижний угол - центр координат

    // Ячеек в сетке
    const int halfQtyCell = (int)qtyCell / 2;

    // Размер одной ячейки
    const float sideCell = sizeWorld.get<0>() / (float)qtyCell;
    const float maxHalfSideCell = halfQtyCell * sideCell;

    // Не рисуем сетку, если она уже есть
    const bool virgin = (snGrid->numChildren() == 0);
    if ( virgin ) {

        auto mo = sm->createManualObject();
        mo->begin( "combat/grid", Ogre::RenderOperation::OT_LINE_LIST );

        for (int i = -halfQtyCell; i <= halfQtyCell; ++i) {
            // OX
            const float z  = (float)i * sideCell;
            mo->position( Ogre::Vector3( -maxHalfSideCell, 0.0f, z ) );
            mo->position( Ogre::Vector3( sizeWorld.get<1>() - maxHalfSideCell, 0.0f, z ) );
            // OZ
            const float x  = (float)i * sideCell;
            mo->position( Ogre::Vector3( x, 0.0f, -maxHalfSideCell  ) );
            mo->position( Ogre::Vector3( x, 0.0f, sizeWorld.get<0>() - maxHalfSideCell ) );
        }

        mo->end();

        // Каждому образу - свой холст
        auto lineScene = snGrid->createChildSceneNode( "Line::" + sceneGridName );
        lineScene->attachObject( mo );
        //lineScene->setScale( Ogre::Vector3( visualScale ) );
        
        // Отмечаем центр координат (0; 0)
        auto centerScene = snGrid->createChildSceneNode( "Center::" + sceneGridName );
        //auto point = sm->createEntity( "axes.mesh" );
        auto point = sm->createEntity( "sphere.mesh" );
        centerScene->attachObject( point );
        // Масштабируем размер центра
        const float scalePoint = 0.5f / point->getBoundingRadius();
        centerScene->setScale( Ogre::Vector3( scalePoint ) );

    } // if ( !virgin )

}







void PhysicsPort::bodyForceAndTorqueCallback(
    const NewtonBody* const body,
    dFloat timestep,
    int threadIndex
) {
    //std::cout << "PhysicsPort::bodyForceAndTorqueCallback()" << std::endl;

    /* - @test
    // Постоянно действующая сила
    const float gravity = -9.81f;

    float mass;
    float ix;
    float iy;
    float iz;
    NewtonBodyGetMassMatrix( body, &mass, &ix, &iy, &iz );

    // Сила действует на тело именно сейчас
    const float force[4] = {
        0.0f * mass,
        0.0f * mass,
        gravity * mass,
        1.0f
    };
    NewtonBodySetForce( body, force );
    */


    // Тело тоже меняет своё состояние, оказывая воздействие на мир

    // @test Дрожание.
    // @idea Может подключаться по требованию, не меняя существенно положение
    //       тела, но добавляя ему некоторой экспрессии.
    const auto humanInWorld = search( body );
    const size_t K = 10;
    humanInWorld->set(
        boost::assign::list_of
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[0])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[1])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[2])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[3])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[4])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[5])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[6])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[7])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[8])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[9])
            (d::coord3d_t( (float)(rand() % K), (float)(rand() % K), (float)(rand() % K) ) + humanInWorld->startCoord[10])
    );
    /**/


    /* @test Дрожание со смещением.
    // @idea Эффект: когда крутит не по-детски.
    const auto humanInWorld = search( body );
    const auto randomShift = [] () -> d::coord3d_t {
        const int K = 10 + 1;
        return d::coord3d_t(
            (float)(rand() % K - K / 2),
            (float)(rand() % K - K / 2),
            (float)(rand() % K - K / 2)
        );
    };
    humanInWorld->setShift(
        boost::assign::list_of
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
            (randomShift())
    );
    */

}







void PhysicsPort::bodyDestructorCallback(
    const NewtonBody* body
) {
    std::cout << "PhysicsPort::bodyDestructorCallback()" << std::endl;

    // Получаем связывающую структуру
    auto e = search( body );

    // Удаляем визуальный образ
    e->sceneImage->getParentSceneNode()->removeAndDestroyChild( e->sceneImage->getName() );

    // Удаляем связывающую структуру (клонировали при вызове set())
    delete e;
}





    
    
    
void PhysicsPort::bodyLeaveWorldCallback(
    const NewtonBody* body,
    int threadIndex
) {
    std::cout << "PhysicsPort::bodyLeaveWorldCallback()" << std::endl;
    // Удаляем физический образ
    NewtonWorld* nw = NewtonBodyGetWorld( body );
    NewtonDestroyBody( nw, body );
    // Об удалении визуального образа и связывающей структуры
    // позаботится bodyDestructorCallback()
}








void PhysicsPort::bodyTransformCallback(
    const NewtonBody* body,
    const dFloat* tm,
    int threadIndex
) {
    //std::cout << "PhysicsPort::bodyTransformCallback()" << std::endl;

    // Отражаем положение тела в визуальный образ

    /*
        @example From Ogre position/orientation to Newton matrix.
            Matrix4 mat(mySceneNode->getOrientation());
            mat.setTrans(mySceneNode->getPosition());
            mat = mat.transpose();
            NewtonBodySetMatrix(pRigidBodyBox, mat[0]);

        @example From Newton matrix to Ogre position/orientation.
            Matrix4 mat;
            NewtonBodyGetMatrix(m_pPhysicWorld->GetBody(), mat[0]);
            mat = mat.transpose();
            SceneNode* pNode = m_pScene->getSceneNode("Holzkiste");
            pNode->setOrientation(mat.extractQuaternion());
            pNode->setPosition(mat.getTrans()); 
    */

    /* - optimize Сразу инициализируем матрицу Ogre значениями из 'tm'. См. ниже.
    Ogre::Matrix4 mat();
    NewtonBodyGetMatrix( body, mat[0] );
    mat = mat.transpose();
    */
    // @see О матрице >
    const Ogre::Matrix4 mat(
        tm[0], tm[4], tm[8],  tm[12],
        tm[1], tm[5], tm[9],  tm[13],
        tm[2], tm[6], tm[10], tm[14],
        tm[3], tm[7], tm[11], tm[15]
    );

    auto e = search( body );
    e->sceneImage->setPosition( mat.getTrans() ); 
    e->sceneImage->setOrientation( mat.extractQuaternion() );
}
