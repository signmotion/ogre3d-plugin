#include "SamplePlugin.h"
#include "AccurateHeightmap.h"
#include "helper.h"


#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;


extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_AccurateHeightmap;
	sp = OGRE_NEW SamplePlugin(s->getInfo()["Title"] + " Sample");
	sp->addSample(s);
	Root::getSingleton().installPlugin(sp);
}


extern "C" _OgreSampleExport void dllStopPlugin()
{
	Root::getSingleton().uninstallPlugin(sp); 
	OGRE_DELETE sp;
	delete s;
}

#endif




void Sample_AccurateHeightmap::setupContent() {

	setlocale( LC_ALL, "Russian" );
    // Для разделителя '.' вместо ','
    setlocale( LC_NUMERIC, "C" );


    // Можем захотеть воспользоваться Tray::p() из любого класса
    helper::Tray::init( mTrayMgr );


    // setup some basic lighting for our scene
    //mSceneMgr->setAmbientLight( ColourValue::White );
    mSceneMgr->setAmbientLight( ColourValue( 0.5f, 0.5f, 0.5f ) );
    //mSceneMgr->createLight()->setPosition( 0.0f, 0.0f, 0.0f );
    

    // Инициализируем карту высот
    const std::string file = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateHeightmap/media/map/a/"
      //+ "elevation-with-water.png";
      + "elevation-without-water.png";
    const size_t maxDepth = 0;
    const size_t maxHeight = 255;
    const size_t seaLevel = (size_t)( (maxHeight - maxDepth) / 2 );
    // Размер одного пикселя карты высоты, м
    const size_t cellCount = 100;
    const size_t cellSize = 5;
    const size_t side = cellCount * cellSize;
    // Сколько метров содержится в одном пикселе карты высоты
    const float inPoint = side;
    // Максимальная разница между высотой и глубиной карты, м
    const float hd = 40.0f * 1000.0f;
    // Размер мира
    const size3d_t worldSize =
        size3d_t( (float)528 * inPoint,  (float)528 * inPoint,  hd );
    ahm = std::shared_ptr< AHeightmap >( new AHeightmap(
        file, maxDepth, seaLevel, maxHeight, worldSize
    ) );


    // setup viewport
    mViewport->setBackgroundColour( Ogre::ColourValue::Black );
    mViewport->setDimensions( 0.0f, 0.0f, 1.0f, 1.0f );
    mViewport->setShadowsEnabled( true );
    mViewport->setOverlaysEnabled( true );
    //mViewport->setClearEveryFrame( true );
    

    // setup camera
    mCameraMan->setStyle( CS_ORBIT );

    // axis Z direct to up, X - right, Y - forward 
    // Смотрим на плоскость XY (центр - слева внизу)
    mCamera->pitch( Ogre::Degree( 90.0f ) );
    //mCamera->setFixedYawAxis( true, Ogre::Vector3::UNIT_X );
    mCamera->setPosition(
        0.0f,
        //-worldSize.get<1>() / 2.0f * SCALE,
        0.0f,
        // Глядим на уровне моря
        0.0f
    );

    // @info
    // if far/near>2000, you can get z buffer problem.
    // eg : far/near = 10000/5 = 2000 . it's ok.
    // If (far/near)>2000 then you will likely get 'z fighting' issues.
    // @source http://www.ogre3d.org/tikiwiki/MadMarx+Tutorial+3
    mCamera->setNearClipDistance( 0.1f );
	const bool ifp = mRoot->getRenderSystem()->getCapabilities()->hasCapability( RSC_INFINITE_FAR_PLANE );
    mCamera->setFarClipDistance( ifp ? 0.0f : 100000.0f );

    mCamera->setRenderingDistance( 0.0f );
    mCamera->setUseRenderingDistance( false );

    mCamera->setAspectRatio(
        (float)( mViewport->getActualWidth() ) / (float)( mViewport->getActualHeight() )
	);
    //mCamera->setAutoAspectRatio( true );

    mCamera->setProjectionType( Ogre::PT_PERSPECTIVE );
    mCamera->setPolygonMode( Ogre::PM_WIREFRAME );

    mCameraMan->setTopSpeed( 2000 );

    setDragLook( true );


    // Set default mipmap level (NB some APIs ignore this)
    //TextureManager::getSingleton().setDefaultNumMipmaps( 5 );


    // Показываем образ
    showLowHM();
    showPointer();
    //showHightHM();


    // Открываем панель управления
    setupControls();

}





