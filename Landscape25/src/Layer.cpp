#include "Layer.h"
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include "helper.h"
#include "Ogre.h"



surface2d_t Layer::gradientPoint() const {

    std::vector< coord2d_t >  v;

    // (!) mt19937 даёт действительно качественное распределение.
    // @see http://www.boost.org/doc/libs/1_47_0/doc/html/boost_random/reference.html
    boost::mt19937 rg( feature.seed );
    // Генератор для разброса точек по всей длине 'length'
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > lengthGenerator( rg, boost::uniform_int<>( 0, feature.length ) );
    // Генератор для смещения точек по высоте
    // Если = 0, все точки будут лежать в одной плоскости
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > heightGenerator( rg, boost::uniform_int<>(
         -(int)feature.surfaceComplexity, (int)feature.surfaceComplexity
    ) );


    // Создаём нужное кол-во точек на осям OX и OZ
    std::vector< float >  ox;
    std::vector< float >  oz;
    // Точки по оси OX не должны лежать слишком близко
    /* - @todo Отладить.
    std::vector< float >  ux;
    while ( (ox.size() == 0)
         || (ox.size() != 0) && (ux.size() != ox.size())
    ) {
        ox.clear();
        oz.clear();
        for (size_t i = 0; i < biome->countGradient; ++i) {
            ox.push_back( (float)lengthGenerator() );
            oz.push_back( (float)heightGenerator() );
        }
        //std::unique_copy( ox.cbegin(), ox.cend(), &ux );
        ux = ox;
        std::unique( ux.begin(), ux.end(),  [] ( float a, float b ) {
            return abs(abs(a) - abs(b)) < 10.0f;
        } );
    }
    */
    for (size_t i = 0; i < feature.countGradient; ++i) {
        ox.push_back( (float)lengthGenerator() );
        oz.push_back( (float)heightGenerator() );
    }


    // Точки упорядочим по возрастанию X-координат: так проще
    // рисовать образ слоя
    std::sort( ox.begin(), ox.end() );

    // Крайние точки - на края по OX
    ox.front() = 0.0f;
    ox.back() = (float)(feature.length - 1);

    // Собираем сырые координаты точек перегиба
    for (size_t i = 0; i < feature.countGradient; ++i) {
        const coord2d_t coord( ox[i], oz[i] );
        v.push_back( coord );
    }


    /* - @todo
    // Проверяем условия размещения точек
    // Biome::minDistance
    bool correct = true;
    for (auto itr = v.cbegin(); itr != v.cend(); ++itr) {
        const coord2d_t coord = *itr;
        const auto distance = ...
    }

    // Просим генераторы постараться... но не бесконечно, нет
    while (  ) {
        ...
    }
    */

    return v;
}







void Layer::gradientPointDraw(
    Ogre::SceneManager* sm,
    Ogre::SceneNode* sn,
    const surface2d_t& gp
) const {

    assert( (gp.size() > 1) && "Точки перепада не получены." );
    assert( (gp.size() == feature.countGradient)
        && "Количество полученных точек перепада не соотв. ожидаемому." );

    // Создаём образ

    // Каждую точку представим в виде меша
    for (auto itr = gp.cbegin(); itr != gp.cend(); ++itr) {
        const coord2d_t coord = *itr;
        auto point = sm->createEntity( "sphere.mesh" );
        auto childScene = sn->createChildSceneNode();
        childScene->attachObject( point );
        childScene->setPosition( coord.get<0>(),  0.0f,  coord.get<1>() );
    }


    // Соединим точки
    auto image = sm->createManualObject();
    int i = 0;

    image->begin( "landscape25/red", Ogre::RenderOperation::OT_LINE_STRIP );
    for (auto itr = gp.cbegin(); itr != gp.cend(); ++itr) {
        const coord2d_t coord = *itr;
        image->position( coord.get<0>(),  0.0f,  coord.get<1>() );
        image->index( i ); ++i;
    }
    image->end();

    image->setCastShadows( false );

    sn->createChildSceneNode()->attachObject( image );

}







