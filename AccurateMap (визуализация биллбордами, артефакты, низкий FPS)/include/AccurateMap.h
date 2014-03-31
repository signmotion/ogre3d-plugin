#ifndef __AccurateMap_H__
#define __AccurateMap_H__

/**
* Используются для отладки.
*/
// Включить отладочные тесты
//#define RUN_TEST_ONLY
#define EXIT_AFTER_TEST

#ifdef RUN_TEST_ONLY
#include "../test/DoxelTest.h"
#endif


#include "SdkSample.h"
#include "default.h"
#include "MapDoxel.h"

using namespace Ogre;
using namespace OgreBites;


/**
* @info
* 
* Для включения консольного лога:
*     1. SampleBrowser / Свойства / Компоновщик / Система = CONSOLE
*     2. SampleBrowser / SampleBrowser.cpp / "INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)"
*        заменяем на "int main(int argc, char *argv[])".
*/
class _OgreSampleClassExport Sample_AccurateMap : public SdkSample {
public:

	inline Sample_AccurateMap() {
		mInfo["Title"] = "Accurate Map";
		mInfo["Description"] = "Create detailed map by levels.";
		mInfo["Thumbnail"] = "thumb_accuratemap.png";
		mInfo["Category"] = "Environment";


#ifdef RUN_TEST_ONLY
        std::cout << "\n\n\n";
        int argc = 0;
        ::testing::InitGoogleTest( &argc, (char**)nullptr );
        RUN_ALL_TESTS();
#ifdef EXIT_AFTER_TEST
        std::cin.ignore();
        exit( 0 );
#endif
#endif
    }



    inline ~Sample_AccurateMap() {
        std::cout << "~AccurateMap()" << std::endl;
        // @see cleanupContent()
    }





protected:

	void setupContent();


    void setupControls();


    void cleanupContent();


    /**
    * Горячие клавиши.
    */
    virtual bool keyPressed( const OIS::KeyEvent& evt );



    /**
    * Обработчики команд визуального интерфейса SDKTrays
    *
    * @see http://www.ogre3d.org/tikiwiki/SdkTrays
    */
	virtual inline void buttonHit(Button* button) {}
	virtual inline void itemSelected(SelectMenu* menu) {}
	virtual inline void labelHit(Label* label) {}

	virtual void sliderMoved( Slider* slider );

	virtual void checkBoxToggled( CheckBox* box );

	virtual inline void okDialogClosed(const Ogre::DisplayString& message) {}
	virtual inline void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit) {}




private:
    /**
    * Показывает/прячет визуальные образы.
    */
    void showMap() const;
    void hideMap() const;
    void showLowHM() const;
    void hideLowHM() const;


    /**
    * Методы строят меш по карте высоты.
    *
    * @copy from project "AccurateHeightmap".
    *//*
    Ogre::MeshPtr lowMesh(
        Ogre::SceneManager* sm,
        const d::size3d_t& worldSize
    ) const;
    */




private:
    /**
    * Текущий доксель == отображаемая карта.
    */
    std::shared_ptr< MapDoxel >  currentMap;

    /**
    * Уровень детализации отображаемой доксельной карты.
    */
    size_t detailedLevel;


    /**
    * Файл карты высот. По нему мы строим доксельную карту и визуализируем
    * карту высот.
    *
    * @see showLowHM()
    */
    std::string sourceFileHM;

    /**
    * Размер мира по осям, м.
    */
    d::size3d_t worldSize;


    /**
    * Максимальное кол-во отображаемых докселей.
    */
    size_t maxCountDoxelForVisual;


    /**
    * Кеш для методов.
    * Введён, чтобы "летало".
    */
    //mutable Ogre::MeshPtr lowMesh_;
    //mutable std::pair< d::sizeInt2d_t, std::shared_ptr< float > >  hmImage_;
};



#endif
