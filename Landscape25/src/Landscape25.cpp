#include "SamplePlugin.h"
#include "Landscape25.h"
#include "helper.h"

using namespace Ogre;
using namespace OgreBites;

#ifndef OGRE_STATIC_LIB

SamplePlugin* sp;
Sample* s;


extern "C" _OgreSampleExport void dllStartPlugin()
{
	s = new Sample_Landscape25;
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







void Sample_Landscape25::setupContent() {

    // Можем захотеть воспользоваться Tray::p() из любого класса
    helper::Tray::init( mTrayMgr );


    // setup some basic lighting for our scene
    mSceneMgr->setAmbientLight( ColourValue::White );
    //mSceneMgr->createLight()->setPosition( 100, 100, 100 );
    

    // Параметры визуализации
    show.gradient = true;
    show.surface = true;
    show.trend = false;

    // Создаём биом
    biome = std::shared_ptr< Biome >( new SandDesertBiome(
        // Протяжённость мира, ось OX, м
        100,
        // Расстояние до горизонта, ось OZ, м
        1000,
        // Высота / глубина слоя: чем больше - тем больше
        20,
        // Количество слоёв
        1,
        // Количество точек перепада
        // (минимумально - 2: абсолютная равнина или уклон)
        3,
        // Зерно для формирования прочих характеристик биома
        1
    ) );
    // Настраиваем дополнительные характеристики
    biome->feature.natural = natural_t( 0.2f, 0.2f );


    // setup camera
	mCameraMan->setStyle( CS_FREELOOK );
	// axis Z direct to up, X - right, Y - forward
    mCamera->pitch( Ogre::Degree( 70 ) );
    mCamera->setFixedYawAxis( true, Ogre::Vector3::UNIT_Z );
    mCamera->setPosition( Ogre::Vector3( 500.0f, -1000.0f, 500.0f ) );

    setDragLook( true );


    // Метод showBiome() вызывается при смене характеристик биома, а
    // setupControls() меняет характеристики
    //showBiome();
    setupControls();

}







void Sample_Landscape25::showBiome() {
    try {
        showBiomeWithThrow();

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






void Sample_Landscape25::showBiomeWithThrow() {
    assert( biome && "Мир не найден.");

    const std::string sceneName = "SandDesertBiome";
    Ogre::SceneNode* scene = nullptr;
    try {
        scene = static_cast< Ogre::SceneNode* >(
            mSceneMgr->getRootSceneNode()->getChild( sceneName )
        );
    } catch ( ... ) {
        scene = mSceneMgr->getRootSceneNode()->createChildSceneNode( sceneName );
    }

    scene->removeAndDestroyAllChildren();

    const auto layer = biome->layer();
    //assert( (layer.size() > 0) && "Don't anything created for this biome." );
    for (auto itr = layer.cbegin(); itr != layer.cend(); ++itr) {
        auto l = *itr;
        /* - Заменено. См. ниже.
        auto sn = scene->createChildSceneNode();
        //const size_t index = std::distance( layer.cbegin(), itr );
        const size_t y = l->index * biome->distanceLayer();
        sn->setPosition( 0, y, 0 );
        // Т.к. вершины задаются в метрах, масштаба 1:1 может не хватать
        auto image = l->image(
            mSceneMgr, includeGradientPoint, includeSurfaceLine
        );
        sn->attachObject( image );
        */
        auto sn = scene->createChildSceneNode();
        sn->setScale( Ogre::Vector3( 10.0f ) );
        const size_t index = std::distance( layer.cbegin(), itr );
        const size_t y = index * biome->distanceLayer();
        sn->setPosition( 0, y, 0 );
        l->draw( mSceneMgr, sn, show );
    }


    /*
    // create the billboard for parallax scrolling
    // @source http://89.151.96.106/forums/viewtopic.php?f=10&t=60455

    // store for billboard sets
    std::unordered_map< std::string, Ogre::BillboardSet* >  bs;

    createBillboardSet( &bs, "desert" );

    bs["desert"]->createBillboard( 0, 0, 0 );
    */

}






void Sample_Landscape25::setupControls() {

    assert( biome && "Мир должен быть инициализирован до вызова этого метода." );

    const float PW = 200.0f;
    const float FW = 60.0f;

	mTrayMgr->createLabel( TL_TOPRIGHT, "BiomeLabel", "World", PW );

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "LengthSlider", "length", PW, FW,
        100, 10000, (10000 - 100) / 100 + 1
    )->setValue( biome->feature.length );

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "DepthSlider", "depth", PW, FW,
        100, 10000, (10000 - 100) / 100 + 1
    )->setValue( biome->feature.depth );

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "SurfaceComplexity", "surfaceComplexity", PW, FW,
        0, 500, (500 - 0) / 1 + 1
    )->setValue( biome->feature.surfaceComplexity );

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "CountGradientSlider", "countGradient", PW, FW,
        2, 50, (50 - 2) / 1 + 1
    )->setValue( biome->feature.countGradient );

	mTrayMgr->createThickSlider(
        TL_TOPRIGHT, "SeedSlider", "seed", PW, FW,
        -10000, 10000, (10000 + 10000) / 1 + 1
    )->setValue( biome->feature.seed );

    mTrayMgr->createCheckBox(
        TL_TOPRIGHT, "ShowGradient", "gradient"
    )->setChecked( show.gradient );
    mTrayMgr->createCheckBox(
        TL_TOPRIGHT, "ShowSurface", "surface"
    )->setChecked( show.surface );
    mTrayMgr->createCheckBox(
        TL_TOPRIGHT, "ShowTrend", "trend"
    )->setChecked( show.trend );



    // Окно детального наблюдения
    /* - Вынесено в p().
    //mTrayMgr->createLabel( TL_BOTTOMRIGHT, "DetailLabel", "Detail", PW );
    Ogre::StringVector param;
    param.push_back( "Throw" );
    paramsPanel = mTrayMgr->createParamsPanel( TL_BOTTOMRIGHT, "ParamsPanel", PW * 3, param );
    */

    
    mTrayMgr->showCursor();

}





void Sample_Landscape25::cleanupContent() {
    std::cout << "Landscape25::cleanupContent() start" << std::endl;

    biome.reset();

    std::cout << "Landscape25::cleanupContent() stop" << std::endl;
}






void Sample_Landscape25::sliderMoved( Slider* slider ) {

    auto bf = &biome->feature;

    if (slider->getName() == "LengthSlider") {
        bf->length = (size_t)slider->getValue();
    }

    if (slider->getName() == "DepthSlider") {
        bf->depth = (size_t)slider->getValue();
    }

    if (slider->getName() == "SurfaceComplexity") {
        bf->surfaceComplexity = (size_t)slider->getValue();
    }

    if (slider->getName() == "CountGradientSlider") {
        bf->countGradient = (size_t)slider->getValue();
    }

    if (slider->getName() == "SeedSlider") {
        bf->seed = (int)slider->getValue();
    }

    showBiome();

}




void Sample_Landscape25::checkBoxToggled( CheckBox* box ) {

    if (box->getName() == "ShowGradient") {
        show.gradient = box->isChecked();
    }

    if (box->getName() == "ShowSurface") {
        show.surface = box->isChecked();
    }

    if (box->getName() == "ShowTrend") {
        show.trend = box->isChecked();
    }

    showBiome();

}