void Layer::surfaceLineDraw(
    Ogre::SceneManager* sm,
    Ogre::SceneNode* sn,
    const surface2d_t& sl
) const {

    assert( (sl.size() > 1) && "Линия поверхности не получена, образ не может быть создан." );

    // Рисуем образ линии поверхности
    auto slImage = sm->createManualObject();
    int i = 0;


    // Подготавливаем и накладываем текстуру
    // @see http://www.gamedev.ru/code/articles/?id=4191\
    // @see Тест-блоки ниже.
    // Достаточно указать текстурные координаты для каждой вершины. Наложение
    // размазано по построению вершин. См. ниже.

    // @todo Текстура должна заполнять поверхность. Получается - только границы поверхности.

    // Размер объекта, по крайним координатам
    const box2d_t slBox = calcBox( sl );
    const size2d_t slSize = calcSize( slBox );

    // Размер текстуры (материала)
    const std::string materialName = "landscape25/test";
    const size2d_t texture( 1024.0f, 1024.0f );
    // Накладываем 1 текстуру на 1 объект: вычисляем масштаб
    const scale2d_t ts = ONE2D / slSize;

    // Текстурные координаты для вершин
    coord2d_t uv( 0.0f, 0.0f );


    // Рисуем поверхность
    slImage->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP );
    //slImage->begin( materialName, Ogre::RenderOperation::OT_TRIANGLE_FAN );
    //slImage->begin( materialName, Ogre::RenderOperation::OT_TRIANGLE_STRIP );
    const int firstIndexSL = i;

    /* @test
    slImage->position( 0.0f,    0.0f,  0.0f   ); slImage->index( i ); ++i;
    slImage->position( 100.0f,  0.0f,  0.0f   ); slImage->index( i ); ++i;
    slImage->position( 100.0f,  0.0f, -200.0f ); slImage->index( i ); ++i;
    slImage->position( 0.0f,    0.0f, -200.0f ); slImage->index( i ); ++i;
    slImage->index( 0 );
    */

    /*
    // Создаём текстуру для слоя
    // @source http://www.ogre3d.ru/forum/viewtopic.php?f=8&t=4345
    Ogre::ColourValue diffuse;
    diffuse.a = 0.5f;
    Ogre::MaterialPtr materialPtr =
        Ogre::MaterialManager::getSingleton().create( "Red", "General" );
    Ogre::TextureUnitState* Texture =
        materialPtr->getTechnique(0)->getPass(0)->createTextureUnitState();
    // Задаём параметры отображения текстуры, цвет
    Texture->setColourOperationEx(
        Ogre::LBX_SOURCE1, 
        Ogre::LBS_MANUAL,
        Ogre::LBS_CURRENT, 
        Ogre::ColourValue::Red
    );
    // Режим прозрачности
    materialPtr->getTechnique(0)->getPass(0)->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );
    materialPtr->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
    materialPtr->getTechnique(0)->getPass(0)->setDiffuse( diffuse );
    */

    // Получаем координаты крайних точек
    //const coord2d_t first = sl.front();
    //const coord2d_t last = sl.back();
    //const size2d_t size = calcSize( std::make_pair( last, first ) );

    // Рисуем полученную выше линию поверхности
    for (auto itr = sl.cbegin(); itr != sl.cend(); ++itr) {
        const coord2d_t coord = *itr;
        slImage->position( coord.get<0>(), 0.0f, coord.get<1>() );
        slImage->index( i ); ++i;
        // Текстура
        uv = coord * ts;
        slImage->textureCoord( uv.get<0>(), uv.get<1>() );
    }


    // Замыкаем образ внизу по крайним точкам 'sl'
    const coord2d_t lastCoord = sl.back();
    const coord2d_t firstCoord = sl.front();
    const int bottomSpace = 500;
    //const coord2d_t bottomCoord = std::min( lastCoord.get<1>(), firstCoord.get<1>() );
    //const float bottomCoordZ = min<1>( sl ) - bottomSpace;
    // Стабильная нижняя координата смотрится красивее скачущего слоя
    const float bottomCoordZ = -bottomSpace;
    slImage->position(
        lastCoord.get<0>(),
        0.0f,
        bottomCoordZ
    );
    slImage->index( i ); ++i;
    uv = lastCoord * ts;
    slImage->textureCoord( uv.get<0>(), uv.get<1>() );

    slImage->position(
        firstCoord.get<0>(),
        0.0f,
        bottomCoordZ
    );
    slImage->index( i ); ++i;
    uv = firstCoord * ts;
    slImage->textureCoord( uv.get<0>(), uv.get<1>() );

    slImage->index( firstIndexSL );


    // Завершаем построение
    slImage->end();
    slImage->setCastShadows( false );


    const std::string meshName = sn->getName() + "-layer.mesh";
    auto mesh = slImage->convertToMesh( meshName );

    
    // Помещаем объект на сцену
    auto e = sm->createEntity( meshName );
    //e->setMaterialName( "landscape25/test" );
    sn->attachObject( e );



    // <@test>
    // @source http://www.packtpub.com/article/materials-ogre-3d
    /*{
    auto manual = sm->createManualObject( "Quad" );
    manual->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST );
    manual->position( 50.0, 0.0, 0.0 );
    manual->textureCoord( 0, 2 );
    manual->position( -50.0, 0.0, 100.0 );
    manual->textureCoord( 2, 0 );
    manual->position( -50.0, 0.0, 0.0 );
    manual->textureCoord( 2, 2 );
    manual->position( 50.0, 0.0, 100.0 );
    manual->textureCoord( 0, 0 );

    manual->index( 0 );
    manual->index( 1 );
    manual->index( 2 );

    manual->index( 0 );
    manual->index( 3 );
    manual->index( 1 );

    manual->end();

    manual->convertToMesh( "Quad" );

    auto e = sm->createEntity( "Quad" );
    e->setMaterialName( "landscape25/test" );
    //auto node = sn->createChildSceneNode();
    auto node = sm->getRootSceneNode()->createChildSceneNode( "Node1" );
    node->showBoundingBox( true );
    //const Ogre::Quaternion rx( Ogre::Degree( -90 ), Ogre::Vector3::UNIT_X ); 
    //node->rotate( rx );
    node->attachObject( e );
    }*/
    // </@test>


    // <@test>
    // @example http://pastebin.com/MZVQDENZ
    /*{
    const std::string name = "Test2";
    try {
        sm->destroyManualObject( name );
        sm->destroyEntity( name );
        sm->destroySceneNode( name );
    } catch ( ... ) {
    }
    auto m = sm->createManualObject( name );
    m->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_STRIP );
    m->position(   0.0f,  0.0f,    0.0f );  m->textureCoord( 0, 0 );
    m->position(   0.0f,  0.0f, -200.0f );  m->textureCoord( 0, 1 );    
    m->position( 100.0f,  0.0f,    0.0f );  m->textureCoord( 1, 0 );
    m->position( 100.0f,  0.0f, -200.0f );  m->textureCoord( 1, 1 );
    m->index( 0 );  m->index( 1 ); m->index( 2 );
    m->index( 1 );  m->index( 3 ); m->index( 2 );
    m->end();
    auto mesh = m->convertToMesh( name );
    auto e = sm->createEntity( name );
    e->setMaterialName( "landscape25/test" );
    auto node = sm->getRootSceneNode()->createChildSceneNode( name );
    node->showBoundingBox( true );
    node->attachObject( e );
    }*/
    // <@test>


    // <@test>
    // 1 треугольник
    /*{
    const std::string name = "Test3";
    try {
        sm->destroyManualObject( name );
        sm->destroyEntity( name );
        sm->destroySceneNode( name );
    } catch ( ... ) {
    }

    // @source http://www.gamedev.ru/code/articles/?id=4191
    // Вершины треугольника в плоскости XZ
    const coord2d_t a(   0.0f, 0.0f );
    const coord2d_t b(   0.0f, 300.0f );
    const coord2d_t c( 100.0f, 200.0f );

    // Размер объекта, по крайним координатам
    const size2d_t object( 100.0f, 300.0f );

    // Размер текстуры
    const size2d_t texture( 1024.0f, 1024.0f );

    // Накладываем текстуру на объект целиком: вычисляем масштаб
    //const scale2d_t s = texture / object;

    // Размер текселя
    //const size2d_t texel( 1.0f, 1.0f );

    // Рисуем объект, вычисляя текстурные координаты грани
    coord2d_t uv( 0.0f, 0.0f );
    auto m = sm->createManualObject( name );
    m->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST );

    m->position( a.get<0>(), 0.0f, a.get<1>() );
    uv = a / object;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->position( b.get<0>(), 0.0f, b.get<1>() );
    uv = b / object;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->position( c.get<0>(), 0.0f, c.get<1>() );
    uv = c / object;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->triangle( 1, 0, 2 );
    m->end();
    auto mesh = m->convertToMesh( name );
    auto e = sm->createEntity( name );
    e->setMaterialName( "landscape25/test" );
    auto node = sm->getRootSceneNode()->createChildSceneNode( name );
    node->showBoundingBox( true );
    node->attachObject( e );
    }*/
    // <@test>


    // <@test>
    // Фигура из 3 треугольников
    /*{
    const std::string name = "Test3-3";
    try {
        sm->destroyManualObject( name );
        sm->destroyEntity( name );
        sm->destroySceneNode( name );
    } catch ( ... ) {
    }

    // @source http://www.gamedev.ru/code/articles/?id=4191
    // Вершины треугольника в плоскости XZ
    const coord2d_t a(   0.0f,   0.0f );
    const coord2d_t b(   0.0f, 300.0f );
    const coord2d_t c( 100.0f, 200.0f );
    const coord2d_t d( 400.0f, 200.0f );
    const coord2d_t e( 400.0f,   0.0f );

    // Размер объекта, по крайним координатам
    const size2d_t object( 400.0f, 300.0f );

    // Размер текстуры
    const size2d_t texture( 1024.0f, 1024.0f );

    // Накладываем 1 текстуру на 1 объект: вычисляем масштаб
    const scale2d_t s = ONE2D / object;

    // Размер текселя
    //const size2d_t texel( 1.0f, 1.0f );

    // Рисуем объект, вычисляя текстурные координаты грани
    coord2d_t uv( 0.0f, 0.0f );
    auto m = sm->createManualObject( name );
    m->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_FAN );
    int i = 0;

    // 1-й треугольник
    m->position( a.get<0>(), 0.0f, a.get<1>() );
    m->index( i );  ++i;
    uv = a * s;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->position( b.get<0>(), 0.0f, b.get<1>() );
    m->index( i );  ++i;
    uv = b * s;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->position( c.get<0>(), 0.0f, c.get<1>() );
    m->index( i );  ++i;
    uv = c * s;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->index( 0 );  m->index( 2 ); m->index( 1 );

    // 2-й треугольник
    m->position( d.get<0>(), 0.0f, d.get<1>() );
    m->index( i );  ++i;
    uv = d * s;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->index( 0 );  m->index( 3 ); m->index( 2 );

    // 3-й треугольник
    m->position( e.get<0>(), 0.0f, e.get<1>() );
    m->index( i );  ++i;
    uv = e * s;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->index( 0 );  m->index( 4 ); m->index( 3 );

    m->end();

    auto mesh = m->convertToMesh( name );
    auto entity = sm->createEntity( name );
    entity->setMaterialName( "landscape25/test" );
    auto node = sm->getRootSceneNode()->createChildSceneNode( name );
    node->showBoundingBox( true );
    node->attachObject( entity );
    }*/
    // <@test>


    


    /* - ?
    unsigned short src, dest;
    if (!mesh->suggestTangentVectorBuildParams( Ogre::VES_TANGENT, src, dest) ) {
        mesh->buildTangentVectors( Ogre::VES_TANGENT, src, dest );
    }
    */

}