void Sample_AccurateHeightmap::setupControls() {

    assert( ahm && "Карта высот должна быть инициализирована." );

    const float PW = 200.0f;
    const float FW = 60.0f;

	mTrayMgr->createLabel( TL_TOPRIGHT, "PointerLabel", "Pointer", PW );

    const auto a = ahm->about();
    const auto fromX = -a.mapCenter.get<0>();
    const auto toX = a.mapSize.get<0>() - a.mapCenter.get<0>() - 1;
    const auto fromY = -a.mapCenter.get<1>();
    const auto toY = a.mapSize.get<1>() - a.mapCenter.get<1>() - 1;

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "XPointerSlider", "X", PW, FW,
        fromX, toX,  toX - fromX + 1
    )->setValue( pointer.get<0>() );

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "YPointerSlider", "Y", PW, FW,
        fromY, toY,  toY - fromY + 1
    )->setValue( pointer.get<1>() );

    mTrayMgr->createCheckBox(
        TL_TOPRIGHT, "LowMesh", "Low mesh", PW
    )->setChecked( true );

    mTrayMgr->createCheckBox(
        TL_TOPRIGHT, "HightMesh", "Hight mesh", PW
    )->setChecked( true );

    
    mTrayMgr->showCursor();

}





void Sample_AccurateHeightmap::cleanupContent() {
    std::cout << "AccurateHeightmap::cleanupContent() start" << std::endl;

    ahm.reset();

    std::cout << "AccurateHeightmap::cleanupContent() stop" << std::endl;
}






bool Sample_AccurateHeightmap::keyPressed( const OIS::KeyEvent& evt ) {
    
    if (evt.key == OIS::KC_SPACE) {
        showHightHM();
        static_cast< OgreBites::CheckBox* >( mTrayMgr->getWidget( "LowMesh" ) )
            ->setChecked( false, true );
        static_cast< OgreBites::CheckBox* >( mTrayMgr->getWidget( "HightMesh" ) )
            ->setChecked( true, false );
    }

    return SdkSample::keyPressed( evt );
}





void Sample_AccurateHeightmap::sliderMoved( Slider* slider ) {

    if (slider->getName() == "XPointerSlider") {
        pointer.get<0>() = (size_t)slider->getValue();
    }

    if (slider->getName() == "YPointerSlider") {
        pointer.get<1>() = (size_t)slider->getValue();
    }


    showPointer();
}




void Sample_AccurateHeightmap::checkBoxToggled( CheckBox* box ) {

    if (box->getName() == "LowMesh") {
        if ( box->isChecked() ) {
            showLowHM();
        } else {
            hideLowHM();
        }
    }

    if (box->getName() == "HightMesh") {
        if ( box->isChecked() ) {
            showHightHM();
        } else {
            hideHightHM();
        }
    }

}




void Sample_AccurateHeightmap::showLowHM() const {

    assert( ahm && "Карта высот должна быть инициализирована." );

    const std::string dynamicSceneName = "LowHM::Dynamic::AccurateHeightmap";
    Ogre::SceneNode* dynamicScene = nullptr;
    try {
        dynamicScene = static_cast< Ogre::SceneNode* >(
            mSceneMgr->getRootSceneNode()->getChild( dynamicSceneName )
        );
    } catch ( ... ) {
        dynamicScene = mSceneMgr->getRootSceneNode()->createChildSceneNode( dynamicSceneName );
    }
    dynamicScene->removeAndDestroyAllChildren();

    const std::string staticSceneName = "LowHM::Static::AccurateHeightmap";
    Ogre::StaticGeometry* staticScene = nullptr;
    try {
        staticScene = mSceneMgr->getStaticGeometry( staticSceneName );
    } catch ( ... ) {
        staticScene = mSceneMgr->createStaticGeometry( staticSceneName );
    }
    staticScene->reset();


    // Рисуем низкодетализированную карту
    const auto lowMesh = ahm->lowMesh( mSceneMgr );
    assert( !lowMesh.isNull() );
    auto lowEntity = mSceneMgr->createEntity( lowMesh->getName() );
    lowEntity->setCastShadows( false );
    lowEntity->setRenderingDistance( 0.0f );
    /*
    auto material = Ogre::MaterialManager::getSingleton().getByName( "accurateheightmap/test" );
    assert( !material.isNull() && "Не обнаружен материал." );
    lowEntity->setMaterial( material );
    */
#if 1
    // Динамическая модель
    auto lowScene = dynamicScene->createChildSceneNode( Ogre::Vector3::ZERO );
    lowScene->attachObject( lowEntity );
    lowScene->setScale( Ogre::Vector3( SCALE ) );
    lowScene->showBoundingBox( true );

#else
    // Статическая модель
    staticScene->setOrigin( Ogre::Vector3::ZERO );
    //staticScene->setRegionDimensions( lowEntity->getBoundingBox().getSize() * SCALE );
    staticScene->addEntity(
        lowEntity,
        Ogre::Vector3::ZERO, Ogre::Quaternion::ZERO, Ogre::Vector3( SCALE )
    );
    staticScene->build();
#endif

}





