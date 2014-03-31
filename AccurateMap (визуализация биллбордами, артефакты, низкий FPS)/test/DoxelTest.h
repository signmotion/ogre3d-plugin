#pragma once

#include <gtest/gtest.h>
#include "../include/Doxel.h"

#define TEST_IMAGE_HEIGHTMAP_1x1
#define TEST_IMAGE_HEIGHTMAP_2x2
#define TEST_IMAGE_HEIGHTMAP_4x4
#define TEST_IMAGE_HEIGHTMAP_8x8
#define TEST_IMAGE_HEIGHTMAP_16x16



namespace {

/**
* �������� ��� Doxel.h
*/
class DoxelTest : public ::testing::Test {
  protected:
    inline DoxelTest() {
	    setlocale( LC_ALL, "Russian" );
        // ��� ����������� '.' ������ ','
        setlocale( LC_NUMERIC, "C" );
    }

    virtual inline ~DoxelTest() {
    }

    //virtual inline void SetUp() {
    //}

    //virtual void TearDown() {
    //}
};






#ifdef TEST_IMAGE_HEIGHTMAP_1x1

// � ���� ���� ������ - ������������, ������ ���� 1 x 1
TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ) {

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-maxheight.png";

    // �� ����� ����� ������ ���������� �����

    // ������ ����� � ������ �����, 1 ��� = 1 ��
    // (���������� ��� �������� �������� ������� ����)
    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    // ������ ����, �� (�������� ���� �������)
    const float worldHeight = (float)hmSize.get<0>();
    // �������� ������ ���� �� ��� ����, �� ->
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
        // -> ��������� � �����
    ) * 1000.0f;

    // ������ ������� (������� � ���� �����)
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // ���������

    // ���������� �������.
    // ����������, ��� ���������� ��������� ���� ��������.
    // ���������� ����� ��������������� ���������� ��������� ������� ��������,
    // � �� ����� ��������� ������� ������� ������  2 ^ ( D * L ).
    //
    // *������������* ������� 3D-������� � ����������� �� ������:
    //   0-�  -  1
    //   1-�  -  8
    //   2-�  -  64
    //   3-�  -  512
    //   4-�  -  4 096
    //   5-�  -  32 768
    //   6-�  -  262 144
    //   7-�  -  2 097 152
    //   8-�  -  16 777 216
    //   9-�  -  134 217 728
    //  10-�  -  1 073 741 824
    //
    // *������������* ������� 2D-������� � ����������� �� ������:
    //   0-�  -  1
    //   1-�  -  4
    //   2-�  -  16
    //   3-�  -  64
    //   4-�  -  256
    //   5-�  -  1 024
    //   6-�  -  4 096
    //   7-�  -  16 384
    //   8-�  -  65 536
    //   9-�  -  262 144
    //  10-�  -  1 048 576
    //
    // ������������ ������� �������� �������, �������������� ������ �����
    // �������� 528 x 528 (1 ������� = 1 ������) ����������� ...
    // ...

    // ��������� ������������� �������
    //const size_t volume = doxel.volume( 0 );

    // ���������� ������������ �������� �� ������ �������.
    // ������� ������� ������ ������� - ��� 2^3 = 8 �������� ��� ��
    // ������ ����������. �� ����������� ������� (������� ��������)
    // ���������� �������� ����� ������������� �� ����������.
    EXPECT_EQ( 0,  doxel->countChild( 0 ) );
    EXPECT_EQ( 0,  doxel->countChild( 1 ) );
    EXPECT_EQ( 0,  doxel->countChild( 2 ) );
    EXPECT_EQ( 0,  doxel->countChild( 3 ) );
    // � �.�.
}







// � ���� ���� ������ - �� ������ ����, ������ ���� 1x1
TEST_F( DoxelTest, ValueOf_HMImage_1x1_Sealine ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    EXPECT_EQ( 0,  doxel->countChild( 0 ) );
    EXPECT_EQ( 0,  doxel->countChild( 1 ) );
    EXPECT_EQ( 0,  doxel->countChild( 2 ) );
    EXPECT_EQ( 0,  doxel->countChild( 3 ) );
    // � �.�.
}









// � ���� ���� ������� - ������������, ������ ���� 1x1
TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxDepth ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    EXPECT_EQ( 0,  doxel->countChild( 0 ) );
    EXPECT_EQ( 0,  doxel->countChild( 1 ) );
    EXPECT_EQ( 0,  doxel->countChild( 2 ) );
    EXPECT_EQ( 0,  doxel->countChild( 3 ) );
    // � �.�.
}

#endif








#ifdef TEST_IMAGE_HEIGHTMAP_2x2

