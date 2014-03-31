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
    * @return ����� ��� �����������.
    *
    * @template N ������ ������� (� �������), ��� ������� ����������� �����.
    *
    * @param source ������ ���� � �����, ������ ������� ����� ������.
    * @param platoSize �������� ������ �����.
    * @param normal ���������� ���� ����� ���������� OXY � �������. ����������,
    *        �.�. ����������� ������ ������ �������� ������� (�� ����� �����).
    */
    template< size_t N >
    std::shared_ptr< PlatoTrend< N > >  platoTrend(
        const std::string& source,
        const size3d_t& platoSize,
        const normal3d_t& normal
    ) const;







private:
    /**
    * ���������.
    */
    static TrendManager* instance;

};

