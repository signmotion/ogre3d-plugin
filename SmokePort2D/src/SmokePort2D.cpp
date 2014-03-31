#include "SamplePlugin.h"
#include "SmokePort2D.h"
#include "helper.h"
#include "PulseActor.h"


#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;


extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_SmokePort2D;
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







void Sample_SmokePort2D::setupContent() {

    // Можем захотеть воспользоваться Tray::p() из любого класса
    helper::Tray::init( mTrayMgr );


    // setup some basic lighting for our scene
    mSceneMgr->setAmbientLight( ColourValue::White );
    //mSceneMgr->createLight()->setPosition( 100, 100, 100 );
    

    // Параметры визуализации
    const float visualScale = 100.0f;

    // Создаём порт, моделирующий распространение дыма
    const size_t N = 30;
    const size_t M = 15;
    smoke = std::shared_ptr< GasCellPort >( new GasCellPort(
        mSceneMgr,
        visualScale,
        // Размер порта, ячейка
        size2d_t( N, M ),
        // Размер ячейки, м
        1.0f
    ) );

    // Заполняем порт
    GasFD sc1;
    sc1.uid = 101;
    sc1.pressure = 100.0f;
    sc1.deltaPressure = 0.0f;
    for (size_t z = 0; z < 5; ++z) {
        assert(z < M);
        for (size_t x = 0; x < 10; ++x) {
            assert(x < N);
            const coord2d_t coord( x, z );
            smoke->set( coord, sc1 );
        }
    }


    // setup camera
	mCameraMan->setStyle( CS_FREELOOK );
	// axis Z direct to up, X - right, Y - forward
    mCamera->pitch( Ogre::Degree( 90 ) );
    mCamera->setFixedYawAxis( true, Ogre::Vector3::UNIT_Z );
    mCamera->setPosition( Ogre::Vector3( 0.0f, -1000.0f, 0.0f ) );

    setDragLook( true );


    // Метод pulsePort() вызывается при смене характеристик дыма, поэтому
    // если setupControls() меняет характеристики, pulsePort() можно упустить
    //pulsePort( 0 );
    //setupControls();


    // Запускаем пульсацию
    // Задействуем Theron
    std::cout << "Init Theron::Framework()" << std::endl;
    flicker = std::shared_ptr< Theron::Framework >( new Theron::Framework( 1 ) );
    std::cout << "Prepare 'PulseActor'..." << std::endl;
    pulseActor = flicker->CreateActor< PulseActor >();
    pulseActor.Push(
        PortMessage( smoke.get() ),
        Theron::Address::Null()
    );
    std::cout << "'PulseActor' is started." << std::endl;
    // Не ждём завершения

}






#if 0
// - Пульсацию порта взял на себя актор.
void Sample_SmokePort2D::pulsePort( int tact ) {
    assert( smoke && "Порт не инициализирован.");

    try {
        smoke->pulse( tact );

    } catch ( const std::exception& ex ) {
        if ( helper::Tray::initiated() ) {
            helper::Tray::p( "Throw", std::string( ex.what() ) );
            return;
        }
        throw ex;
    /*
    } catch ( ... ) {
        paramsPanel->setParamValue( "Throw", "Undefined exception." );
    */
    }
}
#endif





void Sample_SmokePort2D::setupControls() {

    assert( smoke && "Мир должен быть инициализирован до вызова этого метода." );
    
    mTrayMgr->showCursor();

}





void Sample_SmokePort2D::cleanupContent() {
    std::cout << "SmokePort2D::cleanupContent() start" << std::endl;

    flicker.reset();
    smoke.reset();

    std::cout << "SmokePort2D::cleanupContent() stop" << std::endl;
}






void Sample_SmokePort2D::sliderMoved( Slider* slider ) {

    //pulsePort( 0 );

}




void Sample_SmokePort2D::checkBoxToggled( CheckBox* box ) {

    //pulsePort( 0 );

}
