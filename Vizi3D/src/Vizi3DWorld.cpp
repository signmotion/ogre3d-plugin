#include "Vizi3DWorld.h"
#include <boost/filesystem.hpp>


void Vizi3DWorld::load( d::coordInt3d_t observer ) {

    // ����: �������� � mAbout ���������� ���������� � ����� ����.
    // @see about_t

    // ����� ���������� � ���� ��� �������� � ������������

    // ������������� ����������� ������ � ��� ������ ������
    for (size_t k = 0; k <= 26; ++k) {
        const d::coordInt3d_t shift = helper::isc3D( k );
        const auto c = observer + shift;
        // �������� ������ ������: ��� ��� ������ - � ����
        // *��������� ������* ���
        loadArea( mAbout.workArea[k], c );
    }

    // �������������� ��������� ��� ���������
    drawItr = cbegin();
}






void Vizi3DWorld::loadResource( const std::string& pathToResource, bool depthFolder ) {

    using namespace boost::filesystem;

    // ����� � ��������� '-' ����������
    if ( pathToResource.empty() || boost::starts_with( pathToResource, "-" ) ) {
        return;
    }

    Ogre::ResourceGroupManager::getSingleton()
        .addResourceLocation( pathToResource, "FileSystem" );
    if ( !exists( pathToResource ) ) {
        throw "Path '" + pathToResource + "' not found.";
    }
    if ( !is_directory( pathToResource ) ) {
        throw "Path '" + pathToResource + "' must be a folder, not file.";
    }

    // �������� ��������� �����
    if ( depthFolder ) {
        directory_iterator endItr;
        for (directory_iterator itr( pathToResource ); itr != endItr; ++itr) {
            if ( is_directory( itr->status() ) ) {
                const std::string p = itr->path().string();
                if ( !boost::starts_with( p, "-" ) ) {
                    Ogre::ResourceGroupManager::getSingleton()
                        .addResourceLocation( itr->path().string(), "FileSystem" );
                }
            }
        }
    }
}






Vizi3DWorld::infoWorld_t Vizi3DWorld::loadInfo( const std::string& folder ) {

    const std::string name = folder + "/about.json";

    // @see http://code.google.com/p/vjson
    FILE *fp = fopen( name.c_str(), "rb" );
    if ( !fp ) {
        throw "Can't open file '" + name + "'";
    }

    fseek( fp, 0, SEEK_END );
    // ���������� �������� � �������� ������ {}
    const auto fileSize = ftell( fp );
    const auto contentSize = fileSize + 1 /* ����. { */ + 1 /* ����. } */ + 1 /* ����� ������ */;
    fseek( fp, 0, SEEK_SET );

    block_allocator allocator( 1024 );
    char* source = ( char* )allocator.malloc( contentSize );
    source[0] = '{';
    fread( source + 1 /* ����. { */,  1,  fileSize,  fp );
    source[ contentSize - 1 ] = '}';
    source[ contentSize ] = 0;

    fclose( fp );

    char *errorPos = 0;
    char *errorDesc = 0;
    int errorLine = 0;
    json_value* root = json_parse(
        source,
        &errorPos, &errorDesc, &errorLine,
        &allocator
    );
    if ( !root ) {
        std::cerr << "File: " << name << std::endl;
        std::cerr << "Position: " << errorPos << std::endl;
        std::cerr << "Description: " << errorDesc << std::endl;
        std::cerr << "Line: " << errorLine << std::endl;
        throw "File corrupt.";
    }

    // ��������� ��������� �� JSON-�����

    infoWorld_t infoWorld;

    //std::cout <<  "Parse json-map:" << std::endl;
    for (json_value* itr = root->first_child; itr; itr = itr->next_sibling) {
        //if ( itr->name ) {
            //std::cout << "    " << itr->name << std::endl;
        //}
        const std::string field = (std::string)itr->name;

        if ( field == "name" ) {
            assert( itr->type == JSON_STRING );
            infoWorld.name = (std::string)itr->string_value;
            continue;
        }

        if ( field == "description" ) {
            assert( itr->type == JSON_STRING );
            infoWorld.description = (std::string)itr->string_value;
            continue;
        }

        if ( field == "path-to-resource" ) {
            assert( itr->type == JSON_OBJECT );
            for (json_value* child = itr->first_child; child; child = child->next_sibling) {
                assert( child->type == JSON_OBJECT );
                for (json_value* res = child->first_child; res; res = res->next_sibling) {
                    assert( res->type == JSON_STRING );
                    const auto s = (std::string)( child->string_value );
                    assert( !s.empty() );
                    infoWorld.pathToResource.push_back( s );
                }
            }
            continue;
        }

        /* - @example http://code.google.com/p/vjson
        switch ( itr->type ) {
            case JSON_ARRAY:
                printf( "start array\n" );
                break;
            case JSON_BOOL:
                printf(itr->int_value ? "true\n" : "false\n");
                break;
            case JSON_INT:
                printf("%d\n", itr->int_value);
                break;
            case JSON_FLOAT:
                printf("%f\n", itr->float_value);
                break;
            case JSON_NULL:
                printf("null\n");
                break;
            case JSON_OBJECT:
                printf( "start object\n" );
                break;
            case JSON_STRING:
                printf("\"%s\"\n", itr->string_value);
                break;
        }
        */
    }

    free( source );  source = nullptr;
}