void Sample_AccurateHeightmap::hideLowHM() const {
    // @see showLowHM()
    const std::string dynamicSceneName = "LowHM::Dynamic::AccurateHeightmap";
    try {
        mSceneMgr->destroySceneNode( dynamicSceneName );
    } catch ( ... ) {
    }
    const std::string staticSceneName = "LowHM::Static::AccurateHeightmap";
    try {
        mSceneMgr->destroyStaticGeometry( staticSceneName );
    } catch ( ... ) {
    }
}






void Sample_AccurateHeightmap::showPointer() const {

    assert( ahm && "Карта высот должна быть инициализирована." );

    const std::string dynamicSceneName = "Pointer::Dynamic::AccurateHeightmap";
    Ogre::SceneNode* dynamicScene = nullptr;
    try {
        dynamicScene = static_cast< Ogre::SceneNode* >(
            mSceneMgr->getRootSceneNode()->getChild( dynamicSceneName )
        );
    } catch ( ... ) {
        dynamicScene = mSceneMgr->getRootSceneNode()->createChildSceneNode( dynamicSceneName );
    }
    //dynamicScene->removeAndDestroyAllChildren();
    dynamicScene->removeAllChildren();


    const float size = ahm->sizeCell();
    const float halfSize = size / 2.0f;

    // Рисуем курсор в центральной ячейке и ячейках вокруг
    const auto a = ahm->about();
    for (size_t k = 0; k <= 8; ++k) {
        const auto shift = helper::isc( k );
        const auto p = pointer + shift;
        const auto hmCoord = p + a.mapCenter;
        if ( helper::outsideMatrix( hmCoord, a.mapSize ) ) {
            // Не рисуем, если на границе
            continue;
        }

        // Высоту вершин курсора вычисляем с учётом высоты соседей
        // Порядок вершин см. helper::isc() = 5, 6, 7, 8
        const auto vdc = ahm->vertexDeltaCell( p );
        auto mo = mSceneMgr->createManualObject();
        //mo->setUseIdentityProjection( true );
        //mo->setUseIdentityView( true );
        mo->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP );
        mo->position( -halfSize,  -halfSize,  vdc[2] );
        mo->position(  halfSize,  -halfSize,  vdc[1] );
        mo->position(  halfSize,   halfSize,  vdc[0] );
        mo->position( -halfSize,   halfSize,  vdc[3] );
        mo->index( 0 );  mo->index( 1 );  mo->index( 2 );
        mo->index( 3 );  mo->index( 0 );
        mo->end();

        //Ogre::AxisAlignedBox aab;
        //aab.setInfinite();
        //mo->setBoundingBox( aab );
        mo->setCastShadows( false );
        mo->setRenderingDistance( 0.0f );

        const coord3d_t cc = ahm->coordCell( p );
        auto pointerScene = dynamicScene->createChildSceneNode( Ogre::Vector3(
            cc.get<0>(), cc.get<1>(), cc.get<2>()
        ) );
        pointerScene->createChildSceneNode()->attachObject( mo );

    } // for (size_t k = 0; k < 8; ++k)


    // В центральную ячейку (плато) дорисовываем нормали
    const coord3d_t cc = ahm->coordCell( pointer );
    const auto vnc = ahm->vertexNormalCell( pointer );
    for (auto itr = vnc.cbegin(); itr != vnc.cend(); ++itr) {
        const vector3d_t normal = *itr;

        auto mo = mSceneMgr->createManualObject();
        mo->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST );
        mo->position( 0.0f, 0.0f, 0.0f );
        mo->position( Ogre::Vector3( normal.get<0>(), normal.get<1>(), normal.get<2>() ) * size );
        mo->index( 0 );  mo->index( 1 );
        mo->end();

        mo->setCastShadows( false );
        mo->setRenderingDistance( 0.0f );

        const size_t k = std::distance( vnc.cbegin(), itr );
        //const coord2d_t shift = helper::isc( k );
        const coord2d_t shift = ZERO2D;
        auto normalScene = dynamicScene->createChildSceneNode( Ogre::Vector3(
            cc.get<0>() + shift.get<0>() * size / 4.0f,
            cc.get<1>() + shift.get<1>() * size / 4.0f,
            cc.get<2>()
        ) );
        normalScene->createChildSceneNode()->attachObject( mo );
    }

}






