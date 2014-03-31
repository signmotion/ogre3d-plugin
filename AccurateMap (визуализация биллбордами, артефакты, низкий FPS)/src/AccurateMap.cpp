#include "SamplePlugin.h"
#include "AccurateMap.h"
#include "helper.h"


#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;


extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_AccurateMap;
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




void Sample_AccurateMap::setupContent() {

	setlocale( LC_ALL, "Russian" );
    // Для разделителя '.' вместо ','
    setlocale( LC_NUMERIC, "C" );


    // Можем захотеть воспользоваться Tray::p() из любого класса
    helper::Tray::init( mTrayMgr );


    // setup some basic lighting for our scene
    //mSceneMgr->setAmbientLight( ColourValue::White );
    mSceneMgr->setAmbientLight( ColourValue( 0.5f, 0.5f, 0.5f ) );
    //mSceneMgr->createLight()->setPosition( 0.0f, 0.0f, 0.0f );
    

    // По карте высот построим доксельную карту
    /*
    worldSize = d::size3d_t(
        // Реальный размер мира по трём осям, км ->
        528.0f,  528.0f,  528.0f
        // -> переводим в метры
    ) * 1000.0f;
    sourceFileHM = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/media/map/a/"
      //+ "elevation-with-water.png";
      + "elevation-without-water.png";
    */
    // Реальный размер мира по трём осям, км
    worldSize = d::size3d_t( 8.0f,  8.0f,  8.0f );
    sourceFileHM = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-16x16-gradient.png";
    // Мир
    try {
        currentMap = std::shared_ptr< MapDoxel >(
            new MapDoxel( sourceFileHM, worldSize, mSceneMgr ) 
        );
    } catch ( ... ) {
        throw "The map of doxel is not created.";
        //std::cout << ex.what() << std::endl;
    }


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
        -1000.0f,
        // Глядим на уровне моря
        0.0f
    );

    // @info
    // if far/near>2000, you can get z buffer problem.
    // eg : far/near = 10000/5 = 2000 . it's ok.
    // If (far/near)>2000 then you will likely get 'z fighting' issues.
    // @source http://www.ogre3d.org/tikiwiki/MadMarx+Tutorial+3
    mCamera->setNearClipDistance( 0.01f );
	const bool ifp = mRoot->getRenderSystem()->getCapabilities()->hasCapability( RSC_INFINITE_FAR_PLANE );
    mCamera->setFarClipDistance( ifp ? 0.0f : 100000.0f );

    mCamera->setRenderingDistance( 0.0f );
    mCamera->setUseRenderingDistance( false );

    mCamera->setAspectRatio(
        (float)( mViewport->getActualWidth() ) / (float)( mViewport->getActualHeight() )
	);
    //mCamera->setAutoAspectRatio( true );

    mCamera->setProjectionType( Ogre::PT_PERSPECTIVE );
    mCamera->setPolygonMode( Ogre::PM_SOLID );

    mCameraMan->setTopSpeed( 1500 );

    setDragLook( true );


    // Set default mipmap level (NB some APIs ignore this)
    //TextureManager::getSingleton().setDefaultNumMipmaps( 5 );


    // Показываем образ
    showLowHM();
    showMap();


    // Открываем панель управления
    setupControls();

}





void Sample_AccurateMap::setupControls() {

    assert( currentMap && "Карта должна быть инициализирована." );

    const float PW = 200.0f;
    const float FW = 60.0f;

    // Уровни прорисовки карты
    const int minDetailedLevel = 0;
    const int maxDetailedLevel = 5;

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "DetailedLevel", "Detailed", PW, FW,
        minDetailedLevel, maxDetailedLevel,  maxDetailedLevel - minDetailedLevel + 1
    )->setValue( 0 );

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "MaxCountDoxelForVisual", "Count of doxel", PW, FW,
        0, 70,  70 - 0 + 1
    )->setValue( 0 );


    mTrayMgr->showCursor();
}





void Sample_AccurateMap::cleanupContent() {
    std::cout << "AccurateMap::cleanupContent() start" << std::endl;

    currentMap.reset();

    std::cout << "AccurateMap::cleanupContent() stop" << std::endl;
}






bool Sample_AccurateMap::keyPressed( const OIS::KeyEvent& evt ) {
    
    // ...

    return SdkSample::keyPressed( evt );
}





void Sample_AccurateMap::sliderMoved( Slider* slider ) {

    if (slider->getName() == "DetailedLevel") {
        detailedLevel = (size_t)slider->getValue();
    }

    if (slider->getName() == "MaxCountDoxelForVisual") {
        maxCountDoxelForVisual = (size_t)slider->getValue();
    }


    showMap();
}




