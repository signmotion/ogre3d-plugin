#ifndef __Landscape25_H__
#define __Landscape25_H__

#include "SdkSample.h"
#include "default.h"
#include "Biome.h"

using namespace Ogre;
using namespace OgreBites;


/**
* @info
* Для включения консольного лога:
*     1. SampleBrowser / Свойства / Компоновщик / Система = CONSOLE
*     2. SampleBrowser / SampleBrowser.cpp / "INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)"
*        заменяем на "int main(int argc, char *argv[])".
*/
class _OgreSampleClassExport Sample_Landscape25 : public SdkSample {
public:

	inline Sample_Landscape25() :
        biome( nullptr )
    {
		mInfo["Title"] = "Landscape 2.5d";
		mInfo["Description"] = "A demonstration of parallax scrolling.";
		mInfo["Thumbnail"] = "thumb_landscape25.png";
		mInfo["Category"] = "Environment";


        // Log
        /* - @example
		logMgr = new LogManager();
		logMgr->createLog("Temporary log", false, true, true); 
		//XmlOptions opts = parseArgs(numargs, args);
        // use the log specified by the cmdline params
        logMgr->setDefaultLog(logMgr->createLog(opts.logFile, false, !opts.quietMode)); 
        // get rid of the temporary log as we use the new log now
		logMgr->destroyLog("Temporary log");
        */

        //auto lm = LogManager::getSingletonPtr();
        //lm->setDefaultLog( &Ogre::Log( "landscape25.log", true, false ) );

    }





protected:

	void setupContent();


    void setupControls();


    void cleanupContent();



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

    void showBiome();
    void showBiomeWithThrow();



    /**
    * Create billboard set, add it to map 'bs' and put to scene.
    *//*
    inline void createBillboardSet(
        std::unordered_map< std::string, Ogre::BillboardSet* >*  bs,
        const std::string& name
    ) {
        (*bs)[name] = mSceneMgr->createBillboardSet( name, 50 );
        (*bs)[name]->setMaterialName( "landscape25/2d" );
        (*bs)[name]->setAutoextend( false );
        (*bs)[name]->setBillboardType( Ogre::BBT_PERPENDICULAR_COMMON );
        (*bs)[name]->setCommonUpVector( Ogre::Vector3::UNIT_Z );
        (*bs)[name]->setCommonDirection( -Ogre::Vector3::UNIT_Y );
        (*bs)[name]->setBillboardsInWorldSpace( true );
        (*bs)[name]->setBillboardOrigin( Ogre::BBO_CENTER );

        mSceneMgr->getRootSceneNode()->attachObject( (*bs)[name] );
    }
    */




private:
    /**
    * Current biome of the world.
    */
    std::shared_ptr< Biome >  biome;

    /**
    * Параметры визуализации.
    */
    show_t show;

};



#endif