void Vizi3DWorld::loadArea( aboutAreaPtr_t area, const d::coordInt3d_t& location ) {

    using namespace boost::filesystem;

    assert( area );

    // 'location' ���������� �������������� ����� � �������
    const int z = location.get<2>();
    const int y = location.get<1>();
    const int x = location.get<0>();
    const std::string path = sourceWorld
        + "/" + Ogre::StringConverter::toString( z )
        + "/" + Ogre::StringConverter::toString( y )
        + "/" + Ogre::StringConverter::toString( x );
    
    if ( !exists( path ) ) {
        // ���������� ���� - ���������� ������� (�� ������)
        return;
    }

    if ( !is_directory( path ) ) {
        // ����� �������, ��� ��� �� ��������� ���� ������ �����
        throw "Path '" + path + "' need be a folder, not file, sorry.";
    }

    // ������������� ��� json-����� �� ����� � �� ������� ��������� ����������.
    // ����� � ��������� '-' ����������.
    // ������������� �� ���������.
    directory_iterator endItr;
    for (directory_iterator itr( path ); itr != endItr; ++itr) {
        if ( is_regular_file( itr->status() ) ) {
            const std::string file = itr->path().string();
            if ( boost::ends_with( file, ".json" ) ) {
                loadArea( area, file );
            }
        }
    }


    area->worldCoord = location;

    // �������������� ������� ������ ��� ����������� ��������� �������
    //prepareVisualBitImage( area );

}





void Vizi3DWorld::loadArea( aboutAreaPtr_t area, const std::string& fileName ) {

    assert( area );

    // @see http://code.google.com/p/vjson
    FILE *fp = fopen( fileName.c_str(), "rb" );
    if ( !fp ) {
        throw "Can't open file '" + fileName + "'";
    }

    fseek( fp, 0, SEEK_END );
    // ���������� �������� � �������� ������ {}
    const auto fileSize = ftell( fp );
    const auto contentSize = fileSize + 1 /* ����. { */ + 1 /* ����. } */ + 1 /* ����� ������ */;
    fseek( fp, 0, SEEK_SET );

    block_allocator allocator( 1024 );
    char* source = ( char* )allocator.malloc( contentSize );
    source[0] = '{';
    fread( source + 1 /* ����. { */,  1,  fileSize,  fp );
    source[ contentSize - 1 ] = '}';
    source[ contentSize ] = 0;

    fclose(fp);

    char *errorPos = 0;
    char *errorDesc = 0;
    int errorLine = 0;
    json_value* root = json_parse(
        source,
        &errorPos, &errorDesc, &errorLine,
        &allocator
    );
    if ( !root ) {
        std::cerr << "File: " << fileName << std::endl;
        std::cerr << "Position: " << errorPos << std::endl;
        std::cerr << "Description: " << errorDesc << std::endl;
        std::cerr << "Line: " << errorLine << std::endl;
        throw "File corrupt.";
    }

    // ��������� ��������� �� JSON-�����

    //std::cout <<  "Parse json-map:" << std::endl;
    for (json_value* itr = root->first_child; itr; itr = itr->next_sibling) {
        if ( itr->name ) {
            //std::cout << "    " << itr->name << std::endl;
        }
        const std::string field = (std::string)itr->name;

        if ( field == "name" ) {
            assert( itr->type == JSON_STRING );
            const auto v = (std::string)itr->string_value;
            area->name.push_back( v );
            continue;
        }

        if ( field == "description" ) {
            assert( itr->type == JSON_STRING );
            const auto v = (std::string)itr->string_value;
            area->description.push_back( v );
            continue;
        }

        if ( field == "note-common" ) {
            assert( itr->type == JSON_OBJECT );
            fillNoteMap( area->note.common,  itr->first_child );
            continue;
        }
        /* - @todo
        if ( field == "note-physics" ) {
            assert( itr->type == JSON_OBJECT );
            fillNoteMap( area->note.physics,  itr->first_child );
            continue;
        }

        if ( field == "note-visual" ) {
            assert( itr->type == JSON_OBJECT );
            fillNoteMap( area->note.visual, itr->first_child );
            continue;
        }
        */

        /* - @todo ...
        if ( boost::starts_with( field, "content-ceil" ) ) {
        }

        if ( boost::starts_with( field, "content-right" ) ) {
        }
        */

        if ( boost::starts_with( field, "content-floor" ) ) {
            assert( itr->type == JSON_OBJECT );
            fillContentArea( area->content.at( 2 ), area->note, itr->first_child );
            continue;
        }

    }

    free( source );  source = nullptr;

}






