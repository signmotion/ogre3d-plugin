#pragma once

#include <Ogre.h>
#include <Newton.h>
#include "default.h"


// ��������� ��� ������� ��������.

// ��������, ���������� ��������� '=', �������������� �� ������ ������ �������� �
// �������������, � ������ ������� ��� ��������� ��������.


#if 0
// - ��� �������� �������� � ������ ����� (����������, ����������). ���
//   ���������� ������������ �������������� ���������.
/**
* "������� ��������� ������" ��� ������������� � ������.
*/
struct FastData {
};



/**
* �������� ��� RealTimePort.
* ���������� � ����� �������� ������������ �� ������� �����. ��� ���������
* ������ ���� ������ ��� �������� ������ ��������� �� UID.
*/
struct EntityRTPortFD : public FastData {

    inline EntityRTPortFD() :
        //uid( "" ),
        visualImage( nullptr ),
        physicsImage( nullptr )
    {
    }

    /**
    * UID ��������.
    *//* - ������ ��� ���� � ����� �����.
    uid_t uid;
    */

    /**
    * ���������� ����� ��������.
    */
    Ogre::SceneNode* visualImage;

    /**
    * ���������� ����� ��������.
    */
    NewtonBody* physicsImage;

};
#endif

// @todo ��������� ���������, ����� �� ������������ ����������������� ������. ������.
struct EntityRTPortFD {
};




/**
* ������� ��������� ��� ��������� ������ ����������.
* ������� � ���������� �������� � ��, �.�. ��������������� � ������ Ogre
* �������� ���� �� ������ ������� ��� ��������� ������� 1.0.
*/
struct InitEntity {

    enum type_e {
        TYPE_UNDEFINED = 0,
        TYPE_FLAT,
        TYPE_VOLUME
    };

    enum form_e {
        FORM_UNDEFINED = 0,
        // ������������� ��� ��������������
        FORM_BOX,
        // ���� ��� �����
        FORM_SPHERE
    };

    // ��� ��������
    const type_e type;
    // ����� ��������
    const form_e form;

    //  UID ��������
    uid_t uid;
    // ������� ��������, c�
    // ������� ��� �������� � ������� ���� - �����
    float boundingRadius;
    size3d_t size;
    // ����� ��������, ��
    float mass;
    // ���������� ��������, c�
    coord2d_t coord;
    // ���������� ����� ��������
    std::string model;
    // ����, ����������� � ������ ������ � ��������, �
    vector2d_t force;


    /**
    * @return true, ���� ��������� ������������� ������ ���������.
    */
    virtual bool correct() const = 0;


protected:
    inline InitEntity(
        type_e type, form_e form
    ) :
        type( type ),
        form( form ),
        uid( "" ),
        boundingRadius( 0.0f ),
        size( ZERO3D ),
        mass( 0.0f ),
        coord( ZERO2D ),
        model( "" ),
        force( ZERO2D )
    {
    }

};




/**
* ����� ����������� ������� ������������.
*/
struct FlatInitEntity : public InitEntity {
    inline FlatInitEntity( form_e form ) : InitEntity( TYPE_FLAT, form ) {
    }

    virtual inline bool correct() const {
        return ( ((form == InitEntity::FORM_BOX)  && !emptyAny( size ))
            || ((form == InitEntity::FORM_SPHERE) && !empty( boundingRadius ))
        ) && !uid.empty()
          && !model.empty();
    }
};



/**
* ����� ����������� 3D-������������.
*/
struct VolumeInitEntity : public InitEntity {
    inline VolumeInitEntity( form_e form ) : InitEntity( TYPE_VOLUME, form ) {
    }

    virtual inline bool correct() const {
        return ( ((form == InitEntity::FORM_BOX)  && !emptyAny( size ))
            || ((form == InitEntity::FORM_SPHERE) && !empty( boundingRadius ))
        ) && !uid.empty()
          && !model.empty();
    }
};
