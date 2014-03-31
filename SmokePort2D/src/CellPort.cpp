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
    assert( ( (size.get<0>() > 0) && (size.get<1>() > 0) ) && "Задана пустая матрица." );

    // Сразу подготавливаем матрицы для моделирования
    content = new C[NM];
    for (size_t i = 0; i < NM; ++i) {
        // Конструктор по умолчанию для 'C' должен быть определён
        content[i] = C();
    }

    contentT = new C[NM];


    // Рисуем сетку
    if ( sm ) {
        const float S = scale * visualScale;
        /* - Непредсказуемо вылетает. Заменено на вызов статики.
        drawActor.Push(
            GridDrawMessage( sm, S, 10 ),
            Theron::Address::Null()
        );
        */
        const GridDrawMessage m( sm, S, 10 );
        CellDrawActor< GasFD >::gridDraw( m );
    }

    
    // Готовим ядро OpenCL
#if 0
    // - Не удалось подключить из-за разных сред выполнения.
    //   Работаем простым кодом.

    // Параметры сетки
    const size_t GRID_LWS = 16;
    assert( ( (N >= GRID_LWS) && (M >= GRID_LWS) ) && "Слишком малая сетка для моделирования." );
    const size_t GRID_WORK_DIM = 2;
    const size_t GRID_GLOBAL_WORK_SIZE[] = { N, M };
    assert( ( (N % GRID_LWS == 0) && (M % GRID_LWS == 0) ) && "Указанная кратность сетки недопустима." );
    const size_t GRID_LOCAL_WORK_SIZE[] = { GRID_LWS, GRID_LWS };
    // (!) Не каждая видеокарта поддерживает установливаемые здесь для OpenCL
    // параметры.

    cl_int status = CL_SUCCESS;

    // Получаем платформу
    cl_platform_id platform;
    status = oclGetPlatformID( &platform );
    assert( status == CL_SUCCESS );

    // Получаем устройства
    cl_uint numDevices;
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 0, nullptr, &numDevices );
    assert( status == CL_SUCCESS );
    devices = new cl_device_id[ numDevices ];
    assert( devices );
    status = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, numDevices, devices, nullptr );
    assert( status == CL_SUCCESS );

    // Выбираем устройство
    const cl_uint deviceUsed = 0;
    const unsigned int endDevice = numDevices - 1;
    cl_device_id device = devices[deviceUsed];

    // Создаём контекст
    cl_context_properties props[] = {
        CL_CONTEXT_PLATFORM,  (cl_context_properties)platform,  0
    };
    context = clCreateContext( props, 1, &devices[deviceUsed], nullptr, nullptr, &status );
    assert( status == CL_SUCCESS );

    // Покажем информацию о выбранном устройстве
    oclPrintDevInfo( LOGBOTH, devices[deviceUsed] );

    // Строим очередь команд
    commandQueue = clCreateCommandQueue( context, device, 0, &status );
    assert( status == CL_SUCCESS );

    // Получаем текст программы
    const std::string kernelName = "pulse";
    size_t programLength;
    const std::string pathAndName = PATH_OPENCL + "/" + kernelName + ".cl";

    // (!)
    // Построенная программа кешируется (кем?). И т.к. проект разбит на множество
    // hcl-файлов, изменение в одном из них никак не влияет на код целиком.
    // Сейчас решается добавлением каждый раз нового заголовка.
    // @todo fine Избавиться от этого костыля.
    std::ostringstream header;
    header << "// " << GetTickCount();
    auto sourceCL = oclLoadProgSource(
        pathAndName.c_str(),
        header.str().c_str(),
        &programLength
    );
    assert( sourceCL );
    
    // Создаём программу
    program = clCreateProgramWithSource(
        context,
        1,
		(const char **) &sourceCL,
        &programLength,
        &status
    );
    assert( status == CL_SUCCESS );
    
    // Компилируем программу, передавая ей значения для ускорения работы
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
    // @todo optimize Параметры > "-cl-fast-relaxed-math", "-cl-mad-enable", "-cl-opt-disable" and other
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

    // Создаём исполняемое ядро.
    // Принято: Ядро программы всегда называется по имени файла.
    kernel = clCreateKernel( program, kernelName.c_str(), &status );
    assert( status == CL_SUCCESS );


    // Подготавливаем данные для OpenCL
    // @todo ...


    // Ядро OpenCL создано, данные для него подготовлены
#endif

}



template CellPort< GasFD >::CellPort(
    Ogre::SceneManager* sm,
    float visualScale,
    const sizeInt2d_t& size,
    float scale
);
