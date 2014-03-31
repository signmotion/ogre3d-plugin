#pragma once

#include <gtest/gtest.h>
#include "../include/Vizi3DWorld.h"
#include "default.h"



namespace {

/**
* �������� ��� Vizi3DMapTest.
*/
class Vizi3DMapTest : public ::testing::Test {
  protected:
    inline Vizi3DMapTest() {
	    setlocale( LC_ALL, "Russian" );
        // ��� ����������� '.' ������ ','
        setlocale( LC_NUMERIC, "C" );
    }


    virtual inline ~Vizi3DMapTest() {
    }


    virtual inline void SetUp() {
    }


    virtual inline void TearDown() {
    }


};







TEST_F( Vizi3DMapTest, Load_A ) {

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/Vizi3D/test/data/world/"
      + "a-4x3x2.json";

    auto world = Vizi3DWorld( nullptr );
    world.load( source );


    ASSERT_EQ( 1, world.tinyMap().size() ) << "���������� ������� ������� � ���������� �� ��������� � ���������.";

    // Test

    // map
    const auto oneTinyMap = world.tinyMap( "Test" );
    ASSERT_TRUE( oneTinyMap != nullptr ) << "������� � ����� ������ ������ ������������.";
    EXPECT_TRUE( d::coord3d_t( 0.0f, 0.0f, 0.0f ) == oneTinyMap->coord ) << "���������� �� ��������� � ����������."
        << " ��������: " << oneTinyMap->coord;
    EXPECT_TRUE(
           (oneTinyMap->size.get<0>() == 4)
        && (oneTinyMap->size.get<1>() == 3)
        && (oneTinyMap->size.get<2>() == 2)
    ) << "������ ������� �� ��������� � ���������."
      << " ��������: " << (d::coordInt3d_t)oneTinyMap->size;

    EXPECT_FLOAT_EQ( world.about().scale, oneTinyMap->scale ) << "������� ������� ����� �� ��������� � ���������.";

    auto expected = BitContent3D( d::sizeInt3d_t( 4, 3, 2 ) );
    expected.raw().set();
    const auto result = *oneTinyMap->content.cbegin()->second;
    EXPECT_TRUE( expected == result ) << "���������� �� ��������� � ���������."
        << " ��������: " << result.raw();

    // note
    const auto oneNoteMap = world.noteMap( 'X' );
    ASSERT_TRUE( oneNoteMap != nullptr );
    EXPECT_EQ( "�����", oneNoteMap->name );
    EXPECT_EQ( "block-1x1", oneNoteMap->form );
    EXPECT_EQ( "root/rock/slate", oneNoteMap->material );
    EXPECT_EQ( Vizi3DWorld::TYPE_ELEMENT_SOLID, oneNoteMap->type );

    // �� ������������ �������
    ASSERT_TRUE( world.noteMap( 'U' ) == nullptr );
}







TEST_F( Vizi3DMapTest, Load_B ) {

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/Vizi3D/test/data/world/"
      + "a-4x3x2-b-1x1x1.json";

    auto world = Vizi3DWorld( nullptr );
    world.load( source );

    ASSERT_EQ( 2, world.tinyMap().size() ) << "���������� ������� ������� � ���������� �� ��������� � ���������.";

    // TestA
    auto oneTinyMap = world.tinyMap( "TestA" );
    ASSERT_TRUE( oneTinyMap != nullptr ) << "������� � ����� ������ ������ ������������.";
    EXPECT_TRUE( d::coord3d_t( 0.0f, 0.0f, 0.0f ) == oneTinyMap->coord ) << "���������� �� ��������� � ����������."
        << " ��������: " << oneTinyMap->coord;
    EXPECT_TRUE(
           (oneTinyMap->size.get<0>() == 4)
        && (oneTinyMap->size.get<1>() == 3)
        && (oneTinyMap->size.get<2>() == 2)
    ) << "������ ������� �� ��������� � ���������."
      << " ��������: " << (d::coordInt3d_t)oneTinyMap->size;

    EXPECT_FLOAT_EQ( world.about().scale, oneTinyMap->scale ) << "������� ������� ����� �� ��������� � ���������.";

    auto expected = BitContent3D( d::sizeInt3d_t( 4, 3, 2 ) );
    expected.raw().set();
    auto result = *oneTinyMap->content.cbegin()->second;
    EXPECT_TRUE( expected == result ) << "���������� �� ��������� � ���������."
        << " ��������: " << result.raw();


    // TestB
    oneTinyMap = world.tinyMap( "TestB" );
    ASSERT_TRUE( oneTinyMap != nullptr ) << "������� � ����� ������ ������ ������������.";
    EXPECT_TRUE( d::coord3d_t( 0.0f, 1.0f, -1.0f ) == oneTinyMap->coord ) << "���������� �� ��������� � ����������."
        << " ��������: " << oneTinyMap->coord;
    EXPECT_TRUE(
           (oneTinyMap->size.get<0>() == 1)
        && (oneTinyMap->size.get<1>() == 1)
        && (oneTinyMap->size.get<2>() == 1)
    ) << "������ ������� �� ��������� � ���������."
      << " ��������: " << (d::coordInt3d_t)oneTinyMap->size;

    EXPECT_FLOAT_EQ( 1500, oneTinyMap->scale ) << "������� ������� ����� ������ ���� ������������.";

    expected = BitContent3D( d::sizeInt3d_t( 1, 1, 1 ) );
    expected.raw().set();
    result = *oneTinyMap->content.cbegin()->second;
    EXPECT_TRUE( expected == result ) << "���������� �� ��������� � ���������."
        << " ��������: " << result.raw();

}







// ��������� ����������� ��� ������������ �����
TEST_F( Vizi3DMapTest, Load_A_5x3x3 ) {

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/Vizi3D/test/data/world/"
      + "a-5x3x3.json";

    auto world = Vizi3DWorld( nullptr );
    world.load( source );


    const auto oneTinyMap = world.tinyMap( "Test" );
    ASSERT_TRUE( oneTinyMap != nullptr ) << "������� � ����� ������ ������ ������������.";

    const BitContent3D realContent = *oneTinyMap->content.cbegin()->second;
    const BitContent3D visualSolidContent = *oneTinyMap->visualContent.solidTypeBitImage;
    EXPECT_EQ( 45, realContent.count() );
    EXPECT_EQ( 45 - 3, visualSolidContent.count() ) << "���������� ���������� ����� ������ ���������� �� ���������.";

}



} // namespace
