#pragma once

#include <Ogre.h>
#include <boost/algorithm/string.hpp>
#include <boost/array.hpp>
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

    // @info ���������� ���� ��������� � ���� ��������, ����� �������������
    //       ��������: ����������� *����������* ������������ *������*.
    //       ����� Vizi3DWorld ���������� ����������� ��� ����� ������.

    /**
    * ������ ������� ������� �� ����� ������������ ���������� (� ���� �������)
    * ������.
    * � �� �� �����, ������� ������ �������� � ��������� � �������� ��������,
    * �.�. ������ ����� (�������) �� ������������� �������� � �������������
    * ������� ���� ����� �� �������.
    */
    typedef char sign_t;

    /**
    * ���������� �������.
    * ������������ ��� �������� �������� (������ - ��. ����. � 'sign_t') �
    * ������� 3D-����� �������, ��� ������� �����������.
    * �.�. ������ *����������* ������� *�������* �������� � ������ ����� ���
    * ����� ������� � ������� / 8 ���. ��� ������� 20 x 20 x 5 ��� ����� =
    * = 2000 / 8 = 250 ����.
    *
    * @todo ��� ���� � ������? ��, ������, ����� ���� *�����*. � ��� �����
    *       ������������� � ����� ������.
    */
    typedef std::unordered_map<
        std::string,
        // ������ � ���������, ����� �� ���������� ��� BitContent3D
        // ����������� �� ���������
        std::shared_ptr< BitContent3D >
    > element_t;

    typedef std::shared_ptr< element_t >  elementPtr_t;


    /**
    * �������, �������������� ��� ������.
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
    } visualContentElement_t;

    typedef std::shared_ptr< visualContentElement_t >  visualContentElementPtr_t;


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
    * ����� �������� �������� ����.
    */
    struct noteCommon_t {
        friend class note_t;

        /**
        * ����� ��������.
        * ������ ��� ����������� ����������� �������� �� �����.
        * �� �������� ������, �.�. ������� ������������ �� ����.
        * json-���������� � ����� ��������� ����� 256 ���������.
        */
        sign_t sign;

        /**
        * �������� ��������.
        * ������ ��������������� ��� ������� ����� �������� � �������.
        * ������������ � �������� ����� 'map'.
        */
        std::string name;

        /**
        * �������� ��������.
        */
        std::string description;

        inline bool operator==( const noteCommon_t& b ) {
            return (sign == b.sign)
                && (name == b.name)
                && (description == b.description);
        }
    };

    typedef std::shared_ptr< noteCommon_t >  noteCommonPtr_t;




    /**
    * �������� ���������� ������������� �������� ����.
    */
    struct notePhysics_t {
        /**
        * ����� ��������.
        */
        float mass;

        /**
        * ��������, �� �������� ������ �������.
        */
        std::string material;

        /**
        * ���������� ��������� ��������.
        *//* - @todo
        enum physicsState_e state;
        */

        /**
        * ���������� ���������� ��������.
        * ��������� ��������������� �. �� ����� �� ������, � �������
        * ��� �� ��������� %.
        *//* - @todo
        d::coord3d_t accurateCoord;
        */

        inline bool operator==( const notePhysics_t& b ) {
            return (
                (abs(mass - b.mass) < d::PRECISION)
             && (material == b.material)
            );
        }
    };

    typedef std::shared_ptr< notePhysics_t >  notePhysicsPtr_t;


    /**
    * �������� ����������� ������ �������� ����.
    */
    struct noteVisual_t {
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


        inline noteVisual_t(
        ) :
            type( TYPE_ELEMENT_UNDEFINED )
            // ��������� ���-�� ����� ���� ����������� �� ���������
        {
        }

        inline bool operator==( const noteVisual_t& b ) {
            return (
                (form == b.form)
             && (material == b.material)
             && (type == b.type)
            );
        }
    };

    typedef std::shared_ptr< noteVisual_t >  noteVisualPtr_t;



    /**
    * ����� ���������� � ����.
    */
    struct infoWorld_t {
        /**
        * �������� ����.
        */
        std::string name;

        /**
        * �������� ����.
        */
        std::string description;

        /**
        * ���� � ����������� ���� ��������.
        */
        //std::unordered_map< std::string, std::string >  pathToImage;
        std::vector< std::string >  pathToResource;

        inline bool operator==( const infoWorld_t& b ) {
            return (
                (name == b.name)
             && (description == b.description)
             && (pathToResource == b.pathToResource)
            );
        }
    };

    typedef std::shared_ptr< infoWorld_t >  infoWorldPtr_t;


    /**
    * ���������� �������. ���������.
    *
    * ������ ������������� ����������� ������, �� ������ ����������
    * ������ ���������� ��������������� ����������� ������� �������.
    * ������� ��� ����, ����� ����� ���� ���������� ����� / �������,
    * �������� ������, � �� ������ ��������� ���� ��� "����� �� �����".
    *
    * ������ ������� ������-����, �����-�������. ����� �� ������� �������,
    * �������� ������:
    *   0 - �������
    *   1 - ������
    *   2 - ��� (����� ������ �����)
    *   3 - �����
    *   4 - �����
    *   5 - ������
    */
    typedef boost::array< elementPtr_t, 6 >  content_t;


    /**
    * �� ���������, ������������� � �������.
    * ������ ��������� ������� ������� ������ ���� ������������ �����.
    * ������ ������� ���� ����� ��������� �������� � ���������� ������,
    * �� ������ ��������� - � ��� *������* ��������.
    */
    struct note_t {
        std::unordered_map< std::string, noteCommon_t  >  common;
        std::unordered_map< std::string, notePhysics_t >  physics;
        std::unordered_map< std::string, noteVisual_t  >  visual;

        inline bool operator==( const note_t& b ) {
            return (
                (common == b.common)
             && (physics == b.physics)
             && (visual == b.visual)
            );
        }

    };

    typedef std::shared_ptr< note_t >  notePtr_t;



    /**
    * ��������� � ����������� � ����.
    * �������������� ��� ������� ������.
    * ��� - ����� �������� �������������� ������� � ��������� ������������
    * (������� ������� �� ������). �� ����, ������� - ��� �� ���, ��:
    *   1. ������ �������� �������.
    *   2. ����� ��������� "����� ����������" - ��, ��� �� ��������������.
    */
    struct aboutArea_t {
        /**
        * �������� �������.
        * ������������ � ���� ������, �.�. ������� ����� ������������ �� ����.
        * ������ (���������� ��� ��������). ������ ���� ��������� ��������.
        */
        std::vector< std::string >  name;

        /**
        * �������� �������.
        * @see ����. � 'name'.
        */
        std::vector< std::string >  description;

        /**
        * ������ ��������, ������� �������� � ��� �������. ������� � �������.
        * ��� �������� � ��� (== � �������) ������� ������� ������ ����
        * ����������� �������.
        * ��� �������, ������� �� �������� ������ ��������, �������� �������
        * �� ����� ������.
        *//* - ��. @todo � 'tinyArea'.
        d::sizeInt3d_t sizeTinyArea;
        */

        /**
        * �� ���������, ������������� � �������.
        * ������ ��������� ������� ������� ������ ���� ������������ �����.
        * ������ ������� ���� ����� ��������� �������� � ���������� ������,
        * �� ������ �����������.
        *//* - �������� �� ���������.
        std::unordered_map< sign_t, noteCommon_t >   noteCommonMap;
        std::unordered_map< sign_t, noteVisual_t >   noteVisualMap;
        std::unordered_map< sign_t, notePhysics_t >  notePhysicsMap;
        */
        note_t note;

        /**
        * ���������� ������� ������������ ������������� ���� ��� - @todo -
        * ������ �������. ��� ��������� ���� �������� �� ����� ������.
        * ������� ��������������� ������������ ������ ������.
        */
        d::coordInt3d_t worldCoord;

        /**
        * ������� ������� ����, ���������������� ��� OZ: ������ �������
        * ������-����, �����-�������.
        * ����� �������������, ���� � ������� ���� ���������� (��. ����).
        *//* - @todo extend ������ ������ �����������: �������� � �����,
                     � �������� ���� ����������.
        std::shared_ptr< aboutArea_t >  tinyArea;
        */

        /**
        * ���������� �������.
        * ����� �������������, ���� � ������� ���� ������� ������� 'tinyArea'.
        */
        content_t content;

        /**
        * ���������� �������, �������������� ��� ������.
        * ������������ ��� ����������� ���������.
        * ����� ����� ������ ��� ������� �����������.
        */
        visualContentElementPtr_t visualContentElement;


        inline aboutArea_t(
        ) :
            //sizeTinyArea( d::ZEROINT3D ),
            worldCoord( d::ZERO3D )
            // ��������� ���-�� ����� ���� ����������� �� ���������
        {
        }


        /**
        * @return ������ ������� � �������. ��� ������� ���� - �����������
        *         �������. ��� ���������� ���� - ����������� �������.
        */
        inline d::sizeInt3d_t size() const {
            // ������ ������� ��������� �� ������� �� ������� ����������
            for (auto itr = content.cbegin(); itr != content.cend(); ++itr) {
                const auto element = *itr;
                if ( element ) {
                    const auto elementItr = element->cbegin();
                    return (elementItr == element->cend())
                        ? (d::sizeInt3d_t)d::ZEROINT3D
                        : elementItr->second->size();
                }
            }
            return (d::sizeInt3d_t)d::ZEROINT3D;
        }


        inline bool operator==( const aboutArea_t& b ) {
            return (
                (name == b.name)
             && (description == b.description)
             && (note == b.note)
             && ( (d::coordInt3d_t)worldCoord == (d::coordInt3d_t)b.worldCoord )
             && (content == b.content)
             && (visualContentElement = b.visualContentElement)
            );
        }
    };

    typedef std::shared_ptr< aboutArea_t >  aboutAreaPtr_t;


    /**
    * ����������� ����� ���� (������� �������).
    * ��. �������� ��������� ����� � helper::isc().
    */
    typedef boost::array< aboutAreaPtr_t, 27 >  workArea_t;


    /**
    * ���������� � ����:
    *   - ������ � ������� ��������� ������������
    *   - ����������� � ��� ������
    * 3D, ����� - 27 �����.
    */
    struct about_t {
        /**
        * ����� ����������.
        */
        infoWorld_t infoWorld;

        /**
        * ������� ������� (����������� ����� ����).
        * 
        * @see load()
        */
        workArea_t workArea;


        /**
        * @return ������ ���� � �������, � ������� � ������ ������
        *         ����� ��������. ������������ ��� ��� ������� �����������
        *         ������� (��� ������� - ����������� �������).
        */
        inline d::sizeInt3d_t size() const {
            if ( !workArea[0] ) {
                return d::ZEROINT3D;
            }
            return workArea[0]->worldCoord * 3;
        }

        inline bool operator==( const about_t& b ) {
            return (
                (infoWorld == b.infoWorld)
             && (workArea == b.workArea)
            );
        }
    };




    /**
    * ��������� ��� ��������� ���������� �� �������� ��� ��� ������������.
    * ����������� �������� ��� �������� ������� (��� ���������� ��� �����������
    * �������� �����).
    */
    struct vce_t {
        /**
        * ����� ��������.
        * ������� �������� ����� (��� char).
        */
        const sign_t sign;

        /**
        * ��� ����� ������������ �������.
        */
        const noteVisual_t& note;

        /**
        * ������� � ������� ����, ��� ������������� �������.
        */
        const BitContent3D& bitContent;

        /**
        * ������� ���������� � ������ ������� ����, �������
        * �������� ���� �������.
        */
        const d::coordInt3d_t& worldCoord;
        const d::sizeInt3d_t& size;

        inline vce_t(
            sign_t sign,
            const noteVisual_t& note,
            const BitContent3D& bitContent,
            const d::coordInt3d_t& worldCoord,
            const d::sizeInt3d_t& size
        ) :
            sign( sign ),
            note( note ),
            bitContent( bitContent ),
            worldCoord( worldCoord ),
            size( size )
        {
        }
    };

    typedef std::shared_ptr< vce_t >  vcePtr_t;



    /**
    * �������� �� ��������� ���� (��� �������� � 'about_t').
    * �������� ���������������:
    *   - �� 27 �������� 'about_t'
    *   - �� 6 �������� ���������� 'content_t'
    *   - �� ������� �������� ���������� 'element_t'
    */
    class WorldElementIterator :
        public std::iterator< std::forward_iterator_tag, Vizi3DWorld >
    {
    public:
        inline WorldElementIterator(
            about_t* world = nullptr,
            // ������� ����, 27 ������
            workArea_t::const_iterator  workArea = workArea_t::const_iterator(),
            // ���������� �������, 6 ������
            // �� ����� ������, ���� workArea ������ �� ���������
            content_t::const_iterator  content = content_t::const_iterator(),
            // ������� ������� ����������, ������� ������ ������
            // �� ����� ������, ���� workArea ������ �� ���������
            element_t::const_iterator  element = element_t::const_iterator()
        ) :
            world( world ),
            workArea( workArea ),
            content( content ),
            element( element )
        {
            if ( world && (workArea != world->workArea.cend()) ) {
                // ���� ��������������� �� ������ ������� ������� ����������
                // ��������� ������� �������?
                if (content != workArea->get()->content.cend()) {
                    content = workArea->get()->content.cbegin();
                    if (element != content->get()->cend()) {
                        element = content->get()->cbegin();
                    }
                }
            }
        }


        /**
        * @return ��� �� �������� �� ����� ������� (��� ������ ������) ���
        *         �������� �� ������������������ (world == nullptr).
        */
        inline bool empty() const {
            if ( !world ) {
                return true;
            }
            // ��������� 27-�� ��������
            for (auto itr = world->workArea.cbegin(); itr != world->workArea.cend(); ++itr) {
                if ( *itr ) {
                    return false;
                }
            }
            return true;
        }


        /**
        * @return �������� ��������� �� ��������� ������� ����. �.�.,
        *         ������ ������������ ��������� ���.
        */
        inline bool onEnd() const {
            return world && (workArea == world->workArea.cend());
        }


        inline WorldElementIterator& operator=( const WorldElementIterator& itr ) {
            world = itr.world;
            workArea = itr.workArea;
            content = itr.content;
            element = itr.element;
            return *this;
        }


        inline bool operator==( const WorldElementIterator& itr ) const {
            const bool ea = empty();
            const bool eb = itr.empty();
            if ( ea && !eb ) {
                return false;
            }
            if ( !ea && eb ) {
                return false;
            }
            if ( ea && eb ) {
                return true;
            }
            return (workArea == itr.workArea)
                && (content == itr.content)
                && (element == itr.element);
        }


        /**
        * @see nextContentElement()
        */
        inline WorldElementIterator& operator++() {
            nextContentElement();
            return *this;
        }


        /**
        * ��������� �������� �� ����. �������.
        *
        * @alias operator++
        */
        // ��������� �� ��������� (����� �������� �������)
        void nextContentElement() {
            if ( empty() ) {
                // ������� �� ��������� ��������
                return;
            }
            /* - ��������� ����.
            // �������� ��������������� �������?  27 ��.
            if (workArea == world->workArea.cend()) {
                // ���
                return;
            }
            // �������� ��������������� ����������?  6 ��.
            if (content == workArea->get()->content.cend()) {
                // ���
                return;
            }
            // �������� ��������������� ��������?  ~ ��.
            if (element == content->get()->cend()) {
                // ���
                return;
            }
            */

            // ��� ��������� �������?
            if (element == content->get()->cend()) {
                if (content == workArea->get()->content.cend()) {
                    // �� ���������� �����������. ��� �� ���� ��������?
                    if (workArea == world->workArea.cend()) {
                        // ��� ������� �����������
                        // ������ ������ ������
                    } else {
                        // ��� ���� ������������ �������
                        ++workArea;
                        content = workArea->get()->content.cbegin();
                    }
                    return;
                } else {
                    // ��� ���� ������������ ����������
                    ++content;
                    element = content->get()->cbegin();
                    return;
                }
            } else {
                // ������������ �� ����. ������� ����
                ++element;
            }
        }


    public:
        about_t* world;

        /**
        * �������� �� 27-�� *��������* ����.
        */
        workArea_t::const_iterator  workArea;

        /**
        * �������� �� 6 *�����������* ��������.
        * ������ ������� ����� ��������� 6-��� �������� - �� ������� (0)
        * �� �������� (5).
        */
        content_t::const_iterator  content;

        /**
        * �������� �� *���������* ������ ������� �����.
        * ������� �������� � ���� UID � �������� ������ � ����� UID
        * (����������� �������� / ������).
        */
        element_t::const_iterator  element;

        /**
        * ������� ������� ��������� �� ����������� �������� (�� �������� ������ ��������).
        * ������������ ����� ���������� ��������� �� �������.
        * @see BitContent3D::ic()
        *//* - ���������� ��������� �� ��������� ������� �����.
        size_t elementContent;
        */
    };





