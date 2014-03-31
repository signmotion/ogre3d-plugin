#include "AHeightmap.h"
#include "helper.h"
#include "TrendManager.h"


AHeightmap::AHeightmap(
    const std::string& file,
    size_t maxDepth,
    size_t seaLevel,
    size_t maxHeight,
    const size3d_t& worldSize
) :
    lowMesh_( nullptr ),
    image_( nullptr )
{
    lowHM.source = file;
    lowHM.maxDepth = maxDepth;
    lowHM.seaLevel = seaLevel;
    lowHM.maxHeight = maxHeight;

    // Загружаем изображение карты высот
    const auto r = loadImage( lowHM.source );
    lowHM.mapSize = r.first;
    image_ = r.second;
    assert( image_ && "Образ карты высот загрузить не удалось." );

    lowHM.mapCenter = coordInt2d_t( lowHM.mapSize.get<0>() / 2, lowHM.mapSize.get<1>() / 2 );
    lowHM.worldSize = worldSize;

    std::cout << "Карта: " << lowHM.source << std::endl;
    std::cout << "Размер карты, пкс: " << lowHM.mapSize.get<0>() << " x " << lowHM.mapSize.get<1>() << std::endl;
    std::cout << "Размер мира, км: " << lowHM.worldSize.get<0>() / 1000.0f << " x " << lowHM.worldSize.get<1>() / 1000.0f << std::endl;

    assert( ((lowHM.mapSize.get<0>() > 0) && (lowHM.mapSize.get<1>() > 0)) && "Размер карты определить не удалось." );
    assert( ((lowHM.worldSize.get<0>() / (float)lowHM.mapSize.get<0>()) == (lowHM.worldSize.get<1>() / (float)lowHM.mapSize.get<1>()))
         && "Пропорции размеров по осям XY должны быть одинаковы, т.к. плоскость ячейки на поверхности XY представляет собой квадрат."
    );
    assert( !emptyAny( lowHM.worldSize ) && "Не указан размер мира." );
    assert(seaLevel  >= maxDepth);
    assert(seaLevel  <= maxHeight);
    assert(maxHeight >= maxDepth);
}








