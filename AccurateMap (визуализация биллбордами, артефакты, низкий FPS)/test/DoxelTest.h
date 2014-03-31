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
* Фикстура для Doxel.h
*/
class DoxelTest : public ::testing::Test {
  protected:
    inline DoxelTest() {
	    setlocale( LC_ALL, "Russian" );
        // Для разделителя '.' вместо ','
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

// В мире одна высота - максимальная, размер мира 1 x 1
TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ) {

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-maxheight.png";

    // По карте высот строим доксельную карту

    // Размер файла с картой высот, 1 пкс = 1 км
    // (используем для указания кратного размера мира)
    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    // Высота мира, км (заполнен весь доксель)
    const float worldHeight = (float)hmSize.get<0>();
    // Реальный размер мира по трём осям, км ->
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
        // -> переводим в метры
    ) * 1000.0f;

    // Строим доксель (который и есть карта)
    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // Проверяем

    // Заполнение докселя.
    // Технически, это количество созданных выше докселей.
    // Заполнение прямо пропорционально количеству созданных уровней точности,
    // и не может превышать ёмкости докселя равную  2 ^ ( D * L ).
    //
    // *Максимальная* ёмкость 3D-докселя в зависимости от уровня:
    //   0-й  -  1
    //   1-й  -  8
    //   2-й  -  64
    //   3-й  -  512
    //   4-й  -  4 096
    //   5-й  -  32 768
    //   6-й  -  262 144
    //   7-й  -  2 097 152
    //   8-й  -  16 777 216
    //   9-й  -  134 217 728
    //  10-й  -  1 073 741 824
    //
    // *Максимальная* ёмкость 2D-докселя в зависимости от уровня:
    //   0-й  -  1
    //   1-й  -  4
    //   2-й  -  16
    //   3-й  -  64
    //   4-й  -  256
    //   5-й  -  1 024
    //   6-й  -  4 096
    //   7-й  -  16 384
    //   8-й  -  65 536
    //   9-й  -  262 144
    //  10-й  -  1 048 576
    //
    // Максимальный уровень точности докселя, представленный картой высот
    // размером 528 x 528 (1 пиксель = 1 высота) вычисляется ...
    // ...

    // Проверяем заполненность докселя
    //const size_t volume = doxel.volume( 0 );

    // Количество получившихся потомков на разных уровнях.
    // Нулевой уровень любого докселя - это 2^3 = 8 потомков или их
    // полное отсутствие. На последующих уровнях (уровнях точности)
    // количество потомков может увеличиваться до восьмикрат.
    EXPECT_EQ( 0,  doxel->countChild( 0 ) );
    EXPECT_EQ( 0,  doxel->countChild( 1 ) );
    EXPECT_EQ( 0,  doxel->countChild( 2 ) );
    EXPECT_EQ( 0,  doxel->countChild( 3 ) );
    // и т.д.
}







// В мире одна высота - на уровне моря, размер мира 1x1
TEST_F( DoxelTest, ValueOf_HMImage_1x1_Sealine ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // Проверяем

    // Количество получившихся потомков на разных уровнях
    EXPECT_EQ( 0,  doxel->countChild( 0 ) );
    EXPECT_EQ( 0,  doxel->countChild( 1 ) );
    EXPECT_EQ( 0,  doxel->countChild( 2 ) );
    EXPECT_EQ( 0,  doxel->countChild( 3 ) );
    // и т.д.
}









// В мире одна глубина - максимальная, размер мира 1x1
TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxDepth ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-1x1-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 1, 1 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // Проверяем

    // Количество получившихся потомков на разных уровнях
    EXPECT_EQ( 0,  doxel->countChild( 0 ) );
    EXPECT_EQ( 0,  doxel->countChild( 1 ) );
    EXPECT_EQ( 0,  doxel->countChild( 2 ) );
    EXPECT_EQ( 0,  doxel->countChild( 3 ) );
    // и т.д.
}

#endif








#ifdef TEST_IMAGE_HEIGHTMAP_2x2

// В мире одна высота - максимальная, размер мира 2x2
TEST_F( DoxelTest, ValueOf_HMImage_2x2_MaxHeight ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-maxheight.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // Проверяем

    // Количество получившихся потомков на разных уровнях
    EXPECT_EQ( 8,  doxel->countChild( 0 ) );
    EXPECT_EQ( 8,  doxel->countChild( 1 ) );
    EXPECT_EQ( 8,  doxel->countChild( 2 ) );
    EXPECT_EQ( 8,  doxel->countChild( 3 ) );
    // и т.д.
}








// В мире одна высота - на уровне моря, размер мира 2x2
TEST_F( DoxelTest, ValueOf_HMImage_2x2_Sealine ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // Проверяем

    // Количество получившихся потомков на разных уровнях.
    // Верхние октанты тоже заполнены, т.к. уровень моря (z == 0) принадлежит им.
    EXPECT_EQ( 8,  doxel->countChild( 0 ) );
    EXPECT_EQ( 8,  doxel->countChild( 1 ) );
    EXPECT_EQ( 8,  doxel->countChild( 2 ) );
    EXPECT_EQ( 8,  doxel->countChild( 3 ) );
    // и т.д.
}









