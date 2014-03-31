#pragma once

#include <gtest/gtest.h>
#include "../include/Vizi3DWorld.h"
#include "default.h"



namespace {

/**
* Фикстура для Vizi3DMapTest.
*/
class Vizi3DMapTest : public ::testing::Test {
  protected:
    inline Vizi3DMapTest() {
	    setlocale( LC_ALL, "Russian" );
        // Для разделителя '.' вместо ','
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

    /* - Заменено. См. ниже.
    EXPECT_EQ( 0, world.countWorkArea() ) << "Областей быть не должно.";
    EXPECT_EQ( d::ZEROINT3D, (d::coordInt3d_t)about.size() ) << "Размер мира должен быть неопределён.";
    EXPECT_FALSE( about.infoWorld.pathToImageForm.empty() );
    EXPECT_FALSE( about.infoWorld.pathToImageMaterial.empty() );
    */
    Vizi3DWorld::about_t expected;
    expected.infoWorld.name = "Тестовый мир А";
    expected.infoWorld.description = "Размер мира декларируется наличием папок, где хранятся области. Размеры всех областей - одинаковы. Порядок вложенности папок: Z->Y->X.";
    EXPECT_TRUE( expected == about );
}



} // namespace