void Layer::trendDraw(
    Ogre::SceneManager* sm,
    Ogre::SceneNode* sn,
    const trend2d_t& tr
) const {

    assert( (tr.size() > 1) && "Координаты трендов не получены, образ не может быть создан." );

    // Отмечаем тренды
    for (auto itr = tr.cbegin(); itr != tr.cend(); ++itr) {
        auto tbImage = sm->createManualObject();
        int i = 0;
        tbImage->begin( "landscape25/white", Ogre::RenderOperation::OT_LINE_STRIP );

        // @see box2d_t
        const coord2d_t p1 = itr->first;
        const coord2d_t p2 = itr->second;
        tbImage->position( p1.get<0>(),  0.0f,  p1.get<1>() );
        tbImage->index( i ); ++i;
        tbImage->position( p1.get<0>(),  0.0f,  p2.get<1>() );
        tbImage->index( i ); ++i;
        tbImage->position( p2.get<0>(),  0.0f,  p2.get<1>() );
        tbImage->index( i ); ++i;
        tbImage->position( p2.get<0>(),  0.0f,  p1.get<1>() );
        tbImage->index( i );
        tbImage->index( 0 );

        tbImage->end();
        tbImage->setCastShadows( false );
        sn->attachObject( tbImage );
    }
    
}






