#pragma once

#include "default.h"
#include "FeatureTrend.h"


/**
* Тренд - то, что собрашись вместе определяет состав, форму, протяжённость
* биома. Форма тренда - это конструктор для слоёв.
* Почему тренды? Анализируя дюну в пустыне, мы можем заметить для неё,
* как минимум, след. особенности: спуск, подъём, зыбь и яма. Эти особенности
* называем "тренды".
*
* @see DuneLayer
*/
class Trend {

public:
    /**
    * Характеристики тренда.
    */
    FeatureTrend feature;



// Класс только для наследования
protected:
    /**
    * Вершины тренда всегда упорядочены по оси OX.
    * 
    * @param size Оригинальные размеры тренда по осям координат.
    * @param scale Масштаб фигуры. Если меньше 1, выравнивается к 1.
    * @param См. FeatureTrend.
    */
    Trend(
        const std::vector< form2d_t >&  all,
        const FeatureTrend& ft
    );


    
    virtual inline ~Trend() {
    }




public:
    /**
    * @return Вершины линии тренда (форма поверхности).
    *
    * Каждый тренд берёт на себя обязательства:
    *     - предоставлять одни и те же формы поверхности при подаче одинаковых
    *       параметров
    *     - размеры форм одного тренда не должны существенно отличаться
    *
    * При одинаковых семенах (seed) - одни и те же деревья.
    *
    * @param natural true - к вершинам примешиваются искажения, что позволяет
    *        разнообразить возвращаемые результаты (получить т.н. натуральные
    *        вершины).
    */
    virtual form2d_t form( bool natural ) const;



    /**
    * @return Коробка минимальных рамеров, в которую можно заключить тренд.
    */
    inline box2d_t box( bool natural ) const {
        const auto f = form( natural );
        return calcBox( f.first );
    }



    /**
    * @return Координаты первой (a) и последней (b) точек формы.
    *
    * Параметры необходимы, т.к. форма берётся из vertex().
    */
    inline std::pair< coord2d_t, coord2d_t >  path( bool natural ) const {
        const auto v = form( natural ).first;
        return std::make_pair( v.front(), v.back() );
    }



    /**
    * @return Градиент формы тренда. Т.к. форма рисуется из точки А в Б,
    *         градиент вычисляется как разница координат A и Б. Служит
    *         мерой, насколько приблизимся / отдалимся от заданной точки
    *         при включении формы тренда в поверхность (вычисляется простым
    *         сложением координаты и градиента).
    *
    * Параметры необходимы, т.к. форма берётся из vertex().
    *
    * @see path()
    */
    inline vector2d_t grad( bool natural ) const {
        const auto p = path( natural );
        return (p.second - p.first);
    }




protected:
    /**
    * Смещает координаты случайным образом, но в пределах указанной амплитуды.
    */
    static inline void distort( form2d_t* r, int seed, const size2d_t& amplitude ) {
        const auto ax = amplitude.get<0>();
        assert( (ax >= 0) && "Амплитуда 1 задаётся положительным числом." );
        const auto az = amplitude.get<1>();
        assert( (az >= 0) && "Амплитуда 2 задаётся положительным числом." );

        // Создаём генератор для добавления поверхности зыби
        // (!) mt19937 даёт действительно качественное распределение.
        // @see http://www.boost.org/doc/libs/1_47_0/doc/html/boost_random/reference.html
        boost::mt19937 rg( seed );
        boost::variate_generator<
            boost::mt19937,
            boost::uniform_int<>
        > axDistortGenerator( rg,  boost::uniform_int<>( -(int)ax, (int)ax ) );
        boost::variate_generator<
            boost::mt19937,
            boost::uniform_int<>
        > azDistortGenerator( rg,  boost::uniform_int<>( -(int)az, (int)az ) );

        // Первую и последнюю координаты не трогаем
        if (r->first.size() < 3) {
            return;
        }
        for (size_t i = 1; i < (r->first.size() - 1); ++i) {
            // Трусим координату
            // @todo Некрасивое решение, т.к. ограничивает определение
            //       формы, которую можно потрусить целым числом по оси OX.
            const float dx = (float)axDistortGenerator();
            if ((r->first[i].get<0>() + dx) > 0) {
                r->first[i].get<0>() += dx;
            }
            const float dz = (float)azDistortGenerator();
            r->first[i].get<1>() += dz;
        }

        // Корректируем нормаль
        /* - Нет надобности: крайние координаты ост. без изм.
        r->second = calcNormal( r->first.front(),  r->first.back() );
        */

    }





protected:
    /**
    * Возможные *оригинальные* формы для этого тренда. На основе этих форм
    * строится всё многообразие *натуральных* форм.
    * @todo Форм может быть несколько. Заполняются (инициируются) потомками.
    */
    std::vector< form2d_t >  all;

};










// Список предопределённых трендов, которые могут использоваться Layer.
// Помимо того, что тренды определены формой, они могут принимать
// параметры, существенно меняющие их форму.
// (!) Размеры форм трендов задаются в метрах.





/**
* Подъём (справа налево).
*/
class AscendTrend :
    public Trend
{
public:
    inline AscendTrend(
        const FeatureTrend& ft
    ) :
        Trend( surface(), ft )
    {
        // Амплитуда искажения для поверхностей. Задаётся в диапазоне [0.0f; 1.0f]
        // и учитывается как процент относительно box() тренда.
        // Для дюны амплитуда должна быть небольшой, чтобы сохранить плавность.
        // x, z
        feature.natural.get<0>() = 0.2f;
        feature.natural.get<1>() = 0.2f;
    }


    virtual inline ~AscendTrend() {
    }


    /**
    * @return Формы тренда для инициализации.
    *
    * Метод введён для подготовки передачи параметров родителю.
    */
    static std::vector< form2d_t >  surface();

};





/**
* Спуск (слева направо).
*/
class DescendTrend :
    public Trend
{
public:
    inline DescendTrend(
        const FeatureTrend& ft
    ) :
        Trend( surface(), ft )
    {
        // @see AscendTrend
        feature.natural.get<0>() = 0.2f;
        feature.natural.get<1>() = 0.2f;
    }


    virtual inline ~DescendTrend() {
    }


    /**
    * @see AscendTrend
    */
    static std::vector< form2d_t >  surface();

};






/**
* Впадина, яма, выемка.
*/
class HollowTrend :
    public Trend
{
public:
    inline HollowTrend(
        const FeatureTrend& ft
    ) :
        Trend( surface(), ft )
    {
        // @see AscendTrend
        feature.natural.get<0>() = 0.2f;
        feature.natural.get<1>() = 0.2f;
    }


    virtual inline ~HollowTrend() {
    }


    /**
    * @see AscendTrend
    */
    static std::vector< form2d_t >  surface();

};
