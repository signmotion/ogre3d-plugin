#pragma once

#include "default.h"
#include "PlatoTrend.h"


/**
* ������ ��������� ������� - ������� (���������, ��� �������������) ����������
* �����.
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
    * @return ����� ��� �����������.
    *
    * @template N �� ������� ����� ������� ������� �����. ������ ����� ��
    *           ��������� ��� ������ ���������� ����� �����.
    *
    * @param source ������ ���� � �����, ������ ������� ����� ������.
    */
    template< size_t N >
    std::shared_ptr< PlatoTrend< N > >  platoTrend(
        const std::string& source,
        const Plato< N >& plato
    ) const;







private:
    /**
    * ���������.
    */
    static TrendManager* instance;

};

