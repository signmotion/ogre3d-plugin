#pragma once

#include "default.h"


/**
* ���������� � �����.
*/
struct FeatureBiome {
    /**
    * ������������ �����, �.
    */
    size_t length;

    /**
    * ������� ����� �� �����������, �.
    * ������������� ���������� �� ������ �������� ����.
    */
    size_t depth;

    /**
    * C������ ���� ������� ���������� � �����.
    */
    size_t countLayer;

    /**
    * ���������� �������� ������ / ������� ������ ���� �����, �.
    * ��� ������������ ����, ����� ���������, ����� ���� �� ���������
    * ���������� ������ �/��� �������.
    */
    //const boost::numeric::interval< int >  fatLayer;

    /**
    * ������ ��� "������� ����" ������� ����.
    * ������������ �� ��� OZ ����. ���� ����������� ������������
    * ���� ������.
    */
    int elevation;

    /**
    * ������ ���������. � ���� ������ ��������� ��������� ����.
    * ����� ���� ������ '0'.
    */
    int altitude;

    /**
    * ��������� ����������� �����. ����� �������������� ����� ��-�������.
    * ��������, ���� ����� �������, ��� ������ *��������* ����� "�������"
    * ���� �� ����� - ������ "������" ������� � ������ - ��� �������
    * �������� ���������� �� ��� OZ.
    * ���������� �� ������������ ��������� ��� Layer ���, ��� ��������������
    * ������ ��� 3D-���������.
    *
    * 0 - ��� ����� ����� �� ����� ����� OX.
    */
    size_t surfaceComplexity;

    /**
    * ���������� ����� �������� ��� ������� ���� �����. ������ ����� ���� �
    * 3D-������������.
    *
    * @see FeatureLayer::countGradient
    */
    size_t countGradient;

    /**
    * ���������.
    * ��������� ����� ����� �� ��������� �����������.
    * �������� � ���� ���������� �������, ����� ������������
    * ������ / ����� / ������� ������ ������� � �����������
    * ��������.
    */
    natural_t  natural;

    /**
    * ����� ���������� ��������� ����� ��� �����.
    */
    int seed;

};