Ogre::MeshPtr AHeightmap::lowMesh( Ogre::SceneManager* sm ) const {

    // @example http://www.ogre3d.org/tikiwiki/Heightmap+to+Mesh+by+MOGRE

#if 0
    /// Create the mesh via the MeshManager
    Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual("ColourCube", "General");
 
    /// Create one submesh
    Ogre::SubMesh* sub = msh->createSubMesh();
 
    const float sqrt13 = 0.577350269f; /* sqrt(1/3) */
 
    /// Define the vertices (8 vertices, each consisting of 2 groups of 3 floats
    const size_t nVertices = 8;
    const size_t vbufCount = 3*2*nVertices;
    float vertices[vbufCount] = {
            -100.0,100.0,-100.0,        //0 position
            -sqrt13,sqrt13,-sqrt13,     //0 normal
            100.0,100.0,-100.0,         //1 position
            sqrt13,sqrt13,-sqrt13,      //1 normal
            100.0,-100.0,-100.0,        //2 position
            sqrt13,-sqrt13,-sqrt13,     //2 normal
            -100.0,-100.0,-100.0,       //3 position
            -sqrt13,-sqrt13,-sqrt13,    //3 normal
            -100.0,100.0,100.0,         //4 position
            -sqrt13,sqrt13,sqrt13,      //4 normal
            100.0,100.0,100.0,          //5 position
            sqrt13,sqrt13,sqrt13,       //5 normal
            100.0,-100.0,100.0,         //6 position
            sqrt13,-sqrt13,sqrt13,      //6 normal
            -100.0,-100.0,100.0,        //7 position
            -sqrt13,-sqrt13,sqrt13,     //7 normal
    };
 
    Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();
    Ogre::RGBA colours[nVertices];
    Ogre::RGBA *pColour = colours;
    // Use render system to convert colour value since colour packing varies
    rs->convertColourValue(Ogre::ColourValue(1.0,0.0,0.0), pColour++); //0 colour
    rs->convertColourValue(Ogre::ColourValue(1.0,1.0,0.0), pColour++); //1 colour
    rs->convertColourValue(Ogre::ColourValue(0.0,1.0,0.0), pColour++); //2 colour
    rs->convertColourValue(Ogre::ColourValue(0.0,0.0,0.0), pColour++); //3 colour
    rs->convertColourValue(Ogre::ColourValue(1.0,0.0,1.0), pColour++); //4 colour
    rs->convertColourValue(Ogre::ColourValue(1.0,1.0,1.0), pColour++); //5 colour
    rs->convertColourValue(Ogre::ColourValue(0.0,1.0,1.0), pColour++); //6 colour
    rs->convertColourValue(Ogre::ColourValue(0.0,0.0,1.0), pColour++); //7 colour
 
    /// Define 12 triangles (two triangles per cube face)
    /// The values in this table refer to vertices in the above table
    const size_t ibufCount = 36;
    unsigned short faces[ibufCount] = {
            0,2,3,
            0,1,2,
            1,6,2,
            1,5,6,
            4,6,5,
            4,7,6,
            0,7,4,
            0,3,7,
            0,5,1,
            0,4,5,
            2,7,3,
            2,6,7
    };
 
    /// Create vertex data structure for 8 vertices shared between submeshes
    msh->sharedVertexData = new Ogre::VertexData();
    msh->sharedVertexData->vertexCount = nVertices;
 
    /// Create declaration (memory format) of vertex data
    Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;
    // 1st buffer
    decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
    offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    Ogre::HardwareVertexBufferSharedPtr vbuf = 
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    /// Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);
 
    /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
    bind->setBinding(0, vbuf);
 
    // 2nd buffer
    offset = 0;
    decl->addElement(1, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_COLOUR );
    /// Allocate vertex buffer of the requested number of vertices (vertexCount) 
    /// and bytes per vertex (offset)
    vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    /// Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), colours, true);
 
    /// Set vertex buffer binding so buffer 1 is bound to our colour buffer
    bind->setBinding(1, vbuf);
 
    /// Allocate index buffer of the requested number of vertices (ibufCount) 
    Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
        createIndexBuffer(
        Ogre::HardwareIndexBuffer::IT_16BIT, 
        ibufCount, 
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
 
    /// Upload the index data to the card
    ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);
 
    /// Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = ibufCount;
    sub->indexData->indexStart = 0;
 
    /// Set bounding information (for culling)
    msh->_setBounds( Ogre::AxisAlignedBox(-100,-100,-100,100,100,100 ));
    msh->_setBoundingSphereRadius( Ogre::Math::Sqrt(3*100*100) );
 
    /// Notify -Mesh object that it has been loaded
    msh->load();

    return msh;

#else

    // Используем кеш
    if ( !lowMesh_.isNull() ) {
        return lowMesh_;
    }
 
    auto mesh = Ogre::MeshManager::getSingleton().createManual( lowHM.source, "General" );
    auto sub = mesh->createSubMesh();

    const auto vd = AHeightmap::vertexData(
        lowHM.source,
        //boxInt2d_t( sizeInt2d_t( 200, 300 ), lowHM.mapSize ),
        boxInt2d_t( ZEROINT2D, lowHM.mapSize ),
        lowHM.maxDepth,
        lowHM.seaLevel,
        lowHM.maxHeight,
        lowHM.worldSize
    );
    // Получаем откорректированный размер визуализируемой области
    const sizeInt2d_t sizeInCell = vd.get<0>();
    const size_t a = (size_t)sizeInCell.get<0>();
    const size_t b = (size_t)sizeInCell.get<1>();
    const size_t AB = a * b;

    // Create vertex data structure for vertices shared between submeshes
    mesh->sharedVertexData = new Ogre::VertexData();
    mesh->sharedVertexData->vertexCount = AB;
 
    // Create declaration (memory format) of vertex data
    Ogre::VertexDeclaration* decl = mesh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;

    // 1st buffer
    decl->addElement(
        0, offset,
        Ogre::VET_FLOAT3,
        Ogre::VES_POSITION
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    decl->addElement(
        0, offset,
        Ogre::VET_FLOAT3,
        Ogre::VES_NORMAL
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    // Allocate vertex buffer of the requested number of vertices (vertexCount)
    // and bytes per vertex (offset)
    Ogre::HardwareVertexBufferSharedPtr vbuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            offset,
            mesh->sharedVertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
        );
    // Upload the vertex data to the card
    const auto vertices = vd.get<1>();
    vbuf->writeData( 0, vbuf->getSizeInBytes(), vertices.get(), true );
 
    // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    mesh->sharedVertexData->vertexBufferBinding->setBinding( 0, vbuf );
 
    
    // Vertex color
    size_t* color = new size_t[AB];
    for (size_t i = 0; i < AB; ++i) {
        color[i] = 0xFFFFFFFF;
    }

    // Строим индексы
    std::vector< size_t >  index;
    size_t jf = 0;
    for (size_t j = 0; j < b - 1; ++j) {
        for (size_t i = 0; i < a; ++i) {
            if ((i - 1) >= 0) {
                index.push_back( jf - 1 );
                index.push_back( jf );
                index.push_back( a + jf );
            }
            if ((i + 1) <= a) {
                index.push_back( jf );
                index.push_back( a + 1 + jf );
                index.push_back( a + jf );                       
            }
            jf++;
        }
    }

    const size_t indexSize = index.size();
    size_t* face = new size_t[ indexSize ];
    for (size_t i = 0; i < indexSize; ++i) {
        face[i] = index[i];
    }
 
    // 2nd buffer
    offset = 0;
    decl->addElement(
        1, offset,
        Ogre::VET_COLOUR,
        Ogre::VES_DIFFUSE
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_COLOUR );
    // Allocate vertex buffer of the requested number of vertices (vertexCount)
    // and bytes per vertex (offset)
    vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        offset,
        mesh->sharedVertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
    );
    // Upload the vertex data to the card
    vbuf->writeData( 0, vbuf->getSizeInBytes(), color, true );
 
    // Set vertex buffer binding so buffer 1 is bound to our colour buffer
    mesh->sharedVertexData->vertexBufferBinding->setBinding( 1, vbuf );
 
    /// Allocate index buffer of the requested number of vertices (ibufCount)
    Ogre::HardwareIndexBufferSharedPtr ibuf =
        Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_32BIT,
            indexSize,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
        );
 
    // Upload the index data to the card
    ibuf->writeData( 0, ibuf->getSizeInBytes(), face, true );
 
    /// Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = indexSize;
    sub->indexData->indexStart = 0;
 
    // Set bounding information (for culling)
    // (!) Если границу не задать или задать не правильно, меш будет исчезать
    // как только граница выйдет из области видимости камеры.
    const Ogre::AxisAlignedBox aab = vd.get<2>();
    mesh->_setBounds( aab );
    const float radius = vd.get<3>();
    mesh->_setBoundingSphereRadius( radius );

    // Notify - Mesh object that it has been loaded
    mesh->load();


    // Убираем за собой
    delete[] face;
    delete[] color;

    // Кешируем результат
    lowMesh_ = mesh;

    return mesh;

