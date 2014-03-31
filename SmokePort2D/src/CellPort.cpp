#include "CellPort.h"
#include "struct.h"


template < typename C >
CellPort< C >::CellPort(
    Ogre::SceneManager* sm,
    float visualScale,
    const sizeInt2d_t& size,
    float scale
) :
    Port( sm, visualScale ),
    N( (size_t)size.get<0>() ),
    M( (size_t)size.get<1>() ),
    NM( N * M ),
    scale( scale)
{
    assert( ( (size.get<0>() > 0) && (size.get<1>() > 0) ) && "������ ������ �������." );

    // ����� �������������� ������� ��� �������������
    content = new C[NM];
    for (size_t i = 0; i < NM; ++i) {
        // ����������� �� ��������� ��� 'C' ������ ���� ��������
        content[i] = C();
    }

    contentT = new C[NM];


    // ������ �����
    if ( sm ) {
        const float S = scale * visualScale;
        /* - �������������� ��������. �������� �� ����� �������.
        drawActor.Push(
            GridDrawMessage( sm, S, 10 ),
            Theron::Address::Null()
        );
        */
        const GridDrawMessage m( sm, S, 10 );
        CellDrawActor< GasFD >::gridDraw( m );
    }

    
    // ������� ���� OpenCL
#if 0
    // - �� ������� ���������� ��-�� ������ ���� ����������.
    //   �������� ������� �����.

    // ��������� �����
    const size_t GRID_LWS = 16;
    assert( ( (N >= GRID_LWS) && (M >= GRID_LWS) ) && "������� ����� ����� ��� �������������." );
    const size_t GRID_WORK_DIM = 2;
    const size_t GRID_GLOBAL_WORK_SIZE[] = { N, M };
    assert( ( (N % GRID_LWS == 0) && (M % GRID_LWS == 0) ) && "��������� ��������� ����� �����������." );
    const size_t GRID_LOCAL_WORK_SIZE[] = { GRID_LWS, GRID_LWS };
    // (!) �� ������ ���������� ������������ ��������������� ����� ��� OpenCL
    // ���������.

    cl_int status = CL_SUCCESS;

    // �������� ���������
    cl_platform_id platform;
    status = oclGetPlatformID( &platform );
    assert( status == CL_SUCCESS );

    // �������� ����������
    cl_uint numDevices;
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices );
    assert( status == CL_SUCCESS );
    devices = new cl_device_id[ numDevices ];
    assert( devices );
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, numDevices, devices, nullptr );
    assert( status == CL_SUCCESS );

    // �������� ����������
    const cl_uint deviceUsed = 0;
    const unsigned int endDevice = numDevices - 1;
    cl_device_id device = devices[deviceUsed];

    // ������ ��������
    cl_context_properties props[] = {
        CL_CONTEXT_PLATFORM,  (cl_context_properties)platform,  0
    };
    context = clCreateContext( props, 1, &devices[deviceUsed], nullptr, nullptr, &status );
    assert( status == CL_SUCCESS );

    // ������� ���������� � ��������� ����������
    oclPrintDevInfo( LOGBOTH, devices[deviceUsed] );

    // ������ ������� ������
    commandQueue = clCreateCommandQueue( context, device, 0, &status );
    assert( status == CL_SUCCESS );

    // �������� ����� ���������
    const std::string kernelName = "pulse";
    size_t programLength;
    const std::string pathAndName = PATH_OPENCL + "/" + kernelName + ".cl";

    // (!)
    // ����������� ��������� ���������� (���?). � �.�. ������ ������ �� ���������
    // hcl-������, ��������� � ����� �� ��� ����� �� ������ �� ��� �������.
    // ������ �������� ����������� ������ ��� ������ ���������.
    // @todo fine ���������� �� ����� �������.
    std::ostringstream header;
    header << "// " << GetTickCount();
    auto sourceCL = oclLoadProgSource(
        pathAndName.c_str(),
        header.str().c_str(),
        &programLength
    );
    assert( sourceCL );
    
    // ������ ���������
    program = clCreateProgramWithSource(
        context,
        1,
		(const char **) &sourceCL,
        &programLength,
        &status
    );
    assert( status == CL_SUCCESS );
    
    // ����������� ���������, ��������� �� �������� ��� ��������� ������
    std::ostringstream ss;
    ss << " -D N=" << N
       << " -D M=" << M
       << " -D NM=" << NM
       << "";
    std::string option = " \
        -I " + PATH_OPENCL + " \
        -cl-opt-disable \
        -Werror \
    " + ss.str();
    // @todo optimize ��������� > "-cl-fast-relaxed-math", "-cl-mad-enable", "-cl-opt-disable" and other
    // @see OpenCL spec. 5.4.3, http://www.khronos.org/registry/cl/sdk/1.0/docs/man/xhtml/clBuildProgram.html
    status = clBuildProgram(
        program, 0, nullptr,
        option.c_str(),
        nullptr, nullptr
    );
    if (status != CL_SUCCESS) {
        shrLogEx( LOGBOTH | ERRORMSG, status, STDERROR );
        oclLogBuildInfo( program, oclGetFirstDev( context ) );
        //oclLogPtx( program, oclGetFirstDev( context ), "CellPort.ptx" );
        oclLogPtx( program, oclGetFirstDev( context ), nullptr );
        throw "Failure build program.";
    }
    assert( status == CL_SUCCESS );

    // ������ ����������� ����.
    // �������: ���� ��������� ������ ���������� �� ����� �����.
    kernel = clCreateKernel( program, kernelName.c_str(), &status );
    assert( status == CL_SUCCESS );


    // �������������� ������ ��� OpenCL
    // @todo ...


    // ���� OpenCL �������, ������ ��� ���� ������������
#endif

}



template CellPort< GasFD >::CellPort(
    Ogre::SceneManager* sm,
    float visualScale,
    const sizeInt2d_t& size,
    float scale
);
