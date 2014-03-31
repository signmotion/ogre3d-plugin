#ifndef __AccurateHeightmap_H__
#define __AccurateHeightmap_H__

#include "SdkSample.h"
#include "default.h"
#include "AHeightmap.h"

using namespace Ogre;
using namespace OgreBites;


/**
* @info
* 
* ��� ��������� ����������� ����:
*     1. SampleBrowser / �������� / ����������� / ������� = CONSOLE
*     2. SampleBrowser / SampleBrowser.cpp / "INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)"
*        �������� �� "int main(int argc, char *argv[])".
*/
class _OgreSampleClassExport Sample_AccurateHeightmap : public SdkSample {
public:

	inline Sample_AccurateHeightmap() :
        SCALE( 1.0f / 1.0f ),
        pointer( -120, -7 )
    {
		mInfo["Title"] = "Accurate Heightmap";
		mInfo["Description"] = "Create hight detailed heightmap from low detailed.";
		mInfo["Thumbnail"] = "thumb_accurateheightmap.png";
		mInfo["Category"] = "Environment";
    }



    inline ~Sample_AccurateHeightmap() {
        std::cout << "~AccurateHeightmap()" << std::endl;
        // @see cleanupContent()
    }





protected:

	void setupContent();


    void setupControls();


    void cleanupContent();


    /**
    * ������� �������.
    */
    virtual bool keyPressed( const OIS::KeyEvent& evt );



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
    /**
    * ����������/������ ���������� ������.
    */
    void showLowHM() const;
    void hideLowHM() const;
    void showPointer() const;
    void showHightHM() const;
    void hideHightHM() const;



private:
    /**
    * Low detailed heightmap.
    */
    std::shared_ptr< AHeightmap >  ahm;

    // @todo ��� �������� ������ ������ ����. ���. ��������, �� ����������
    //       ����� ����� ������������ �����. ���������. ������ �����
    //       �������������� �����������.
    //       (!) ��� ��������� �� ����������� ����� - �� ��� ����.
    // � Ogre3D ���� ����������� �� ������ ��������������� �� ��� �������. ����
    // �������� < 1.0 ����� ������ ��������������� ������.
    const float SCALE;


    /**
    * ��������� �� ������ ������, ������� ������� ��������������.
    * ������ ��������� - ����� �����, ����������� ���� - ������ �����.
    */
    coordInt2d_t pointer;

};



#endif
