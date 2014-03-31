#pragma once

#include "default.h"
#include "CollisionManager.h"
#include <Newton.h>
#include <Ogre.h>
#include <boost/array.hpp>


/**
* Тело - базовый класс для всех существ.
*
* Класс оптимизирован для работы внутри PhysicsPort.
*/
/* - Кол-во подвижных точек у существа передаём в качестве параметра:
     шаблон усложняет реализацию и работу с PhysicsPort.
template< size_t N >
*/
class LiveBody {
public:
    /**
    * UID сущности.
    */
    const d::uid_t uid;

    /**
    * Начальные координаты подвижных точек тела, в см. Можем позволить
    * себе std::vector, т.к. с подвижными точками всё равно работаем
    * через визуальный образ.
    */
    typedef std::vector< d::coord3d_t >  positionCoord_t;
    const positionCoord_t startCoord;

    /**
    * Физический образ сущности.
    */
    NewtonBody* body;

    /**
    * Визуальный образ сущности.
    */
    Ogre::SceneNode* sceneImage;

    /**
    * Менеджер сцены добавлен после того, как решили хранить координаты вершин
    * только в визуальном образе сущности.
    */
    Ogre::SceneManager* sm;

    /**
    * Сила, действующая в данный момент на тело.
    * Рассчитывается при изменении положения тела в пространстве.
    * Хранится в формате, удобном NewtonDynamics: ( x, y, z, K ).
    */
    float force[4];


    /**
    * Возможные положения тела.
    * Многие положения - спорны. Например, как определить "свободное падение"
    * в условиях невесомости? Или "лежит на спине", когда на тело действует
    * сила Кориолиса?
    *
    * @see position()
    */
    // @todo ? Реализовать через машину состояний? http://www.boost.org/doc/libs/1_48_0/libs/statechart/doc/tutorial.html
    struct Position {
        typedef size_t state;

        // Свободная стойка
        state free : 1;

        // Делает шаг вперёд
        state forwardStep : 1;
        // Делает шаг назад
        state backwardStep : 1;
        // Делает шаг в сторону
        state sidewayStep : 1;

        // Лежит
        state down : 1;
        // Лежит на спине
        state tergumDown : 1;
        // Лежит на животе
        state stomachDown : 1;
        // Лежит на боку
        state sideDown : 1;

        // Сидит на корточках
        state squat : 1;
    };





public:
    inline LiveBody( const d::uid_t& uid, const positionCoord_t& startCoord ) :
        uid ( uid ),
        startCoord( startCoord ),
        body( nullptr ),
        sceneImage( nullptr ),
        moImage( nullptr )
    {
        std::cout << "LiveBody()" << std::endl;

        // @todo optimize? Не инициализировать некоторые массивы
        //       (для скорости): пусть позаботится потомок.

        force[0] = force[1] = force[2] = 0.0f;
        force[3] = 1.0f;

        // Счётчик будет только увеличиваться: исп. для формирования UID
        ++LiveBody::count;
    }



    inline virtual ~LiveBody() {
        std::cout << "~LiveBody()" << std::endl;
    }



    /**
    * @return Количество подвижных точек у существа.
    */
    inline size_t countMovablePoint() const {
        return startCoord.size();
    }