#endif;

}








template< size_t N >
std::pair< Ogre::MeshPtr, normal3d_t >  AHeightmap::hightMesh(
    Ogre::SceneManager* sm,
    const coordInt2d_t& aim
) const {

    // Делим поверхность (плато) в точке 'aim' на прямоугольные ячейки
    //const float detailSize = sizeCell() / (float)N;
    //boost::multi_array< float, 3 >  hightMap( boost::extents[N][N] );

    // Делим плато на 4 треугольника, чтобы определить нормали и применить
    // подходящий тренд
    const auto vnc = vertexNormalCell( aim );

    // Определяем угол между плоскостью XY и нормалью плато, приводим угол к
    // требуемому для определения тренда положению
    //auto cn = Ogre::Vector3( vnc[0].get<0>(), vnc[0].get<1>(), vnc[0].get<2>() );
    //const float angle = Ogre::Vector3( 1.0f, 1.0f, 0.0f ).angleBetween( cn ).valueDegrees() - 90.0f;
    //std::cout << "Угол " << angle << std::endl;


    // Работаем с готовыми трендами в виде карт высот

    // I Определяем тренд, подходящий этому плато
    const float size = sizeCell();
    const auto platoSize = size3d_t(
        // X
        (float)N * size,
        // Y
        (float)N * size,
        // Z
        // Высота тренда не может превышать размер ячейки (куб).
        // Для идеально ровной поверхности тренду достаточно предоставить
        // изображение со значением 127 в R-канале (см. Trend).
        (float)N * size
    );
    auto trend = TrendManager::singleton()->platoTrend< N >(
        "D:/Projects/workspace/ogre3d/AccurateHeightmap/media/map/a/trend/",
        platoSize,
        vnc[0]
    );

    // II Формируем меш карты высот
    auto mesh = trend->mesh( sm );

    // Нормаль пригодится для поворота полученной модели относительно нд-мира
    return std::make_pair( mesh, trend->normal() );
}