public:
    /**
    * ��������� ������� ���� �� ����������� ������������.
    * ���������� � ���� �������� � ������, ������� ������������ �� ������.
    * ����� ������ ������� � ������� ���� � ����� � ������� Z->Y->X (��������
    * ����� - ��� ���������� ������� � ����).
    * ������ ���� ������������ �� ������� �����, ��� �������� �������.
    * ������� ���� �������� - ���������.
    *
    * @param sourceWorld �����, ��� �������� ���.
    * @param sm ��� ������������ ����. ��������� ��������� nullptr, �����
    *        �� ����������� ���. ������������.
    */
    inline Vizi3DWorld(
        const std::string& sourceWorld,
        Ogre::SceneManager* sm
    ) :
        sourceWorld( sourceWorld ),
        sm( sm ),
        mAbout()
    {
        //assert( sm ); - ���������.

        // �������� ����� ���������� � ���� �����, ����� ����� load() ��������
        // ����������� ������
        mAbout.infoWorld = loadInfo( sourceWorld );

        /* - ������� ������������ �� ���������� ��� ������ �������.
        // ��������� ��������� ��������� ���� �������
        loadResource( mAbout.infoWorld.pathToImageForm );
        loadResource( mAbout.infoWorld.pathToImageMaterial );
        */
    }



    virtual inline ~Vizi3DWorld() {
    }




    /**
    * ��������� ������� ���� �� ���������� ������������.
    * ����� JSON � ��������� '-' � ����� �� ��������� � ��������.
    *
    * @see about_t
    */
    void load( d::coordInt3d_t observer );



    /**
    * @return ���������� � �����.
    */
    inline const about_t& about() const {
        return mAbout;
    }


    inline about_t& about() {
        return mAbout;
    }



    /**
    * @return ���������� ����������� ������� ��������.
    */
    inline const size_t countWorkArea() const {
        // ��������� ��� 27-�� ��������
        /*
        size_t n = 0;
        for (auto itr = mAbout.workArea.cbegin(); itr != mAbout.workArea.cend(); ++itr) {
            if ( *itr ) {
                ++n;
            }
        }
        */
        /*
        return ( mAbout.workArea.static_size - std::count(
            mAbout.workArea.cbegin(), mAbout.workArea.cend(), nullptr
        ) );
        */
        return std::count_if(
            mAbout.workArea.cbegin(),
            mAbout.workArea.cend(),
            [] ( const aboutAreaPtr_t& aa ) {
                return (aa != nullptr);
            }
        );
    }



    /**
    * @return ���������� ������ ���������� � ������� �������.
    */
    inline const size_t countContent( const aboutArea_t& aa ) const {
        // ��������� ��� 6-�� ������ ����������
        /*
        return ( aa.content.static_size - std::count(
            aa.content.cbegin(), aa.content.cend(), nullptr
        ) );
        */
        return std::count_if(
            aa.content.cbegin(),
            aa.content.cend(),
            [] ( const elementPtr_t& e ) {
                return (e != nullptr);
            }
        );
    }



    /**
    * ���������� �� ������ ����. ��� �� ������������ �������.
    */
    void drawNext();




    /**
    * ��������� ��� ����������������� ������ ��������� ����.
    */
    inline WorldElementIterator cbegin() {
        // �� ������ ������� ������� ���������� ������ ������� �������
        return WorldElementIterator( &mAbout, mAbout.workArea.cbegin() );
    }


    inline WorldElementIterator cend() {
        // �� ��������� ��������� ���������� ���������� ��������� ������� �������
        return WorldElementIterator( &mAbout, mAbout.workArea.cend() );
    }




    /**
    * @return �� �������� ����.
    *         nullptr, ���� �������� �� ��������� �� �������.
    */
    inline vcePtr_t get( WorldElementIterator& itr ) const {

        return vcePtr_t();

        /*
        if ( itr.empty() ) {
            // �������� �� ��������� �� �� ���� ������� �����
            return vePtr_t();
        }

        const auto tinyMapItr = itr.tinyMap();
        const auto mapElementItr = itr.mapElement();

        const float scaleWorld = mAbout.scale;
        const sign_t sign = mapElementItr->first;

  
        assert( (mAbout.noteVisualMap.find( sign ) != mAbout.noteVisualMap.cend())
            && "������ ������� ������ ���� ������������ � 'note'." );
        const noteVisual_t& note = mAbout.noteVisualMap.at( sign );

        const BitContent3D& bitContent = *mapElementItr->second;
        const d::coord3d_t& coord = tinyMapItr->second.coord;
        const d::sizeInt3d_t& size = tinyMapItr->second.size;
        const float scale = tinyMapItr->second.scale;

        return vePtr_t( new visualElement_t(
            scaleWorld,
            sign,
            note,
            bitContent,
            coord,
            size,
            scale
        ) );
        */
    }






    inline bool operator==( const Vizi3DWorld& b ) {
        return ( about() == b.about() );
    }






