// Для fopen()
#pragma warning( disable : 4996 )

#include "Doxel.h"

#define PNG_DEBUG 3
#include <png.h>



size_t Doxel3D::countDoxel_ValueOf = 0;



std::unordered_set<
    std::shared_ptr< Doxel3D >
> Doxel3D::sampling(
    const d::box3d_t& box,
    const boost::numeric::interval< float >& size
) {
    // @todo ...

    return std::unordered_set< std::shared_ptr< Doxel3D > >();
}




std::shared_ptr< Doxel3D >  Doxel3D::valueOf(
    std::shared_ptr< Doxel3D > parent,
    const d::dant_t& dant,
    const hmImage_t& hmImage,
    const d::size3d_t& worldSize
) {
    // Задача: поместить 2D карту высот в 3D доксель.

    // Размер карты высот
    const size_t N = (size_t)hmImage.first.get<0>();
    const size_t M = (size_t)hmImage.first.get<1>();
    const size_t NM = N * M;
    assert( (N == M) && "Работаем только с квадратными картами." );

    if ( (N != 1) && ((N & (N - 1)) != 0) ) {
        throw "The side of heightmap need be the power of 2.";
    }

    // Центром мировой карты будет центр карты высот 'hmImage'
    const size_t halfN = N / 2;
    const size_t halfM = M / 2;
    const size_t halfNM = halfN * halfM;

    // Последний доксель встретим особо (см. ниже)
    const bool notLastDoxel = (N > 1);

    // Размер докселя - максимальная сторона карты высот (мир должен
    // целиком поместиться в этом докселе).

    // Размер ячейки карты высот
    const float cellSize = worldSize.get<0>() / N;
    // Высота мира в ячейках карты высот
    size_t L = (size_t)( worldSize.get<2>() / cellSize );
    if (L < 1)  { L = 1; }
    // @todo Позволить высоте мира превышать размеры, заданные длиной и шириной.
    assert( ((L <= N) && (L <= M)) && "Высота не может превышать длину / ширину карты высот." );

    // Размер докселя (см. выше TODO для L)
    const float doxelSize = std::max(
        worldSize.get<0>(),  std::max( worldSize.get<1>(), worldSize.get<2>() )
    );

    // Доксель делит объём тремя перпендикулярными плоскостями -
    // образуется 8-мь октантов и, соотв., новых докселей. Пройдёмся
    // по всей карте высот и расширим её на 1 измерение.

    // Коэффициент для расчёта высоты
    // Высота хранится в диапазоне [-1.0; 1.0]
    //const float kh = (float)L / 2.0f;

    // Подготовим квадранты карты высот - отдадим их потомкам.
    // смотрим сверху на плоскость XY. КВ полностью заполняет
    // плоскость XY. Поэтому можем выделить 2 группы горизонтальных
    // квадрантов.
    // @info Контроль чётности сделали выше.
    /* - Заменено. См. ниже.
    float* qI    = notLastDoxel ? new float[ halfNM ] : nullptr;
    float* qII   = notLastDoxel ? new float[ halfNM ] : nullptr;
    float* qIII  = notLastDoxel ? new float[ halfNM ] : nullptr;
    float* qIV   = notLastDoxel ? new float[ halfNM ] : nullptr;
    float* qV    = notLastDoxel ? new float[ halfNM ] : nullptr;
    float* qVI   = notLastDoxel ? new float[ halfNM ] : nullptr;
    float* qVII  = notLastDoxel ? new float[ halfNM ] : nullptr;
    float* qVIII = notLastDoxel ? new float[ halfNM ] : nullptr;
    *//* - Заменено. См. ниже.
    boost::array< float*,  8 >  dant = {
        // верхние квадранты (z >= 0)
        nullptr, nullptr, nullptr, nullptr,
        // нижние квадранты (z < 0)
        nullptr, nullptr, nullptr, nullptr
    };
    */
    // Только один октант будет инициирован *точкой* высоты. См. причину ниже.
    boost::array< std::shared_ptr< float >,  8 >  dantMap = {};


    // Сколько и каких элементов содержит этот доксель
    // @see TODO в цикле ниже.
    boost::array< size_t, 10 >  countElement;
    countElement.fill( 0 );

    // I Определяем содержание самого докселя.
    // II Создаём потомков, передаём им части карты высот.
    // 8 докселей = 8 частей карты.

    // Надо понимать, что докселя не интересует сколько и чего содержится в
    // потомках. Ему только интересно, что ОН САМ хранит в себе. А хранит он
    // *обобщённые* данные о 3D-карте, данные, которые можно поместить в AboutCellCD.
    for (size_t j = 0; j < M; ++j) {
        for (size_t i = 0; i < N; ++i) {
            /* - Не то.
            // Переводим координаты карты высот в 3D пространство докселя
            const float height = kh * hmImage.second.get()[ i + j * N ];
            const auto worldCoord = d::coord3d_t(
                (i - halfHM.get<0>()) * cellSize,
                (j - halfHM.get<1>()) * cellSize,
                height
            );
            */

            // [-1.0; 1.0]x
            const float height = hmImage.second.get()[ i + j * N ];
            if (height == std::numeric_limits< float >::infinity()) {
                // Высота может отсутствовать
                continue;
            }

            // Переводим координаты карты высот в 3D пространство
            const int x = (int)i - (int)halfN;
            const int y = (int)j - (int)halfM;
            //const int z = (int)( kh * height );

            // Считаем сколько и каких элементов содержит этот доксель.
            // Сейчас для простоты полагаем, что доксель содержит 1 элемент.
            // @todo Подключить работу с картой биомов, чтобы разнообразить
            //       кол-во элементов.
            //++countElement[0]; - Без TODO не имеет смысла.

            // Смотрим, как карта высот заполнила октанты докселя и
            // тут же формируем карты для потомков. Заполнение октанта
            // (он же - потомок д.) зависит от значеня координаты высоты.
            // Причём, т.к. карта высот - квадратная и полностью заполняет
            // плоскость XY, т.о. можем выделить 2 группы квадрантов, лежащих
            // в плоскости XY.

            //assert( ((x != 0) && (y != 0) && (z != 0)) && "Не должно быть такой координаты: работаем только с кратными 2-ке сторонами карты." );

            if ( notLastDoxel ) {

                const size_t qx = (size_t)( (x >= 0) ? x : (x + (int)halfN) );
                const size_t qy = (size_t)( (y >= 0) ? y : (y + (int)halfM) );
                const size_t u = qx + qy * halfN;

                /* - Если высота мира много меньше высоты докселя, объём данных
                     получается слишком большой (карта размещается в центре
                     докселя). Заменено. См. ниже Формируем *только*
                     поверхность, глубины не трогаем.
                // Квадранты карты высот заполняем по мере необходимости.
                // Нельзя утверждать, что первая координата октанта окажется
                // первой координатой квадранта. Поэтому инициализация отдана
                // отдельному методу.
                const auto initDantMap = [ &dantMap, halfNM ] ( int k ) -> int {
                    if ( !dantMap[k] )  {
                        dantMap[k] = std::shared_ptr< float >( new float[ halfNM ] );
                        std::fill_n( dantMap[k].get(),  halfNM,  std::numeric_limits< float >::infinity() );
                    }
                    return k;
                };

                // Выберем октант
                int kTop = -1;
                int kBottom = -1;
                if (height >= 0.0f) {
                    // высота достаёт до верхнего октанта
                    // (инициируются обе плоскости)
                    if ( (x >= 0) && (y >= 0) ) {
                        kTop = initDantMap( 0 );
                        kBottom = initDantMap( 4 );
                    } else if ( (x >= 0) && (y < 0) ) {
                        kTop = initDantMap( 1 );
                        kBottom = initDantMap( 5 );
                    } else if ( (x < 0) && (y < 0) ) {
                        kTop = initDantMap( 2 );
                        kBottom = initDantMap( 6 );
                    } else if ( (x < 0) && (y >= 0) ) {
                        kTop = initDantMap( 3 );
                        kBottom = initDantMap( 7 );
                    } else {
                        assert( false && "Здесь нас быть не должно." );
                    }

                // (height < 0.0f)
                } else {
                    // высота остаётся в нижних октантах
                    if ( (x >= 0) && (y >= 0) ) {
                        kBottom = initDantMap( 4 );
                    } else if ( (x >= 0) && (y < 0) ) {
                        kBottom = initDantMap( 5 );
                    } else if ( (x < 0) && (y < 0) ) {
                        kBottom = initDantMap( 6 );
                    } else if ( (x < 0) && (y >= 0) ) {
                        kBottom = initDantMap( 7 );
                    } else {
                        assert( false && "Здесь нас быть не должно." );
                    }
                }

                // @test
                if ((kTop == -1) && (kBottom == -1)) {
                    const bool test = true;
                }

                // Заполним октант картой высот.
                // То, что октанты могут быть не затронуты высотой, проверили выше.
                assert( ((kTop != -1) || (kBottom != -1)) && "Координата должна была попасть хотя бы в один октант." );

                //float childHeight = std::numeric_limits< float >::infinity();
                //assert( (childHeight == std::numeric_limits< float >::infinity()) || ((childHeight >= -1.0f) && (childHeight <= 1.0f)) );

                // Может быть затронуты сразу два октанта (КВ - снизу-вверх)
                // Высота в потомке - уже другая высота: доксель делит её навпил.
                if (kTop != -1) {
                    assert( (height >= 0.0f) && "В эту плоскость не должна попадать отрицательная высота." );
                    // В верхнюю плоскость попадает верхушка высота. И тут же проверяем:
                    assert( (kBottom != -1) && "Высота не может заполнить верхний октант, пропустив нижний." );
                    const float hh = (height - 0.5f) * 2.0f;
                    assert( ((hh >= -1.0f) && (hh <= 1.0f)) && "Высота для верхней плоскости октанта вне допустимых пределов." );
                    dantMap[ kTop ].get()[u] = hh;
                }
                if (kBottom != -1) {
                    // И в нижнюю плоскость попадает верхушка высоты. Но в эту
                    // плоскость попадает и положительная (заполняет вертикаль
                    // целиком) высота, и отрицательная (некоторых потомков у
                    // докселя не будет).
                    assert( ((height < 0.0f) || ((height >= 0.0f) && (kTop != -1)))
                        && "В нижнюю плоскость не должна попадать положительная высота, если она не попала в верхнюю плоскость." );
                    const float hh = (height >= 0.0f) ? 1.0f : (height + 0.5f) * 2.0f;
                    assert( ((hh >= -1.0f) && (hh <= 1.0f)) && "Высота для нижней плоскости октанта вне допустимых пределов." );
                    dantMap[ kBottom ].get()[u] = hh;
                }
                */

                // Один и только один октант должен быть инициирован

                // Определяем дант точки высоты в докселе
                d::dant_t dt;
                dt[0] = (height < 0.0f) ? 0 : 1;
                dt[1] = (x < 0.0f) ? 0 : 1;
                dt[2] = (y < 0.0f) ? 0 : 1;
                const auto dtK = (size_t)dt.to_ulong();
                // Квадранты карты высот заполняем по мере необходимости.
                // Нельзя утверждать, что первая координата октанта окажется
                // первой координатой квадранта. Поэтому инициализация отдана
                // отдельному методу.
                const auto initDantMap = [ &dantMap, halfNM ] ( int k ) -> void {
                    if ( !dantMap[k] )  {
                        dantMap[k] = std::shared_ptr< float >( new float[ halfNM ] );
                        std::fill_n( dantMap[k].get(),  halfNM,  std::numeric_limits< float >::infinity() );
                    }
                };
                initDantMap( dtK );

                // Высота в потомке - уже другая высота: доксель делит её навпил
                if ( dt[0] ) {
                    // верхняя плоскость
                    assert( (height >= 0.0f) && "В эту плоскость не должна попадать отрицательная высота." );
                    const float hh = (height - 0.5f) * 2.0f;
                    assert( ((hh >= -1.0f) && (hh <= 1.0f)) && "Высота для верхней плоскости октанта вне допустимых пределов." );
                    dantMap[ dtK ].get()[u] = hh;
                } else {
                    // нижняя плоскость
                    // Высота попадает исключительно в одну плоскость. См.
                    // предв. реализацию и причину отказа от неё выше.
                    const float hh = (height >= 0.0f) ? 1.0f : (height + 0.5f) * 2.0f;
                    assert( ((hh >= -1.0f) && (hh <= 1.0f)) && "Высота для нижней плоскости октанта вне допустимых пределов." );
                    dantMap[ dtK ].get()[u] = hh;
                }

            } // if ( !lastDoxel )

        } // for (size_t i = 0; i < N; ++i)

    } // for (size_t j = 0; j < M; ++j)


    // III Создаём доксель.
    // @see TODO в цикле выше.
    auto content = std::shared_ptr< AboutCellCD >( new AboutCellCD( 1 ) );
#ifdef INCLUDE_DEBUG_INFO_IN_CONTENT
    // Вводим дополнительную информацию, чтобы идентифицировать это доксель
    content->test["N"] = boost::any( N );
    content->test["M"] = boost::any( M );
    std::ostringstream os;
    os << worldSize;
    content->test["worldSize"] = boost::any( os.str() );
    content->test["height-first-point"] = (hmImage.second.get()[0] == std::numeric_limits< float >::infinity())
        ? boost::any( (std::string)"infinity" )
        : boost::any( hmImage.second.get()[0] );
    content->test["sequence"] = boost::any( countDoxel_ValueOf );
#endif
    auto doxel = std::shared_ptr< Doxel3D >( new Doxel3D( doxelSize, content, parent, dant ) );


    // IV Формируем потомков.
    // Размер потомков всегда в 2 раза меньше размера родителя по каждому
    // измерению докселя. Все потомки - одного размера. Все они - тоже доксели.
    // Создаём потомков:
    //   + если размер докселя больше 1 (пиксель карты высот) (доксель
    //     не последний)
    //   + если потомок имеет какое-либо содержание (октант инициализировали
    //     при просмотре карты высот)
    if ( notLastDoxel ) {
        // Потомки есть у всех докселей, кроме представляющих самый
        // глубокий уровень карты
        assert( !doxel->child() && "Потомков быть ещё не должно." );
        const d::size3d_t childWorldSize = d::size3d_t(
            worldSize.get<0>() / 2.0f,
            worldSize.get<1>() / 2.0f,
            // Высоту - сохраняем
            //worldSize.get<2>()
            worldSize.get<2>() / 2.0f
        );
        const d::sizeInt2d_t childHMSize = hmImage.first / 2;
        for (auto itr = dantMap.cbegin(); itr != dantMap.cend(); ++itr) {
            // Не каждый октант инициализирован (см. выше)
            if ( *itr != nullptr ) {
                const auto k = std::distance( dantMap.cbegin(), itr );
                const hmImage_t hmImage = std::make_pair( childHMSize,  dantMap[k] );
                const auto ch = valueOf(
                    // = parent
                    doxel,
                    // = dant
                    d::dant_t( k ),
                    hmImage,
                    childWorldSize
                );
                doxel->child( k, ch );

                // Тут же выгружаем потомка во внешнее хранилище, если хотим
                // съесть карту высот размером более 256 x 256 пикселей.
                //std::cout << "before unload " << *doxel << std::endl;
                /*ch->lazySave( true );*/
                // lazySave() полностью сохраняет д., если последним идёт корневой д..
                // Здесь же мы получаем д. наоборот: начиная с самого большого и заканчивая
                // мелочью. Поэтому следует обязательно вызвать flush().

            } else {
                // @test
                //const bool test = true;
            }
        }
    } // if ( notLastDoxel )


    // Выгружаем корневой доксель во внешнее хранилище (см. потомков)
    // lazySave() полностью сохраняет д., если последним идёт корневой д..
    // Здесь же мы получаем д. наоборот: начиная с самого большого и заканчивая
    // мелочью. Поэтому для КД следует обязательно вызвать flush().
    /*
    if ( doxel->root() ) {
        //std::cout << "before unload " << *doxel << std::endl;
        doxel->lazySave( !true );
        doxel->flush();

    } else {
        doxel->lazySave( !true );
    }
    */


    // @todo ...

    // Уборка...
    /* - std::shared_ptr
    for (auto itr = dant.cbegin(); itr != dant.cend(); ++itr) {
        float* h = *itr;
        delete[] h;
    }
    */

    // Считаем кол-во созданных методом докселей
    // (!) Помним, что кол-во создаваемых докселей всегда больше кол-ва потомков.
    // @see countChild()
    ++countDoxel_ValueOf;

    // @test Отчёт о получившемся докселе
    //std::cout << "[ " << countDoxel_ValueOf << " ]" << std::endl;
    //print( *doxel,  (std::string)"[ " + boost::lexical_cast< std::string >( countDoxel_ValueOf ) + " ]" );

    return doxel;
}