template
std::pair< Ogre::MeshPtr, normal3d_t >  AHeightmap::hightMesh< 100 >(
    Ogre::SceneManager* sm,
    const coordInt2d_t& aim
) const;









float AHeightmap::heightCell( const coordInt2d_t& aim ) const {

    // Размер файла образа мы выяснили в конструкторе (mapSize). Здесь
    // учитываем, что входящая координата указана относительно hm_t::mapCenter.
    const coordInt2d_t hmCoord = aim + lowHM.mapCenter;
    if ( (hmCoord >= lowHM.mapSize) || (hmCoord < ZEROINT2D) ) {
        throw "Coord is out of bounds.";
    }

    // Получаем высоту

    // Оказывается, полученное изображение - перевёрнуто
    const png_byte* row = image_[ lowHM.mapSize.get<1>() - hmCoord.get<1>() - 1 ];
    // height mesh, as grayscale, for example (255,255,255); r,g,b are equals
    const png_byte* ptr = &( row[ hmCoord.get<0>() ] );
    // Для высоты организована нормализация
    // Диапазон высот после нормализации выше: [-1; 1]
    // @see Комм. к 'hm_t'
    float height = (float)( ptr[0] );
    if (height < (float)lowHM.maxDepth) {
        height = (float)lowHM.maxDepth;
    } else if (height > (float)lowHM.maxHeight) {
        height = (float)lowHM.maxHeight;
    }
    height -= (float)lowHM.seaLevel;
    const float kn = 1.0f / (float)(lowHM.maxHeight - lowHM.maxDepth);
    height *= kn;
    assert( (height >= -1.0f) && (height <= 1.0f) );

    const float kz = lowHM.worldSize.get<2>() / 2.0f;
    height *= kz;

    return height;
}









boost::array< float, 4 >  AHeightmap::vertexDeltaCell( const coordInt2d_t& aim ) const {

    boost::array< float, 4 >  vdc;

    // Получаем высоту самой вершины и соседей рядом с ней
    // @see Нумерацию в helper::isc()
    boost::array< float, 9 >  h;
    for (size_t k = 0; k <= 8; ++k) {
        const auto shift = helper::isc( k );
        auto p = aim + shift;
        const auto hmCoord = p + lowHM.mapCenter;
        if ( helper::outsideMatrix( hmCoord, lowHM.mapSize ) ) {
            // Если ячейка лежит на границе - высоту берём ту же, что у центральной
            p = aim;
        }
        h[k] = heightCell( p );
    }

    // Считаем значения для своих четырёх вершин. Обходим по часовой стрелке.
    // @see Комм. к методу.

    // правая верхняя
    vdc[0] = (h[5] + h[2] + h[0] + h[1]) / 4.0f - h[0];
    // правая нижняя
    vdc[1] = (h[2] + h[6] + h[3] + h[0]) / 4.0f - h[0];
    // левая нижняя
    vdc[2] = (h[0] + h[3] + h[7] + h[4]) / 4.0f - h[0];
    // левая верхняя
    vdc[3] = (h[1] + h[0] + h[4] + h[8]) / 4.0f - h[0];

    return vdc;
}








boost::array< coord3d_t, 5 >  AHeightmap::vertexCoordCell( const coordInt2d_t& aim ) const {

    // @see Комм. к методу.
    boost::array< coord3d_t, 5 >  r;

    const auto vdc = vertexDeltaCell( aim );
    // Высота ячейки нд-карты
    const float h = heightCell( aim );
    // Сторона ячейки нд-карты
    const float size = sizeCell();
    // центр
    r[0] = coord3d_t(
        aim.get<0>() * size,
        aim.get<1>() * size,
        h
    );
    for (size_t i = 1; i <= 4; ++i) {
        const coordInt2d_t shift = helper::isc( i );
        // вершины вокруг центра
        r[i] = coord3d_t(
            (aim.get<0>() + (float)shift.get<0>() / 2.0f) * size,
            (aim.get<1>() + (float)shift.get<1>() / 2.0f) * size,
            h + vdc[i-1]
        );
    }

    return r;
}