private:
    /**
    * ��������� ����� ���������� � ����.
    * ���������� ������ ���� ������������� � ����� 'about.json'.
    */
    static infoWorld_t loadInfo( const std::string& folder );


    /**
    * ��������� � Ogre3D �������.
    * ������� ����� ���������� �� ��������� ������.
    * ����� � ��������� '-' ������������.
    *
    * @param depthFolder true, ���� ����� ���������� ��� ��������� �����
    */
    static void loadResource( const std::string& pathToResource, bool depthFolder );


    /**
    * ��������� ������� ����.
    * ����������� 'area'.
    *
    * @param location ������� ���� ���������� ���������������� �����������.
    *
    * @see about_t
    */
    void loadArea( aboutAreaPtr_t area, const d::coordInt3d_t& location );

    
    /**
    * ��������� ������� ���� ������� �� ���������� �����.
    */
    static void loadArea( aboutAreaPtr_t area, const std::string& fileName );




    /**
    * ��������� ������� ����� �� JSON-��������� � *����������* ������
    * � 'area'.
    *
    * @param ptr ��������� �� ������ ������� ������ ����������.
    */
    static void fillContentArea( elementPtr_t element, const note_t& note, json_value* ptr );


    /**
    * ��������� ���������� �������� ����� �� JSON-���������.
    */
    static void fillNoteMap( std::unordered_map< std::string, noteCommon_t  >&  r,  json_value* ptr );
    static void fillNoteMap( std::unordered_map< std::string, notePhysics_t >&  r,  json_value* ptr );
    static void fillNoteMap( std::unordered_map< std::string, noteVisual_t  >&  r,  json_value* ptr );


    /**
    * @return �������� �������� �� ��� ����� ��� ������ ������, ����
    *         ������� �� ������.
    */
    static inline std::string nameElement( sign_t sign, const note_t& note ) {
        const auto itr = std::find_if(
            note.common.cbegin(),
            note.common.cend(),
            [ sign ] ( const std::pair< std::string, noteCommon_t >&  el ) {
                return (el.second.sign == sign);
            }
        );
        return (itr == note.common.cend()) ? "" : itr->second.name;
    }



    /**
    * @return ������ ��������, �������� ������������-��������.
    *
    * � ���������� ����� ����������� ������ ������� ������� ��������, �����
    * ���������� ����� �������������� ���������.
    */
    static inline std::string prepareBitLine( const char* line ) {
        std::string r = "";
        std::string t = "_";
        for (size_t i = 0; line[i] != 0; i += 2) {
            t[0] = line[i];
            r += t;
            // ������ ����� ���� ���� ��� ������ ����� ��������� � ������ ��������
            if (line[ i + 1 ] == 0) {
                break;
            }
        }
        return r;
    }



    /**
    * �������������� ������� ����� ��� ������������ ����.
    */
    static void prepareVisualBitImage( aboutAreaPtr_t area );




private:
    /**
    * �������� � ����������� � ���� (����� � �������).
    */
    std::string sourceWorld;


    /**
    * ���������������� ���������� � ����.
    */
    about_t mAbout;


    /**
    * ������������ ��� ������������.
    */
    Ogre::SceneManager* sm;

    /**
    * ��������� �� ����. �������������� ������� �����.
    * == cend(), ����� ��� �������� ����������.
    */
    WorldElementIterator drawItr;

};