std::shared_ptr< Trend >  Layer::bestNaturalTrend(
    const coord2d_t& current,
    const coord2d_t& aim
) const {

    assert( (current != aim) && "Текущая и целевая координаты совпадают." );

    // Инициируем генераторы для получения одинаковой конфигурации слоя
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > indexTrendGenerator(
        boost::mt19937( feature.seed ),  
        boost::uniform_int<>( 0,  tr.size() - 1 )
    );

    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > seedTrendGenerator(
        boost::mt19937( feature.seed + 1 ),
        boost::uniform_int<>( 0, INT_MAX )
    );


    // Выбираем лучший тренд для дюны. Достаточно одного.
    // Проходимся по всем известным дюне трендам, инициируем их и смотрим,
    // насколько близко каждый из них приближают нас к заветным
    // координатам 'aim'.

    std::map< float, std::shared_ptr< Trend > >  vote;
    for (auto itr = tr.cbegin(); itr != tr.cend(); ++itr) {
        const std::string name = itr->first;
        const FeatureTrend ft = itr->second;
        std::shared_ptr< Trend > trend;
        if (name == "AscendTrend") {
            trend = std::shared_ptr< Trend >( new AscendTrend( ft ) );
        } else if (name == "DescendTrend") {
            trend = std::shared_ptr< Trend >( new DescendTrend( ft ) );
        } else if (name == "HollowTrend") {
            trend = std::shared_ptr< Trend >( new HollowTrend( ft ) );
        }
        assert( trend && "Тренд с указанным именем не найден." );

        const coord2d_t g = trend->grad( true );
        const coord2d_t nextCoord = current + g;
        const float score = calcDistance( nextCoord, aim );
        // Не страшно, что последние тренды могут затереть первые
        vote[score] = trend;
    }

    if (vote.size() == 0) {
        throw "Best trend not found.";
    }

    // Карта сразу сортируется по ключу; первый ключ это - мин. расстояние
    // и лучший тренд для достижения 'aim'.
    const auto best = vote.cbegin();

    return best->second;
}