boost::array< normal3d_t, 5 >  AHeightmap::vertexNormalCell( const coordInt2d_t& aim ) const {

    // @see Комм. к методу.

    // Получаем вершины, усреднённые по соседям - это поверхность (плато)
    // @copy из vertexCoordCell():
    //   4         1
    //   .---------.
    //   |         |
    //   |    0    |
    //   |         |
    //   .---------.
    //   3         2
    //
    const auto vcc = vertexCoordCell( aim );
    const coord3d_t v0 = vcc[0];

    // Делим полученную поверхность на 4 треугольника, чтобы определить
    // нормали для каждого из них и для поверхности в целом
    // @copy из quadroTriangle3d_t:
    //   .---------.
    //   | \  0  / |
    //   |  \   /  |
    //   | 3  *  1 |
    //   |  /   \  |
    //   | /  2  \ |
    //   .---------.
    //
    const triangle3d_t a = { v0, vcc[4], vcc[1] };
    const triangle3d_t b = { v0, vcc[1], vcc[2] };
    const triangle3d_t c = { v0, vcc[2], vcc[3] };
    const triangle3d_t d = { v0, vcc[3], vcc[4] };
    const quadroTriangle3d_t qt = { a, b, c, d };

    // Вычисляем нормаль для каждого треугольника и общую нормаль для плато
    boost::array< normal3d_t, (qt.static_size + 1) >  qn;
    qn[0] = ZERO3D;
    for (size_t i = 0; i < qt.static_size; ++i) {
        //std::cout << "Traingle " << i << " " << qt[i] << std::endl;
        const coord3d_t v = qt[i][2] - qt[i][0];
        const coord3d_t w = qt[i][1] - qt[i][0];
        const auto o = Ogre::Vector3( v.get<0>(), v.get<1>(), v.get<2>() )
            .crossProduct( Ogre::Vector3( w.get<0>(), w.get<1>(), w.get<2>() ) )
            .normalisedCopy();
        qn[ i + 1 ] = vector3d_t( o.x, o.y, o.z );
        /*
        // @source http://forum.sources.ru/index.php?showtopic=63444
        // N = a x b = [(a_y*b_z - a_z*b_y), (a_z*b_x - a_x*b_z), (a_x*b_y - a_y*b_x)]
        const auto vx = v.get<0>();
        const auto vy = v.get<1>();
        const auto vz = v.get<2>();
        const auto wx = w.get<0>();
        const auto wy = w.get<1>();
        const auto wz = w.get<2>();
        qn[i] = Ogre::Vector3(
            vy * wz - vz * wy,
            vz * wx - vx * wz,
            vx * wy - vy * wx
        );
        */
        qn[0] += qn[ i + 1 ];
    }
    qn[0] /= (float)qt.static_size;

    return qn;
}