    /**
    * @return Текущая координата подвижной точки *внутри* тела.
    *
    * @param i Номер подвижной точки тела.
    *
    * (!) Это не координата *тела* в мировом пространстве.
    *
    * @see coord()
    */
    inline d::coord3d_t operator[]( size_t i ) const {
        // Образ ещё может не существовать (см. visual())
        if ( !moImage ) {
            // Вернём тогда фиксированную координату
            return startCoord.at( i );
        }

        const auto ns = moImage->getNumSections();
        assert( (ns == 1) && "Работаем с односекционными объектами." );

        // @source http://ogre3d.org/tikiwiki/Read+raw+data+from+ManualObject+-+MOGRE
        // CONVERSION TO WORLD POSITIONS:
        // Grabb world properties of parent SceneNode
        // by _getDerivedPosition() / _getDerivedOrientation() / _getDerivedScale()
        // and apply this calculation:
        //   vertexPos = (orientation * (vertexPos * scale)) + position;

        const auto section = this->moImage->getSection( 0 );
        assert( section );

        const auto operation = section->getRenderOperation();
        assert( (operation->operationType == Ogre::RenderOperation::OT_LINE_LIST) && "Ожидался список линий." );
        assert( (operation->useIndexes == true) && "Необходим проиндексированный объект." );
        assert( (operation->indexData->indexCount > i) && "Координата с заданным индексом в объекте отсутствует." );

        auto pos = operation->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );
        auto vBuf = operation->vertexData->vertexBufferBinding->getBuffer( pos->getSource() );
        auto pVertex = (char*)( vBuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ) );
        char* pStartVertex = pVertex;

        auto iBuf = operation->indexData->indexBuffer;
        assert( !iBuf.isNull() && "У объекта для выбранных параметров поиска не существует вершин." );
        auto pLong = (unsigned int*)( iBuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ) );
        auto pShort = (unsigned short*)pLong;

        // Точка может быть ассоциирована с неск. вершинами
        // @see associatePoint
        const auto itr = associatePoint.find( i );
        assert( (itr != associatePoint.cend()) && "П-точка с таким номером не найдена в списке ассоциированных вершин." );
        assert( (itr->second.size() > 0) && "С п-точкой не ассоциирована ни одна из вершин модели." );
        // Здесь нам годится любая вершина, с которой ассоциирована точка:
        // ведь просят только вернуть координату п-точки
        const size_t k = *itr->second.cbegin();
        const auto index =
            (iBuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT)
              ? (unsigned int)pLong[k]
              : (unsigned int)pShort[k];
        pVertex = pStartVertex + (vBuf->getVertexSize() * index);
        float* pReal;
        pos->baseVertexPointerToElement( pVertex, &pReal );
        const auto rc = d::coord3d_t( pReal[0], pReal[1], pReal[2] );

        iBuf->unlock();
        vBuf->unlock();

        return rc;
    }



    /**
    * Установить желаемую координату п-точки.
    */
    inline void set( size_t i, const d::coord3d_t& coord ) {

        assert( moImage && "Невозможно установить желаемую координату для несуществующего образа. См. visual()." );

        const auto ns = moImage->getNumSections();
        assert( (ns == 1) && "Работаем с односекционными объектами." );

        // @source http://ogre3d.org/tikiwiki/Read+raw+data+from+ManualObject+-+MOGRE
        // CONVERSION TO WORLD POSITIONS:
        // Grabb world properties of parent SceneNode
        // by _getDerivedPosition() / _getDerivedOrientation() / _getDerivedScale()
        // and apply this calculation:
        //   vertexPos = (orientation * (vertexPos * scale)) + position;

        const auto section = this->moImage->getSection( 0 );
        assert( section );

        const auto operation = section->getRenderOperation();
        assert( (operation->operationType == Ogre::RenderOperation::OT_LINE_LIST) && "Ожидался список линий." );
        assert( (operation->useIndexes == true) && "Необходим проиндексированный объект." );

        auto pos = operation->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );
        auto vBuf = operation->vertexData->vertexBufferBinding->getBuffer( pos->getSource() );
        auto pVertex = (char*)( vBuf->lock( Ogre::HardwareBuffer::HBL_NORMAL ) );
        char* pStartVertex = pVertex;

        auto iBuf = operation->indexData->indexBuffer;
        assert( !iBuf.isNull() && "У объекта для выбранных параметров поиска не существует вершин." );
        auto pLong = (unsigned int*)( iBuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ) );
        auto pShort = (unsigned short*)pLong;

        // Точка может быть ассоциирована с неск. вершинами
        // @see associatePoint
        const auto itr = associatePoint.find( i );
        assert( (itr != associatePoint.cend()) && "П-точка с таким номером не найдена в списке ассоциированных вершин." );
        assert( (itr->second.size() > 0) && "С п-точкой не ассоциирована ни одна из вершин модели." );
        // Здесь нам нужны все вершины, с которыми ассоциирована точка:
        // просят изменить координату п-точки
        for (auto a = itr->second.cbegin(); a != itr->second.cend(); ++a) {
            const size_t k = *a;
            const auto index =
                (iBuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT)
                  ? (unsigned int)pLong[k]
                  : (unsigned int)pShort[k];
            pVertex = pStartVertex + (vBuf->getVertexSize() * index);
            float* pReal;
            pos->baseVertexPointerToElement( pVertex, &pReal );
            pReal[0] = coord.get<0>();
            pReal[1] = coord.get<1>();
            pReal[2] = coord.get<2>();
        }

        iBuf->unlock();
        vBuf->unlock();
    }



    /**
    * Установить для каждой точки тела новое желаемое положение.
    * Кол-во и порядок переданных п-точек должны соотв. заданным при
    * создании тела.
    *
    * @see setShift()
    */
    inline void set( const positionCoord_t& newCoord ) {

        assert( moImage && "Невозможно установить желаемую координату для несуществующего образа. См. visual()." );
        assert( (newCoord.size() == startCoord.size()) && "Кол-во и порядок п-точек должны соотв. заданным при создании тела." );

        const auto ns = moImage->getNumSections();
        assert( (ns == 1) && "Работаем с односекционными объектами." );

        // @source http://ogre3d.org/tikiwiki/Read+raw+data+from+ManualObject+-+MOGRE
        // CONVERSION TO WORLD POSITIONS:
        // Grabb world properties of parent SceneNode
        // by _getDerivedPosition() / _getDerivedOrientation() / _getDerivedScale()
        // and apply this calculation:
        //   vertexPos = (orientation * (vertexPos * scale)) + position;

        const auto section = this->moImage->getSection( 0 );
        assert( section );

        const auto operation = section->getRenderOperation();
        assert( (operation->operationType == Ogre::RenderOperation::OT_LINE_LIST) && "Ожидался список линий." );
        assert( (operation->useIndexes == true) && "Необходим проиндексированный объект." );

        auto pos = operation->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );
        auto vBuf = operation->vertexData->vertexBufferBinding->getBuffer( pos->getSource() );
        auto pVertex = (char*)( vBuf->lock( Ogre::HardwareBuffer::HBL_NORMAL ) );
        char* pStartVertex = pVertex;

        auto iBuf = operation->indexData->indexBuffer;
        assert( !iBuf.isNull() && "У объекта для выбранных параметров поиска не существует вершин." );
        auto pLong = (unsigned int*)( iBuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ) );
        auto pShort = (unsigned short*)pLong;

        // Проходим по всем полученным п-точкам, устанавливаем для них новые координаты
        for (auto mpItr = newCoord.cbegin(); mpItr != newCoord.cend(); ++mpItr) {
            const d::coord3d_t coord = *mpItr;
            const size_t i = std::distance( newCoord.cbegin(), mpItr );
            // Точка может быть ассоциирована с неск. вершинами
            // @see associatePoint
            const auto itr = associatePoint.find( i );
            assert( (itr != associatePoint.cend()) && "П-точка с таким номером не найдена в списке ассоциированных вершин." );
            assert( (itr->second.size() > 0) && "С п-точкой не ассоциирована ни одна из вершин модели." );
            // Здесь нам нужны все вершины, с которыми ассоциирована точка:
            // просят изменить координату п-точки
            for (auto a = itr->second.cbegin(); a != itr->second.cend(); ++a) {
                const size_t k = *a;
                const auto index =
                    (iBuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT)
                      ? (unsigned int)pLong[k]
                      : (unsigned int)pShort[k];
                pVertex = pStartVertex + (vBuf->getVertexSize() * index);
                float* pReal;
                pos->baseVertexPointerToElement( pVertex, &pReal );
                pReal[0] = coord.get<0>();
                pReal[1] = coord.get<1>();
                pReal[2] = coord.get<2>();
            }

        } // for (auto mpItr = newCoord.cbegin(); mpItr != newCoord.cend(); ++mpItr)

        iBuf->unlock();
        vBuf->unlock();
    }



    /**
    * То же, что set( positionCoord_t ), но координаты воспринимаются
    * относительно текущих.
    */
    inline void setShift( const positionCoord_t& shiftCoord ) {

        assert( moImage && "Невозможно установить желаемую координату для несуществующего образа. См. visual()." );
        assert( (shiftCoord.size() == startCoord.size()) && "Кол-во и порядок п-точек должны соотв. заданным при создании тела." );

        const auto ns = moImage->getNumSections();
        assert( (ns == 1) && "Работаем с односекционными объектами." );

        // @source http://ogre3d.org/tikiwiki/Read+raw+data+from+ManualObject+-+MOGRE
        // CONVERSION TO WORLD POSITIONS:
        // Grabb world properties of parent SceneNode
        // by _getDerivedPosition() / _getDerivedOrientation() / _getDerivedScale()
        // and apply this calculation:
        //   vertexPos = (orientation * (vertexPos * scale)) + position;

        const auto section = this->moImage->getSection( 0 );
        assert( section );

        const auto operation = section->getRenderOperation();
        assert( (operation->operationType == Ogre::RenderOperation::OT_LINE_LIST) && "Ожидался список линий." );
        assert( (operation->useIndexes == true) && "Необходим проиндексированный объект." );

        auto pos = operation->vertexData->vertexDeclaration->findElementBySemantic( Ogre::VES_POSITION );
        auto vBuf = operation->vertexData->vertexBufferBinding->getBuffer( pos->getSource() );
        auto pVertex = (char*)( vBuf->lock( Ogre::HardwareBuffer::HBL_NORMAL ) );
        char* pStartVertex = pVertex;

        auto iBuf = operation->indexData->indexBuffer;
        assert( !iBuf.isNull() && "У объекта для выбранных параметров поиска не существует вершин." );
        auto pLong = (unsigned int*)( iBuf->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ) );
        auto pShort = (unsigned short*)pLong;

        // Проходим по всем полученным п-точкам, устанавливаем для них новые координаты
        std::vector< Ogre::Vector3 >  nc;
        // Собираем ссылки на вершины (сокр. - old coord), чтобы позже быстро
        // обновить их. См. ниже 'pReal'.
        std::vector< float* >  oc;
        for (auto mpItr = shiftCoord.cbegin(); mpItr != shiftCoord.cend(); ++mpItr) {
            const d::coord3d_t coord = *mpItr;
            const size_t i = std::distance( shiftCoord.cbegin(), mpItr );
            // Точка может быть ассоциирована с неск. вершинами
            // @see associatePoint
            const auto itr = associatePoint.find( i );
            assert( (itr != associatePoint.cend()) && "П-точка с таким номером не найдена в списке ассоциированных вершин." );
            assert( (itr->second.size() > 0) && "С п-точкой не ассоциирована ни одна из вершин модели." );
            // Здесь нам нужны все вершины, с которыми ассоциирована точка:
            // просят изменить координату п-точки
            for (auto a = itr->second.cbegin(); a != itr->second.cend(); ++a) {
                const size_t k = *a;
                const auto index =
                    (iBuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT)
                      ? (unsigned int)pLong[k]
                      : (unsigned int)pShort[k];
                pVertex = pStartVertex + (vBuf->getVertexSize() * index);
                float* pReal;
                pos->baseVertexPointerToElement( pVertex, &pReal );
                pReal[0] += coord.get<0>();
                pReal[1] += coord.get<1>();
                pReal[2] += coord.get<2>();
                /* - @todo Добавить контроль размеров структуры.
                           Перейти на калькуляцию точек без связывания с вершинами?
                           Обновлять по событию "объект показался в камере"?
                nc.push_back( Ogre::Vector3(
                    pReal[0] + coord.get<0>(),
                    pReal[1] + coord.get<1>(),
                    pReal[2] + coord.get<2>()
                ) );
                oc.push_back( pReal );
                */
            }

        } // for (auto mpItr = newCoord.cbegin(); mpItr != newCoord.cend(); ++mpItr)

        // Обновляем положение вершин по возможностям тела
        updateWithConstraint( oc, nc );

        iBuf->unlock();
        vBuf->unlock();
    }





    /**
    * @return Форму для добавления сущности в физический мир.
    */
    virtual NewtonCollision* collision( NewtonWorld* nw ) const = 0;



    /**
    * @return Образ для визуализации сущности.
    *         Размещается на сцене 'sn'.
    */
    inline Ogre::SceneNode* visual( Ogre::SceneManager* sm, Ogre::SceneNode* sn ) {
        assert( sm );
        assert( sn );

        // Используем кеш
        if ( this->moImage ) {
            assert( this->sm );
            assert( this->sceneImage );
            assert( (this->sm == sm) && "Попытка использовать другой менеджер сцены для построенного ранее объекта." );
            assert( (this->sceneImage == sn) && "Попытка использовать другую сцену для построенного ранее объекта." );
            return this->sceneImage;
        }

        // Пришло время проинициализировать менеджера сцены...
        this->sm = sm;

        // ...и нарисовать тело на указанном холсте
        this->moImage = manualObject( sm );
        sn->attachObject( moImage );
        this->sceneImage = sn;

        return this->sceneImage;
    }




    /**
    * @return Копию сущности. (!) Вызывающий метод должен позаботиться
    *         об очистке памяти.
    */
    virtual LiveBody* clone() const = 0;



    /**
    * @return Состояние тела. Определяется по положению подвижных точек для
    *         каждого тела *индивидуально*.
    *
    * @see Position
    */
    //virtual inline Position position( NewtonBody* body ) const = 0;



    /**
    * @return Как сильно текущие координаты (текущая позиция) отличается от
    *         желаемой. Значение лежит в диапазоне [0.0; 1.0] ==
    *         == ["точно совпадает"; "очень отличается"].
    *
    * @param precissionGrid Точность сетки: как далеко может находиться
    *        координата, чтобы она считалась как "совпадающая с желаемой".
    */
    virtual inline float distinguishCoord( float precissionGrid ) {
        /* - @todo ...
        // Смотрим отклонение каждой точки от желаемых координат.
        // Вычислим отклонения для каждой точки и возьмём среднее
        // арифметическое.
        float s = 0.0f;
        for (size_t i = 0; i < MOVABLE_POINT_N * 3; i += 3) {
            // текущая
            const float cx = currentCoord[i + 0];
            const float cy = currentCoord[i + 1];
            const float cz = currentCoord[i + 2];
            // желаемая
            const float ex = estimateCoord[i + 0];
            const float ey = estimateCoord[i + 1];
            const float ez = estimateCoord[i + 2];
            // степень отклонения
            const float sx = ex - cx;
            const float sy = ey - cy;
            const float sz = ez - cz;
            //s = std::max( sx, std::max( sy, sz ) );
            // @todo optimize Избавиться от корня.
            s += std::sqrt( sx * sx + sy * sy + sz * sz );
        }
        s /= (float)MOVABLE_POINT_N;

        return (s > precissionGrid) ? 1.0 : (s / precissionGrid);
        */

        return 0.0f;
    }



    /**
    * Стремится привести подвижные точки тела к состоянию 'estimateCoord'.
    */
    virtual inline void desirePulse() {
        /* - @todo ...
        // Смотрим отклонение каждой точки от желаемых координат и
        // подводим точку к этим координатам
        const float k = 1.0f / 100.0f;
        for (size_t i = 0; i < MOVABLE_POINT_N * 3; i += 3) {
            // текущая
            const float cx = currentCoord[i + 0];
            const float cy = currentCoord[i + 1];
            const float cz = currentCoord[i + 2];
            // желаемая
            const float ex = estimateCoord[i + 0];
            const float ey = estimateCoord[i + 1];
            const float ez = estimateCoord[i + 2];
            // вектор отклонения
            const float sx = (ex - cx) * k;
            const float sy = (ey - cy) * k;
            const float sz = (ez - cz) * k;
            // новые координаты
            currentCoord[i + 0] += sx;
            currentCoord[i + 1] += sy;
            currentCoord[i + 2] += sz;
        }

        // Синхронизируем визуальный образ
        //synchronizeVisual();
        */
    }






