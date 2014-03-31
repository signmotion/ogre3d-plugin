#include "SamplePlugin.h"
#include "Physics2D.h"
#include "helper.h"


#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;


extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_Physics2D;
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




// Приказ потоку остановиться
bool Sample_Physics2D::interruptPulseThread = false;



void Sample_Physics2D::setupContent() {

	setlocale( LC_ALL, "Russian" );
    // Для разделителя '.' вместо ','
    setlocale( LC_NUMERIC, "C" );


    // Можем захотеть воспользоваться Tray::p() из любого класса
    helper::Tray::init( mTrayMgr );


    // setup some basic lighting for our scene
    //mSceneMgr->setAmbientLight( ColourValue::White );
    mSceneMgr->setAmbientLight( ColourValue( 0.5f, 0.5f, 0.5f ) );
    mSceneMgr->createLight()->setPosition( 0.0f, 0.0f, 0.0f );
    

    // Параметры визуализации
    /* - Отказываемся от масштабирования, чтобы не морочиться с согласованием
         форм сущности и корректированием камеры.
    const float visualScale = 1.0f;
    */

    // Создаём порт для моделирования взаимодействий в реальном времени
    realtimeWorld = std::shared_ptr< RealtimePhysicsPort >( new RealtimePhysicsPort(
        mSceneMgr,
        // Размер порта, cм
        size2d_t( 1000.0f, 1000.0f ) * 100.0f
    ) );


    // Заполняем порт сущностями
    VolumeInitEntity ieHull ( InitEntity::FORM_SPHERE );
    ieHull.uid = "T1";
    ieHull.boundingRadius = 3.0f * 100.0f;
    ieHull.mass = 200.0f;
    ieHull.coord = coord2d_t( 0.0f, 80.0f ) * 100.0f;
    ieHull.model = "hull-03.mesh";
    ieHull.force = vector2d_t( 0.0f, 0.0f );
    assert( ieHull.correct() );
    realtimeWorld->set( &ieHull );

    /* - @test
    ie.uid = "T2";
    ie.boundingRadius = 6.0f * 100.0f;
    ie.mass = 0.0f;
    ie.coord = coord2d_t( 0.0f, 50.0f ) * 100.0f;
    ie.model = "hull-03.mesh";
    ie.force = vector2d_t( 0.0f, 1000.0f );
    realtimeWorld->set( ie );
    */

    // Добавляем пол
    VolumeInitEntity ieFloor( InitEntity::FORM_BOX );
    ieFloor.uid = "Floor";
    ieFloor.size = size3d_t( 500.0f, 3.0f, 10.0f ) * 100.0f;
    // Пол - неподвижный
    ieFloor.mass = 0.0f;
    ieFloor.coord = coord2d_t( 0.0f, -50.0f ) * 100.0f;
    ieFloor.model = "floor.mesh";
    ieFloor.force = vector2d_t( 0.0f, 0.0f );
    assert( ieFloor.correct() );
    realtimeWorld->set( &ieFloor );


    // setup viewport
    mViewport->setBackgroundColour( Ogre::ColourValue::Black );
    mViewport->setDimensions( 0.0f, 0.0f, 1.0f, 1.0f );
    mViewport->setShadowsEnabled( true );
    mViewport->setOverlaysEnabled( true );
    //mViewport->setClearEveryFrame( true );
    

    // setup camera
    mCameraMan->setStyle( CS_MANUAL );

    // axis Z direct to up, X - right, Y - forward
    mCamera->pitch( Ogre::Degree( 90 ) );
    mCamera->setFixedYawAxis( true, Ogre::Vector3::UNIT_X );
    mCamera->setPosition( Ogre::Vector3( 0.0f, -200.0f, 0.0f ) * 100.0f );

    // @info
    // if far/near>2000, you can get z buffer problem.
    // eg : far/near = 10000/5 = 2000 . it's ok.
    // If (far/near)>2000 then you will likely get 'z fighting' issues.
    // @source http://www.ogre3d.org/tikiwiki/MadMarx+Tutorial+3
    //mCamera->setNearClipDistance( 0.1f );
    //mCamera->setFarClipDistance( 1000.0f * 100.0f );
    /*
    mCamera->setAspectRatio(
        (float)( mViewport->getActualWidth() ) / (float)( mViewport->getActualHeight() )
	);
    */
    mCamera->setAutoAspectRatio( true );

    mCamera->setRenderingDistance( 0.0f );
    mCamera->setUseRenderingDistance( false );
    mCamera->setProjectionType( Ogre::PT_PERSPECTIVE );

    mCameraMan->setTopSpeed( 500 );

    setDragLook( false );


    // Рисуем сетку
    realtimeWorld->grid( 10 );

    // Показываем начальный образ и запускаем пульсацию
    realtimeWorld->pulse( 0, 0.0f );
    const float timestep = 1.0f / 60.0f;
    pulseThread = boost::thread(
        &Sample_Physics2D::pulse,
        realtimeWorld.get(), timestep
    );
    // Не ждём завершения

}





