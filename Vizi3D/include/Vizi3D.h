#ifndef __Vizi3D_H__
#define __Vizi3D_H__

#include "SdkSample.h"
#include "default.h"
#include "configure.h"
#include "Vizi3DWorld.h"

#ifdef RUN_TEST
    #include "../test/Vizi3DWorldTest.h"
#endif


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
class _OgreSampleClassExport Sample_Vizi3D : public SdkSample {
public:

	inline Sample_Vizi3D() {
		mInfo["Title"] = "Vizi3D";
		mInfo["Description"] = "Visualizer.";
		mInfo["Thumbnail"] = "thumb_vizi3d.png";
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



    inline ~Sample_Vizi3D() {
        std::cout << "~Vizi3D()" << std::endl;
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
    std::shared_ptr< Vizi3DWorld >  world;

};



#endif
