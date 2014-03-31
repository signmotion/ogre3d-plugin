#include "PlatoTrend.h"


template< size_t N >
Ogre::MeshPtr PlatoTrend< N >::mesh(
    Ogre::SceneManager* sm
) const {

    auto mesh = ahm->lowMesh( sm );

    return mesh;
}



template
Ogre::MeshPtr PlatoTrend< 100 >::mesh(
    Ogre::SceneManager* sm
) const;





/*
template< size_t N >
std::pair< sizeInt2d_t, png_bytep* >  PlatoTrend< N >::loadImage( const std::string& source ) {

    assert( !source.empty() );

    FILE* fp = fopen( source.c_str(), "rb" );
    if ( !fp ) {
        throw "File with heightmap is not open.";
    }
    unsigned char sig[8];
    fread( sig, 1, 8, fp );
    if ( !png_check_sig( sig, 8 ) ) {
        throw "File with heightmap is not in PNG format.";
    }

    png_structp png = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr
    );
    if ( !png ) {
        throw "PNG struct is not created.";
    }

    png_infop info = png_create_info_struct( png );
    if ( !info ) {
        png_destroy_read_struct( &png, nullptr, nullptr );
        throw "Info struct is not created.";
    }

    if ( setjmp( png_jmpbuf( png) ) ) {
        throw "Error during init_io.";
    }
    png_init_io( png, fp );
    png_set_sig_bytes( png, 8 );

    // @info Если выбрасывает ошибку, вероятно библиотека lpng скомпилирована
    //       в другой среде. Сделать одинаковыми.
    png_read_info( png, info );


    // Размеры карты
    const size_t a = png_get_image_width( png, info );
    const size_t b = png_get_image_height( png, info );
    assert( ((a == N) && (b == N)) && "Размер карты высот должен быть N x N (см. template)." );

    // Резервируем место под образ...
    png_bytep* image = (png_bytep*)malloc( sizeof( png_bytep ) * b );
    for (size_t j = 0; j < b; ++j) {
        image[j] = (png_byte*)malloc( png_get_rowbytes( png, info ) );
    }
    // ...и читаем изображение карты высот
    png_read_image( png, image );

    fclose( fp );

    return std::make_pair( sizeInt2d_t( a, b ),  image );
}



template
std::pair< sizeInt2d_t, png_bytep* >  PlatoTrend< 100 >::loadImage( const std::string& source );
*/
