#pragma once

#include <gtest/gtest.h>
#include "../include/Doxel.h"
#include "../include/DStore.h"


//#define TEST_IMAGE_HEIGHTMAP_1x1
//#define TEST_IMAGE_HEIGHTMAP_2x2
//#define TEST_IMAGE_HEIGHTMAP_4x4
//#define TEST_IMAGE_HEIGHTMAP_8x8
//#define TEST_IMAGE_HEIGHTMAP_16x16
#define TEST_IMAGE_HEIGHTMAP_32x32
//#define TEST_IMAGE_HEIGHTMAP_64x64
//#define TEST_IMAGE_HEIGHTMAP_128x128
//#define TEST_IMAGE_HEIGHTMAP_256x256  //- �� ������� ������


namespace {

/**
* �������� ��� Doxel.
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


    virtual inline void SetUp() {
    }


    virtual inline void TearDown() {
        // ������� ���������� singleton-����������. ������� ������...
        CDManager::singleton()->clear();
        // ...�� ��������� ������ � �������������� ���������.
    }


    /**
    * ��������� ��������� CouchDB.
    */
    static inline DStore* initStore( const std::string& name ) {
        assert( !name.empty() );
        std::string user = "dalion";
        std::string password = "visor888";
        std::string url = "127.0.0.1:5984";
        auto store = DStore::newSingleton( name, user, password, url );
        store->clear( false );
        return store;
    }

};






#ifdef TEST_IMAGE_HEIGHTMAP_1x1

// � ���� ���� ������ - ������������, ������ ���� 1 x 1
TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ) {

    // ������� ����� - ��� ��������� CouchDB
    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

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
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );


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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );


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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );


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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-maxheight.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );


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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );


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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );


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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-4x4-maxheight.png";

    const auto hmSize = d::sizeInt2d_t( 4, 4 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-4x4-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 4, 4 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );


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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-4x4-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 4, 4 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-8x8-maxheight.png";

    const auto hmSize = d::sizeInt2d_t( 8, 8 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
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

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-8x8-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 8, 8 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
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

    auto store = initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-8x8-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 8, 8 );
    // ������ ����� �� ������ �� �-�� �������� (������ ������ �����������)
    const float worldHeight = (float)hmSize.get<0>() / 3.0f;
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������.
    // ��������� ������ ������ ������� ������� (z < 0).
    EXPECT_EQ(  4,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.


    // ���������
    doxel->lazySave( false );
    doxel->flush();

    EXPECT_EQ( Doxel3D::countDoxel_ValueOf,  store->count() ) <<
        "�-�� ���������� �������� �� �����. ����������. ������� � " <<
        Doxel3D::countDoxel_ValueOf << *doxel;

}

#endif







#ifdef TEST_IMAGE_HEIGHTMAP_16x16

// ������ - ����������� �������� �� ������ ������� ���� � ������� ������,
// ������ ���� 16x16
TEST_F( DoxelTest, ValueOf_HMImage_16x16_Gradient ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-16x16-gradient.png";

    const auto hmSize = d::sizeInt2d_t( 16, 16 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
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







#ifdef TEST_IMAGE_HEIGHTMAP_32x32

// ������ - ����������� �������� �� ������ ������� ���� � ������� ������,
// ������ ���� 32x32
TEST_F( DoxelTest, ValueOf_HMImage_32x32_Gradient ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    auto store = initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-64x64-gradient.png";

    const auto hmSize = d::sizeInt2d_t( 32, 32 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );

    // ���������� ������������ �������� �� ������ �������
    // @info ����� "�� �������": ������� ������� - ���������, ������������ - ���������� ���������
    EXPECT_EQ(    8,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(   50,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  331,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 2396,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.


    // ���������
    doxel->lazySave( false );
    doxel->flush();

    EXPECT_EQ( Doxel3D::countDoxel_ValueOf,  store->count() ) <<
        "�-�� ���������� �������� �� �����. ����������. ������� � " <<
        Doxel3D::countDoxel_ValueOf << *doxel;

}

#endif







#ifdef TEST_IMAGE_HEIGHTMAP_64x64

// �������� ����� ����� (�������� - DF), ������ ���� 64x64
TEST_F( DoxelTest, ValueOf_HMImage_64x64_Map ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-64x64-map.png";

    const auto hmSize = d::sizeInt2d_t( 64, 64 );
    const float worldHeight = (float)hmSize.get<0>() / 6.4f * 2.0f;  // = 20 ��
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    // @info ����� "�� �������": ������� ������� - ���������, ������������ - ���������� ���������
    EXPECT_EQ(    8,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(   37,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  251,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 1878,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}

#endif







#ifdef TEST_IMAGE_HEIGHTMAP_128x128

// �������� ����� ����� (�������� - DF), ������ ���� 128x128
TEST_F( DoxelTest, ValueOf_HMImage_128x128_Map ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-128x128-map.png";

    const auto hmSize = d::sizeInt2d_t( 128, 128 );
    const float worldHeight = (float)hmSize.get<0>() / 12.8f * 2.0f;  // = 20 ��
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    // @info ����� "�� �������": ������� ������� - ���������, ������������ - ���������� ���������
    EXPECT_EQ(    8,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(   38,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  268,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 1997,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}

#endif








#ifdef TEST_IMAGE_HEIGHTMAP_256x256

// �������� ����� ����� (�������� - DF), ������ ���� 256x256
TEST_F( DoxelTest, ValueOf_HMImage_256x256_Map ) {

    // @see ����. � TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    initStore( (std::string)"t-" + ::testing::UnitTest::GetInstance()->current_test_info()->name() );

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-256x256-map.png";

    const auto hmSize = d::sizeInt2d_t( 256, 256 );
    const float worldHeight = (float)hmSize.get<0>() / 25.6f * 2.0f;  // = 20 ��
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    Doxel3D::countDoxel_ValueOf = 0;
    const auto doxel = Doxel3D::valueOf( nullptr, d::dant_t(), source, worldSize, 0, 127, 255 );
    //print( *doxel, "", false );


    // ���������

    // ���������� ������������ �������� �� ������ �������
    // @info ����� "�� �������": ������� ������� - ���������, ������������ - ���������� ���������
    EXPECT_EQ(    0,  doxel->countChild( 0 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(   0,  doxel->countChild( 1 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  0,  doxel->countChild( 2 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 0,  doxel->countChild( 3 ) ) << "������� � " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // � �.�.
}

#endif



} // namespace
