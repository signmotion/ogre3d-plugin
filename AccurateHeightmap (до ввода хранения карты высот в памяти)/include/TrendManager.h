#pragma once

#include "default.h"
#include "PlatoTrend.h"


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
    * @template N Размер области (в ячейках), для которой подбирается тренд.
    *
    * @param source Полный путь к папке, откуда следует брать тренды.
    * @param platoSize Реальный размер плато.
    * @param normal Определяет угол между плоскостью OXY и трендом. Необходима,
    *        т.к. поверхность тренда всегда рисуется плоской (по карте высот).
    */
    template< size_t N >
    std::shared_ptr< PlatoTrend< N > >  platoTrend(
        const std::string& source,
        const size3d_t& platoSize,
        const normal3d_t& normal
    ) const;







private:
    /**
    * Синглетон.
    */
    static TrendManager* instance;

};