// Декларируем тренды для дюны



DunaLayer::DunaLayer(
    size_t length,
    size_t surfaceComplexity,
    size_t countGradient,
    int seed
) :
    Layer( length, surfaceComplexity, countGradient, seed )
{
    // На базе полученных характеристик будем строить тренды слоя

    // Декларируем тренды, известные слою. Позже эти тренды используются для
    // формирования визуального образа слоя.

/* - Заменено на название трендов. См. ниже
    // Зерно для трендов
    int trendSeed = l->feature.seed + 1;

    std::shared_ptr< Trend >  trend;


    // Подъём
    ++trendSeed;
    trend = std::shared_ptr< Trend >(
        new AscendTrend(
            // Размер оригинальных форм тренда, м
            size2d_t( 1.0f, 1.0f ),
            // Диапазон изменения размера, м
            interval2d_t(
                scale2d_t(  1.0f,  1.0f ),
                scale2d_t( 10.0f, 10.0f )
            ),
            // Одинаковое зерно - одинаковые всходы
            trendSeed
        )
    );
    l->add( trend );


    // Спад
    ++trendSeed;
    trend = std::shared_ptr< Trend >(
        new DescendTrend(
            size2d_t( 1.0f, 1.0f ),
            interval2d_t(
                scale2d_t(  1.0f,  1.0f ),
                scale2d_t( 10.0f, 10.0f )
            ),
            trendSeed
        )
    );
    l->add( trend );


    // Яма, углубление
    ++trendSeed;
    trend = std::shared_ptr< Trend >(
        new HollowTrend(
            size2d_t( 1.0f, 1.0f ),
            interval2d_t(
                scale2d_t(  1.0f,  1.0f ),
                scale2d_t( 10.0f, 10.0f )
            ),
            trendSeed
        )
    );
    l->add( trend );
*/

    // @todo optimize Зерно для трендов может вычисляться как счётчик от
    //       зерна слоя.
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > seedTrendGenerator(
        boost::mt19937( feature.seed + 1 ),
        boost::uniform_int<>( 0, INT_MAX )
    );

    FeatureTrend ft;

    // Храним названия и характеристики трендов
    // (!) Декларированные здесь тренды должны быть
    // отработаны в Layer::bestNaturalTrend()

    // Подъём
    ft.scale = interval2d_t(
        scale2d_t( 1.0f, 1.0f ),
        scale2d_t( 5.0f, 5.0f )
    );
    ft.natural = feature.natural;
    ft.seed = seedTrendGenerator();
    tr.push_back( std::make_pair( "AscendTrend", ft ) );

    // Спуск
    ft.scale = interval2d_t(
        scale2d_t( 1.0f, 1.0f ),
        scale2d_t( 5.0f, 5.0f )
    );
    ft.natural = feature.natural;
    ft.seed = seedTrendGenerator();
    tr.push_back( std::make_pair( "DescendTrend", ft ) );

    // Яма, выемка
    ft.scale = interval2d_t(
        scale2d_t( 1.0f, 1.0f ),
        scale2d_t( 5.0f, 5.0f )
    );
    ft.natural = feature.natural;
    ft.seed = seedTrendGenerator();
    tr.push_back( std::make_pair( "HollowTrend", ft ) );

}






