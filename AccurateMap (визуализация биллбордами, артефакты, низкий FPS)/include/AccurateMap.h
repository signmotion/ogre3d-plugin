#ifndef __AccurateMap_H__
#define __AccurateMap_H__

/**
* ������������ ��� �������.
*/
// �������� ���������� �����
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
* ��� ��������� ����������� ����:
*     1. SampleBrowser / �������� / ����������� / ������� = CONSOLE
*     2. SampleBrowser / SampleBrowser.cpp / "INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)"
*        �������� �� "int main(int argc, char *argv[])".
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
    void showMap() const;
    void hideMap() const;
    void showLowHM() const;
    void hideLowHM() const;


    /**
    * ������ ������ ��� �� ����� ������.
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
    * ������� ������� == ������������ �����.
    */
    std::shared_ptr< MapDoxel >  currentMap;

    /**
    * ������� ����������� ������������ ���������� �����.
    */
    size_t detailedLevel;


    /**
    * ���� ����� �����. �� ���� �� ������ ���������� ����� � �������������
    * ����� �����.
    *
    * @see showLowHM()
    */
    std::string sourceFileHM;

    /**
    * ������ ���� �� ����, �.
    */
    d::size3d_t worldSize;


    /**
    * ������������ ���-�� ������������ ��������.
    */
    size_t maxCountDoxelForVisual;


    /**
    * ��� ��� �������.
    * �����, ����� "������".
    */
    //mutable Ogre::MeshPtr lowMesh_;
    //mutable std::pair< d::sizeInt2d_t, std::shared_ptr< float > >  hmImage_;
};



#endif