// � ���� ���� ������ - ������������, ������ ���� 2x2
TEST_F( DoxelTest, ValueOf_HMImage_2x2_MaxHeight ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-maxheight.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    EXPECT_EQ( 8,  doxel->countChild( 0 ) );
    EXPECT_EQ( 8,  doxel->countChild( 1 ) );
    EXPECT_EQ( 8,  doxel->countChild( 2 ) );
    EXPECT_EQ( 8,  doxel->countChild( 3 ) );
    // � �.�.
}








// � ���� ���� ������ - �� ������ ����, ������ ���� 2x2
TEST_F( DoxelTest, ValueOf_HMImage_2x2_Sealine ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // ���������

    // ���������� ������������ �������� �� ������ �������.
    // ������� ������� ���� ���������, �.�. ������� ���� (z == 0) ����������� ��.
    EXPECT_EQ( 8,  doxel->countChild( 0 ) );
    EXPECT_EQ( 8,  doxel->countChild( 1 ) );
    EXPECT_EQ( 8,  doxel->countChild( 2 ) );
    EXPECT_EQ( 8,  doxel->countChild( 3 ) );
    // � �.�.
}









// � ���� ���� ������� - ������������, ������ ���� 2x2
TEST_F( DoxelTest, ValueOf_HMImage_2x2_MaxDepth ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // ���������

    // ���������� ������������ �������� �� ������ �������.
    // ��������� ������ ������ ������� ������� (z < 0).
    EXPECT_EQ( 4,  doxel->countChild( 0 ) );
    EXPECT_EQ( 4,  doxel->countChild( 1 ) );
    EXPECT_EQ( 4,  doxel->countChild( 2 ) );
    EXPECT_EQ( 4,  doxel->countChild( 3 ) );
    // � �.�.
}

#endif








#ifdef TEST_IMAGE_HEIGHTMAP_4x4

// � ���� ���� ������ - ������������, ������ ���� 4x4
TEST_F( DoxelTest, ValueOf_HMImage_4x4_MaxHeight ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-4x4-maxheight.png";

    const auto hmSize = d::sizeInt2d_t( 4, 4 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    // (!) ���������� �������� ������ ������ ���-�� ��������� ��������.
    EXPECT_EQ(  8,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}







// � ���� ���� ������ - �� ������ ����, ������ ���� 4x4
TEST_F( DoxelTest, ValueOf_HMImage_4x4_Sealine ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-4x4-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 4, 4 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // ���������

    // ���������� ������������ �������� �� ������ �������.
    // ������� ������� ��������� *��������*, �.�. ������� ���� (z == 0) ����������� ��.
    EXPECT_EQ(  8,  doxel->countChild( 0 ) );
    EXPECT_EQ( 48,  doxel->countChild( 1 ) );
    EXPECT_EQ( 48,  doxel->countChild( 2 ) );
    EXPECT_EQ( 48,  doxel->countChild( 3 ) );
    // � �.�.
}








// � ���� ���� ������� - ������������, ������ ���� 4x4
TEST_F( DoxelTest, ValueOf_HMImage_4x4_MaxDepth ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-4x4-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 4, 4 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������.
    // ��������� ������ ������ ������� ������� (z < 0).
    EXPECT_EQ(  4,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}

#endif








#ifdef TEST_IMAGE_HEIGHTMAP_8x8

// � ���� ���� ������ - ������������, ������ ���� 8x8
TEST_F( DoxelTest, ValueOf_HMImage_8x8_MaxHeight ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-8x8-maxheight.png";

    const auto hmSize = d::sizeInt2d_t( 8, 8 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    EXPECT_EQ(   8,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  64,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 512,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 512,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}








// � ���� ���� ������ - �� ������ ����, ������ ���� 8x8
TEST_F( DoxelTest, ValueOf_HMImage_8x8_Sealine ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-8x8-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 8, 8 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������.
    // ������� ������� ���� ���������, �.�. ������� ���� (z == 0) ����������� ��.
    EXPECT_EQ(   8,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  48,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 320,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 320,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}









// � ���� ���� ������� - ������������, ������ ���� 8x8
TEST_F( DoxelTest, ValueOf_HMImage_8x8_MaxDepth ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-8x8-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 8, 8 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������.
    // ��������� ������ ������ ������� ������� (z < 0).
    EXPECT_EQ(  4,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}

#endif







#ifdef TEST_IMAGE_HEIGHTMAP_16x16

// ������ - ����������� �������� �� ������ ������� ���� � ������� ������,
// ������ ���� 16x16
TEST_F( DoxelTest, ValueOf_HMImage_16x16_Gradient ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-16x16-gradient.png";

    const auto hmSize = d::sizeInt2d_t( 16, 16 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    // @info ����� "�� �������": ������� ������� - ���������, ������������ - ���������� ���������
    EXPECT_EQ(    7,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(   47,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  319,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 2300,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}

#endif



} // namespace