protected:
    /**
    * Строит ManualObject для этого тела.
    */
    virtual Ogre::ManualObject* manualObject( Ogre::SceneManager* sm ) = 0;





private:
    /**
    * Устанавливает новые координаты для п-точек тела согласно его физическим
    * возможностям.
    *
    * @param oc Были координаты.
    * @param nc Стали координаты.
    *
    * @see set()
    */
    inline void updateWithConstraint(
        const std::vector< float* >&  oc,
        const std::vector< Ogre::Vector3 >&  nc
    ) const {
        // Проверяем, чтобы конечные координаты тела оказались в пределах
        // его физических характеристик

        // Размеры конечностей

        // @todo ...

    }






protected:
    /**
    * Подвижные точки тела. Текущее состояние.
    * Сокр. - movable points.
    * Характеризуются:
    *   - координатой (3D)
    *   - вектором скорости (3D)
    *//* - @todo optimize Наверняка, быстрее будет работать с чистыми указателями.
    float* currentCoord;
    float* currentSpeed;
    */
    /**
    * Храним подвижные точки в ManualObject.
    */
    Ogre::ManualObject* moImage;

    /**
    * Т.к. ManualObject может объединять несколько вершин в одной точке,
    * вынуждены хранить список подвижных точек и ассоциированных с ними
    * вершин в ManualObject, чтобы затрагивать все вершины, исходящие из
    * п-точки объекта.
    */
    std::unordered_map<
        // Номер подвижной точки в объекте
        size_t,
        // С какими вершинами (индексами) в 'moImage' связана эта точка
        std::unordered_set< size_t >
    > associatePoint;


    /**
    * Положение, к которому следует привести тело.
    * Сокр. - estimate movable points.
    */
    positionCoord_t estimateCoord;
    

    /**
    * Количество созданных тел. Может использоваться как основа для
    * формирования UID нового тела.
    */
    static size_t count;

};








