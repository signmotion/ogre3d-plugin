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







TEST_F( Vizi3DMapTest, Create_A_WorldInfo ) {

    const std::string source = "D:/Projects/workspace/ogre3d/Vizi3D/test/data/world/a";
    auto world = Vizi3DWorld( source, nullptr );

    const auto about = world.about();

    /* - ��������. ��. ����.
    EXPECT_EQ( 0, world.countWorkArea() ) << "�������� ���� �� ������.";
    EXPECT_EQ( d::ZEROINT3D, (d::coordInt3d_t)about.size() ) << "������ ���� ������ ���� ����������.";
    EXPECT_FALSE( about.infoWorld.pathToImageForm.empty() );
    EXPECT_FALSE( about.infoWorld.pathToImageMaterial.empty() );
    */
    Vizi3DWorld::about_t expected;
    expected.infoWorld.name = "�������� ��� �";
    expected.infoWorld.description = "������ ���� ������������� �������� �����, ��� �������� �������. ������� ���� �������� - ���������. ������� ����������� �����: Z->Y->X.";
    EXPECT_TRUE( expected == about );
}



} // namespace