void Sample_Physics2D::setupControls() {

    assert( realtimeWorld && "Мир должен быть инициализирован до вызова этого метода." );
    
    mTrayMgr->showCursor();

}





void Sample_Physics2D::cleanupContent() {
    std::cout << "Physics2D::cleanupContent() start" << std::endl;

    /* - Поток продолжает работать. Заменено. См. ниже.
    pulseThread.interrupt();
    pulseThread.join();
    */
    interruptPulseThread = true;
    pulseThread.join();

    realtimeWorld.reset();

    std::cout << "Physics2D::cleanupContent() stop" << std::endl;
}






bool Sample_Physics2D::keyPressed( const OIS::KeyEvent& evt ) {

    // Выпускаем дронов
    if (evt.key == OIS::KC_1) {
        //realtimeWorld->pushDroneMesh( "T1" );
        realtimeWorld->pushDroneBillboard( "T1" );
        return true;
    }

    return SdkSample::keyPressed( evt );
}





void Sample_Physics2D::sliderMoved( Slider* slider ) {

    //realtimeWorld->pulse( 0 );

}




void Sample_Physics2D::checkBoxToggled( CheckBox* box ) {

    //realtimeWorld->pulse( 0 );

}





void Sample_Physics2D::pulse( Port* port, float timestep ) {
    assert( port );
    assert( (timestep > 0.0f) && (timestep <= 1.0f) );

    std::cout << "Physics2D::pulse() start" << std::endl;

    // Шаг времени в мс для упрощения расчётов
    const auto timestepInMs = (unsigned long)(timestep * 1000.0f);

    for ( ; ; ) {
        //std::cout << "Physics2D::pulse() next state" << std::endl;

        // Пришла команда завершить работу потока?
        if ( interruptPulseThread ) {
            break;
        }

        // На случай, если порт был неосторожно (не через
        // 'interruptPulseThread') ликвидирован
        if ( !port ) {
            break;
        }

        // Засекаем время выполнения, чтобы ниже сделать паузу
        const auto t0 = (unsigned long)clock();

        /* - Завершается исключением. Работаем через 'interruptPulseThread'.
        // Оборачиваем в try/catch, т.к. порт может быть закрыт и
        // простая проверка if ( port ) здесь не помогает
        try {
            // Переводим порт в новое состояние, только если он свободен
            if ( port && port->isFree() ) {
                // port->isFree() - достаточно времени, чтобы удалить порт
                if ( port ) {
                    port->pulse( 1, timestep );
                }
            }
        } catch ( ... ) {
            // Корректное завершение работы
            break;
        }
        */
        port->pulse( 1, timestep );

        // Выдерживаем паузу перед след. циклом. Смотрим, чтобы расчёты выше
        // не оказались быстрее реальности.
        /* - Делаем проще. См. ниже.
        const auto t1 = (unsigned long)clock();
        const auto beOver = t1 - t0;
        if (beOver < stepTimeInMs) {
            const auto needRest = timestepInMs - beOver;
            std::cout << "Physics2D::pulse() Need rest " << needRest << std::endl;
            const boost::posix_time::milliseconds restTime( needRest );
            boost::this_thread::sleep( restTime );
        }
        */
        const auto needTime = t0 + timestepInMs - 11;
        while (needTime > (unsigned long)clock()) {
        }

    }

    std::cout << "Physics2D::pulse() stop" << std::endl;
}
