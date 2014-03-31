#pragma once

#include "CellPort.h"
#include "default.h"
#include "struct.h"


class GasCellPort :
    public CellPort< GasFD >
{
public:
    inline GasCellPort(
        Ogre::SceneManager* sm,
        float visualScale,
        const sizeInt2d_t& size,
        float scale
    ) : CellPort( sm, visualScale, size, scale ) {
    }



    virtual inline ~GasCellPort() {
    }




    /**
    * @see Port
    */
    virtual void pulse( int tact );






protected:
    /**
    * ������ ����� ������ � ��������� �����������.
    * ������ ������ - �������� �������� 'multiple'.
    *
    * ����� �� ������������ � CellPort, �.�. �� ������ ���� �����
    * ������� ���� ����� �� ��������� �������.
    */
    //void cellDraw( const coordInt2d_t& coord ) const;


};

