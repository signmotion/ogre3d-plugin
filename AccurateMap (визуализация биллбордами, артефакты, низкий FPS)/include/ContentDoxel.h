#pragma once

//#include <unordered_map>
#include <boost/any.hpp>


/**
* Содержание докселя.
* Структура предназначена для наследования.
*
* @see Doxel
*/
struct ContentDoxel {
    /**
    * Данные докселя.
    */
    //std::unordered_map< std::string, boost::any >  data;



    inline ContentDoxel() {
    }


    virtual inline ~ContentDoxel() {
    }

};





/**
* Информация о ячейке карты.
*/
struct CellCD : public ContentDoxel {

    /**
    * Код элемента, находящегося в этой ячейке.
    * Попроуем сделать константным, чтобы уменьшить ентропию при
    * манипуляции с картой. @todo optimize Производительность?
    */
    const int element;


    inline CellCD(
        int element = 0
    ) :
        element( element )
    {
    }


    virtual inline ~CellCD() {
    }

};