std::pair< trend2d_t, surface2d_t >  DunaLayer::surfaceLine(
    const surface2d_t&  gp
) const {
    assert( (gp.size() >= 2) && "Должно быть указано как минимум 2 точки перепада." );

    trend2d_t   trendBox;
    surface2d_t surfaceVertex;

    // Дюне известно неск. трендов - pitTrend, descendTrend, ... - и она может
    // использовать их, чтобы создать свой образ. Размеры образа:
    //   - по оси OX ~ biome.length
    //   - по оси OZ - определяется высотой горизонта и номером слоя
    //   - по оси OY - это смещение слоя от наблюдателя; определяется  biome.depth
    // (!) Размер образа может быть чуть больше характеристик биома, т.к.
    // образ складывается из НЕДЕЛИМЫХ карт, предлагаемых трендами.

    // (!) Слой отвечает за корректные настройки самого себя.
    // @see SandDesertBiome(), прим. в конструкторе.

    // Запоминаем крайнюю точку перепада, чтобы не рисовать за её пределами
    const coord2d_t lastAim = gp.back();

    // Начинаем с крайней левой точки перепада и стремимся выбрать те тренды
    // слоя, которые образуют максимально близкую к точкам перепада форму слоя.
    // Движемся "трендовыми" шагами к след. точке перепада.
    coord2d_t current = gp.front();
    // Первая координата остаётся неизменной
    surfaceVertex.push_back( current );

    for (auto itr = gp.cbegin() + 1; itr != gp.cend(); ++itr) {

        // Сразу проверяем: нет надобности выходить за крайнюю точку.
        // Контроль ведём только по OX.
        /* - Лишнее.
        if (current.get<0>() >= lastAim.get<0>()) {
            // Приводим последнюю координату к правой границе биома
            current.get<0>() = lastAim.get<0>();
            surfaceVertex.push_back( current );
            break;
        }
        */

        // Цель - приблизиться к точке перепада
        const coord2d_t aim = *itr;

        // С помощью трендов приближаемся к точке перепада: мы должны пересечь
        // её формой тренда по оси OX (движемся слева направо) или по оси OZ
        // (движемся вверх И вниз)
        //bool aimToUp   = (current.get<1>() < aim.get<1>());
        //bool aimToDown = (current.get<1>() > aim.get<1>());
        while (current.get<0>() < aim.get<0>()) {

            // Находим тренд слоя, который лучшим образом приблизит нас к
            // след. точке перепада (aim).
            // Всегда работаем с натуральной формой: это позволяет получать
            // случайный набор без необходимости перемешивать тренды одного
            // размера.
            const int naturalSeed =
                feature.seed + std::distance( gp.cbegin(), itr )
              + (int)current.get<0>()
              + (int)current.get<1>() * 500;
            const auto best = bestNaturalTrend( current, aim );
            const form2d_t form = best->form( true );
            // @test
            //const form2d_t form = best->form( false );

            // @test
            //auto first = surfaceVertex.front();
            //auto last = surfaceVertex.back();

            // Получив поверхность (набор лучших вершин), дописываем вершины
            // к собранной ранее поверхности.
            //v.insert( v.end(), best.first.cbegin(), best.first.cend() ); - не достаточно
            // Каждая форма "крепится" к последней собранной вершине. Точкой
            // прикрепления служит самая первая точка формы (формы трендов
            // декларируются слева направо).
            const coord2d_t P0 = form.first.front();
            for (auto q = form.first.cbegin(); q != form.first.cend(); ++q) {
                const coord2d_t c = *q;
                // Текущей координатой становится последняя добавленная
                surfaceVertex.push_back( current + c - P0 );
            }

            // @test
            //first = surfaceVertex.front();
            //last = surfaceVertex.back();

            // @test
            //const float d = distance( current, v.back() );
            //assert( (d > 0.001f) && "Форма тренда не помогла продвинуться ни на йоту :(" );

            // Собираем информацию о тренде
            const vector2d_t grad = best->grad( true );
            assert( ( (aim.get<0>() + grad.get<0>()) > aim.get<0>() )
                && "Выбранный тренд не приближает нас к цели." );
            const coord2d_t firstTrendCoord = current;
            const box2d_t box = best->box( true );
            const size2d_t sizeBox = calcSize( box );
            box2d_t currentBox = box + firstTrendCoord;
            if (grad.get<1>() < 0.0f) {
                currentBox.first.get<1>()  -= sizeBox.get<1>();
                currentBox.second.get<1>() -= sizeBox.get<1>();
            }
            assert( !empty( currentBox ) && "Коробка для тренда не создана." );
            trendBox.push_back( currentBox );

            current = surfaceVertex.back();

        } // while ( (current.get<0>() < aim.get<0>())

    } // for (auto itr = gp.cbegin(); itr != gp.cend(); ++itr)


    // @todo ? Убираем возможные дубликаты.

    
    // @test
    if (surfaceVertex.size() < 2) {
        assert( (surfaceVertex.size() < 2) && "Поверхность не создана." );
    }

    // Показываем результат в цифрах
    /*
    helper::Tray::p( Ogre::StringConverter::toString( index ) + " Точек перепада",
        Ogre::StringConverter::toString( gp.size() ) );
    helper::Tray::p( Ogre::StringConverter::toString( index ) + " Количество трендов",
        Ogre::StringConverter::toString( trendBox.size() ) );
    helper::Tray::p( Ogre::StringConverter::toString( index ) + " Вершин поверхности",
        Ogre::StringConverter::toString( surfaceVertex.size() ) );
    */

    return std::make_pair( trendBox, surfaceVertex );
}