#if 0
// - @todo fine �������� � load().
std::shared_ptr< char >  Vizi3DWorld::loadJSON( const std::string& name ) {

    // @see http://code.google.com/p/vjson
    FILE *fp = fopen( name.c_str(), "rb" );
    if ( !fp ) {
        throw "Can't open file '" + name + "'";
    }

    fseek( fp, 0, SEEK_END );
    // ���������� �������� � �������� ������ {}
    const auto fileSize = ftell( fp );
    const auto contentSize = fileSize + 1 /* ����. { */ + 1 /* ����. } */ + 1 /* ����� ������ */;
    fseek( fp, 0, SEEK_SET );

    block_allocator allocator( 1024 );
    char* src = ( char* )allocator.malloc( contentSize );
    src[0] = '{';
    fread( src + 1 /* ����. { */,  1,  fileSize,  fp );
    src[ contentSize - 1 ] = '}';
    src[ contentSize ] = 0;

    fclose(fp);

}
#endif







void Vizi3DWorld::drawNext() {

    // @init load()
    if ( drawItr.onEnd() ) {
        return;
    }

    // ������� �� ����� ����� ��� ������� ����� �������� ��������

    // �������� ����� ������������� ��� ��� ����������� ����������
    const auto info = *get( drawItr );

#if 0
// - @todo
    // ������ - ������ ���������� ��������
    if (info.sign != ' ') {

        // ��������� ����� ��� ����. ���������
        const std::string signInString = std::string( &info.sign, 1 );
        Ogre::StaticGeometry* staticScene =
            sm->createStaticGeometry( signInString + "::Vizi3DWorld" );
        //staticScene->setRegionDimensions( ... );


        // ������ �������
        const std::string meshName = info.noteElement.form + ".mesh";
        auto e = sm->createEntity( meshName );

        // ���� ��� ���������� ����������
        // @see http://ogre3d.org/forums/viewtopic.php?f=2&t=32172&start=0
        e->setRenderQueueGroup( Ogre::RENDER_QUEUE_9 );

        const std::string materialName = info.noteElement.material;
        if ( !materialName.empty() ) {
            // �������� �������� �� ���������
            const auto material = Ogre::MaterialManager::getSingleton().getByName( materialName );

#ifdef TEST_TRANSPARENT_CELL
            // @test ��������� ������������ ���� ����������
            const auto mt = static_cast< Ogre::Material* >( material.get() );
            mt->setSceneBlending( Ogre::SBT_TRANSPARENT_COLOUR );
            //mt->setParameter( "set_alpha_rejection", "greater 128" );
            mt->setDepthWriteEnabled( false );
            mt->setDepthCheckEnabled( true );
#endif

            e->setMaterial( material );
            e->setCastShadows( true );

        } // if ( !materialName.empty() )
    

        // ������ �������
        const size_t N = info.bitContent.size().get<0>();
        const size_t M = info.bitContent.size().get<1>();
        const size_t L = info.bitContent.size().get<2>();

        // ������������� ������� � ������� ������������
        const float halfN = (float)N / 2.0f;
        const float halfM = (float)M / 2.0f;
        const float halfL = (float)L / 2.0f;
        staticScene->setOrigin( Ogre::Vector3(
            info.coord.get<0>() * info.scaleWorld - halfN * info.scale,
            info.coord.get<1>() * info.scaleWorld - halfM * info.scale,
            info.coord.get<2>() * info.scaleWorld - halfL * info.scale
        ) );


                                                                                                                        /* - �������� �� ��������������� ���������� ������ � load(). ��. ����.
    // ��������� ������� � *�������*, ��������� ��� ������������ ���������
    // ������� ����� �������
    for (size_t l = 0; l < L; ++l) {
        for (size_t m = 0; m < M; ++m) {
            for (size_t n = 0; n < N; ++n) {
                // 3D-��������� ��� ����������� ��������� ��������
                const auto c = d::coordInt3d_t( n, m, l );
                // ������ ������������� ��� - ������� ������� �������� � �������
                // @todo optimize ������� ������ �� 1D-���������
                if ( info.bitContent.test( c ) ) {
                    staticScene->addEntity(
                        e,
                        // ��������� ��������, ���������� ���� �����
                        Ogre::Vector3(
                            (float)c.get<0>(),
                            (float)c.get<1>(),
                            (float)c.get<2>()
                        ) * info.scale,
                        Ogre::Quaternion::ZERO,
                        Ogre::Vector3( info.scale )
                    );
                }

            } // for (size_t n = 0; n < N; ++n)

        } // for (size_t m = 0; m < M; ++m

    } // for (size_t l = 0; l < L; ++l)
    */

        // �� ��� �������� ������� ���������
        // @see prepareVisualBitImage()
        const auto content = *drawItr.mapElement()->second;
        const auto& alphaBI = *drawItr.tinyMap()->second.visualContent.alphaTypeBitImage;
        const auto& solidBI = *drawItr.tinyMap()->second.visualContent.solidTypeBitImage;
        assert( (alphaBI.raw().size() == solidBI.raw().size()) && "������� ������� ������� ������ ���������." );
        for (size_t l = 0; l < L; ++l) {
            for (size_t m = 0; m < M; ++m) {
                for (size_t n = 0; n < N; ++n) {
                    // 3D-��������� ��� ����������� ��������� ��������
                    const auto c = d::coordInt3d_t( n, m, l );
                    // ������ ������������� ��� - ������� ������� �������� � �������
                    // @todo optimize ������� ������ �� 1D-���������
                    if ( content.test( c )
                      && ( solidBI.test( c ) || alphaBI.test( c ) )
                    ) {
                        staticScene->addEntity(
                            e,
                            // ��������� �������� � ����������� ����� (����
                            // ������������ ������)
                            Ogre::Vector3(
                                ((float)c.get<0>() - (float)halfN),
                               -((float)c.get<1>() - (float)halfM),
                               -((float)c.get<2>() - (float)halfL)
                            ) * info.scale,
                            Ogre::Quaternion::ZERO,
#ifdef TEST_VISUAL_CELL
                            Ogre::Vector3( info.scale / 1.80f )
#else
                            Ogre::Vector3( info.scale )
#endif
                        );
                    }

                } // for (size_t n = 0; n < N; ++n)

            } // for (size_t m = 0; m < M; ++m

        } // for (size_t l = 0; l < L; ++l)

        
        // ������!
        staticScene->build();

    } // if (info.sign != ' ')

#endif

    // ��������� �������� �� ����. �������
    ++drawItr;

}







