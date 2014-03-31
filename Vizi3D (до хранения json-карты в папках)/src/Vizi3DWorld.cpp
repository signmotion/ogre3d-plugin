#include "Vizi3DWorld.h"


void Vizi3DWorld::load( const std::string& name ) {

    // @see http://code.google.com/p/vjson
    FILE *fp = fopen( name.c_str(), "rb" );
    if ( !fp ) {
        throw "Can't open file '" + name + "'";
    }

    fseek( fp, 0, SEEK_END );
    const auto size = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    block_allocator allocator( 1024 );
    char* source = (char*)allocator.malloc( size + 1 );
    fread( source, 1, size, fp );
    source[size] = 0;

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
        std::cerr << "Position: " << errorPos << std::endl;
        std::cerr << "Description: " << errorDesc << std::endl;
        std::cerr << "Line: " << errorLine << std::endl;
        throw "File corrupt.";
    }

    // Заполняем структуры из JSON-файла

    //std::cout <<  "Parse json-map:" << std::endl;
    for (json_value* itr = root->first_child; itr; itr = itr->next_sibling) {
        if ( itr->name ) {
            //std::cout << "    " << itr->name << std::endl;
        }

        if ( (std::string)itr->name == "name" ) {
            assert( itr->type == JSON_STRING );
            mAbout.name = (std::string)itr->string_value;
            continue;
        }

        if ( (std::string)itr->name == "scale" ) {
            assert( (itr->type == JSON_FLOAT) || (itr->type == JSON_INT) );
            mAbout.scale = (itr->type == JSON_FLOAT) ? itr->float_value : (float)itr->int_value;
            continue;
        }

        if ( (std::string)itr->name == "path-to-image" ) {
            assert( itr->type == JSON_OBJECT );
            for (json_value* child = itr->first_child; child; child = child->next_sibling) {
                assert( child->type == JSON_STRING );
                mAbout.pathToImage[ (std::string)child->name ] = (std::string)child->string_value;
            }
            assert( (mAbout.pathToImage.find( "form" ) != mAbout.pathToImage.cend())
                && (mAbout.pathToImage.find( "material" ) != mAbout.pathToImage.cend())
                && "Ждём как минимум путь к формам и материалам." );
            continue;
        }

        if ( boost::starts_with( (std::string)itr->name, "map-" ) ) {
            assert( itr->type == JSON_OBJECT );
            // В этом ключе хранится и название участка карты
            const std::string tinyMapName = parseTinyMapName( (std::string)itr->name);
            auto tm = mAbout.tinyMap.find( tinyMapName );
            assert( (tm == mAbout.tinyMap.cend()) && "Каждый участок карты должен обладать уникальным именем." );
            assert( (mAbout.scale > 0.0f) && "Масштаб 'scale' должен быть определён и определён до 'map-'." );
            mAbout.tinyMap[ tinyMapName ] = parseTinyMap( itr, mAbout.scale );
            continue;
        }

        if ( (std::string)itr->name == "note" ) {
            assert( itr->type == JSON_OBJECT );
            mAbout.noteMap = parseNoteMap( itr );
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


    // Если в файле не декларирован пустой элемент (пробел),
    // исправляем это
    if ( mAbout.noteMap.find( ' ' ) == mAbout.noteMap.cend() ) {
        mAbout.noteMap[' '] = noteElement_t();
    }


    // Подготавливаем битовые образы для оптимизации отрисовки карты
    prepareVisualBitImage();


    // Инициализируем указатель для отрисовки
    drawItr = cbegin();
}






void Vizi3DWorld::drawNext() {

    // @init load()
    if ( drawItr.empty() ) {
        return;
    }

    // Наносим на холст сразу всю битовую карту текущего элемента

    // Чудесный метод предоставляет нам всю необходимую информацию
    const auto info = *get( drawItr );

    // Пробел - всегда отсутствие элемента
    if (info.signElement != ' ') {

        // Оформлять будем как стат. геометрию
        const std::string signInString = std::string( &info.signElement, 1 );
        Ogre::StaticGeometry* staticScene =
            sm->createStaticGeometry( signInString + "::Vizi3DWorld" );
        //staticScene->setRegionDimensions( ... );


        // Создаём элемент
        const std::string meshName = info.noteElement.form + ".mesh";
        auto e = sm->createEntity( meshName );

        // Фикс для прозрачных материалов
        // @see http://ogre3d.org/forums/viewtopic.php?f=2&t=32172&start=0
        e->setRenderQueueGroup( Ogre::RENDER_QUEUE_9 );

        const std::string materialName = info.noteElement.material;
        if ( !materialName.empty() ) {
            // Заменяем материал по умолчанию
            const auto material = Ogre::MaterialManager::getSingleton().getByName( materialName );

#ifdef TEST_TRANSPARENT_CELL
            // @test Добавляем прозрачность всем материалам
            const auto mt = static_cast< Ogre::Material* >( material.get() );
            mt->setSceneBlending( Ogre::SBT_TRANSPARENT_COLOUR );
            //mt->setParameter( "set_alpha_rejection", "greater 128" );
            mt->setDepthWriteEnabled( false );
            mt->setDepthCheckEnabled( true );
#endif

            e->setMaterial( material );
            e->setCastShadows( true );

        } // if ( !materialName.empty() )
    

        // Размер области
        const size_t N = info.bitContent.size().get<0>();
        const size_t M = info.bitContent.size().get<1>();
        const size_t L = info.bitContent.size().get<2>();

        // Позиционируем область в мировом пространстве
        const float halfN = (float)N / 2.0f;
        const float halfM = (float)M / 2.0f;
        const float halfL = (float)L / 2.0f;
        staticScene->setOrigin( Ogre::Vector3(
            info.coord.get<0>() * info.scaleWorld - halfN * info.scale,
            info.coord.get<1>() * info.scaleWorld - halfM * info.scale,
            info.coord.get<2>() * info.scaleWorld - halfL * info.scale
        ) );


                                                                                                                        /* - Заменено на предварительную подготовку образа в load(). См. ниже.
    // Добавляем элемент в *область*, определяя его расположение благодаря
    // битовой карте области
    for (size_t l = 0; l < L; ++l) {
        for (size_t m = 0; m < M; ++m) {
            for (size_t n = 0; n < N; ++n) {
                // 3D-указатель для определения положения элемента
                const auto c = d::coordInt3d_t( n, m, l );
                // Каждый установленный бит - признак наличия элемента в области
                // @todo optimize Быстрый доступ по 1D-указателю
                if ( info.bitContent.test( c ) ) {
                    staticScene->addEntity(
                        e,
                        // положение элемента, центрируем блок позже
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

        // Не все элементы требуют отрисовки
        // @see prepareVisualBitImage()
        const auto content = *drawItr.mapElement()->second;
        const auto& alphaBI = *drawItr.tinyMap()->second.visualContent.alphaTypeBitImage;
        const auto& solidBI = *drawItr.tinyMap()->second.visualContent.solidTypeBitImage;
        assert( (alphaBI.raw().size() == solidBI.raw().size()) && "Размеры битовых образов должны совпадать." );
        for (size_t l = 0; l < L; ++l) {
            for (size_t m = 0; m < M; ++m) {
                for (size_t n = 0; n < N; ++n) {
                    // 3D-указатель для определения положения элемента
                    const auto c = d::coordInt3d_t( n, m, l );
                    // Каждый установленный бит - признак наличия элемента в области
                    // @todo optimize Быстрый доступ по 1D-указателю
                    if ( content.test( c )
                      && ( solidBI.test( c ) || alphaBI.test( c ) )
                    ) {
                        staticScene->addEntity(
                            e,
                            // положение элемента в статическом блоке (блок
                            // центрировали раньше)
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

        
        // Строим!
        staticScene->build();

    } // if (info.signElement != ' ')


    // Переводим итератор на след. элемент
    ++drawItr;

}







Vizi3DWorld::tinyMap_t Vizi3DWorld::parseTinyMap( json_value* ptr, float defaultScale ) {

    assert( ptr );
    assert( ptr->type == JSON_OBJECT );
    assert( defaultScale > 0.0f );

    auto tinyMap = tinyMap_t();

    // Область может переопределить масштаб, указанный для мира
    tinyMap.scale = defaultScale;

    for (json_value* child = ptr->first_child; child; child = child->next_sibling) {

        if ( (std::string)child->name == "position" ) {
            assert( child->type == JSON_ARRAY );
            // x
            auto childCoord = child->first_child;
            assert( (childCoord->type == JSON_FLOAT) || (childCoord->type == JSON_INT) );
            tinyMap.coord.get<0>() = (childCoord->type == JSON_FLOAT)
                ? childCoord->float_value : (float)childCoord->int_value;
            // y
            childCoord = childCoord->next_sibling;
            assert( (childCoord->type == JSON_FLOAT) || (childCoord->type == JSON_INT) );
            tinyMap.coord.get<1>() = (childCoord->type == JSON_FLOAT)
                ? childCoord->float_value : (float)childCoord->int_value;
            // z
            childCoord = childCoord->next_sibling;
            assert( (childCoord->type == JSON_FLOAT) || (childCoord->type == JSON_INT) );
            tinyMap.coord.get<2>() = (childCoord->type == JSON_FLOAT)
                ? childCoord->float_value : (float)childCoord->int_value;

            continue;

        } // if (itr->name == "position")


        if ( (std::string)child->name == "scale" ) {
            assert( (child->type == JSON_FLOAT) || (child->type == JSON_INT) );
            tinyMap.scale = (child->type == JSON_FLOAT) ? child->float_value : (float)child->int_value;
            continue;
        }


        if ( (std::string)child->name == "content") {

            // Каждый элемент карты будем хранить в битовом образе

            // I Оцениваем размеры карты, проверяем её правильность.

            // Первая строка декларирует протяжённость карты по OX и
            // сразу же задаёт размер карты
            size_t N = 0;
            size_t M = 0;
            size_t L = 0;
            for (json_value* chZ = child->first_child; chZ; chZ = chZ->next_sibling) {
                assert( (chZ->type == JSON_ARRAY) && "Ожидается список в списке - список плоскостей карты." );
                // Размер просматриваемой плоскости по OY (высота блока на json-карте)
                size_t oy = 0;
                for (json_value* chXY = chZ->first_child; chXY; chXY = chXY->next_sibling) {
                    assert( (chXY->type == JSON_STRING) && "Ожидается список строк, описывающих плоскость карты." );
                    const std::string line = (std::string)chXY->string_value;
                    // Каждая строка декларирует протяжённость карты по OX и
                    // сразу же задаёт размер карты
                    if (N == 0) {
                        N = line.length();
                    }
                    assert( (N == line.length()) && "Размеры OX плоскостей должны быть одинаковыми." );

                    // Накаплиаваем OY
                    ++oy;

                } // for (json_value* chXY = chZ->first_child; ...

                // Накопленный OY - один из размеров карты
                assert( ((M == 0) || (M == oy)) && "Размеры OY плоскостей должны быть одинаковыми." );
                M = oy;

                // Накапливаем высоту OZ
                ++L;

            } // for (json_value* chZ = ptr->first_child; ...

            assert( N > 0 );
            assert( M > 0 );
            assert( L > 0 );

        
            // II Зная размеры, можем сформировать карту содержания.
            tinyMap.size = d::sizeInt3d_t( N, M, L );
            size_t l = 0;
            for (json_value* chZ = child->first_child; chZ; chZ = chZ->next_sibling) {
                // Здесь не повторяем проверки: они сделаны выше
                size_t m = 0;
                for (json_value* chXY = chZ->first_child; chXY; chXY = chXY->next_sibling) {
                    size_t n = 0;
                    const std::string line = (std::string)chXY->string_value;
                    // Проходим по каждому элементу строки и
                    // наполняем карту содержания
                    for (auto itr = line.cbegin(); itr != line.cend(); ++itr) {
                        char el = *itr;
                        if ( tinyMap.content.find( el ) == tinyMap.content.cend()) {
                            //auto& cnt = ( *content )[ el ];
                            tinyMap.content[el] =
                                std::shared_ptr< BitContent3D >( new BitContent3D( tinyMap.size ) );
                        }
                        //auto& cnt = *( *content )[ el ];
                        const auto c = d::coordInt3d_t( n, m, l );
                        tinyMap.content[el]->set( c );

                        ++n;
                    }

                    ++m;

                } // for (json_value* chXY = chZ->first_child; ...

                ++l;

            } // for (json_value* chZ = ptr->first_child; ...

            assert( (tinyMap.content.cbegin()->second->count() > 0) && "Карта не создана." );

            continue;

        } // if ( (std::string)child->name == "content")

    } // for (json_value* child = itr->first_child; ...


    return tinyMap;
}






std::unordered_map< Vizi3DWorld::signElement_t, Vizi3DWorld::noteElement_t >
Vizi3DWorld::parseNoteMap( json_value* ptr ) {

    assert( ptr );
    assert( ptr->type == JSON_OBJECT );

    std::unordered_map< signElement_t, noteElement_t >  r;

    for (json_value* child = ptr->first_child; child; child = child->next_sibling) {
        // Перечисляются элементы
        assert( child->type == JSON_OBJECT );
        const signElement_t sign = child->name[0];

        auto el = noteElement_t();
        for (json_value* childSign = child->first_child; childSign; childSign = childSign->next_sibling) {

            if ( (std::string)childSign->name == "name" ) {
                assert( childSign->type == JSON_STRING );
                el.name = (std::string)childSign->string_value;
                continue;
            }
            if ( (std::string)childSign->name == "form" ) {
                assert( childSign->type == JSON_STRING );
                el.form = (std::string)childSign->string_value;
                continue;
            }
            if ( (std::string)childSign->name == "material" ) {
                assert( childSign->type == JSON_STRING );
                el.material = (std::string)childSign->string_value;
                continue;
            }
            if ( (std::string)childSign->name == "type" ) {
                assert( childSign->type == JSON_STRING );
                const std::string t = (std::string)childSign->string_value;
                el.type =
                    (t == "alpha") ? TYPE_ELEMENT_ALPHA :
                    (t == "solid") ? TYPE_ELEMENT_SOLID :
                    TYPE_ELEMENT_UNDEFINED;
                continue;
            }

        } // for (json_value* childSign = child->first_child; ...

        r[ sign ] = el;

    } // for (json_value* child = ptr->first_child; ...

    return r;
}








void Vizi3DWorld::prepareVisualBitImage() {

    // Не все элементы требуют отрисовки. Воспользуемся этим, чтобы
    // подготовить оптимизированный битовый образ содержимого карты.

    // Пройдём по детальным битовым образам и оставим те элементы,
    // которые требуется визуализировать.

    for (auto tinyMapItr = mAbout.tinyMap.begin();
         tinyMapItr != mAbout.tinyMap.end();
         ++tinyMapItr
    ) {
        const std::string tinyMapName = tinyMapItr->first;

        // Размер области
        const d::sizeInt3d_t size = tinyMapItr->second.size;
        const size_t N = size.get<0>();
        const size_t M = size.get<1>();
        const size_t L = size.get<2>();

        // I Собираем специальные битовые образы области из
        // образов элементов.

        // Бит-карты по типу элемента
        tinyMapItr->second.visualContent.alphaTypeBitImage =
            std::shared_ptr< BitContent3D >( new BitContent3D( size ) );
        auto& alphaBI = *tinyMapItr->second.visualContent.alphaTypeBitImage;
        alphaBI.reset();

        tinyMapItr->second.visualContent.solidTypeBitImage =
            std::shared_ptr< BitContent3D >( new BitContent3D( size ) );
        auto& solidBI = *tinyMapItr->second.visualContent.solidTypeBitImage;
        solidBI.reset();


        // Проходим по каждой битовой карте элемента области,
        // накапливая результат
        for (auto mapElementItr = tinyMapItr->second.content.cbegin();
             mapElementItr != tinyMapItr->second.content.cend();
             ++mapElementItr
        ) {
            // Каждый элемент имеет свои особенности визуализации
            const signElement_t signElement = mapElementItr->first;
            // Пробел - всегда отсутствие элемента
            if (signElement == ' ') {
                continue;
            }

            assert( (mAbout.noteMap.find( signElement ) != mAbout.noteMap.cend())
                && "Каждый элемент должен быть декларирован в 'note'." );
            const noteElement_t& noteElement = mAbout.noteMap.at( signElement );
            const typeElement_e typeElement = noteElement.type;

            switch ( typeElement ) {
                case TYPE_ELEMENT_ALPHA :
                    alphaBI |= *mapElementItr->second;
                    break;

                case TYPE_ELEMENT_SOLID :
                    solidBI |= *mapElementItr->second;
                    break;

                default:
                    // Неизвестный тип отнесём к ALPHA
                    alphaBI |= *mapElementItr->second;

            } // switch ( typeElement )

        } // for (auto mapElementItr = ...


        // II Анализируем полученные битовые образы на предмет необходимости
        // визуализировать ячейку.

        // solidTypeBitImage
        const auto staticSolidBI = solidBI;
        for (size_t l = 0; l < L; ++l) {
            for (size_t m = 0; m < M; ++m) {
                for (size_t n = 0; n < N; ++n) {
                    // 3D-указатель для определения положения элемента
                    const auto c = d::coordInt3d_t( n, m, l );

                    // solidTypeBitImage
                    // Каждый установленный бит - признак наличия элемента в
                    // области. Но нам необходимы не все...
                    // @todo optimize Быстрый доступ по 1D-указателю
                    if ( !staticSolidBI.test( c ) ) {
                        // пропускаем отсутствующие
                        continue;
                    }

                    // Полностью цельные, непрозрачные элементы, окружённые
                    // такими же непрозрачными цельными блоками, можем
                    // исключить. Проверяем 6 сторон. См. нумерацию
                    // в helper::isc3D().
                    if (
                         // верхняя
                         staticSolidBI.has( c, 1 )
                         // правая
                      && staticSolidBI.has( c, 2 )
                         // нижняя
                      && staticSolidBI.has( c, 3 )
                         // левая
                      && staticSolidBI.has( c, 4 )
                         // ближняя
                      && staticSolidBI.has( c, 9 )
                         // дальняя
                      && staticSolidBI.has( c, 18 )
                    ) {
                        // Невидим с 6-ти сторон. Нам не нужен этот элемент.
                        //std::cout << "reset " << c << std::endl;
                        solidBI.reset( c );
                    }

                } // for (size_t n = 0; n < N; ++n)

            } // for (size_t m = 0; m < M; ++m

        } // for (size_t l = 0; l < L; ++l)


    } // for (auto tinyMapItr = ...

}