void Sample_AccurateHeightmap::showHightHM() const {

    assert( ahm && "Карта высот должна быть инициализирована." );

    const std::string dynamicSceneName = "HightHM::Dynamic::AccurateHeightmap";
    Ogre::SceneNode* dynamicScene = nullptr;
    try {
        dynamicScene = static_cast< Ogre::SceneNode* >(
            mSceneMgr->getRootSceneNode()->getChild( dynamicSceneName )
        );
    } catch ( ... ) {
        dynamicScene = mSceneMgr->getRootSceneNode()->createChildSceneNode( dynamicSceneName );
    }
    dynamicScene->removeAndDestroyAllChildren();

    const std::string staticSceneName = "HightHM::Static::AccurateHeightmap";
    Ogre::StaticGeometry* staticScene = nullptr;
    try {
        staticScene = mSceneMgr->getStaticGeometry( staticSceneName );
    } catch ( ... ) {
        staticScene = mSceneMgr->createStaticGeometry( staticSceneName );
    }
    staticScene->reset();


    // Рисуем выскодетализированную карту в самой ячейке и ячейках вокруг
    const auto a = ahm->about();
    for (size_t k = 0; k <= 8; ++k) {
        const auto shift = helper::isc( k );
        const auto p = pointer + shift;
        const auto hmCoord = p + a.mapCenter;
        if ( helper::outsideMatrix( hmCoord, a.mapSize ) ) {
            // Не рисуем, если на границе
            continue;
        }

        const auto r = ahm->hightMesh< 100 >( mSceneMgr, p );
        const Ogre::MeshPtr hightMesh = r.first;
        assert( !hightMesh.isNull() );

        // Покажем модель над курсором
        coord3d_t cc =
            ahm->coordCell( p )
          + coord3d_t(
              (float)shift.get<0>() * ahm->sizeCell() * (float)(100 - 2),
              (float)shift.get<1>() * ahm->sizeCell() * (float)(100 - 2),
              0.0f
        );
        cc.get<2>() = cc.get<2>() + a.worldSize.get<2>();
        const auto occ = Ogre::Vector3( cc.get<0>(), cc.get<1>(), cc.get<2>() );

        // Поверхность должна быть повёрнута перпендикулярно нормали
        const Ogre::Vector3 normal = Ogre::Vector3(
            r.second.get<0>(), r.second.get<1>(), r.second.get<2>()
        );
        const Ogre::Quaternion rotation = Vector3::UNIT_Z.getRotationTo( normal );
        // @test
        //const Ogre::Quaternion rotation = Ogre::Quaternion::IDENTITY;

        auto hightEntity = mSceneMgr->createEntity( hightMesh->getName() );
        hightEntity->setCastShadows( false );
        hightEntity->setRenderingDistance( 0.0f );
        auto material = Ogre::MaterialManager::getSingleton().getByName( "accurateheightmap/white" );
        assert( !material.isNull() && "Не обнаружен материал." );
        hightEntity->setMaterial( material );

#if 1
        // Динамическая модель
        auto hightScene = dynamicScene->createChildSceneNode( occ, rotation );
        hightScene->attachObject( hightEntity );
        hightScene->setScale( Ogre::Vector3( SCALE ) );
        hightScene->showBoundingBox( true );

#else
        // Статическая модель
        staticScene->setOrigin( Ogre::Vector3::ZERO );
        //staticScene->setRegionDimensions( lowEntity->getBoundingBox().getSize() * SCALE );
        staticScene->addEntity(
            hightEntity,
            occ, rotation, Ogre::Vector3( SCALE )
        );
        staticScene->build();
#endif
    } // for (size_t k = 0; k <= 8; ++k)

}







void Sample_AccurateHeightmap::hideHightHM() const {
    // @see showHightHM()
    const std::string dynamicSceneName = "HightHM::Dynamic::AccurateHeightmap";
    try {
        mSceneMgr->destroySceneNode( dynamicSceneName );
    } catch ( ... ) {
    }
    const std::string staticSceneName = "HightHM::Static::AccurateHeightmap";
    try {
        mSceneMgr->destroyStaticGeometry( staticSceneName );
    } catch ( ... ) {
    }
}
