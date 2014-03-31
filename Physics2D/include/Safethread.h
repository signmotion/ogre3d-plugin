#pragma once

#include <assert.h>


/**
* ��������� ��� ���������� ���������������� �������.
*
* (!) ������ ����� ������, ������� �������� ��������� ������� ��������,
* ������ ���������� ����� lock() ��� waitFreeSetLock() � ������ ������
* � unlock() �� ����������.
*/
class Safethread {
public:
    inline Safethread() :
        busy( 0 )
    {
    }



    inline ~Safethread() {
    }



    /**
    * @return ����� �����? == !isFree()
    */
    virtual inline bool isBusy() const {
        return (busy > 0);
    }



    /**
    * @return ����� ��������? == !isBusy()
    */
    virtual inline bool isFree() const {
        return (busy == 0);
    }



    /**
    * ���, ���� ������ �� ������, ��� �� "��������" (����
    * �� ����� ������ ����� unlock() ).
    */
    virtual inline void waitFree() const {
        while (busy > 0)
        { };
        // @todo �������� �������� ��������, ����� ����������� ������.
    }



    /**
    * ���, ���� ������ �� ������, ��� �� "��������", �������������
    * ��� ���� ������ "�����".
    */
    virtual inline void waitFreeSetLock() const {
        waitFree();
        lock();
    }



// �������������� ��������� ����� ������� �������
public:

    /**
    * ������������� ������� ������ "�����".
    *
    * @see assert() � ���� ������
    */
    virtual inline void lock() const {
        ++busy;
        assert( (busy <= 1)
          && "������� ������������ �������������� ����������. ������. �����������. ��������� �����." );
    }



    virtual inline void unlock() const {
        --busy;
        assert( (busy >= 0) && "����� ������ ��� ������������ lock()." );
    }



private:
    /**
    * ����������� ������� "�����".
    * ����������� ��� �������, ����� ��������� �������� ��� ��������
    * ������������ ������������� �����������.
    *   0 - ����� "��������".
    *   1 - ����� "�����".
    *   ������ �������� - ������.
    */
    mutable int busy;
};