/**
* Тело человека.
* Важные точки - вначале. Центр модели - точно посередине, чтобы совпадать с
* системами отсчёта для моделей в физическом и графическом движке.
* Здесь шкала задана в дм.

 9       /---\
 8      |  0  |
 7       \---/
 6         |
 5       --1--
 4      /  |  \
 3     /   |   \
 2   7/    |    \8
 1   |     |     |
 0   |    -2-    |
-1   3   /   \   4
-2      /     \
-3     /       \
-4   9/         \10
-5   |           |
-6   |           |
-7   |           |
-8   |           |
-9   5           6
.
ZX. -6543210123456+
*/
class HumanLB: public LiveBody {
public:
    inline HumanLB(
        const d::uid_t& uid = (d::uid_t)(LiveBody::count + 1)
    ) :
        LiveBody(
            uid,
            boost::assign::list_of
                (d::coord3d_t( 0.0f, 0.0f, 80.0f * 2.0f ))
                (d::coord3d_t( 0.0f, 0.0f, 50.0f * 2.0f ))
                (d::coord3d_t( 0.0f, 0.0f, 0.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, -10.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, -10.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, -90.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, -90.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, 20.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, 20.0f * 2.0f ))
                (d::coord3d_t( -60.0f, 0.0f, -40.0f * 2.0f ))
                (d::coord3d_t( 60.0f, 0.0f, -40.0f * 2.0f ))
        )
    {
        // Масштабируем фигуру
        /* - Вынесено в конструктор выше.
        const auto S = d::coord3d_t( 1.0f, 1.0f, 2.0f );
        startCoord = boost::assign::list_of
            (S * d::coord3d_t( 0.0f, 0.0f, 80.0f ))
            (S * d::coord3d_t( 0.0f, 0.0f, 50.0f ))
            (S * d::coord3d_t( 0.0f, 0.0f, 0.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, -10.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, -10.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, -90.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, -90.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, 20.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, 20.0f ))
            (S * d::coord3d_t( -60.0f, 0.0f, -40.0f ))
            (S * d::coord3d_t( 60.0f, 0.0f, -40.0f ))
        ;
        */
    }



