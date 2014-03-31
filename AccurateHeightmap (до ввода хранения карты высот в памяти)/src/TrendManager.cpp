#include "TrendManager.h"


TrendManager* TrendManager::instance = nullptr;


template< size_t N >
std::shared_ptr< PlatoTrend< N > >  TrendManager::platoTrend(
    const std::string& source,
    const size3d_t& platoSize,
    const normal3d_t& normal
) const {

    // Определяем подходящий файл
    const std::string file = source
        + Ogre::StringConverter::toString( N ) + "x"
        + Ogre::StringConverter::toString( N ) + "-"
        // @todo Ранжировать тренды по наклону поверхности.
        //+ Ogre::StringConverter::toString( (int)angle )
        + "0"
        + ".png";

    auto trend = new PlatoTrend< N >( file, platoSize, normal );

    return std::shared_ptr< PlatoTrend< N > >( trend );
}



template
std::shared_ptr< PlatoTrend< 100 > >  TrendManager::platoTrend< 100 >(
    const std::string& source,
    const size3d_t& platoSize,
    const normal3d_t& normal
) const;
