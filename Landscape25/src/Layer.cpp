#include "Layer.h"
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include "helper.h"
#include "Ogre.h"



surface2d_t Layer::gradientPoint() const {

    std::vector< coord2d_t >  v;

    // (!) mt19937 ��� ������������� ������������ �������������.
    // @see http://www.boost.org/doc/libs/1_47_0/doc/html/boost_random/reference.html
    boost::mt19937 rg( feature.seed );
    // ��������� ��� �������� ����� �� ���� ����� 'length'
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > lengthGenerator( rg, boost::uniform_int<>( 0, feature.length ) );
    // ��������� ��� �������� ����� �� ������
    // ���� = 0, ��� ����� ����� ������ � ����� ���������
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > heightGenerator( rg, boost::uniform_int<>(
         -(int)feature.surfaceComplexity, (int)feature.surfaceComplexity
    ) );


    // ������ ������ ���-�� ����� �� ���� OX � OZ
    std::vector< float >  ox;
    std::vector< float >  oz;
    // ����� �� ��� OX �� ������ ������ ������� ������
    /* - @todo ��������.
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


    // ����� ���������� �� ����������� X-���������: ��� �����
    // �������� ����� ����
    std::sort( ox.begin(), ox.end() );

    // ������� ����� - �� ���� �� OX
    ox.front() = 0.0f;
    ox.back() = (float)(feature.length - 1);

    // �������� ����� ���������� ����� ��������
    for (size_t i = 0; i < feature.countGradient; ++i) {
        const coord2d_t coord( ox[i], oz[i] );
        v.push_back( coord );
    }


    /* - @todo
    // ��������� ������� ���������� �����
    // Biome::minDistance
    bool correct = true;
    for (auto itr = v.cbegin(); itr != v.cend(); ++itr) {
        const coord2d_t coord = *itr;
        const auto distance = ...
    }

    // ������ ���������� �����������... �� �� ����������, ���
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

    assert( (gp.size() > 1) && "����� �������� �� ��������." );
    assert( (gp.size() == feature.countGradient)
        && "���������� ���������� ����� �������� �� �����. ����������." );

    // ������ �����

    // ������ ����� ���������� � ���� ����
    for (auto itr = gp.cbegin(); itr != gp.cend(); ++itr) {
        const coord2d_t coord = *itr;
        auto point = sm->createEntity( "sphere.mesh" );
        auto childScene = sn->createChildSceneNode();
        childScene->attachObject( point );
        childScene->setPosition( coord.get<0>(),  0.0f,  coord.get<1>() );
    }


    // �������� �����
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

    assert( (sl.size() > 1) && "����� ����������� �� ��������, ����� �� ����� ���� ������." );

    // ������ ����� ����� �����������
    auto slImage = sm->createManualObject();
    int i = 0;


    // �������������� � ����������� ��������
    // @see http://www.gamedev.ru/code/articles/?id=4191\
    // @see ����-����� ����.
    // ���������� ������� ���������� ���������� ��� ������ �������. ���������
    // ��������� �� ���������� ������. ��. ����.

    // @todo �������� ������ ��������� �����������. ���������� - ������ ������� �����������.

    // ������ �������, �� ������� �����������
    const box2d_t slBox = calcBox( sl );
    const size2d_t slSize = calcSize( slBox );

    // ������ �������� (���������)
    const std::string materialName = "landscape25/test";
    const size2d_t texture( 1024.0f, 1024.0f );
    // ����������� 1 �������� �� 1 ������: ��������� �������
    const scale2d_t ts = ONE2D / slSize;

    // ���������� ���������� ��� ������
    coord2d_t uv( 0.0f, 0.0f );


    // ������ �����������
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
    // ������ �������� ��� ����
    // @source http://www.ogre3d.ru/forum/viewtopic.php?f=8&t=4345
    Ogre::ColourValue diffuse;
    diffuse.a = 0.5f;
    Ogre::MaterialPtr materialPtr =
        Ogre::MaterialManager::getSingleton().create( "Red", "General" );
    Ogre::TextureUnitState* Texture =
        materialPtr->getTechnique(0)->getPass(0)->createTextureUnitState();
    // ����� ��������� ����������� ��������, ����
    Texture->setColourOperationEx(
        Ogre::LBX_SOURCE1, 
        Ogre::LBS_MANUAL,
        Ogre::LBS_CURRENT, 
        Ogre::ColourValue::Red
    );
    // ����� ������������
    materialPtr->getTechnique(0)->getPass(0)->setSceneBlending( Ogre::SBT_TRANSPARENT_ALPHA );
    materialPtr->getTechnique(0)->getPass(0)->setDepthWriteEnabled( false );
    materialPtr->getTechnique(0)->getPass(0)->setDiffuse( diffuse );
    */

    // �������� ���������� ������� �����
    //const coord2d_t first = sl.front();
    //const coord2d_t last = sl.back();
    //const size2d_t size = calcSize( std::make_pair( last, first ) );

    // ������ ���������� ���� ����� �����������
    for (auto itr = sl.cbegin(); itr != sl.cend(); ++itr) {
        const coord2d_t coord = *itr;
        slImage->position( coord.get<0>(), 0.0f, coord.get<1>() );
        slImage->index( i ); ++i;
        // ��������
        uv = coord * ts;
        slImage->textureCoord( uv.get<0>(), uv.get<1>() );
    }


    // �������� ����� ����� �� ������� ������ 'sl'
    const coord2d_t lastCoord = sl.back();
    const coord2d_t firstCoord = sl.front();
    const int bottomSpace = 500;
    //const coord2d_t bottomCoord = std::min( lastCoord.get<1>(), firstCoord.get<1>() );
    //const float bottomCoordZ = min<1>( sl ) - bottomSpace;
    // ���������� ������ ���������� ��������� �������� ��������� ����
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


    // ��������� ����������
    slImage->end();
    slImage->setCastShadows( false );


    const std::string meshName = sn->getName() + "-layer.mesh";
    auto mesh = slImage->convertToMesh( meshName );

    
    // �������� ������ �� �����
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
    // 1 �����������
    /*{
    const std::string name = "Test3";
    try {
        sm->destroyManualObject( name );
        sm->destroyEntity( name );
        sm->destroySceneNode( name );
    } catch ( ... ) {
    }

    // @source http://www.gamedev.ru/code/articles/?id=4191
    // ������� ������������ � ��������� XZ
    const coord2d_t a(   0.0f, 0.0f );
    const coord2d_t b(   0.0f, 300.0f );
    const coord2d_t c( 100.0f, 200.0f );

    // ������ �������, �� ������� �����������
    const size2d_t object( 100.0f, 300.0f );

    // ������ ��������
    const size2d_t texture( 1024.0f, 1024.0f );

    // ����������� �������� �� ������ �������: ��������� �������
    //const scale2d_t s = texture / object;

    // ������ �������
    //const size2d_t texel( 1.0f, 1.0f );

    // ������ ������, �������� ���������� ���������� �����
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
    // ������ �� 3 �������������
    /*{
    const std::string name = "Test3-3";
    try {
        sm->destroyManualObject( name );
        sm->destroyEntity( name );
        sm->destroySceneNode( name );
    } catch ( ... ) {
    }

    // @source http://www.gamedev.ru/code/articles/?id=4191
    // ������� ������������ � ��������� XZ
    const coord2d_t a(   0.0f,   0.0f );
    const coord2d_t b(   0.0f, 300.0f );
    const coord2d_t c( 100.0f, 200.0f );
    const coord2d_t d( 400.0f, 200.0f );
    const coord2d_t e( 400.0f,   0.0f );

    // ������ �������, �� ������� �����������
    const size2d_t object( 400.0f, 300.0f );

    // ������ ��������
    const size2d_t texture( 1024.0f, 1024.0f );

    // ����������� 1 �������� �� 1 ������: ��������� �������
    const scale2d_t s = ONE2D / object;

    // ������ �������
    //const size2d_t texel( 1.0f, 1.0f );

    // ������ ������, �������� ���������� ���������� �����
    coord2d_t uv( 0.0f, 0.0f );
    auto m = sm->createManualObject( name );
    m->begin( "BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_FAN );
    int i = 0;

    // 1-� �����������
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

    // 2-� �����������
    m->position( d.get<0>(), 0.0f, d.get<1>() );
    m->index( i );  ++i;
    uv = d * s;
    m->textureCoord( uv.get<0>(), uv.get<1>() );

    m->index( 0 );  m->index( 3 ); m->index( 2 );

    // 3-� �����������
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

    assert( (tr.size() > 1) && "���������� ������� �� ��������, ����� �� ����� ���� ������." );

    // �������� ������
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

    assert( (current != aim) && "������� � ������� ���������� ���������." );

    // ���������� ���������� ��� ��������� ���������� ������������ ����
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


    // �������� ������ ����� ��� ����. ���������� ������.
    // ���������� �� ���� ��������� ���� �������, ���������� �� � �������,
    // ��������� ������ ������ �� ��� ���������� ��� � ��������
    // ����������� 'aim'.

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
        assert( trend && "����� � ��������� ������ �� ������." );

        const coord2d_t g = trend->grad( true );
        const coord2d_t nextCoord = current + g;
        const float score = calcDistance( nextCoord, aim );
        // �� �������, ��� ��������� ������ ����� �������� ������
        vote[score] = trend;
    }

    if (vote.size() == 0) {
        throw "Best trend not found.";
    }

    // ����� ����� ����������� �� �����; ������ ���� ��� - ���. ����������
    // � ������ ����� ��� ���������� 'aim'.
    const auto best = vote.cbegin();

    return best->second;
}










// ����������� ������ ��� ����



DunaLayer::DunaLayer(
    size_t length,
    size_t surfaceComplexity,
    size_t countGradient,
    int seed
) :
    Layer( length, surfaceComplexity, countGradient, seed )
{
    // �� ���� ���������� ������������� ����� ������� ������ ����

    // ����������� ������, ��������� ����. ����� ��� ������ ������������ ���
    // ������������ ����������� ������ ����.

/* - �������� �� �������� �������. ��. ����
    // ����� ��� �������
    int trendSeed = l->feature.seed + 1;

    std::shared_ptr< Trend >  trend;


    // ������
    ++trendSeed;
    trend = std::shared_ptr< Trend >(
        new AscendTrend(
            // ������ ������������ ���� ������, �
            size2d_t( 1.0f, 1.0f ),
            // �������� ��������� �������, �
            interval2d_t(
                scale2d_t(  1.0f,  1.0f ),
                scale2d_t( 10.0f, 10.0f )
            ),
            // ���������� ����� - ���������� ������
            trendSeed
        )
    );
    l->add( trend );


    // ����
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


    // ���, ����������
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

    // @todo optimize ����� ��� ������� ����� ����������� ��� ������� ��
    //       ����� ����.
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > seedTrendGenerator(
        boost::mt19937( feature.seed + 1 ),
        boost::uniform_int<>( 0, INT_MAX )
    );

    FeatureTrend ft;

    // ������ �������� � �������������� �������
    // (!) ��������������� ����� ������ ������ ����
    // ���������� � Layer::bestNaturalTrend()

    // ������
    ft.scale = interval2d_t(
        scale2d_t( 1.0f, 1.0f ),
        scale2d_t( 5.0f, 5.0f )
    );
    ft.natural = feature.natural;
    ft.seed = seedTrendGenerator();
    tr.push_back( std::make_pair( "AscendTrend", ft ) );

    // �����
    ft.scale = interval2d_t(
        scale2d_t( 1.0f, 1.0f ),
        scale2d_t( 5.0f, 5.0f )
    );
    ft.natural = feature.natural;
    ft.seed = seedTrendGenerator();
    tr.push_back( std::make_pair( "DescendTrend", ft ) );

    // ���, ������
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
    assert( (gp.size() >= 2) && "������ ���� ������� ��� ������� 2 ����� ��������." );

    trend2d_t   trendBox;
    surface2d_t surfaceVertex;

    // ���� �������� ����. ������� - pitTrend, descendTrend, ... - � ��� �����
    // ������������ ��, ����� ������� ���� �����. ������� ������:
    //   - �� ��� OX ~ biome.length
    //   - �� ��� OZ - ������������ ������� ��������� � ������� ����
    //   - �� ��� OY - ��� �������� ���� �� �����������; ������������  biome.depth
    // (!) ������ ������ ����� ���� ���� ������ ������������� �����, �.�.
    // ����� ������������ �� ��������� ����, ������������ ��������.

    // (!) ���� �������� �� ���������� ��������� ������ ����.
    // @see SandDesertBiome(), ����. � ������������.

    // ���������� ������� ����� ��������, ����� �� �������� �� � ���������
    const coord2d_t lastAim = gp.back();

    // �������� � ������� ����� ����� �������� � ��������� ������� �� ������
    // ����, ������� �������� ����������� ������� � ������ �������� ����� ����.
    // �������� "����������" ������ � ����. ����� ��������.
    coord2d_t current = gp.front();
    // ������ ���������� ������� ����������
    surfaceVertex.push_back( current );

    for (auto itr = gp.cbegin() + 1; itr != gp.cend(); ++itr) {

        // ����� ���������: ��� ���������� �������� �� ������� �����.
        // �������� ���� ������ �� OX.
        /* - ������.
        if (current.get<0>() >= lastAim.get<0>()) {
            // �������� ��������� ���������� � ������ ������� �����
            current.get<0>() = lastAim.get<0>();
            surfaceVertex.push_back( current );
            break;
        }
        */

        // ���� - ������������ � ����� ��������
        const coord2d_t aim = *itr;

        // � ������� ������� ������������ � ����� ��������: �� ������ ��������
        // � ������ ������ �� ��� OX (�������� ����� �������) ��� �� ��� OZ
        // (�������� ����� � ����)
        //bool aimToUp   = (current.get<1>() < aim.get<1>());
        //bool aimToDown = (current.get<1>() > aim.get<1>());
        while (current.get<0>() < aim.get<0>()) {

            // ������� ����� ����, ������� ������ ������� ��������� ��� �
            // ����. ����� �������� (aim).
            // ������ �������� � ����������� ������: ��� ��������� ��������
            // ��������� ����� ��� ������������� ������������ ������ ������
            // �������.
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

            // ������� ����������� (����� ������ ������), ���������� �������
            // � ��������� ����� �����������.
            //v.insert( v.end(), best.first.cbegin(), best.first.cend() ); - �� ����������
            // ������ ����� "��������" � ��������� ��������� �������. ������
            // ������������ ������ ����� ������ ����� ����� (����� �������
            // ������������� ����� �������).
            const coord2d_t P0 = form.first.front();
            for (auto q = form.first.cbegin(); q != form.first.cend(); ++q) {
                const coord2d_t c = *q;
                // ������� ����������� ���������� ��������� �����������
                surfaceVertex.push_back( current + c - P0 );
            }

            // @test
            //first = surfaceVertex.front();
            //last = surfaceVertex.back();

            // @test
            //const float d = distance( current, v.back() );
            //assert( (d > 0.001f) && "����� ������ �� ������� ������������ �� �� ���� :(" );

            // �������� ���������� � ������
            const vector2d_t grad = best->grad( true );
            assert( ( (aim.get<0>() + grad.get<0>()) > aim.get<0>() )
                && "��������� ����� �� ���������� ��� � ����." );
            const coord2d_t firstTrendCoord = current;
            const box2d_t box = best->box( true );
            const size2d_t sizeBox = calcSize( box );
            box2d_t currentBox = box + firstTrendCoord;
            if (grad.get<1>() < 0.0f) {
                currentBox.first.get<1>()  -= sizeBox.get<1>();
                currentBox.second.get<1>() -= sizeBox.get<1>();
            }
            assert( !empty( currentBox ) && "������� ��� ������ �� �������." );
            trendBox.push_back( currentBox );

            current = surfaceVertex.back();

        } // while ( (current.get<0>() < aim.get<0>())

    } // for (auto itr = gp.cbegin(); itr != gp.cend(); ++itr)


    // @todo ? ������� ��������� ���������.

    
    // @test
    if (surfaceVertex.size() < 2) {
        assert( (surfaceVertex.size() < 2) && "����������� �� �������." );
    }

    // ���������� ��������� � ������
    /*
    helper::Tray::p( Ogre::StringConverter::toString( index ) + " ����� ��������",
        Ogre::StringConverter::toString( gp.size() ) );
    helper::Tray::p( Ogre::StringConverter::toString( index ) + " ���������� �������",
        Ogre::StringConverter::toString( trendBox.size() ) );
    helper::Tray::p( Ogre::StringConverter::toString( index ) + " ������ �����������",
        Ogre::StringConverter::toString( surfaceVertex.size() ) );
    */

    return std::make_pair( trendBox, surfaceVertex );
}
