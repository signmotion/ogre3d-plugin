#include "SamplePlugin.h"
#include "Vizi3D.h"
#include "helper.h"


#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;


extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_Vizi3D;
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




void Sample_Vizi3D::setupContent() {

	setlocale( LC_ALL, "Russian" );
    // Для разделителя '.' вместо ','
    setlocale( LC_NUMERIC, "C" );


    // ? Отключаем сглаживание текстур
    // @source http://www.ogre3d.ru/forum/viewtopic.php?f=8&t=4638
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering( Ogre::TFO_NONE );


    // Можем захотеть воспользоваться Tray::p() из любого класса
    helper::Tray::init( mTrayMgr );


    // setup some basic lighting for our scene
    //mSceneMgr->setAmbientLight( ColourValue::White );
    mSceneMgr->setAmbientLight( ColourValue( 0.5f, 0.5f, 0.5f ) );
    //mSceneMgr->createLight()->setPosition( 0.0f, 0.0f, 0.0f );
    

    // Строим игровой мир
    world = std::shared_ptr< Vizi3DWorld >( new Vizi3DWorld(
        "D:/Projects/workspace/ogre3d/Vizi3D/test/data/world/a",
        mSceneMgr
    ) );
    // Позицию наблюдателя задаём в блоках мира
    // (мировая позиция = observer * world.scale)
    const d::coordInt3d_t observer = d::coordInt3d_t( 0, 0, 0 );
    world->load( observer );


    // setup viewport
    mViewport->setBackgroundColour( Ogre::ColourValue::Black );
    mViewport->setDimensions( 0.0f, 0.0f, 1.0f, 1.0f );
    mViewport->setShadowsEnabled( true );
    mViewport->setOverlaysEnabled( true );
    //mViewport->setClearEveryFrame( true );
    

    // setup camera
    mCameraMan->setStyle( CS_FREELOOK );

    // axis Z direct to up, X - right, Y - forward 
    // Смотрим на плоскость XY (центр - слева внизу)
    mCamera->pitch( Ogre::Degree( 90.0f ) );
    //mCamera->setFixedYawAxis( true, Ogre::Vector3::UNIT_X );
    mCamera->setPosition(
        0.0f,
        -1500.0f,
        500.0f
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





void Sample_Vizi3D::setupControls() {

    mTrayMgr->showCursor();
}





void Sample_Vizi3D::cleanupContent() {
    std::cout << "Vizi3D::cleanupContent() start" << std::endl;

    world.reset();

    std::cout << "Vizi3D::cleanupContent() stop" << std::endl;
}






bool Sample_Vizi3D::frameRenderingQueued( const Ogre::FrameEvent& evt ) {

    world->drawNext();

    return SdkSample::frameRenderingQueued( evt );
}




bool Sample_Vizi3D::keyPressed( const OIS::KeyEvent& evt ) {
    
    // ...

    return SdkSample::keyPressed( evt );
}





void Sample_Vizi3D::sliderMoved( Slider* slider ) {
}




void Sample_Vizi3D::checkBoxToggled( CheckBox* box ) {
}





void Sample_Vizi3D::showMap() const {
}







void Sample_Vizi3D::hideMap() const {
    // @see showMap()
    // ...
}
