// Вспомогательные методы.


/**
* Сравнение значений с заданной точностью.
*/
__constant const float PRECISION_POSITIVE = 1e-5f;
__constant const float PRECISION_NEGATIVE = -1e-5f;

inline bool zero(float a) {
    return  ( isless( a, PRECISION_POSITIVE ) && isgreater( a, PRECISION_NEGATIVE ) );
}

inline bool lZero(float a) {
    return isless( a, PRECISION_NEGATIVE );
}

inline bool leZero(float a) {
    return lZero( a ) || zero( a );
}

inline bool gZero(float a) {
    return isgreater( a, PRECISION_POSITIVE );
}

inline bool geZero(float a) {
    return gZero( a ) || zero( a );
}

inline bool equal(float a, float b) {
    return zero( a - b );
}

inline bool positiveOne(float a) {
    return zero( a - 1 );
}

inline bool negativeOne(float a) {
    return zero( a + 1 );
}

inline bool one(float a) {
    return positiveOne( a ) || negativeOne( a );
}





/**
* @return Признак, что указанная точка лежит у соотв. названию метода
* границы матрицы N x M. N, M задаются в pragma.hcl
*/
inline bool borderRight( const uint x, const uint z ) {
    return (x == (N - 1));
}

inline bool borderBottom( const uint x, const uint z ) {
    return (z == 0);
}

inline bool borderLeft( const uint x, const uint z ) {
    return (x == 0);
}

inline bool borderTop( const uint x, const uint z ) {
    return (z == (M - 1));
}


/**
* @return Признак, что указанная точка лежит внутри матрицы N x M.
* == !outsideMatrix()
*/
inline bool insideMatrix( const int x, const int z ) {
    return (
        (x >= 0) && (x < N)
     && (z >= 0) && (z < M)
    );
}



/**
* @return Признак, что указанная точка лежит вне матрицы N x M.
* == !insideMatrix()
*/
inline bool outsideMatrix( const int x, const int z ) {
    return (
        (z < 0) || (z >= M)
     || (x < 0) || (x >= N)
    );
}




/**
* @return Признак, что частица представляет собой вакуум.
*/
inline bool vacuum( const WParticle wp ) {
    return (wp.particle == VACUUM_PARTICLE);
}





/**
* @return Одномерный индекс указанной (относительно текущей) ячейки.
*         Если ячейка выходит за границы мира, возвращает -1.
*
* Расположение ячеек:
*   6   7   8   .   .   N
*   5   0   1
*   4   3   2
*   .
*   .
*   M
* 
*/
inline int near(
    const int x, const int z,
    const int dx, const int dz
) {
    return (
        ( outsideMatrix( x + dx, z + dz ) || outsideMatrix( x, z ) )
          ? -1
          : (x + dx) + (z + dz) * N
    );
}



/**
* @see near()
*/
inline int nearCell(
    const int x, const int z,
    const uint cell
) {
    switch ( cell ) {
        case 1:  return near( x, z, +1, 0 );
        case 2:  return near( x, z, +1, -1 );
        case 3:  return near( x, z, 0, -1 );
        case 4:  return near( x, z, -1, -1 );
        case 5:  return near( x, z, -1, 0 );
        case 6:  return near( x, z, -1, +1 );
        case 7:  return near( x, z, 0, +1 );
        case 8:  return near( x, z, +1, +1 );
        case 0:  return near( x, z, 0, 0 );
    }

    return -1;
}




/**
* @return Давление газа.
*
* p = D / M * R * T
*   где М - молярная масса вещества (воздух = 29 г/моль)
*   R - газовая постоянная
*   D - плотность
*   T - температура
*/
inline float calcGasPressure( const float density, const float molarMass, const float temperature ) {
    return
        zero( molarMass )
          ? 0.0f
          : density / molarMass * GAS_CONSTANT * (temperature - C0);
}



/**
* @return Плотность газа.
*
* D = p * M / R / T
* @see calcGasPressure()
*/
inline float calcGasDensity( const float pressure, const float molarMass, const float temperature ) {
    const float t = temperature - C0;
    return zero( t ) ? 0.0f : ( pressure * molarMass / GAS_CONSTANT / t );
}





/**
* Рассчитывает характеристики для полученной частицы.
*
* @see struct WParticle
*/
inline void calcCharacteristic(
    __global Particle* p,
    __global WParticle* wp,
    const uint i
) {
    if ( vacuum( wp[i] ) ) {
        // Для вакуума считать нечего
        return;
    }

    // Вычисляем характеристики
    
    // Скорость перемещения
    const float forceX = wp[i].forceX + DEFAULT_FORCE_X;
    const float forceZ = wp[i].forceZ + DEFAULT_FORCE_Z;
    const bool zeroMass = zero( wp[i].mass );
    const float accelerationX = zeroMass ? 0.0f : ( forceX / wp[i].mass );
    const float accelerationZ = zeroMass ? 0.0f : ( forceZ / wp[i].mass );
    wp[i].velocityX += accelerationX;
    wp[i].velocityZ += accelerationZ;
    
    // Направление движения
    const float2 velocity = (float2)( wp[i].velocityX, wp[i].velocityZ );
    wp[i].direction = transfer( velocity );
}
