#ifndef __SmokePort2D_H__
#define __SmokePort2D_H__

#include "SdkSample.h"
#include "default.h"
#include "struct.h"
#include "GasCellPort.h"

#include <boost/thread.hpp>
#include <Theron/Framework.h>

using namespace Ogre;
using namespace OgreBites;


/**
* @info
*
* @todo (!) ������������ ������ ��� ���������. ���� � OgreConfig.h �� ��������:
*         #define OGRE_THREAD_SUPPORT 1 or 2
*         #define OGRE_THREAD_PROVIDER 1
* 
* ��� ��������� ����������� ����:
*     1. SampleBrowser / �������� / ����������� / ������� = CONSOLE
*     2. SampleBrowser / SampleBrowser.cpp / "INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)"
*        �������� �� "int main(int argc, char *argv[])".
*/
class _OgreSampleClassExport Sample_SmokePort2D : public SdkSample {
public:

	inline Sample_SmokePort2D() {
		mInfo["Title"] = "SmokePort 2d";
		mInfo["Description"] = "A demonstration of smoke.";
		mInfo["Thumbnail"] = "thumb_smokeport2d.png";
		mInfo["Category"] = "Environment";
    }



    inline ~Sample_SmokePort2D() {
        std::cout << "~SmokePort2D()" << std::endl;
    }





protected:

	void setupContent();


    void setupControls();


    void cleanupContent();



    /**
    * ����������� ������ ����������� ���������� SDKTrays
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

    //void pulsePort( int tact );





private:
    /**
    * Current smoke port of the world.
    */
    std::shared_ptr< GasCellPort >  smoke;

    /**
    * �� ��������� ��������� ������� �������� ������.
    */
    std::shared_ptr< Theron::Framework >  flicker;
    Theron::ActorRef pulseActor;

};



#endif
