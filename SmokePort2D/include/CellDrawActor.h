#pragma once

#include "Actor.h"
#include <Ogre.h>


/**
* ��������� ������.
* ��������������� ����������, �������������� ��������.
*/
template< typename C >
struct AllDrawMessage {

    // @see ��������� > Port.h, CellPort.h
    Ogre::SceneManager* sm;

    /**
    * ����������� ���������������.
    */
    const float scale;

    /**
    * ������� � � �������.
    */
    const C* content;
    const size_t N;
    const size_t M;

    /**
    * �������� ��� ���������� �������.
    * ��. ���������� allDrawHandler().
    */
    const std::string prefixMaterial;

    /**
    * ��������� ����� ��� ������ ��������� (true) ��� ���������� ����
    * ���� ��� (false).
    */
    const bool update;


    inline AllDrawMessage(
        Ogre::SceneManager* sm,
        float scale,
        C* content,
        size_t N, size_t M,
        const std::string& prefixMaterial,
        bool update
    ) :
        sm( sm ),
        scale( scale ),
        content( content ),
        N( N ), M( M ),
        prefixMaterial( prefixMaterial ),
        update( update )
    {
        assert( sm );
        assert( (scale > 0.001f) && "������� ��� ������ ����� ������� ����������." );
        assert( content && "���������� ������ ���� �������." );
        assert( ((N > 0) && (M > 0)) && "�������� ������ �������." );
        assert( !prefixMaterial.empty() && "��� �������� �� ������ ����� ��������." );
    }
        
};







/**
* ��������� ������.
* ���������� ���������� �������� �����.
* ��������� ����� ���������� ���� ���.
*/
struct GridDrawMessage {

    // @see ��������� > Port.h, CellPort.h
    Ogre::SceneManager* sm;

    /**
    * ����������� ���������������.
    */
    const float scale;

    /**
    * ���������� ����� � �����.
    */
    const size_t size;


    inline GridDrawMessage(
        Ogre::SceneManager* sm,
        float scale,
        size_t size
    ) :
        sm( sm ),
        scale( scale ),
        size( size )
    {
        assert( sm );
        assert( (scale > 0.001f) && "������� ��� ������ ����� ������� ����������." );
        assert( (size >= 1) && "����� ��� ����� �� ������." );
    }
        
};






/**
* ��������� ������.
* ��������������� ������ � ���������� ������������.
*/
template< typename C >
struct DrawMessage {

    // @see ��������� > Port.h, CellPort.h
    Ogre::SceneManager* sm;

    /**
    * ����������� ���������������.
    */
    const float scale;

    /**
    * ���������� ������ ��� ������.
    */
    const C cell;

    /**
    * �������� ��� ���������� �������.
    * ��. ���������� drawHandler().
    */
    const std::string prefixMaterial;


    inline DrawMessage(
        Ogre::SceneManager* sm,
        float scale,
        const C& cell,
        const std::string& prefixMaterial,
        bool update
    ) :
        sm( sm ),
        scale( scale ),
        cell( cell),
        prefixMaterial( prefixMaterial )
    {
        assert( sm );
        assert( (scale > 0.001f) && "������� ��� ������ ����� ������� ����������." );
        assert( !prefixMaterial.empty() && "��� �������� �� ������ ����� ��������." );
    }
        
};





/**
* �����.
* ������ �� ������. ����� ������ �� ������.
*/
template< typename C >
class CellDrawActor : public Actor {
public:
    inline CellDrawActor() {
        RegisterHandler( this, &CellDrawActor::allDrawHandler );
        RegisterHandler( this, &CellDrawActor::gridDrawHandler );
        RegisterHandler( this, &CellDrawActor::drawHandler );

    }



    virtual inline ~CellDrawActor() {
    }



    /**
    * @see allDrawHandler()
    * ��� �������.
    */
    static void allDraw( const AllDrawMessage< C >& m );



    /**
    * @see gridDrawHandler()
    * ��� �������.
    */
    static void gridDraw( const GridDrawMessage& m );



protected:
    /**
    * ������������� ��������� ����������.
    * ������� ������ ��� ����� ��������� �������� �������.
    */
    void allDrawHandler(
        const AllDrawMessage< C >& m,
        const Theron::Address from
    );



    /**
    * ������ �������� �����.
    */
    void gridDrawHandler(
        const GridDrawMessage& m,
        const Theron::Address from
    );



    /**
    * ������������� ������ � ���������� ������������.
    */
    void drawHandler(
        const DrawMessage< C >& m,
        const Theron::Address from
    );

};
