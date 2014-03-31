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
* Для включения консольного лога:
*     1. SampleBrowser / Свойства / Компоновщик / Система = CONSOLE
*     2. SampleBrowser / SampleBrowser.cpp / "INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)"
*        заменяем на "int main(int argc, char *argv[])".
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

    // @todo При размерах образа больше неск. тыс. пикселей, не показывает
    //       образ когда приближаемся ближе. Исправить. Сейчас введён
    //       масштабирующий коэффициент.
    //       (!) При отрисовке на статической сцене - всё как надо.
    // У Ogre3D есть ограничение на размер отрисовываемого за раз объекта. Ввод
    // масштаба < 1.0 может помочь визуализировать объект.
    const float SCALE;


    /**
    * Указатель на ячейку высоты, которую следует детализировать.
    * Начало координат - слева внизу, направление осей - вправо вверх.
    */
    coordInt2d_t pointer;

};



#endif