AHeightmap::vertexData_t AHeightmap::vertexData(
    const std::string& file,
    const boxInt2d_t& box,
    size_t maxDepth,
    size_t seaLevel,
    size_t maxHeight,
    const size3d_t& worldSize
) {
    assert( !file.empty() && "Путь к файлу с картой высот должен быть указан." );
    assert( correct( box ) );
    assert( (((coordInt2d_t)box.first >= 0) && ((coordInt2d_t)box.second >= 0))
        && "Координаты коробки должны быть положительными." );

    FILE* fp = fopen( file.c_str(), "rb" );
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


    // Подводим размер под размер коробки
    // На 0 проверили выше, см. correct()
    const auto correctBox = boxInt2d_t(
        coordInt2d_t(
            (box.first.get<0>() >= (int)aAllImage) ? (int)aAllImage : box.first.get<0>(),
            (box.first.get<1>() >= (int)bAllImage) ? (int)bAllImage : box.first.get<1>()
        ),
        coordInt2d_t(
            (box.second.get<0>() >= (int)aAllImage) ? (int)aAllImage : box.second.get<0>(),
            (box.second.get<1>() >= (int)bAllImage) ? (int)bAllImage : box.second.get<1>()
        )
    );
    const sizeInt2d_t correctSize = calcSize( correctBox );
    const size_t a = (size_t)correctSize.get<0>();
    const size_t b = (size_t)correctSize.get<1>();


    // Собираем запрошенную часть поверхности
    const size_t AB = a * b;
    const size_t vbufCount = 3 * 2 * AB;
    float* vertices = new float[vbufCount];
    // Центр карты (аналог hm_t::mapCenter)
    const float aHalf = (float)a / 2.0f;
    const float bHalf = (float)b / 2.0f;
    float hMin = 0;
    float hMax = 0;
    const float kn = 1.0f / (float)(maxHeight - maxDepth);
    const float kx = worldSize.get<0>() / (float)aAllImage;
    const float ky = worldSize.get<1>() / (float)bAllImage;
    const float kz = worldSize.get<2>() / 2.0f;
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
            if (height < (float)maxDepth) {
                height = (float)maxDepth;
            } else if (height > (float)maxHeight) {
                height = (float)maxHeight;
            }
            height -= (float)seaLevel;
            height *= kn;
            assert( (height >= -1.0f) && (height <= 1.0f) );

            // Центром меша делаем координату (0; 0; 0)
            const auto vc = Ogre::Vector3(
                ((float)i - aHalf - a0) * kx,
                ((float)j - bHalf - b0) * ky,
                // Диапазон высот после нормализации выше: [-1; 1]
                (float)height * kz
            );
            const Ogre::Vector3 nc = vc.normalisedCopy();
            vertices[t]     = vc.x;
            vertices[t + 1] = vc.y;
            vertices[t + 2] = vc.z;
            vertices[t + 3] = nc.x;
            vertices[t + 4] = nc.y;
            vertices[t + 5] = nc.z;
            if (hMin > vc.z) {
                hMin = vc.z;
            }
            if (hMax < vc.z) {
                hMax = vc.z;
            }
            t += 6;
        }
    }

    // Вычисляем коробку и радиус для списка вершин
    const float xMax = aHalf * kx;
    const float yMax = bHalf * ky;
    const auto axisAlignedBox = Ogre::AxisAlignedBox(
        -xMax, -yMax, hMin,
        xMax, yMax, hMax
    );
    const float radius = std::max( std::max( xMax, yMax ), hMax );


    // Убираем за собой
    png_destroy_read_struct( &png, nullptr, nullptr );
    fclose( fp );
    for (size_t j = 0; j < bAllImage; ++j) {
        free( image[j] );
    }
    free( image );


    return vertexData_t(
        correctSize,
        std::shared_ptr< float >( vertices ),
        axisAlignedBox,
        radius
    );
}







#if 0
// - Заменено. См. ниже.
sizeInt2d_t AHeightmap::sizeMapFromFile( const std::string& source ) {
    // @use http://www.libpng.org/pub/png/libpng.html
    // @example http://libpng.org/pub/png/book/chapter13.html
    // @example http://zarb.org/~gc/html/libpng.html

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
    //       в другой среде; сделать одинковыми.
    png_read_info( png, info );

    /*
    const auto colorType = png_get_color_type( png, info );
    if (colorType != PNG_COLOR_TYPE_RGBA) {
        throw "Input file must be RGB color type.";
    }
    */
    const size_t a = png_get_image_width( png, info );
    const size_t b = png_get_image_height( png, info );

    fclose( fp );

    return sizeInt2d_t( a, b );
}







void AHeightmap::loadLowHMImage() const {

    // Читаем только один раз
    if ( image_ ) {
        return;
    }

    FILE* fp = fopen( lowHM.source.c_str(), "rb" );
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


    // Резервируем место под образ...
    image_ = (png_bytep*)malloc( sizeof( png_bytep ) * lowHM.mapSize.get<1>() );
    for (size_t j = 0; j < lowHM.mapSize.get<1>(); ++j) {
        image_[j] = (png_byte*)malloc( png_get_rowbytes( png, info ) );
    }
    // ...и читаем изображение карты высот
    png_read_image( png, image_ );

    fclose( fp );
}
#endif






std::pair< sizeInt2d_t, png_bytep* >  AHeightmap::loadImage(
    const std::string& source
) {
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
    assert( (a == b) && "Допустима только квадратная карта высот." );

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
