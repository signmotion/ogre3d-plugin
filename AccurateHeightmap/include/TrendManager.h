#pragma once

#include "default.h"
#include "PlatoTrend.h"


/**
* Задача менеджера трендов - вернуть (построить, при необходимости) подходящий
* тренд.
*/
class TrendManager {

protected:
    inline TrendManager() {
    }


    virtual inline ~TrendManager() {
    }



public:
    static inline TrendManager* singleton() {
        if ( instance ) {
            return instance;
        }
        instance = new TrendManager();
        return instance;
    }



    /**
    * @return Тренд для поверхности.
    *
    * @template N На сколько ячеек следует разбить плато. Служит одним из
    *           оснований для выбора подходящей карты высот.
    *
    * @param source Полный путь к папке, откуда следует брать тренды.
    */
    template< size_t N >
    std::shared_ptr< PlatoTrend< N > >  platoTrend(
        const std::string& source,
        const Plato< N >& plato
    ) const;







private:
    /**
    * Синглетон.
    */
    static TrendManager* instance;

};

