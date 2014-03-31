#include "SamplePlugin.h"
#include "Combat.h"
#include "helper.h"


#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;


extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_Combat;
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




void Sample_Combat::setupContent() {

	setlocale( LC_ALL, "Russian" );
    // Для разделителя '.' вместо ','
    setlocale( LC_NUMERIC, "C" );


    // Можем захотеть воспользоваться Tray::p() из любого класса
    helper::Tray::init( mTrayMgr );


    // setup some basic lighting for our scene
    //mSceneMgr->setAmbientLight( ColourValue::White );
    mSceneMgr->setAmbientLight( ColourValue( 0.5f, 0.5f, 0.5f ) );
    //mSceneMgr->createLight()->setPosition( 0.0f, 0.0f, 0.0f );
    

    // Строим игровой мир

    // Создаём порт для моделирования взаимодействий в реальном времени
    realtimeWorld = std::shared_ptr< RealtimePhysicsPort >( new RealtimePhysicsPort(
        mSceneMgr,
        // Размер порта, cм
        d::size3d_t( 1000.0f, 1000.0f, 1000.0f ) * 100.0f,
        // Длительность одного пульса порта, с
        1.0f
    ) );

#if 1
    // Добавляем в порт сущности
    //auto human = std::shared_ptr< HumanLB >( new HumanLB() );
    const auto human = HumanLB();
    realtimeWorld->set( human, d::ZERO3D, 100.0f );

#else
    // Добавляем много объектов
    // Отчёт по производительности:
    //   10 x 10 x 5 = 500 фигур: 7.5-10 fps
    const size_t N = 10;
    const size_t M = 10;
    const size_t L = 5;
    for (size_t k = 0; k < L; ++k) {
        for (size_t j = 0; j < M; ++j) {
            for (size_t i = 0; i < N; ++i) {
                const auto human = HumanLB();
                const float x = ((float)i - (float)(N / 2)) * 150.0f;
                const float y = ((float)j - (float)(M / 2)) * 200.0f;
                const float z = ((float)k - (float)(L / 2)) * 400.0f;
                const float mass = 100.0;
                realtimeWorld->set( h,  d::coord3d_t( x, y, z ),  mass );
            }
        }
    }

#endif


    /* @test
    // (!) Нельзя брать элемент, который был передан выше в set(), т.к.
    // порт создаёт копию сущности и работает исключительно с ней.
    //const d::coord3d_t c = human[0]; - будем получать одни и те же координаты
    const auto humanInWorld = realtimeWorld->get( human.uid );
    const d::coord3d_t c = (*humanInWorld)[10];
    std::cout << c << std::endl;
    */

    /* @test
    const auto humanInWorld = realtimeWorld->get( human.uid );
    humanInWorld->set( 2, d::coord3d_t( 500.0f, 50.0f, 200.0f ) );
    */


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
        -700.0f,
        200.0f
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

    mCameraMan->setTopSpeed( 1000 );

    setDragLook( true );


    // Set default mipmap level (NB some APIs ignore this)
    //TextureManager::getSingleton().setDefaultNumMipmaps( 5 );


    // Показываем образ
    //showMap();


    // Открываем панель управления
    setupControls();

}





void Sample_Combat::setupControls() {

    mTrayMgr->showCursor();
}





void Sample_Combat::cleanupContent() {
    std::cout << "Combat::cleanupContent() start" << std::endl;

    realtimeWorld.reset();

    std::cout << "Combat::cleanupContent() stop" << std::endl;
}






bool Sample_Combat::frameRenderingQueued( const Ogre::FrameEvent& evt ) {

    realtimeWorld->pulse( 1 );

    return SdkSample::frameRenderingQueued( evt );
}




bool Sample_Combat::keyPressed( const OIS::KeyEvent& evt ) {
    
    // ...

    return SdkSample::keyPressed( evt );
}





void Sample_Combat::sliderMoved( Slider* slider ) {
}




void Sample_Combat::checkBoxToggled( CheckBox* box ) {
}





void Sample_Combat::showMap() const {
}







void Sample_Combat::hideMap() const {
    // @see showMap()
    // ...
}