// В мире одна глубина - максимальная, размер мира 2x2
TEST_F( DoxelTest, ValueOf_HMImage_2x2_MaxDepth ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-2x2-maxdepth.png";

    const auto hmSize = d::sizeInt2d_t( 2, 2 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // Проверяем

    // Количество получившихся потомков на разных уровнях.
    // Заполнены только нижние октанты докселя (z < 0).
    EXPECT_EQ( 4,  doxel->countChild( 0 ) );
    EXPECT_EQ( 4,  doxel->countChild( 1 ) );
    EXPECT_EQ( 4,  doxel->countChild( 2 ) );
    EXPECT_EQ( 4,  doxel->countChild( 3 ) );
    // и т.д.
}

#endif








#ifdef TEST_IMAGE_HEIGHTMAP_4x4

// В мире одна высота - максимальная, размер мира 4x4
TEST_F( DoxelTest, ValueOf_HMImage_4x4_MaxHeight ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

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


    // Проверяем

    // Количество получившихся потомков на разных уровнях
    // (!) Количество потомков всегда меньше кол-ва созданных докселей.
    EXPECT_EQ(  8,  doxel->countChild( 0 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 1 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 2 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 3 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // и т.д.
}







// В мире одна высота - на уровне моря, размер мира 4x4
TEST_F( DoxelTest, ValueOf_HMImage_4x4_Sealine ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

    const std::string source = std::string()
      + "D:/Projects/workspace/ogre3d/AccurateMap/test/data/"
      + "elevation-4x4-sealine.png";

    const auto hmSize = d::sizeInt2d_t( 4, 4 );
    const float worldHeight = (float)hmSize.get<0>();
    const auto worldSize = d::size3d_t(
        (float)hmSize.get<0>(),  (float)hmSize.get<1>(),  worldHeight
    ) * 1000.0f;

    const auto doxel = Doxel3D::valueOf( nullptr, source, worldSize, 0, 127, 255 );


    // Проверяем

    // Количество получившихся потомков на разных уровнях.
    // Верхние октанты заполнены *частично*, т.к. уровень моря (z == 0) принадлежит им.
    EXPECT_EQ(  8,  doxel->countChild( 0 ) );
    EXPECT_EQ( 48,  doxel->countChild( 1 ) );
    EXPECT_EQ( 48,  doxel->countChild( 2 ) );
    EXPECT_EQ( 48,  doxel->countChild( 3 ) );
    // и т.д.
}








// В мире одна глубина - максимальная, размер мира 4x4
TEST_F( DoxelTest, ValueOf_HMImage_4x4_MaxDepth ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

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


    // Проверяем

    // Количество получившихся потомков на разных уровнях.
    // Заполнены только нижние октанты докселя (z < 0).
    EXPECT_EQ(  4,  doxel->countChild( 0 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 1 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 2 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 3 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // и т.д.
}

#endif








#ifdef TEST_IMAGE_HEIGHTMAP_8x8

// В мире одна высота - максимальная, размер мира 8x8
TEST_F( DoxelTest, ValueOf_HMImage_8x8_MaxHeight ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

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


    // Проверяем

    // Количество получившихся потомков на разных уровнях
    EXPECT_EQ(   8,  doxel->countChild( 0 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  64,  doxel->countChild( 1 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 512,  doxel->countChild( 2 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 512,  doxel->countChild( 3 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // и т.д.
}








// В мире одна высота - на уровне моря, размер мира 8x8
TEST_F( DoxelTest, ValueOf_HMImage_8x8_Sealine ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

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


    // Проверяем

    // Количество получившихся потомков на разных уровнях.
    // Верхние октанты тоже заполнены, т.к. уровень моря (z == 0) принадлежит им.
    EXPECT_EQ(   8,  doxel->countChild( 0 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  48,  doxel->countChild( 1 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 320,  doxel->countChild( 2 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 320,  doxel->countChild( 3 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // и т.д.
}









// В мире одна глубина - максимальная, размер мира 8x8
TEST_F( DoxelTest, ValueOf_HMImage_8x8_MaxDepth ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

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


    // Проверяем

    // Количество получившихся потомков на разных уровнях.
    // Заполнены только нижние октанты докселя (z < 0).
    EXPECT_EQ(  4,  doxel->countChild( 0 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 16,  doxel->countChild( 1 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 2 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 64,  doxel->countChild( 3 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // и т.д.
}

#endif







#ifdef TEST_IMAGE_HEIGHTMAP_16x16

// Высота - равномерный градиент из левого нижнего угла в верхний правый,
// размер мира 16x16
TEST_F( DoxelTest, ValueOf_HMImage_16x16_Gradient ) {

    // @see Комм. в TEST_F( DoxelTest, ValueOf_HMImage_1x1_MaxHeight ).

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


    // Проверяем

    // Количество получившихся потомков на разных уровнях
    // @info Взято "по доверию": меньшие размеры - считались, визуализация - смотрелось нормально
    EXPECT_EQ(    7,  doxel->countChild( 0 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(   47,  doxel->countChild( 1 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ(  319,  doxel->countChild( 2 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    EXPECT_EQ( 2300,  doxel->countChild( 3 ) ) << "Создано Д " << Doxel3D::countDoxel_ValueOf << std::endl << *doxel;
    // и т.д.
}

#endif



} // namespace
