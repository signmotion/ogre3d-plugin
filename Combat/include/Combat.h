#ifndef __Combat_H__
#define __Combat_H__

/**
* Отладочные тесты.
*/
//#define RUN_TEST
//#define EXIT_AFTER_TEST

#ifdef RUN_TEST
#include "../test/MyTest.h"
#endif




#include "SdkSample.h"
#include "default.h"
#include "RealtimePhysicsPort.h"


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
class _OgreSampleClassExport Sample_Combat : public SdkSample {
public:

	inline Sample_Combat() {
		mInfo["Title"] = "Combat";
		mInfo["Description"] = "Fighting, gameplay testing.";
		mInfo["Thumbnail"] = "thumb_combat.png";
		mInfo["Category"] = "Environment";


#ifdef RUN_TEST
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



    inline ~Sample_Combat() {
        std::cout << "~Combat()" << std::endl;
        // @see cleanupContent()
    }





protected:

	void setupContent();


    void setupControls();


    void cleanupContent();


    /**
    * Вызвается при отрисовке каждого фрейма.
    */
    virtual bool frameRenderingQueued( const Ogre::FrameEvent& evt );



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





private:
    /**
    * Realtime port of the world.
    */
    std::shared_ptr< RealtimePhysicsPort >  realtimeWorld;

};



#endif