void Vizi3DWorld::fillContentArea( elementPtr_t element, const note_t& note, json_value* ptr ) {

    assert( !note->common.empty()
        && "���������� �� ��������� ������ ���� �������� �� ������ ����� ������." );
    assert( ptr );
    assert( ptr->type == JSON_OBJECT );

    // � ������������ �������� ������ �� �������� 'content-*'

    // ������ ������� ����� ����� ������� � ������� ������

    // I ��������� ������� �����, ��������� � ������������.

    // ������ ������ ����������� ������������ ����� �� OX �
    // ����� �� ����� ������ �����
    size_t N = 0;
    size_t M = 0;
    size_t L = 0;
    for (json_value* chZ = ptr->first_child; chZ; chZ = chZ->next_sibling) {
        assert( (chZ->type == JSON_ARRAY) && "��������� ������ � ������ - ������ ���������� �����." );
        // ������ ��������������� ��������� �� OY (������ ����� �� json-�����)
        size_t oy = 0;
        for (json_value* chXY = chZ->first_child; chXY; chXY = chXY->next_sibling) {
            assert( (chXY->type == JSON_STRING) && "��������� ������ �����, ����������� ��������� �����." );
            // ������ ���� �����������, ����� ������� ��������� ������ �� OX
            const std::string line = prepareBitLine( chXY->string_value );
            // ������ ������ ����������� ������������ ����� �� OX �
            // ����� �� ����� ������ �����
            if (N == 0) {
                N = line.length();
            }
            assert( (N == line.length()) && "������� OX ���������� ������ ���� �����������." );

            // ������������ OY
            ++oy;

        } // for (json_value* chXY = chZ->first_child; ...

        // ����������� OY - ���� �� �������� �����
        assert( ((M == 0) || (M == oy)) && "������� OY ���������� ������ ���� �����������." );
        M = oy;

        // ����������� ������ OZ
        ++L;

    } // for (json_value* chZ = ptr->first_child; ...


    assert( N > 0 );
    assert( M > 0 );
    assert( L > 0 );

    
    // II ���� �������, ����� ������������ ����� ����������.
    const auto size = d::sizeInt3d_t( N, M, L );
    size_t l = 0;
    for (json_value* chZ = ptr->first_child; chZ; chZ = chZ->next_sibling) {
        // ����� �� ��������� ��������: ��� ������� ����
        size_t m = 0;
        for (json_value* chXY = chZ->first_child; chXY; chXY = chXY->next_sibling) {
            size_t n = 0;
            // ������ ���� �����������, ����� ������� ��������� ������ �� OX
            const std::string line = prepareBitLine( chXY->string_value );
            // �������� �� ������� �������� ������ �
            // ��������� ����� ����������
            for (auto itr = line.cbegin(); itr != line.cend(); ++itr) {
                const sign_t symbol = *itr;
                // ���� ���������� - *��������* ��������
                const std::string name = nameElement( symbol, note );
                if ( element->find( name ) == element->cend() ) {
                    element->at( name ) =
                        std::shared_ptr< BitContent3D >( new BitContent3D( size ) );
                }
                const auto c = d::coordInt3d_t( n, m, l );
                element->at( name )->set( c );

                ++n;
            }

            ++m;

        } // for (json_value* chXY = chZ->first_child; ...

        ++l;

    } // for (json_value* chZ = ptr->first_child; ...

    assert( (element->cbegin()->second->count() > 0) && "����� �� �������." );

}







