#pragma once

#include <gtest/gtest.h>
#include "../include/ContentDoxel.h"


namespace {

/**
* �������� ��� CDManager.
*/
class CDManagerTest : public ::testing::Test {
  protected:
    inline CDManagerTest() {
	    setlocale( LC_ALL, "Russian" );
        // ��� ����������� '.' ������ ','
        setlocale( LC_NUMERIC, "C" );
    }

    virtual inline ~CDManagerTest() {
    }

    virtual inline void SetUp() {
        cdm = std::shared_ptr< CDManager >( CDManager::singleton() );
    }

    virtual void TearDown() {
        cdm->clear();
    }


    /**
    * �������������� �������� ���������.
    */
    std::shared_ptr< CDManager >  cdm;
};






// � ���� ���� ������ - ������������, ������ ���� 1 x 1
TEST_F( CDManagerTest, GetAndSetContent ) {

    EXPECT_EQ( 0, cdm->count() );

    const AboutCellCD a( 0 );
    const AboutCellCD b( 100 );
    const AboutCellCD c( 2000 );

    // a
    const auto ap = cdm->content( a );
    EXPECT_EQ( 1, cdm->count() );

    // ������ �������� ���������� ���� �������
    const auto ap1 = cdm->content( a );
    EXPECT_EQ( 1, cdm->count() );
    EXPECT_EQ( ap.get(), ap1.get() );

    // b
    cdm->content( b );
    EXPECT_EQ( 2, cdm->count() );

    // c
    cdm->content( c );
    EXPECT_EQ( 3, cdm->count() );

    // � ��� ��� �������� 'a'
    const auto ap2 = cdm->content( a );
    EXPECT_EQ( 3, cdm->count() );
    EXPECT_EQ( ap1.get(), ap2.get() );
}



} // namespace
