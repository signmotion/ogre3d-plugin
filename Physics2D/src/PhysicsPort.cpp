#include "PhysicsPort.h"
#include "struct.h"


template < typename C >
PhysicsPort< C >::PhysicsPort(
    Ogre::SceneManager* sm,
    const size2d_t& sizeWorld
) :
    Port( sm ),
    sizeWorld( sizeWorld ),
    sceneEntityName(
        (std::string)typeid( C ).name() + "::Entity::PhysicsPort"
          + "::" + Ogre::StringConverter::toString( sizeWorld.get<0>() )
          + "-"  + Ogre::StringConverter::toString( sizeWorld.get<1>() )
    ),
    sceneBillboardName(
        (std::string)typeid( C ).name() + "::Billboard::PhysicsPort"
          + "::" + Ogre::StringConverter::toString( sizeWorld.get<0>() )
          + "-"  + Ogre::StringConverter::toString( sizeWorld.get<1>() )
    ),
    sceneGridName(
        (std::string)typeid( C ).name() + "::Grid::PhysicsPort"
          + "::" + Ogre::StringConverter::toString( sizeWorld.get<0>() )
          + "-"  + Ogre::StringConverter::toString( sizeWorld.get<1>() )
    ),
    cm( CollisionManager::singleton() )
{
    assert( ( (sizeWorld.get<0>() > 0) && (sizeWorld.get<1>() > 0) ) && "Размер порта не задан." );

    // Визуальный мир
    snEntity = sm->getRootSceneNode()->createChildSceneNode( sceneEntityName );
    snBillboard = sm->getRootSceneNode()->createChildSceneNode( sceneBillboardName );

    // Сцена для сетки
    snGrid = sm->getRootSceneNode()->createChildSceneNode( sceneGridName );

    // Физический мир
    nw = NewtonCreate();
    const size2d_t halfSizeWorld = sizeWorld / 2.0f;
    const float depth = halfSizeWorld.get<0>();
    float minCoord[3];
    minCoord[0] = -halfSizeWorld.get<0>();
    minCoord[1] = -depth;
    minCoord[2] = -halfSizeWorld.get<1>();
    float maxCoord[3];
    maxCoord[0] = halfSizeWorld.get<0>();
    maxCoord[1] = depth;
    maxCoord[2] = halfSizeWorld.get<1>();
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



template PhysicsPort< EntityRTPortFD >::PhysicsPort(
    Ogre::SceneManager* sm,
    const size2d_t& size
);







template < typename C >
void PhysicsPort< C >::pulse( int tact, float timestep ) {
    assert( (tact == 0) || ((timestep > 0.0f) && (timestep <= 1.0f)) );

    // Не запускаем пульсацию пока порт занят. Например, если
    // сейчас работает set().
    waitFreeSetLock();

    // Вычисляем изменение физических характеристик порта
    // --------------------------------------------------
    NewtonUpdate( nw, timestep );


    // Синхронизируем изменения с визуальным образом
    // ---------------------------------------------
    // @see bodyTransformCallback()

    unlock();
}



template void PhysicsPort< EntityRTPortFD >::pulse( int tact, float timestep );







template < typename C >
void PhysicsPort< C >::set( InitEntity* ie ) {

    assert( ie );
    /* - Заменено. См. ниже.
    assert( ( (ie->form == InitEntity::FORM_BOX)    && !emptyAny( ie->size )
           || (ie->form == InitEntity::FORM_SPHERE) && !empty( ie->boundingRadius ) )
        && "Параметры формы не заданы."
    );
    */
    assert( ie->correct() );
    assert( cm && "CollisionManager должен быть инициализирован." );

    // Не добавляем сущность пока порт занят
    /* - Атомарность обеспечиваем на уровне выше.
    waitFreeSetLock();
    */

    const bool present = has( ie->uid );
    if ( present ) {
        destroy( ie->uid );
    }

    // По ходу формируем структуру для сохранения в порту
    // (!) Очистка памяти - за деструктором.
    auto ea = new EntityAddon( ie->type, ie->uid );
    ea->force = Ogre::Vector3( ie->force.get<0>(), 0.0f, ie->force.get<1>() );

    // Визуальный образ
    switch ( ie->type ) {
        case InitEntity::TYPE_FLAT :
            ea->visual.b = addVisual( *static_cast< FlatInitEntity* >( ie ) );
            break;
        case InitEntity::TYPE_VOLUME :
            ea->visual.sn = addVisual( *static_cast< VolumeInitEntity* >( ie ) );
            break;
        default :
            throw "Type of visual image is not recognized.";
    }
    assert( (ea->visual.b || ea->visual.sn) && "Образ не получен." );


    // Физический образ
    const auto x = ie->coord.get<0>();
    const auto y = 0.0f;
    const auto z = ie->coord.get<1>();
    const float tm[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        x,    y,    z,    1.0f
    };
    /* - Пущено через менеджер физических форм. См. ниже.
    //NewtonCollision* const collision = NewtonCreateBox( nw,  x, y, z,  0, nullptr );
    NewtonCollision* const collision = NewtonCreateSphere( nw,  x, y, z,  0, nullptr );
    */
    NewtonCollision* collision = nullptr;
    switch ( ie->form ) {
        case InitEntity::FORM_BOX :
            NewtonCollisionInfoRecord::NewtonBoxParam box;
            box.m_x = ie->size.get<0>();
            box.m_y = ie->size.get<1>();
            box.m_z = ie->size.get<2>();
            collision = cm->collision( nw, box );
            break;
        case InitEntity::FORM_SPHERE :
            NewtonCollisionInfoRecord::NewtonSphereParam sphere;
            sphere.m_r0 = sphere.m_r1 = sphere.m_r2 = ie->boundingRadius;
            collision = cm->collision( nw, sphere );
            break;
        default :
            throw "Form of visual image is not recognized.";
    }
    assert( collision );

    ea->body = NewtonCreateBody( nw, collision, tm );
    NewtonBodySetMassMatrix( ea->body, ie->mass, 1.0f, 1.0f, 1.0f );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonBodySetAutoSleep
    NewtonBodySetAutoSleep( ea->body, 1 );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonBodySetAngularDamping
    const float angularDamp[3] = { 0.001f, 0.001f, 0.001f };
    NewtonBodySetAngularDamping( ea->body, angularDamp );
    // @see http://newtondynamics.com/wiki/index.php5?title=NewtonBodySetLinearDamping
    NewtonBodySetLinearDamping( ea->body, 0.001f );

    // События из физического мира
    NewtonBodySetForceAndTorqueCallback( ea->body, PhysicsPort::bodyForceAndTorqueCallback );
    NewtonBodySetDestructorCallback( ea->body, PhysicsPort::bodyDestructorCallback );
    NewtonBodySetTransformCallback( ea->body, PhysicsPort::bodyTransformCallback );


    // Связываем физическую и визуальную части сущности новой структурой.
    // Не сохраняем в виде принятой методом InitEntity, т.к. каждый порт
    // хранит информацию в оптимальном для него виде.
    NewtonBodySetUserData( ea->body, ea );
    /* - Обходимся.
    ea->visual.sn->getUserObjectBindings().setUserAny( "ea", Ogre::Any( ea ) );
    */
    

    /* @test
    std::cout << "Визуальных 3D-образов " << snEntity->numChildren() << std::endl;
    std::cout << "Физических образов " << NewtonWorldGetBodyCount( nw )  << std::endl;
    std::cout << "Уникальных физических форм " << cm->count() << std::endl;
    */

    //unlock();
}



template void PhysicsPort< EntityRTPortFD >::set( InitEntity* ie );






// @return Ссылку на *конкретный* экземпляр биллборда в наборе.
template < typename C >
Ogre::Billboard* PhysicsPort< C >::addVisual( const FlatInitEntity& ie ) {

    // 2D-модель представлена с помощью билборда

    const std::string bsName = billboardSetName( ie.model );
    Ogre::BillboardSet* bs = nullptr;
    if ( sm->hasBillboardSet( bsName ) ) {
        // Биллборд мог быть создан ранее
        bs = sm->getBillboardSet( bsName );

    } else {
        // Биллборд - то, что выглядит одинаково, но может показываться в
        // разных местах сцены
        bs = sm->createBillboardSet( bsName, 500 );
        bs->setAutoextend( true );
        const std::string prefixMaterial = "physics2d/bs/";
        const std::string nameMaterial = prefixMaterial + ie.model;
        bs->setMaterialName( nameMaterial );
        bs->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
        bs->setCommonUpVector( Ogre::Vector3::UNIT_Z );
        bs->setCommonDirection( -Ogre::Vector3::UNIT_Y );
        bs->setBillboardsInWorldSpace( true );
        bs->setBillboardOrigin( Ogre::BBO_CENTER );
        // Удваиваем, т.к. радиус - лишь половина биллборда
        const float scaleEntity = ie.boundingRadius * 2.0f;
        bs->setDefaultDimensions( scaleEntity, scaleEntity );
        snBillboard->attachObject( bs );
    }

    // Создаём экземпляр билборда
    Ogre::Billboard* b = bs->createBillboard(
        ie.coord.get<0>(),
        0.0f,
        ie.coord.get<1>()
    );
    //b->setRotation( Ogre::Degree( (float)rand() ) );

    return b;
}



template Ogre::Billboard* PhysicsPort< EntityRTPortFD >::addVisual( const FlatInitEntity& ie );






// @return Ссылку на *сцену*, в которой лежит меш сущности.
template < typename C >
Ogre::SceneNode* PhysicsPort< C >::addVisual( const VolumeInitEntity& ie ) {

    // 3D-модель представлена с помощью меша, помещённого на
    // созданную специально для него сцену

    const std::string entitySceneName = sceneNodeName( ie.uid );
    assert( !sm->hasSceneNode( entitySceneName ) && "Сцены с таким именем существовать не должно." );
    Ogre::SceneNode* visualImage = snEntity->createChildSceneNode( entitySceneName );
    visualImage->setPosition( ie.coord.get<0>(), 0.0f, ie.coord.get<1>() );
    auto e = sm->createEntity( ie.model );
    visualImage->attachObject( e );
    // Масштабируем образ модели
    /* - Масштаб модели задаём радиусом.
    const auto box = e->getBoundingBox();
    const auto sizeBox = box.getSize();
    c->visualImage->setScale( Ogre::Vector3(
        size.get<0>() / sizeBox.x,
        (size.get<0>() + size.get<1>()) / 2.0f / sizeBox.y,
        size.get<1>() / sizeBox.z
    ) );
    */

    // Масштаб для каждой фигуры свой
    Ogre::Vector3 scaleEntity = Ogre::Vector3::ZERO;
    switch ( ie.form ) {
        case InitEntity::FORM_BOX :
            scaleEntity = Ogre::Vector3(
                ie.size.get<0>(), ie.size.get<1>(), ie.size.get<2>()
            ) / e->getBoundingBox().getSize();
            break;
        case InitEntity::FORM_SPHERE :
            scaleEntity = Ogre::Vector3(
                ie.boundingRadius / e->getBoundingRadius()
            );
            break;
        default :
            throw "Form of visual image is not recognized.";
    }
    visualImage->setScale( scaleEntity );

    return visualImage;
}



template Ogre::SceneNode* PhysicsPort< EntityRTPortFD >::addVisual( const VolumeInitEntity& ie );








template< typename C >
void PhysicsPort< C >::grid( size_t qtyCell ) {

    // Не работаем пока порт занят
    /* - Атомарность обеспечиваем на уровне выше.
    waitFreeSetLock();
    */

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
        mo->begin( "physics2d/grid", Ogre::RenderOperation::OT_LINE_LIST );

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


    //unlock();

}



template void PhysicsPort< EntityRTPortFD >::grid( size_t qtyCell );







template< typename C >
void PhysicsPort< C >::bodyForceAndTorqueCallback(
    const NewtonBody* const body,
    dFloat timestep,
    int threadIndex
) {
    //std::cout << "PhysicsPort::bodyForceAndTorqueCallback()" << std::endl;

    // Постоянно действующая сила
    const float gravity = -9.81f;

    float mass;
    float ix;
    float iy;
    float iz;
    NewtonBodyGetMassMatrix( body, &mass, &ix, &iy, &iz );

    // Сила действует на тело именно сейчас
    // Помним: работаем в одной плоскости - XZ
    auto ea = search( body );
    const float force[4] = {
        ea->force.x * mass,
        0.0f,
        (ea->force.z + gravity) * mass,
        1.0f
    };
    NewtonBodySetForce( body, force );

    // Искусственные силы для тела надо обнулить
    ea->force = Ogre::Vector3::ZERO;
}



template void PhysicsPort< EntityRTPortFD >::bodyForceAndTorqueCallback(
    const NewtonBody* const body,
    dFloat timestep,
    int threadIndex
);







template< typename C >
void PhysicsPort< C >::bodyDestructorCallback(
    const NewtonBody* body
) {
    // Получаем связывающую структуру
    auto ea = search( body );

    // Удаляем визуальный образ
    switch ( ea->type ) {
        case InitEntity::TYPE_FLAT : {
            auto b = static_cast< Ogre::Billboard* >( ea->visual.b );
            b->mParentSet->removeBillboard( b );
            break;
        }
        case InitEntity::TYPE_VOLUME : {
            // @todo optimize А можно проще?
            ea->visual.sn->getParentSceneNode()->
                removeAndDestroyChild( ea->visual.sn->getName() );
            break;
        }
        default :
            throw "Type of visual image is not recognized.";
    }

    // Удаляем связывающую структуру
    delete ea;
}

    

template void PhysicsPort< EntityRTPortFD >::bodyDestructorCallback(
    const NewtonBody* body
);



    
    
    
template< typename C >
void PhysicsPort< C >::bodyLeaveWorldCallback(
    const NewtonBody* body,
    int threadIndex
) {
    //std::cout << "PhysicsPort::bodyLeaveWorldCallback()" << std::endl;

    // Удаляем физический образ
    NewtonWorld* nw = NewtonBodyGetWorld( body );
    NewtonDestroyBody( nw, body );
    // Об удалении визуального образа и связывающей структуры
    // позаботится bodyDestructorCallback()
}



template void PhysicsPort< EntityRTPortFD >::bodyLeaveWorldCallback(
    const NewtonBody* body,
    int threadIndex
);







template< typename C >
void PhysicsPort< C >::bodyTransformCallback(
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
    // Работаем в одной плоскости (XZ):
    //   - координату 'y' всегда сохраняем нулевой
    //   - ? вращение по оси OX недопустимо
    // @see О матрице > 
    const Ogre::Matrix4 mat(
        tm[0], tm[4], tm[8],  tm[12],
        tm[1], tm[5], tm[9],  0.0f,
        tm[2], tm[6], tm[10], tm[14],
        tm[3], tm[7], tm[11], tm[15]
    );

    auto ea = search( body );
    switch ( ea->type ) {
        case InitEntity::TYPE_FLAT : {
            auto b = static_cast< Ogre::Billboard* >( ea->visual.b );
            b->setPosition( mat.getTrans() );
            b->setRotation( -mat.extractQuaternion().getYaw() );
            break;
        }
        case InitEntity::TYPE_VOLUME : {
            auto sn = static_cast< Ogre::SceneNode* >( ea->visual.sn );
            sn->setPosition( mat.getTrans() ); 
            sn->setOrientation( mat.extractQuaternion() );
            // @todo optimize Блокировать вращение прямо в конструкторе?
            //bodySceneNode->pitch( Ogre::Radian( 0.0f ) );
            break;
        }
        default :
            throw "Type of visual image is not recognized.";
    }

}



template void PhysicsPort< EntityRTPortFD >::bodyTransformCallback(
    const NewtonBody* body,
    const dFloat* tm,
    int threadIndex
);