void Sample_AccurateMap::checkBoxToggled( CheckBox* box ) {

}




void Sample_AccurateMap::showLowHM() const {

    assert( !sourceFileHM.empty() && "Файл-источник карты высот должен быть указан." );
    assert( !d::empty( worldSize ) && "Размер мира должен быть указан." );

    const std::string dynamicSceneName = "LowHM::Dynamic::AccurateMap";
    Ogre::SceneNode* dynamicScene = nullptr;
    try {
        dynamicScene = static_cast< Ogre::SceneNode* >(
            mSceneMgr->getRootSceneNode()->getChild( dynamicSceneName )
        );
    } catch ( ... ) {
        dynamicScene = mSceneMgr->getRootSceneNode()->createChildSceneNode( dynamicSceneName );
    }
    dynamicScene->removeAndDestroyAllChildren();

    const std::string staticSceneName = "LowHM::Static::AccurateMap";
    Ogre::StaticGeometry* staticScene = nullptr;
    try {
        staticScene = mSceneMgr->getStaticGeometry( staticSceneName );
    } catch ( ... ) {
        staticScene = mSceneMgr->createStaticGeometry( staticSceneName );
    }
    staticScene->reset();


#if 0
// - @todo
    // Рисуем низкодетализированную карту
    const auto lowMesh = Sample_AccurateMap::lowMesh( mSceneMgr, worldSize );
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
    //lowScene->setScale( Ogre::Vector3( SCALE ) );
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

#endif

}





void Sample_AccurateMap::hideLowHM() const {
    // @see showLowHM()
    const std::string dynamicSceneName = "LowHM::Dynamic::AccurateMap";
    try {
        mSceneMgr->destroySceneNode( dynamicSceneName );
    } catch ( ... ) {
    }
    const std::string staticSceneName = "LowHM::Static::AccurateMap";
    try {
        mSceneMgr->destroyStaticGeometry( staticSceneName );
    } catch ( ... ) {
    }
}












void Sample_AccurateMap::showMap() const {

    assert( currentMap && "Карта должна быть инициализирована." );

    /* - @todo Позже.
    currentMap->draw(
        d::box3d_t(
            d::coord3d_t( -2.0f, -2.0f, -2.0f ) * 1000.0f,
            d::coord3d_t(  2.0f,  2.0f,  2.0f ) * 1000.0f
        )
    );
    */
    currentMap->clearCanvas();
    currentMap->drawChild( d::ZERO3D, detailedLevel, maxCountDoxelForVisual, false );

}







void Sample_AccurateMap::hideMap() const {
    // @see showMap()
    currentMap->clearCanvas();
}





