#include "PlatoTrend.h"



template< size_t N >
Ogre::MeshPtr PlatoTrend< N >::mesh(
    Ogre::SceneManager* sm
) const {

    // template N ����� �� ������ ����, �.�. ����������� ��� ���������
    // ���������� ����� ����� - �������� � � ���������.

    auto mesh = Ogre::MeshManager::getSingleton().createManual( source, "General" );
    auto sub = mesh->createSubMesh();

    // I ������� ����� ����� �� ����������� �����.
    // ����������� ����� ������� N �� ����������� �����, ��������
    // ������ ����� �� ����� ����� ���� ����������. ��� ����� @todo ����������
    // �� 4-� ������������� ����� (��. Plato()).
    /* @copy plane4Triangle3d_t - ������� ������:
    *   4---------1
    *   | \     / |
    *   |  \   /  |
    *   |    0    |
    *   |  /   \  |
    *   | /     \ |
    *   3---------2
    */
    /* @copy normal5Plane3d_t - ������� ��������:
    *   .---------.
    *   | \  1  / |
    *   |  \   /  |
    *   |4   0   2|
    *   |  /   \  |
    *   | /  3  \ |
    *   .---------.
    */

#if 0
// - �� ������������.
    const plane4Triangle3d_t plane = p.planeXY();
    //const normal5Plane3d_t normal = p.normal();

    const triangle3d_t triA = { plane[0], plane[1], plane[2] };
    const triangle3d_t triB = { plane[0], plane[2], plane[3] };
    const triangle3d_t triC = { plane[0], plane[3], plane[4] };
    const triangle3d_t triD = { plane[0], plane[4], plane[1] };
    const fourTriangle3d_t tri = { triA, triB, triC, triD };
#endif


    // ���������� �� ����� ����� ����� � ��������� ������� ���������� ������
    // ������, ����������� ������ ��������������� ����������� �����.
    // ����� ������������:
    //   ���������� (0; 0) ����� ~~ (aHalf; bHalf) hmImage
    //   ������ ������� ���� ����� ~~ ������� �������� ���� hmImage

    // @see ������ AHeightmap::lowMesh()
    const size_t a = hmSize.get<0>();
    const size_t b = hmSize.get<1>();
    assert( ((a < 10000) && (b < 10000)) && "������� ��� ����� ����� �����-�� ��������� �������..." );
    const size_t AB = a * b;
    const size_t vbufCount = 3 * 2 * AB;
    float* vertices = new float[vbufCount];
    // ����� ����� 'hmImage'
    const float aHalf = (float)a / 2.0f;
    const float bHalf = (float)b / 2.0f;
    std::pair< float, float >  xMinMax = std::make_pair( 0.0f, 0.0f );
    std::pair< float, float >  yMinMax = std::make_pair( 0.0f, 0.0f );
    std::pair< float, float >  hMinMax = std::make_pair( 0.0f, 0.0f );


#if 1
// - ��������. ��. ����. �������� ��� ����������.

    // ������������ �� ���� ������� �� ������� �����: ������� � �����
    // ������ - ������.
    /* @copy Plato::se
    *        0
    *   .---------.
    *   |         |
    *   |         |
    * 3 |         | 1
    *   |         |
    *   |         |
    *   |         |
    *   .---------.
    *        2
    */
    const float kx0 = p.sizeEdge( 0 ) / (float)a;
    const float ky1 = p.sizeEdge( 1 ) / (float)b;
    const float kx2 = p.sizeEdge( 2 ) / (float)a;
    const float ky3 = p.sizeEdge( 3 ) / (float)b;

    // @todo !? ������������� ���������� ������� ������ ��� �������� �����?
    const float kz = 20.0f;

    // ��� ���������� ������������� ������ ����� ������ ������ �� �������������
    // �������. ��� ����� �������� ������� �� ��������� � ����� �������� �
    // ����������� � ����������� �� ����, � ����� �������� 'hmImage' ��� ������.
    /* ��������� ����������:
    *   .---------.
    *   |    |    |
    *   |  3 |  0 |
    *   |____|____|  
    *   |    |    |
    *   |  2 |  1 |
    *   |    |    |
    *   .---------.
    *  ������ � ����������� ����� � ������ ����������� ��������� 0.
    */

    size_t t = 0;
    for (size_t j = 0; j < b; ++j) {
        const size_t ja = j * a;
        for (size_t i = 0; i < a; ++i) {
            /* - @todo ������������ �� ������������ � ������ �� ��������.
            // ����������, ������ ������������ ����������� �������
            triangle3d_t* pointer = nullptr;
            for (size_t u = 0; u < tri.static_size; ++u) {
            }
            assert( pointer && "������� ��������� �� ��������� �����." );
            */

            // ���������� ��������� ������������� ����� ����� ��
            // �������������� �����

            // (i; j) - ���������� �� ����� ����� 'hmImage' (��. ����. ����)
            // ��������� ���������� ������� �� ���������, � ������� ��� �����.
            const int ci = (int)i - (int)aHalf;
            const int cj = (int)j - (int)bHalf;
            size_t quadrans = 0;
            if ( (ci > 0) && (cj < 0) ) {
                quadrans = 1;
            } else if ( (ci < 0) && (cj < 0) ) {
                quadrans = 2;
            } else if ( (ci < 0) && (cj > 0) ) {
                quadrans = 3;
            }
            // ������������� ������������ � ����������� �� ���������
            // (��. ������� � ��������� ����)
            float kx = kx0;
            float ky = ky1;
            switch ( quadrans ) {
                case 1 : 
                    kx = kx2;
                    //ky = ky1; - ����������� ����, �� ��������.
                    break;
                case 2 : 
                    kx = kx2;
                    ky = ky3;
                    break;
                case 3 : 
                    //kx = kx0; - ����������� ����, �� ��������.
                    ky = ky3;
                    break;
            }

            // ������ ����� � ��������� [-1.0; 1.0]
            const float height = hmHeight.get()[ i + ja ];
            if ( !empty( height ) ) {
                bool test = true;
            }

            // ���������� ���������� ������ �� �����. ��������
            // ������� ���� ������ ���������� (0; 0; 0)
            const auto vc = Ogre::Vector3(
                ((float)i - aHalf) * kx,
                ((float)j - bHalf) * ky,
                height * kz
            );
            // ? ����������� ������ ���� �������� �������� ������� �����
            //const Ogre::Quaternion rotation = Ogre::Vector3::UNIT_Z.getRotationTo( normal );

            const Ogre::Vector3 nc = vc.normalisedCopy();
            vertices[t]     = vc.x;
            vertices[t + 1] = vc.y;
            vertices[t + 2] = vc.z;
            vertices[t + 3] = nc.x;
            vertices[t + 4] = nc.y;
            vertices[t + 5] = nc.z;

            if (xMinMax.first > vc.x) {
                xMinMax.first = vc.x;
            }
            if (xMinMax.second < vc.x) {
                xMinMax.second = vc.x;
            }
            if (yMinMax.first > vc.y) {
                yMinMax.first = vc.y;
            }
            if (yMinMax.second < vc.y) {
                yMinMax.second = vc.y;
            }
            if (hMinMax.first > vc.z) {
                hMinMax.first = vc.z;
            }
            if (hMinMax.second < vc.z) {
                hMinMax.second = vc.z;
            }

            t += 6;

        } // for (size_t i = 0; i < a; ++i)

    } // for (size_t j = 0; j < b; ++j)

#else



    const plane4Triangle3d_t pln = p.plane();
    // @todo !? ������������� ���������� ������� ������ ��� �������� �����?
    const float hz = 20.0f;

    size_t t = 0;
    for (size_t j = 0; j < b; ++j) {
        const size_t ja = j * a;
        for (size_t i = 0; i < a; ++i) {
            // ���������� ��������� ������������� ����� ����� ��
            // �������������� �����

            // ������ ����� � ��������� [-1.0; 1.0]
            const float height = hmHeight.get()[ i + ja ];
            if ( !empty( height ) ) {
                bool test = true;
            }

            // (i; j) - ���������� �� ����� ����� 'hmImage' (��. ����. ����)
            const int ci = (int)i - (int)aHalf;
            const int cj = (int)j - (int)bHalf;
            size_t vertexPlane = 0;
            if ( (ci > 0) && (cj < 0) ) {
                vertexPlane = 1;
            } else if ( (ci < 0) && (cj < 0) ) {
                vertexPlane = 2;
            } else if ( (ci < 0) && (cj > 0) ) {
                vertexPlane = 3;
            }
            const float kx = std::abs( pln[ vertexPlane ].get<0>() ) / aHalf;
            const float ky = std::abs( pln[ vertexPlane ].get<1>() ) / bHalf;
            const float kz = std::abs( pln[ vertexPlane ].get<2>() ) / hz;
            Ogre::Vector3 vc = Ogre::Vector3(
                (float)ci * kx,
                (float)cj * ky,
                height    * hz
            );

            // ? ����������� ������ ���� �������� �������� ������� �����
            //const Ogre::Quaternion rotation = Ogre::Vector3::UNIT_Z.getRotationTo( normal );

            const Ogre::Vector3 nc = vc.normalisedCopy();
            vertices[t]     = vc.x;
            vertices[t + 1] = vc.y;
            vertices[t + 2] = vc.z;
            vertices[t + 3] = nc.x;
            vertices[t + 4] = nc.y;
            vertices[t + 5] = nc.z;

            if (xMinMax.first > vc.x) {
                xMinMax.first = vc.x;
            }
            if (xMinMax.second < vc.x) {
                xMinMax.second = vc.x;
            }
            if (yMinMax.first > vc.y) {
                yMinMax.first = vc.y;
            }
            if (yMinMax.second < vc.y) {
                yMinMax.second = vc.y;
            }
            if (hMinMax.first > vc.z) {
                hMinMax.first = vc.z;
            }
            if (hMinMax.second < vc.z) {
                hMinMax.second = vc.z;
            }

            t += 6;

        } // for (size_t i = 0; i < a; ++i)

    } // for (size_t j = 0; j < b; ++j)

#endif



    // Create vertex data structure for vertices shared between submeshes
    mesh->sharedVertexData = new Ogre::VertexData();
    mesh->sharedVertexData->vertexCount = AB;
 
    // Create declaration (memory format) of vertex data
    Ogre::VertexDeclaration* decl = mesh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;

    // 1st buffer
    decl->addElement(
        0, offset,
        Ogre::VET_FLOAT3,
        Ogre::VES_POSITION
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    decl->addElement(
        0, offset,
        Ogre::VET_FLOAT3,
        Ogre::VES_NORMAL
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_FLOAT3 );
    // Allocate vertex buffer of the requested number of vertices (vertexCount)
    // and bytes per vertex (offset)
    Ogre::HardwareVertexBufferSharedPtr vbuf =
        Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
            offset,
            mesh->sharedVertexData->vertexCount,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
        );
    // Upload the vertex data to the card
    vbuf->writeData( 0, vbuf->getSizeInBytes(), vertices, true );
 
    // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    mesh->sharedVertexData->vertexBufferBinding->setBinding( 0, vbuf );
 
    
    // Vertex color
    size_t* color = new size_t[AB];
    for (size_t i = 0; i < AB; ++i) {
        //color[i] = 0xFFFFFFFF;
        color[i] = 0xFF0000FF;
    }

    // ������ �������
    std::vector< size_t >  index;
    size_t jf = 0;
    for (size_t j = 0; j < b - 1; ++j) {
        for (size_t i = 0; i < a; ++i) {
            if ((i - 1) >= 0) {
                index.push_back( jf - 1 );
                index.push_back( jf );
                index.push_back( a + jf );
            }
            if ((i + 1) <= a) {
                index.push_back( jf );
                index.push_back( a + 1 + jf );
                index.push_back( a + jf );                       
            }
            jf++;
        }
    }

    const size_t indexSize = index.size();
    size_t* face = new size_t[ indexSize ];
    for (size_t i = 0; i < indexSize; ++i) {
        face[i] = index[i];
    }
 
    // 2nd buffer
    offset = 0;
    decl->addElement(
        1, offset,
        Ogre::VET_COLOUR,
        Ogre::VES_DIFFUSE
    );
    offset += Ogre::VertexElement::getTypeSize( Ogre::VET_COLOUR );
    // Allocate vertex buffer of the requested number of vertices (vertexCount)
    // and bytes per vertex (offset)
    vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        offset,
        mesh->sharedVertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
    );
    // Upload the vertex data to the card
    vbuf->writeData( 0, vbuf->getSizeInBytes(), color, true );
 
    // Set vertex buffer binding so buffer 1 is bound to our colour buffer
    mesh->sharedVertexData->vertexBufferBinding->setBinding( 1, vbuf );
 
    /// Allocate index buffer of the requested number of vertices (ibufCount)
    Ogre::HardwareIndexBufferSharedPtr ibuf =
        Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
            Ogre::HardwareIndexBuffer::IT_32BIT,
            indexSize,
            Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY
        );
 
    // Upload the index data to the card
    ibuf->writeData( 0, ibuf->getSizeInBytes(), face, true );
 
    /// Set parameters of the submesh
    sub->useSharedVertices = true;
    sub->indexData->indexBuffer = ibuf;
    sub->indexData->indexCount = indexSize;
    sub->indexData->indexStart = 0;
 

    // Set bounding information (for culling)
    // (!) ���� ������� �� ������ ��� ������ �� ���������, ��� ����� ��������
    // ��� ������ ������� ������ �� ������� ��������� ������.
    // ������������� ������� � ������ ��� ������ ������
    const auto axisAlignedBox = Ogre::AxisAlignedBox(
        xMinMax.first,  yMinMax.first,  hMinMax.first,
        xMinMax.second, yMinMax.second, hMinMax.second
    );
    mesh->_setBounds( axisAlignedBox );
    const float radius = std::max( std::max( xMinMax.second, yMinMax.second ), hMinMax.second );
    mesh->_setBoundingSphereRadius( radius );


    // Notify - Mesh object that it has been loaded
    mesh->load();


    // ������� �� �����
    delete[] face;
    delete[] color;
    delete[] vertices;

    return mesh;
}



template
Ogre::MeshPtr PlatoTrend< 100 >::mesh(
    Ogre::SceneManager* sm
) const;
