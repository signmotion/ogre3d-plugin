#include "TrendManager.h"


TrendManager* TrendManager::instance = nullptr;


template< size_t N >
std::shared_ptr< PlatoTrend< N > >  TrendManager::platoTrend(
    const std::string& source,
    const Plato< N >& plato
) const {

    // Определяем подходящий файл
    
    // Учитываем пропорции плато
    const float kxy = plato.avgProportion();


    // @todo Учитывать угол наклона (нормали) плато.

    // Имя файла-источника
    const std::string file = source
        + Ogre::StringConverter::toString( 100 ) + "x"
        + Ogre::StringConverter::toString( 100 ) + "-"
        + "0"
        + ".png";

    auto trend = new PlatoTrend< N >( file, plato );

    return std::shared_ptr< PlatoTrend< N > >( trend );
}



template
std::shared_ptr< PlatoTrend< 100 > >  TrendManager::platoTrend<100>(
    const std::string& source,
    const Plato<100>& plato
) const;
