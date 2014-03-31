#pragma once

#include <memory>
#include <unordered_map>
#include <Newton.h>
#include <boost/functional/hash.hpp>


/**
* Управляет созданием физических форм сущностей. Не создаёт дубликаты форм.
* Введён, т.к. NewtonDymnamics позволяет работать с копиями одинаковых форм
* для разных объектов. Оптимизация, да.
*/
class CollisionManager {
public:
    enum shape_e {
        SHAPE_BOX = 101,
        SHAPE_CAPSULE,
        SHAPE_SPHERE
    };


    /**
    * Объединение исп. др. методами для передачи параметров.
    */
    union shape_u {
        NewtonCollisionInfoRecord::NewtonBoxParam box;
        NewtonCollisionInfoRecord::NewtonCapsuleParam capsule;
        NewtonCollisionInfoRecord::NewtonSphereParam sphere;
    };




protected:
    inline CollisionManager() {
    }


public:
    static inline CollisionManager* singleton() {
        if ( instance ) {
            return instance;
        }
        instance = new CollisionManager();
        return instance;
    }



    virtual inline ~CollisionManager() {
        std::cout << "~CollisionManager()" << std::endl;
    }



    /**
    * @return Количество хранимых форм.
    */
    inline size_t count() const {
        return nc.size();
    }




    /**
    * Возвращается ссылка, т.к. при попытке обернуть в std::shared_ptr()
    * деструкторы выбрасывают исключения.
    *
    * (!) Для каждой типовой формы подаётся свой уникальный 'seed'.
    */
    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonBoxParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        const auto kf = search( SHAPE_BOX, p.m_x, p.m_y, p.m_z );
        if ( kf.second) {
            return kf.second;
        }
        nc[ kf.first ] = NewtonCreateBox(
            nw,  p.m_x, p.m_y, p.m_z,  shapeID, offsetMatrix
        );
        return nc[ kf.first ];
    }


    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonCapsuleParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        // Учитывается только 'm_r0', см. NewtonCreateCapsule()
        assert( (p.m_r0 == p.m_r1) && "Капсула создаётся с одним радиусом, сделаем их равными, чтобы позже спать спокойно." );
        assert( (p.m_r0 > 0.0f) && "У капсулы должен быть радиус." );
        assert( (p.m_height > 0.0f) && "Капсула без высоты, так?" );
        const auto kf = search( SHAPE_CAPSULE, p.m_r0, p.m_r0, p.m_height );
        if ( kf.second) {
            return kf.second;
        }
        nc[ kf.first ] = NewtonCreateCapsule(
            nw,  p.m_r0, p.m_height,  shapeID, offsetMatrix
        );
        return nc[ kf.first ];
    }


    inline NewtonCollision* collision(
        const NewtonWorld* const nw,
        const NewtonCollisionInfoRecord::NewtonSphereParam& p,
        int shapeID = 0,
        const dFloat* const offsetMatrix = nullptr
    ) {
        assert( ((p.m_r0 > 0.0f) && (p.m_r1 > 0.0f) && (p.m_r2 > 0.0f)) && "Сфера. Нужны все три радиуса.." );
        const auto kf = search( SHAPE_SPHERE, p.m_r0, p.m_r1, p.m_r2 );
        if ( kf.second) {
            return kf.second;
        }
        nc[ kf.first ] = NewtonCreateSphere(
            nw,  p.m_r0, p.m_r1, p.m_r2,  shapeID, offsetMatrix
        );
        return nc[ kf.first ];
    }







private:
    /**
    * @return Вычисленный ключ и найденную форму. Если формы нет - nullptr.
    *
    * (!) Для каждой типовой формы должен подаваться свой уникальный 'seed'.
    */
    inline std::pair< size_t,  NewtonCollision* >  search(
        shape_e shape, dFloat p1, dFloat p2, dFloat p3
    ) {
        // Параметры представляют собой размеры форм, проверяем их
        assert(p1 > 0.0f);
        assert(p2 > 0.0f);
        assert(p3 > 0.0f);

        size_t key = (size_t)shape;
        boost::hash_combine( key, p1 );
        boost::hash_combine( key, p2 );
        boost::hash_combine( key, p3 );
        const auto sf = nc.find( key );
        const auto form = (sf == nc.cend()) ? nullptr : sf->second;
        return std::make_pair( key, form ) ;
    }




protected:
    /*
    * Карта, где хранятся разные формы.
    * Для вычисления хеш-ключа работаем с
    * http://www.boost.org/doc/libs/1_48_0/doc/html/hash/combine.html
    */
    std::unordered_map< size_t,  NewtonCollision* >  nc;



    static CollisionManager* instance;

};