Doxel3D::hmImage_t Doxel3D::loadHMImage(
    const std::string& source,
    const d::boxInt2d_t& box,
    float kh,
    size_t maxDepth,
    size_t seaLevel,
    size_t maxHeight,
    int chasm
) {
    assert( !source.empty() && "Путь к файлу с картой высот должен быть указан." );
    assert( d::correct( box ) );
    assert( ((box.first >= d::ZEROINT2D) && (box.second >= d::ZEROINT2D))
        && "Координаты коробки должны быть положительными." );
    assert( ((maxDepth <= seaLevel) && (seaLevel <= maxHeight))
        && "Надо так: maxDepth <= seaLevel <= maxHeight" );

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


    // Выясняем размер файла образа
    const size_t aAllImage = png_get_image_width( png, info );
    const size_t bAllImage = png_get_image_height( png, info );


    // Резервируем место под образ...
    auto image = (png_bytep*)malloc( sizeof( png_bytep ) * bAllImage );
    for (size_t j = 0; j < bAllImage; ++j) {
        image[j] = (png_byte*)malloc( png_get_rowbytes( png, info ) );
    }
    // ...и читаем изображение карты высот
    png_read_image( png, image );


    // Нужно использовать коробку или брать изображение целиком?
    const bool useBox = ((box.first > d::ZEROINT2D) || (box.second > d::ZEROINT2D));

    // Подводим размер под размер коробки
    // На 0 проверили выше, см. correct()
    d::boxInt2d_t correctBox = d::boxInt2d_t(
        d::coordInt2d_t( 0, 0 ),
        d::coordInt2d_t( ((int)aAllImage - 1), ((int)bAllImage - 1) )
    );
    if ( useBox ) {
        correctBox = d::boxInt2d_t(
            d::coordInt2d_t(
                (box.first.get<0>() >= (int)aAllImage) ? ((int)aAllImage - 1) : box.first.get<0>(),
                (box.first.get<1>() >= (int)bAllImage) ? ((int)bAllImage - 1) : box.first.get<1>()
            ),
            d::coordInt2d_t(
                (box.second.get<0>() >= (int)aAllImage) ? ((int)aAllImage - 1) : box.second.get<0>(),
                (box.second.get<1>() >= (int)bAllImage) ? ((int)bAllImage - 1) : box.second.get<1>()
            )
        );
    }

    const d::sizeInt2d_t correctSize = d::calcSize( correctBox );
    const size_t a = (size_t)correctSize.get<0>();
    const size_t b = (size_t)correctSize.get<1>();
    assert( (a > 0) && (b > 0) );


    // Собираем запрошенную часть поверхности
    const size_t AB = a * b;
    float* hm = new float[AB];
    // Центр карты (аналог hm_t::mapCenter)
    const float aHalf = (float)a / 2.0f;
    const float bHalf = (float)b / 2.0f;
    const float kn = kh * 2.0f / (float)(maxHeight - maxDepth + 1);
    const bool hasChasm = (chasm != -1);
    size_t t = 0;
    const size_t a0 = correctBox.first.get<0>();
    const size_t b0 = correctBox.first.get<1>();
    for (size_t j = b0; j < (b0 + b); ++j) {
        // Оказывается, полученное изображение - перевёрнуто
        const png_byte* row = image[ b0 + b - j - 1 ];
        for (size_t i = a0; i < (a0 + a); ++i) {
            // height mesh, as grayscale, for example (255,255,255); r,g,b are equals
            const png_byte* ptr = &( row[i] );
            // Для высоты организуем свою нормализацию
            // @see Комм. к 'hm_t'
            float height = (float)( ptr[0] );

            // Пропасть?
            if ( hasChasm && (height <= (float)chasm) ) {
                height = std::numeric_limits< float >::infinity();
            }

            // Обрезаем верхушки, подымаем низины
            if ( height != std::numeric_limits< float >::infinity() ) {
                if (height < (float)maxDepth) {
                    height = (float)maxDepth;
                } else if (height > (float)maxHeight) {
                    height = (float)maxHeight;
                }
                height -= (float)seaLevel;
                height *= kn;
            }

            // Реальная высота в мире
            /* - Нет. Оставляем [-1; 1].
            hm[t] = height * kz;
            */
            hm[t] = height;
            assert( (hm[t] >= -1.0f) && (hm[t] <= 1.0f) );

            ++t;
        }
    }

    // Убираем за собой
    png_destroy_read_struct( &png, nullptr, nullptr );
    fclose( fp );
    for (size_t j = 0; j < bAllImage; ++j) {
        free( image[j] );
    }
    free( image );

    return std::make_pair(
        d::sizeInt2d_t( a, b ),
        std::shared_ptr< float >( hm )
    );
}
