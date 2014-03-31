#pragma once

#include <Ogre.h>
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include "configure.h"
#include "default.h"
#include "BitContent3D.h"
#include "../external/vjson/json.h"


/**
* ������������ ����.
*/
class Vizi3DWorld {
public:
    /**
    * ������ ������� ����� ������������ �������������� ��������� - ������.
    */
    typedef char signElement_t;

    /**
    * ���������� �����.
    * ������������ ��� ����� �������� � ������� 3D-����� �������, ���
    * ����� �����������.
    */
    typedef std::unordered_map<
        signElement_t,
        // ������ � ���������, ����� �� ���������� ��� BitContent3D
        // ����������� �� ���������
        std::shared_ptr< BitContent3D >
    > content_t;

    /**
    * ������� �����, �������������� ��� ������.
    * ��� *�������* ����������� ������� �����, ������� ��������
    * �������������� ������������. ��������, �� �������� ���������
    * ��������.
    *
    * @see prepareVisualBitImage()
    * @see drawNext()
    */
    typedef struct {
        std::shared_ptr< BitContent3D >  alphaTypeBitImage;
        std::shared_ptr< BitContent3D >  solidTypeBitImage;
    } visualContent_t;


    /**
    * ���������� *�������* �����.
    * ������ ����� ������� ���� ������� - 'mapZ'.
    */
    typedef struct {
        /**
        * ������� ���������� �������.
        * ������� ��������������� ������������ ������ ������.
        */
        d::coord3d_t coord;

        /**
        * ������ ������� � �������.
        * �������� �����, �.�. ����� ����� ����� ����� ��������� ��������.
        */
        d::sizeInt3d_t size;

        /**
        * ������� ������� ���, ��. �������, ������ ����� ������.
        * ��� �������� �������������� �������, ��������� ��� ����.
        */
        float scale;

        /**
        * ���������� �������.
        */
        content_t content;

        /**
        * ���������� �������, �������������� ��� ������.
        */
        visualContent_t visualContent;

    } tinyMap_t;


    /**
    * ���� ���������.
    * �������� �������������� ������������ �����.
    * �������������� ����:
    *   - alpha �� ��������� ����� ���� ����� ������ ��������.
    *   - solid ������������ �������, ���������� ��� ������.
    */
    enum typeElement_e {
        TYPE_ELEMENT_UNDEFINED = 0,
        TYPE_ELEMENT_ALPHA,
        TYPE_ELEMENT_SOLID
    };


    /**
    * �������� �������� ����, ������������ �� �����.
    * �� ��������� ����� ����� ��������, �.�. ��� ������������ ��� ����
    * � 'content_t' - � ����� ���������� ��� ������ � ������.
    */
    struct noteElement_t {
        /**
        * �������� ��������.
        */
        std::string name;

        /**
        * �����.
        */
        std::string form;

        /**
        * ��������.
        */
        std::string material;

        /**
        * ���.
        */
        typeElement_e type;


        inline noteElement_t(
        ) :
            type( TYPE_ELEMENT_UNDEFINED )
            // ��������� ���-�� ����� ���� ����������� �� ���������
        {
        }

    };


    /**
    * ��������� � ����������� � ����.
    */
    struct about_t {
        /**
        * �������� �����.
        */
        std::string name;

        /**
        * ������� �����.
        */
        float scale;

        /**
        * ���� � ������� �����.
        */
        std::unordered_map< std::string, std::string >  pathToImage;

        /**
        * ����� c������ �� �������� �� ������ ������������. ������ �������
        * ����� ��������.
        *
        * @todo ������������ ����� ����� ���� �� ������ ���������. ��������� Z.
        */
        std::unordered_map< std::string, tinyMap_t >  tinyMap;

        /**
        * �� ��������� �����.
        * ������ ������� ����� tinyMap_t ������ ���� ������������ �����.
        */
        std::unordered_map< signElement_t, noteElement_t >  noteMap;


        inline about_t(
        ) :
            scale( 0.0f )
            // ��������� ���-�� ����� ���� ����������� �� ���������
        {
        }

    };


    /**
    * ��������� ��� ��������� ���������� �� �������� ��� ��� ������������.
    * ����������� �������� ��� �������� ������� (��� ���������� ��� �����������
    * �������� �����).
    */
    struct visualElement_t {
        /**
        * ������� ���� (��� ���������������� ��������).
        * � ����� ������, ����� ���������� �� �������� ������� ����� (scale).
        */
        const float scaleWorld;

        /**
        * ����� ��������.
        * ������� �������� ����� (��� char).
        */
        const signElement_t signElement;

