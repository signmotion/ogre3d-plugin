#include "helper.h"


OgreBites::SdkTrayManager* helper::Tray::tray = nullptr;



/**
* �������� ��� � �������� �� ������.
* @require ����� init().
*/
void helper::Tray::p( const std::string& name, const std::string& value ) {
    assert( tray && "����� �������������� �����. �������������������. ��. Tray::init()." );

    const std::string widgetName = "ParamsPanel";

    auto w = tray->getWidget( widgetName );
    std::pair< Ogre::StringVector, Ogre::StringVector > nv;
    if ( w ) {
        // �������� ���������
        auto pp = static_cast< OgreBites::ParamsPanel* >( w );
        nv = std::make_pair(
            pp->getAllParamNames(),
            pp->getAllParamValues()
        );
        // ������ ������������
        tray->destroyWidget( widgetName );
    }

    Ogre::StringVector v;

    // �������� ��� ��� ���� � ������
    bool present = false;
    for (size_t i = 0; i < nv.first.size(); ++i) {
		if (nv.first[i] == name) {
            nv.second[i] = value;
            present = true;
        }
        v.push_back( name );
	}
    if ( !present ) {
        v.push_back( name );
    }

    auto paramsPanel = tray->createParamsPanel(
        OgreBites::TL_BOTTOMRIGHT, "ParamsPanel", 500, v
    );
    paramsPanel->setParamValue( name, value );

}
