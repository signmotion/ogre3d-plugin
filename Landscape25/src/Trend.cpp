#include "Trend.h"

// (!) Все размеры - в метрах.


Trend::Trend(
    const std::vector< form2d_t >&  all,
    const FeatureTrend& ft
) :
    all( all ),
    feature( ft )
{
    assert( (all.size() > 0) && "Тренду не назначено ни одной формы." );

    // Проверяем допустимое изменение масштаба
    assert( (feature.scale.get<0>() >= ONE2D) && "Масштаб тренда по осям должен быть >= 1." );
    assert( (feature.scale.get<0>() <= feature.scale.get<1>())
        && "Начальные значения для масштаба должны быть меньше конечных." );
}





form2d_t Trend::form( bool natural ) const {

    // Инициируем генератор случ. чисел, чтобы сохранить детерминированность
    // (!) mt19937 даёт действительно качественное распределение.
    // @see http://www.boost.org/doc/libs/1_47_0/doc/html/boost_random/reference.html
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_int<>
    > indexFormGenerator(
        boost::mt19937( feature.seed ),
        boost::uniform_int<>( 0, all.size() - 1 )
    );
    const auto index = indexFormGenerator();
        

    // Учитываем дополнительные характеристики тренда

    // Форма может быть разных размеров (слушаем 'seed')
    const float x1 = feature.scale.get<0>().get<0>();
    const float x2 = feature.scale.get<1>().get<0>();
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_real< float >
    > kxSizeFormGenerator(
        boost::mt19937( feature.seed + 1 ),
        boost::uniform_real< float >( x1, x2 )
    );

    const float z1 = feature.scale.get<0>().get<1>();
    const float z2 = feature.scale.get<1>().get<1>();
    boost::variate_generator<
        boost::mt19937,
        boost::uniform_real< float >
    > kzSizeFormGenerator(
        boost::mt19937( feature.seed + 2 ),
        boost::uniform_real< float >( z1, z2 )
    );


    // Получаем реальных размеров форму
    form2d_t r;
    const scale2d_t scale( kxSizeFormGenerator(), kzSizeFormGenerator() );
    assert( (scale > ONE2D) && "Масштаб по любой оси должен быть больше 1." );
    for (auto itr = all[index].first.cbegin(); itr != all[index].first.cend(); ++itr) {
        const coord2d_t original = *itr;
        const coord2d_t modify = original * scale;
        r.first.push_back( modify );
    }
    r.second = calcNormal( r.first.front(), r.first.back() );


    // Заботимся об уникальной форме
    if ( natural ) {
        // Максимальная рябь (искажение), которая будет применена к
        // координатам вершин тренда. Для каждой координаты указывается
        // своя степень искажения.
        // Здесь используем только искажение по высоте, ось OZ.
        const auto amplitude = coord2d_t(
            feature.natural.get<0>(),
            feature.natural.get<1>()
        ) * scale;
        Trend::distort( &r, feature.seed, amplitude );
    }

    return r;
}






std::vector< form2d_t >  AscendTrend::surface() {
    typedef coord2d_t c;

    const surface2d_t s1 = boost::assign::list_of
        (c(0,0.01)) (c(1,0.04)) (c(2,0.1)) (c(3,0.19)) (c(4,0.29)) (c(5,0.41)) (c(6,0.54)) (c(7,0.69)) (c(8,0.84)) (c(9,1)) (c(10,1.15)) (c(11,1.3)) (c(12,1.45)) (c(13,1.58)) (c(14,1.7)) (c(15,1.8)) (c(16,1.89)) (c(17,1.95)) (c(18,1.98)) (c(19,2))
    ;
    const normal2d_t n1 = calcNormal( s1.front(), s1.back() );

    std::vector< form2d_t >  v = boost::assign::list_of
        (std::make_pair( s1, n1 ))
    ;

    return v;
}





std::vector< form2d_t >  DescendTrend::surface() {
    typedef coord2d_t c;

    const surface2d_t s1 = boost::assign::list_of
        (c(0,2)) (c(1,1.98)) (c(2,1.95)) (c(3,1.89)) (c(4,1.8)) (c(5,1.7)) (c(6,1.58)) (c(7,1.45)) (c(8,1.3)) (c(9,1.15)) (c(10,1)) (c(11,0.84)) (c(12,0.69)) (c(13,0.54)) (c(14,0.41)) (c(15,0.29)) (c(16,0.19)) (c(17,0.1)) (c(18,0.04)) (c(19,0.01))
    ;
    const normal2d_t n1 = calcNormal( s1.front(), s1.back() );

    std::vector< form2d_t >  v = boost::assign::list_of
        (std::make_pair( s1, n1 ))
    ;

    return v;
}





std::vector< form2d_t >  HollowTrend::surface() {
    typedef coord2d_t c;

    const surface2d_t s1 = boost::assign::list_of
        (c(0,2)) (c(1,1.93)) (c(2,1.8)) (c(3,1.58)) (c(4,1.3)) (c(5,1)) (c(6,0.69)) (c(7,0.41)) (c(8,0.19)) (c(9,0.04)) (c(10,0)) (c(11,0.04)) (c(12,0.19)) (c(13,0.41)) (c(14,0.69)) (c(15,1)) (c(16,1.3)) (c(17,1.58)) (c(18,1.8)) (c(19,1.95))
    ;
    const normal2d_t n1 = calcNormal( s1.front(), s1.back() );

    std::vector< form2d_t >  v = boost::assign::list_of
        (std::make_pair( s1, n1 ))
    ;

    return v;
}