        /**
        * ��� ����� ������������ �������.
        */
        const noteElement_t& noteElement;

        /**
        * ������� � ������� �����, ��� ������������� �������.
        */
        const BitContent3D& bitContent;

        /**
        * ������� ����������, ������ � *������� �������* �����, �������
        * �������� ���� �������.
        */
        const d::coord3d_t& coord;
        const d::sizeInt3d_t& size;
        // ������� �������� ����� (��� float).
        const float scale;

        inline visualElement_t(
            float scaleWorld,
            signElement_t signElement,
            const noteElement_t& noteElement,
            const BitContent3D& bitContent,
            const d::coord3d_t& coord,
            const d::sizeInt3d_t& size,
            float scale
        ) :
            scaleWorld( scaleWorld ),
            signElement( signElement ),
            noteElement( noteElement ),
            bitContent( bitContent ),
            coord( coord ),
            size( size ),
            scale( scale )
        {
        }

    };



    /**
    * ����� �������� �� �����.
    */
    class iterator :
        public std::iterator< std::forward_iterator_tag, Vizi3DWorld >
    {
    public:
        inline iterator(
        ) :
            map( nullptr ),
            mTinyMap( std::unordered_map< std::string, tinyMap_t >::const_iterator() ),
            mMapElement( content_t::const_iterator() )
        {
        }

        inline iterator(
            Vizi3DWorld& map,
            std::unordered_map< std::string, tinyMap_t >::const_iterator  tinyMap
        ) :
            map( &map ),
            mTinyMap( tinyMap )
        {
            // ���������� ������ ���������
            assert( (map.about().tinyMap.size() > 0) 
                && "��� ����������� ������������ ��������� �� ������ ����� (����������� ��������)." );
            auto dest = (tinyMap == map.about().tinyMap.cend())
                ? map.about().tinyMap.cbegin()
                : tinyMap;
            assert( (dest->second.content.cbegin() != dest->second.content.cend()) 
                && "������� ������������ ��������� �� ������ ���������� (����������� �������� � �������)." );
            mMapElement = dest->second.content.cbegin();
        }

        /**
        * @return �������� �� ��������� �� ������� (��������� �� cend()).
        *
        * �������� ������� (itr == cend()).
        */
        inline bool empty() const {
            // ��������� ����� ������ ������ ������ ��������� �� ������� �����
            return !map || (mTinyMap == map->about().tinyMap.cend());
        }

        /**
        * @return ������� ��������� �� ������� �����.
        */
        inline std::unordered_map< std::string, tinyMap_t >::const_iterator  tinyMap() {
            return mTinyMap;
        }

        /**
        * @return ������� ��������� �� ������� ������� �����.
        *         nullptr, ���� ��������� ���.
        */
        inline content_t::const_iterator  mapElement() {
            return mMapElement;
        }

        inline iterator& operator=( const iterator& itr ) {
            map = itr.map;
            mTinyMap = itr.mTinyMap;
            mMapElement = itr.mMapElement;
            return *this;
        }

        inline bool operator==( const iterator& itr ) const {
            if ( empty() && itr.empty() ) {
                return true;
            }
            return (mTinyMap == itr.mTinyMap)
                && (mMapElement == itr.mMapElement);
        }

        inline iterator& operator++() {
            assert( map && "�������� �� ���������������." );
            if ( empty() ) {
                // ������� �� ��������� ��������
                return *this;
            }
            // ��������� �� �������� ������������ � ������������
            ++mMapElement;
            if (mMapElement == mTinyMap->second.content.cend()) {
                // ������� � ����. ������� �����
                ++mTinyMap;
                const auto dest = (mTinyMap == map->about().tinyMap.cend())
                    // ���� ������ ������� �����, ����� ������������ ���������
                    ? map->about().tinyMap.cbegin()
                    : mTinyMap;
                mMapElement = dest->second.content.cbegin();
            }
            return *this;
        }


    private:
        Vizi3DWorld* map;

        /**
        * ������� ������� ��������� �� �������� �����.
        */
        std::unordered_map< std::string, tinyMap_t >::const_iterator  mTinyMap;

        /**
        * ������� ������� ��������� �� ��������� ������� �����.
        * ������ ������� ����� ��������� � ���� ����� �������� � �������� ������
        * � ����� ����� (����������� �������� / ������).
        */
        content_t::const_iterator  mMapElement;

        /**
        * ������� ������� ��������� �� ����������� �������� (�� �������� ������ ��������).
        * ������������ ����� ���������� ��������� �� �������.
        * @see BitContent3D::ic()
        *//* - ���������� ��������� �� ��������� ������� �����.
        size_t elementContent;
        */
    };





public:
    inline Vizi3DWorld(
        Ogre::SceneManager* sm
    ) :
        sm( sm ),
        mAbout()
    {
    }



