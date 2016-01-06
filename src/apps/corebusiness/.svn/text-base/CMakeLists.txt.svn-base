##
 # ----------------------------------------------------------------------------------
 #     系统名称: 列控设备动态监测系统 
 #       文件名: CMakeLists.txt
 #
 #       版本号: 1.0
 #     创建日期: 2014年08月21日 09时55分26秒
 #
 #         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 #  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 #
 #
 #     修改历史:   none
 #
 # ----------------------------------------------------------------------------------
 ##

set(CORESRC
    CoreApp.cpp
    CoreSub.cpp
    main.cpp
    AtpNode.cpp
    CirNode.cpp
    GpsNode.cpp
    TcrNode.cpp
    AtpPlugin.cpp
    ParamsConfig.cpp
    PubService.cpp
    Publisher.cpp
    StepRunner.cpp
    Transmit.cpp
    BaliseParser.cpp
    TrainEncoder.cpp
    Utils.cpp
)

set(ATP200CSRC
    Atp200C.cpp
)

set(ATP200HSRC
    Atp200H.cpp
)

set(ATP300HSRC
    Atp300H.cpp
    RbcTriggerSets.cpp
)

set(ATP300TSRC
    Atp300T.cpp
    StmToJru.cpp
    Sub027JuridicalRec.cpp
    RbcTriggerSets.cpp
)

set(ATP300SSRC
    Atp300S.cpp
    Cpu2ToJru.cpp
    Sub027JuridicalRec.cpp
    RbcTriggerSets.cpp
)

set(LIB200C
    Atp200C
)

set(LIB200H
    Atp200H
)

set(LIB300H
    Atp300H
)

set(LIB300T
    Atp300T
)

set(LIB300S
    Atp300S
)

set(COREEXEC
    coreproc
)

add_library(${LIB200C} ${ATP200CSRC})
set_target_properties(${LIB200C} PROPERTIES VERSION 2.0.3.1013 SOVERSION 2)

add_library(${LIB200H} ${ATP200HSRC})
set_target_properties(${LIB200H} PROPERTIES VERSION 2.0.3.1013 SOVERSION 2)

add_library(${LIB300H} ${ATP300HSRC})
set_target_properties(${LIB300H} PROPERTIES VERSION 2.0.3.1013 SOVERSION 2)

add_library(${LIB300T} ${ATP300TSRC})
set_target_properties(${LIB300T} PROPERTIES VERSION 2.0.3.1030 SOVERSION 2)

add_library(${LIB300S} ${ATP300SSRC})
set_target_properties(${LIB300S} PROPERTIES VERSION 2.0.2.820 SOVERSION 2)

add_executable(${COREEXEC} ${CORESRC})
target_link_libraries(${COREEXEC} dl pthread rt tinyxml helium z)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    get_target_property(lib_location ${LIB200C} LOCATION)
    add_custom_command(TARGET ${LIB200C} POST_BUILD COMMAND ${CMAKE_STRIP} "${lib_location}")

    get_target_property(lib_location ${LIB200H} LOCATION)
    add_custom_command(TARGET ${LIB200H} POST_BUILD COMMAND ${CMAKE_STRIP} "${lib_location}")

    get_target_property(lib_location ${LIB300H} LOCATION)
    add_custom_command(TARGET ${LIB300H} POST_BUILD COMMAND ${CMAKE_STRIP} "${lib_location}")

    get_target_property(lib_location ${LIB300T} LOCATION)
    add_custom_command(TARGET ${LIB300T} POST_BUILD COMMAND ${CMAKE_STRIP} "${lib_location}")

    get_target_property(lib_location ${LIB300S} LOCATION)
    add_custom_command(TARGET ${LIB300S} POST_BUILD COMMAND ${CMAKE_STRIP} "${lib_location}")

    get_target_property(exec_location ${COREEXEC} LOCATION)
    add_custom_command(TARGET ${COREEXEC} POST_BUILD COMMAND ${CMAKE_STRIP} "${exec_location}")
endif()