#if 0
// - Не используется.
Ogre::MeshPtr Sample_AccurateMap::lowMesh(
    Ogre::SceneManager* sm,
    const size3d_t& worldSize
) const {

    assert( !sourceFileHM.empty() && "Файл-источник карты высот должен быть указан." );

    // Используем кеш
    if ( !lowMesh_.isNull() ) {
        return lowMesh_;
    }

    // Получаем данные из PNG-карты высот
    if ( !lowMesh_.isNull() ) {
        const size_t maxDepth = 0;
        const size_t seaLevel = 127;
        const size_t maxHeight = 255;
        hmImage_ = loadHMImage(
            sourceFileHM,
            boxInt2d_t( ZEROINT2D, ZEROINT2D ),
            maxDepth,
            seaLevel,
            maxHeight
        );
    }


    // @example http://www.ogre3d.org/tikiwiki/Heightmap+to+Mesh+by+MOGRE

    auto mesh = Ogre::MeshManager::getSingleton().createManual( sourceFileHM, "General" );
    auto sub = mesh->createSubMesh();


    // Собираем вершины
    const sizeInt2d_t mapSize = hmImage_.first;
    const size_t a = mapSize.get<0>();
    const size_t b = mapSize.get<1>();
    const size_t AB = a * b;
    const size_t vbufCount = 3 * 2 * AB;
    float* vertices = new float[vbufCount];
    // Центр карты (аналог hm_t::mapCenter)
    const float aHalf = (float)a / 2.0f;
    const float bHalf = (float)b / 2.0f;
    float hMin = 0;
    float hMax = 0;
    const float kx = worldSize.get<0>() / (float)a;
    const float ky = worldSize.get<1>() / (float)b;
    const float kz = worldSize.get<2>() / 2.0f;
    size_t t = 0;
    for (size_t j = 0; j < b; ++j) {
        const size_t ja = j * a;
        for (size_t i = 0; i < a; ++i) {
            // Высота лежит в диапазоне [-1.0; 1.0]
            const float height = hmImage_.second.get()[ i + ja ];

            // Центром меша делаем координату (0; 0; 0)
            const auto vc = Ogre::Vector3(
                ((float)i - aHalf) * kx,
                ((float)j - bHalf) * ky,
                height * kz
            );
            const Ogre::Vector3 nc = vc.normalisedCopy();
            vertices[t]     = vc.x;
            vertices[t + 1] = vc.y;
            vertices[t + 2] = vc.z;
            vertices[t + 3] = nc.x;
            vertices[t + 4] = nc.y;
            vertices[t + 5] = nc.z;
            if (hMin > vc.z) {
                hMin = vc.z;
            }
            if (hMax < vc.z) {
                hMax = vc.z;
            }
            t += 6;
        }
    }


    // Create vertex data structure for vertices shared between submeshes
    mesh->sharedVertexData = new Ogre::VertexData();
    mesh->sharedVertexData->vertexCount = AB;
 
    // Create declaration (memory format) of vertex data
    Ogre::VertexDeclaration* decl = mesh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;

    // 1st buffer
    decl->addElement(
        0, offset,
        Ogre::VET_FLOAT3,
        Ogre::VES_POSITION
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    decl->addElement(
        0, offset,
        Ogre::VET_FLOAT3,
        Ogre::VES_NORMAL
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    // Allocate vertex buffer of the requested number of vertices (vertexCount)
    // and bytes per vertex (offset)
    Ogre::HardwareVertexBufferSharedPtr vbuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            offset,
            mesh->sharedVertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
        );
    // Upload the vertex data to the card
    vbuf->writeData( 0, vbuf->getSizeInBytes(), vertices, true );
 
    // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    mesh->sharedVertexData->vertexBufferBinding->setBinding( 0, vbuf );
 
    
    // Vertex color
    size_t* color = new size_t[AB];
    for (size_t i = 0; i < AB; ++i) {
        color[i] = 0xFFFFFFFF;
    }

    // Строим индексы
    std::vector< size_t >  index;
    size_t jf = 0;
    for (size_t j = 0; j < b - 1; ++j) {
        for (size_t i = 0; i < a; ++i) {
            if ((i - 1) >= 0) {
                index.push_back( jf - 1 );
                index.push_back( jf );
                index.push_back( a + jf );
            }
            if ((i + 1) <= a) {
                index.push_back( jf );
                index.push_back( a + 1 + jf );
                index.push_back( a + jf );                       
            }
            jf++;
        }
    }

    const size_t indexSize = index.size();
    size_t* face = new size_t[ indexSize ];
    for (size_t i = 0; i < indexSize; ++i) {
        face[i] = index[i];
    }
 
    // 2nd buffer
    offset = 0;
    decl->addElement(
        1, offset,
        Ogre::VET_COLOUR,
        Ogre::VES_DIFFUSE
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_COLOUR );
    // Allocate vertex buffer of the requested number of vertices (vertexCount)
    // and bytes per vertex (offset)
    vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        offset,
        mesh->sharedVertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
    );
    // Upload the vertex data to the card
    vbuf->writeData( 0, vbuf->getSizeInBytes(), color, true );
 
    // Set vertex buffer binding so buffer 1 is bound to our colour buffer
    mesh->sharedVertexData->vertexBufferBinding->setBinding( 1, vbuf );
 
    /// Allocate index buffer of the requested number of vertices (ibufCount)
    Ogre::HardwareIndexBufferSharedPtr ibuf =
        Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_32BIT,
            indexSize,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
        );
 
    // Upload the index data to the card
    ibuf->writeData( 0, ibuf->getSizeInBytes(), face, true );
 
    /// Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = indexSize;
    sub->indexData->indexStart = 0;
 

    // Set bounding information (for culling)
    // (!) Если границу не задать или задать не правильно, меш будет исчезать
    // как только граница выйдет из области видимости камеры.
    // Вычисляем коробку и радиус для списка вершин
    const float xMax = aHalf * kx;
    const float yMax = bHalf * ky;
    const auto axisAlignedBox = Ogre::AxisAlignedBox(
        -xMax, -yMax, hMin,
        xMax, yMax, hMax
    );
    mesh->_setBounds( axisAlignedBox );
    const float radius = std::max( std::max( xMax, yMax ), hMax );
    mesh->_setBoundingSphereRadius( radius );


    // Notify - Mesh object that it has been loaded
    mesh->load();


    // Убираем за собой
    delete[] face;
    delete[] color;
    delete[] vertices;

    // Кешируем результат
    lowMesh_ = mesh;

    return mesh;
}

#endif