void Vizi3DWorld::fillNoteMap(
    std::unordered_map< std::string, noteCommon_t  >  r,
    json_value* ptr 
) {
    assert( ptr );
    assert( ptr->type == JSON_OBJECT );

    for (json_value* child = ptr->first_child; child; child = child->next_sibling) {
        // ������������� ��������
        assert( child->type == JSON_OBJECT );
        const sign_t sign = child->name[0];

        auto el = noteCommon_t();
        for (json_value* childSign = child->first_child; childSign; childSign = childSign->next_sibling) {

            if ( (std::string)childSign->name == "sign" ) {
                assert( childSign->type == JSON_STRING );
                const auto s = (std::string)childSign->string_value;
                assert( (s.length() == 1)
                    && "����� � ������� ������������� ����� �������� � ������ ���� ������� ��� ������ ��������." );
                el.sign = s[0];
                continue;
            }
            if ( (std::string)childSign->name == "name" ) {
                assert( childSign->type == JSON_STRING );
                const auto s = (std::string)childSign->string_value;
                assert( !s.empty() && "�������� ������ ���� ������� ��� ������ ��������." );
                el.name = s;
                continue;
            }
            if ( (std::string)childSign->name == "description" ) {
                assert( childSign->type == JSON_STRING );
                el.description = (std::string)childSign->string_value;
                continue;
            }
            /*
            if ( (std::string)childSign->name == "type" ) {
                assert( childSign->type == JSON_STRING );
                const std::string t = (std::string)childSign->string_value;
                el.type =
                    (t == "alpha") ? TYPE_ELEMENT_ALPHA :
                    (t == "solid") ? TYPE_ELEMENT_SOLID :
                    TYPE_ELEMENT_UNDEFINED;
                continue;
            }
            */

        } // for (json_value* childSign = child->first_child; ...

        r[ name ] = el;

    } // for (json_value* child = ptr->first_child; ...

    return r;
}