    virtual inline ~Vizi3DWorld() {
    }




    /**
    * ��������� ���� � ������.
    */
    void load( const std::string& name );



    /**
    * @return ���������� � �����.
    */
    inline const about_t& about() const {
        return mAbout;
    }



    /**
    * @return ������� ����� ��� nullptr, ���� �����������.
    */
    inline const tinyMap_t* tinyMap( const std::string& name ) const {
        auto fi = mAbout.tinyMap.find( name );
        return (fi == mAbout.tinyMap.end()) ? nullptr : &fi->second;
    }


    /**
    * @return ������� �����.
    *
    * @alias about().tinyMap
    */
    inline const std::unordered_map< std::string, tinyMap_t >&  tinyMap() const {
        return about().tinyMap;
    }



    /**
    * @return ���� ����� ��� nullptr, ���� �����������.
    */
    inline const noteElement_t* noteMap( signElement_t name ) const {
        auto fi = mAbout.noteMap.find( name );
        return (fi == mAbout.noteMap.end()) ? nullptr : &fi->second;
    }


    /**
    * @return ����� �����.
    *
    * @alias about().noteMap
    */
    inline const std::unordered_map< signElement_t, noteElement_t >&  noteMap() const {
        return about().noteMap;
    }



    /**
    * ���������� �� ������ ����. ��� �� ������������ �������.
    */
    void drawNext();




    /**
    * ��������� ��� ������ �����.
    */
    inline iterator cbegin() {
        // �� ������ ������� �����
        return iterator( *this, mAbout.tinyMap.cbegin() );
    }


    inline iterator cend() {
        // �� ��������� �������� �����
        return iterator( *this, mAbout.tinyMap.cend() );
    }




    /**
    * @return �� �������� �����, ������������� �� ���������.
    *         nullptr, ���� �������� �� ��������� �� �������.
    */
    inline std::shared_ptr< visualElement_t >  get( iterator& itr ) const {

        if ( itr.empty() ) {
            // �������� �� ��������� �� �� ���� ������� �����
            return std::shared_ptr< visualElement_t >();
        }

        const auto tinyMapItr = itr.tinyMap();
        const auto mapElementItr = itr.mapElement();

        const float scaleWorld = mAbout.scale;
        const signElement_t signElement = mapElementItr->first;

  
        assert( (mAbout.noteMap.find( signElement ) != mAbout.noteMap.cend())
            && "������ ������� ������ ���� ������������ � 'note'." );
        const noteElement_t& noteElement = mAbout.noteMap.at( signElement );

        const BitContent3D& bitContent = *mapElementItr->second;
        const d::coord3d_t& coord = tinyMapItr->second.coord;
        const d::sizeInt3d_t& size = tinyMapItr->second.size;
        const float scale = tinyMapItr->second.scale;

        return std::shared_ptr< visualElement_t >( new visualElement_t(
            scaleWorld,
            signElement,
            noteElement,
            bitContent,
            coord,
            size,
            scale
        ) );
    }






private:
    /**
    * @return �������� ������� �����. ��� �����. ���..
    */
    inline std::string parseTinyMapName( const std::string& keyName ) {
        std::vector< std::string >  r;
        boost::split( r, keyName, boost::is_any_of( "-" ), boost::token_compress_off );
        assert( (r.size() == 3) && "���� ������ �������� �� ��� ������." );
        const std::string name = r.back();
        assert( !name.empty() && "�������� ������� ����� �� ����� ���� ������." );

        return name;
    }






private:
    /**
    * ��������� ������� ����� �� JSON-���������.
    *
    * @param defaultScale ������� �� ���������. ����� ���������� ��� �������,
    *        ���� ��� �� �������������� �������, ��������� ��� ����.
    */
    static tinyMap_t parseTinyMap( json_value* ptr, float defaultScale );


    /**
    * ��������� ���������� �������� ����� �� JSON-���������.
    */
    static std::unordered_map< signElement_t, noteElement_t >  parseNoteMap( json_value* ptr );



    /**
    * �������������� ������� ����� ��� ������������ ����.
    */
    void prepareVisualBitImage();




private:
    Ogre::SceneManager* sm;


    /**
    * ��������� �� ����. �������������� ������� �����.
    * == cend(), ����� ��� �������� ����������.
    */
    iterator drawItr;


    /**
    * ���������� � ����.
    */
    about_t mAbout;

};

