// ��������������� ������.


/**
* ��������� �������� � �������� ���������.
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
* @return �������, ��� ��������� ����� ����� � �����. �������� ������
* ������� ������� N x M. N, M �������� � pragma.hcl
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
* @return �������, ��� ��������� ����� ����� ������ ������� N x M.
* == !outsideMatrix()
*/
inline bool insideMatrix( const int x, const int z ) {
    return (
        (x >= 0) && (x < N)
     && (z >= 0) && (z < M)
    );
}



/**
* @return �������, ��� ��������� ����� ����� ��� ������� N x M.
* == !insideMatrix()
*/
inline bool outsideMatrix( const int x, const int z ) {
    return (
        (z < 0) || (z >= M)
     || (x < 0) || (x >= N)
    );
}




/**
* @return �������, ��� ������� ������������ ����� ������.
*/
inline bool vacuum( const WParticle wp ) {
    return (wp.particle == VACUUM_PARTICLE);
}





/**
* @return ���������� ������ ��������� (������������ �������) ������.
*         ���� ������ ������� �� ������� ����, ���������� -1.
*
* ������������ �����:
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
* @return �������� ����.
*
* p = D / M * R * T
*   ��� � - �������� ����� �������� (������ = 29 �/����)
*   R - ������� ����������
*   D - ���������
*   T - �����������
*/
inline float calcGasPressure( const float density, const float molarMass, const float temperature ) {
    return
        zero( molarMass )
          ? 0.0f
          : density / molarMass * GAS_CONSTANT * (temperature - C0);
}



/**
* @return ��������� ����.
*
* D = p * M / R / T
* @see calcGasPressure()
*/
inline float calcGasDensity( const float pressure, const float molarMass, const float temperature ) {
    const float t = temperature - C0;
    return zero( t ) ? 0.0f : ( pressure * molarMass / GAS_CONSTANT / t );
}





/**
* ������������ �������������� ��� ���������� �������.
*
* @see struct WParticle
*/
inline void calcCharacteristic(
    __global Particle* p,
    __global WParticle* wp,
    const uint i
) {
    if ( vacuum( wp[i] ) ) {
        // ��� ������� ������� ������
        return;
    }

    // ��������� ��������������
    
    // �������� �����������
    const float forceX = wp[i].forceX + DEFAULT_FORCE_X;
    const float forceZ = wp[i].forceZ + DEFAULT_FORCE_Z;
    const bool zeroMass = zero( wp[i].mass );
    const float accelerationX = zeroMass ? 0.0f : ( forceX / wp[i].mass );
    const float accelerationZ = zeroMass ? 0.0f : ( forceZ / wp[i].mass );
    wp[i].velocityX += accelerationX;
    wp[i].velocityZ += accelerationZ;
    
    // ����������� ��������
    const float2 velocity = (float2)( wp[i].velocityX, wp[i].velocityZ );
    wp[i].direction = transfer( velocity );
}