/*
void Vizi3DWorld::prepareVisualBitImage( aboutAreaPtr_t area ) {

    // ���������, ��� ���������� ������� �� �������� �������� �
    // ����� ����������
    assert( area->tinyArea && "������� �������� ��������." );
    assert(
     (   area->ceilContent  || area.rightContent
      || area->floorContent || area->leftContent
      || area->nearContent  || area->farContent
     ) && "������� �� ����� ����������."
    );

    // �� ��� �������� ������� ���������. ������������� ����, �����
    // ����������� ���������������� ������� ����� ����������� �������.

    // ������ �� ��������� ������� ������� � ������� �� ��������,
    // ������� ��������� ���������������.

    // @todo ������ �������� ������ � floorContent. �������� � �������.


    ...


    for (auto tinyAreaItr = area->tinyArea->begin();
         tinyMapItr != mAbout.tinyMap.end();
         ++tinyMapItr
    ) {
        const std::string tinyMapName = tinyMapItr->first;

        // ������ �������
        const d::sizeInt3d_t size = tinyMapItr->second.size;
        const size_t N = size.get<0>();
        const size_t M = size.get<1>();
        const size_t L = size.get<2>();

        // I �������� ����������� ������� ������ ������� ��
        // ������� ���������.

        // ���-����� �� ���� ��������
        tinyMapItr->second.visualContent.alphaTypeBitImage =
            std::shared_ptr< BitContent3D >( new BitContent3D( size ) );
        auto& alphaBI = *tinyMapItr->second.visualContent.alphaTypeBitImage;
        alphaBI.reset();

        tinyMapItr->second.visualContent.solidTypeBitImage =
            std::shared_ptr< BitContent3D >( new BitContent3D( size ) );
        auto& solidBI = *tinyMapItr->second.visualContent.solidTypeBitImage;
        solidBI.reset();


        // �������� �� ������ ������� ����� �������� �������,
        // ���������� ���������
        for (auto mapElementItr = tinyMapItr->second.content.cbegin();
             mapElementItr != tinyMapItr->second.content.cend();
             ++mapElementItr
        ) {
            // ������ ������� ����� ���� ����������� ������������
            const sign_t sign = mapElementItr->first;
            // ������ - ������ ���������� ��������
            if (sign == ' ') {
                continue;
            }

            assert( (mAbout.noteMap.find( sign ) != mAbout.noteMap.cend())
                && "������ ������� ������ ���� ������������ � 'note'." );
            const noteElement_t& noteElement = mAbout.noteMap.at( sign );
            const typeElement_e typeElement = noteElement.type;

            switch ( typeElement ) {
                case TYPE_ELEMENT_ALPHA :
                    alphaBI |= *mapElementItr->second;
                    break;

                case TYPE_ELEMENT_SOLID :
                    solidBI |= *mapElementItr->second;
                    break;

                default:
                    // ����������� ��� ������ � ALPHA
                    alphaBI |= *mapElementItr->second;

            } // switch ( typeElement )

        } // for (auto mapElementItr = ...


        // II ����������� ���������� ������� ������ �� ������� �������������
        // ��������������� ������.

        // solidTypeBitImage
        const auto staticSolidBI = solidBI;
        for (size_t l = 0; l < L; ++l) {
            for (size_t m = 0; m < M; ++m) {
                for (size_t n = 0; n < N; ++n) {
                    // 3D-��������� ��� ����������� ��������� ��������
                    const auto c = d::coordInt3d_t( n, m, l );

                    // solidTypeBitImage
                    // ������ ������������� ��� - ������� ������� �������� �
                    // �������. �� ��� ���������� �� ���...
                    // @todo optimize ������� ������ �� 1D-���������
                    if ( !staticSolidBI.test( c ) ) {
                        // ���������� �������������
                        continue;
                    }

                    // ��������� �������, ������������ ��������, ���������
                    // ������ �� ������������� �������� �������, �����
                    // ���������. ��������� 6 ������. ��. ���������
                    // � helper::isc3D().
                    if (
                         // �������
                         staticSolidBI.has( c, 1 )
                         // ������
                      && staticSolidBI.has( c, 2 )
                         // ������
                      && staticSolidBI.has( c, 3 )
                         // �����
                      && staticSolidBI.has( c, 4 )
                         // �������
                      && staticSolidBI.has( c, 9 )
                         // �������
                      && staticSolidBI.has( c, 18 )
                    ) {
                        // ������� � 6-�� ������. ��� �� ����� ���� �������.
                        //std::cout << "reset " << c << std::endl;
                        solidBI.reset( c );
                    }

                } // for (size_t n = 0; n < N; ++n)

            } // for (size_t m = 0; m < M; ++m

        } // for (size_t l = 0; l < L; ++l)


    } // for (auto tinyMapItr = ...

}
*/
