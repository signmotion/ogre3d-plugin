#pragma once

#include <Ogre.h>
#include <SdkSample.h>


/**
* ��������� �������� ������ � ������, ��������������� � ���� �������.
*/
namespace helper {



/**
* ��� �������.
*
* (!) ����� ������� ������� ����� ������� Tray ������ ������ ����
*     ���� �� ���� ����� init().
*/
class Tray {

protected:
    inline Tray() {
    }


public:

    /**
    * �������������.
    */
    static inline void init(
        OgreBites::SdkTrayManager* t
    ) {
        Tray::tray = t;
    }


    /**
    * @return true, ���� Tray ��� ���������������.
    *         ����� �������������� �������� ����� ������� �����������
    *         ������� ������ Tray.
    */
    static bool initiated() {
        return  tray ? true : false;
    }



    virtual inline ~Tray() {
    };




    /**
    * �������� ��� � �������� �� ������.
    * @require ����� init().
    */
    static void p( const std::string& name, const std::string& value );




private:
    /**
    * ��������� ��� ������ � ���������� �������������� ������.
    * ��������, ����������� ����������� ������.
    * ������ ���� ������������������ ����� ������� ������� ������.
    */
    static OgreBites::SdkTrayManager* tray;

};


}