    inline virtual ~HumanLB() {
    }




    /**
    * @see LiveBody
    */
    virtual inline NewtonCollision* collision( NewtonWorld* nw ) const {
        NewtonCollisionInfoRecord::NewtonCapsuleParam capsule;
        capsule.m_r0 = capsule.m_r1 = 120.0f / 2.0f;
        capsule.m_height = 180.0f;
        return CollisionManager::singleton()->collision( nw, capsule );
    }



    /**
    * @see LiveBody
    */
    virtual inline Ogre::ManualObject* manualObject( Ogre::SceneManager* sm ) {

        assert( sm );

        // Задаём постоянное имя: объект должен запрашиваться один раз, если
        // ниже будет ошибка, значит следует пересмотреть порядок вызова этого
        // метода на уровнях выше или убедиться, что UID помещаемых в мир
        // образов - уникальны.
        const std::string imageName =
            Ogre::StringConverter::toString( uid ) + "::MO::HumanLB";

        // См. образ в комм. к классу
        auto mo = sm->createManualObject( imageName );
        mo->begin( "combat/white", Ogre::RenderOperation::OT_LINE_LIST );

        const auto& ptrStartCoord = startCoord;
        auto& ptrAssociatePoint = associatePoint;
        size_t ind = 0;
        const auto addVertex = [ &mo, ptrStartCoord, &ind, &ptrAssociatePoint ] ( size_t ai, size_t bi ) {
            const d::coord3d_t a = ptrStartCoord.at( ai );
            mo->position( a.get<0>(), a.get<1>(), a.get<2>() );
            mo->index( ind );
            auto itr = ptrAssociatePoint.find( ai );
            if (itr == ptrAssociatePoint.end()) {
                itr = ptrAssociatePoint.insert( std::make_pair( ai,  std::unordered_set< size_t >() ) ).first;
            }
            itr->second.insert( ind );
            ++ind;

            const d::coord3d_t b = ptrStartCoord.at( bi );
            mo->position( b.get<0>(), b.get<1>(), b.get<2>() );
            mo->index( ind );
            itr = ptrAssociatePoint.find( bi );
            if (itr == ptrAssociatePoint.end()) {
                itr = ptrAssociatePoint.insert( std::make_pair( bi,  std::unordered_set< size_t >() ) ).first;
            }
            itr->second.insert( ind );
            ++ind;
        };

        addVertex( 0, 1 );
        addVertex( 1, 2 );
        addVertex( 1, 7 );
        addVertex( 1, 8 );
        addVertex( 7, 3 );
        addVertex( 8, 4 );
        addVertex( 2, 9 );
        addVertex( 2, 10 );
        addVertex( 9, 5 );
        addVertex( 10, 6 );

        mo->end();

        /* - Не переводим в меш, чтобы можно было манипулировать вершинами объекта.
        auto mesh = mo->convertToMesh( imageName );
        auto e = sm->createEntity( mesh->getName() );
        */
        mo->setDebugDisplayEnabled( true );
        mo->setBoundingBox( Ogre::AxisAlignedBox(
            -60.0f, -1.0f, -160.0f,
             60.0f,  1.0f,  160.0f
        ) );

        return mo;
    }




    /**
    * @see LiveBody
    */
    virtual inline LiveBody* clone() const {
        return new HumanLB( this->uid );
    }




    /**
    * @see LiveBody
    */
    virtual inline void synchronizeVisual() {
        // 
        /* - @todo Просматривать образ.
        for (size_t i = 0; i < MOVABLE_POINT_N * 3; i += 3) {
            // текущая
            const float cx = currentCoord[i + 0];
            const float cy = currentCoord[i + 1];
            const float cz = currentCoord[i + 2];
            // просмтариваем образ
        }
        */
        // Перерисуем образ
        // @todo ...

    }


};

