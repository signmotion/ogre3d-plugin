#ifndef __Physics2D_H__
#define __Physics2D_H__

#include "SdkSample.h"
#include "default.h"
#include "struct.h"
#include "RealtimePhysicsPort.h"

#include <boost/thread.hpp>

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
class _OgreSampleClassExport Sample_Physics2D : public SdkSample {
public:

	inline Sample_Physics2D() {
		mInfo["Title"] = "Physics 2d";
		mInfo["Description"] = "A demonstration of physics with Newton Dynamics.";
		mInfo["Thumbnail"] = "thumb_physics2d.png";
		mInfo["Category"] = "Environment";
    }



    inline ~Sample_Physics2D() {
        std::cout << "~Physics2D()" << std::endl;
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
    * Меняет состояние мира.
    * Для запуска в потоке.
    */
    static void pulse( Port* port, float timestep );



private:
    /**
    * Realtime port of the world.
    */
    std::shared_ptr< RealtimePhysicsPort >  realtimeWorld;

    /**
    * Для запуска пульсации в потоке.
    */
    boost::thread pulseThread;
    static bool interruptPulseThread;
};



#endif
