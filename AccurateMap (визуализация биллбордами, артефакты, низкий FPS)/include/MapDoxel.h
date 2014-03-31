#pragma once

#include "Doxel.h"
#include <Ogre.h>
#include <boost/array.hpp>


/**
* ���������� �����.
* ����������, ��� �������� ������� � ����������� �� �� �������. ������ ���� ���
* �������� �������: ���� ���������� �������� ����� ���������� � ����������� ��
* ������� �����������. ������� ����������� ������� ���������� ��������
* ����������.
*
* @see Doxel
*/
class MapDoxel {
public:
    /**
    * ������ ����� �� ���������������� ����� �����.
    *
    * @param source ���� � �����, ������� �������� ����� ����� ��� �������.
    * @param size ������ �����, ��. ����� - ���.
    */
    MapDoxel(
        const std::string& source,
        const d::size3d_t& worldSizeInKm,
        Ogre::SceneManager* sm
    );




    virtual ~MapDoxel();




    /**
    * ������ ����� � ��������� ������������ �������.
    *
    * @see clearCanvas() ��� �������.
    *//* - �� ������������.
    void draw(
        const d::box3d_t& box
    ) const;
    */




    /**
    * ���������� ������� �� ��������� ����������.
    *
    * @param maxCount ���� �� 0, ������ ����� ���-�� �������� �� ���������������.
    * @param testMode ����� ������������ ��������� ���������� ������ �������.
    *
    * (!) ������������ ����� ���������� ������� ������ ���� �������.
    * ��. ���������� � ���� draw() � setDefaultDimensions().
    */
    void draw(
        const Doxel3D& doxel,
        const d::coord3d_t& coord,
        size_t maxCount = 0,
        bool testMode = false
    ) const;




    /**
    * ���������� �������� ������� �� ��������� ���������� ��
    * ��������� ������.
    *
    * @todo ��� ������ ���� ������� ���� �� � ����? ������ - �� ������ ������.
    */
    inline void drawChild(
        const d::coord3d_t& coord,
        size_t level,
        size_t maxCount = 0,
        bool testMode = false
    ) const {
        drawChild( *root, coord, level, maxCount, testMode );
    }


    void drawChild(
        const Doxel3D& doxel,
        const d::coord3d_t& coord,
        size_t level,
        size_t maxCount = 0,
        bool testMode = false
    ) const;




    /**
    * ������� �����.
    *
    * @see draw() ��� ���������.
    */
    inline void clearCanvas() {
        for (auto itr = dBS.begin(); itr != dBS.end(); ++itr) {
            auto bs = *itr;
            bs->clear();
        }
        testBS->clear();
        sn->removeAllChildren();
    }






private:
    /**
    * ���������� �����.
    */
    std::shared_ptr< Doxel3D >  root;

    /**
    * �������� - ���� � ������ �����.
    */
    const std::string s;


    /**
    * �� ��� ����� ��� ������������ �������.
    *
    * ���������� ����� �������. ���������:
    *   0 - ����
    *   1 - ������
    *   2 - �����
    *   3 - �����
    *   4 - ������
    *   5 - �������
    */
    Ogre::SceneManager* sm;
    Ogre::SceneNode* sn;
    typedef boost::array< Ogre::BillboardSet*, 6 >  dBS_t;
    dBS_t dBS;

    /**
    * ������������ ��� ��������� ������ ������.
    */
    Ogre::BillboardSet* testBS;

};

