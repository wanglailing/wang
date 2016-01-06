##
 # ----------------------------------------------------------------------------------
 #     系统名称: 列控设备动态监测系统 
 #       文件名: CMakeLists.txt
 #
 #       版本号: 1.0
 #     创建日期: 2014年08月20日 16时40分26秒
 #
 #         作者: lixianghui (rocklee), lxkjlxh@yahoo.com.cn
 #  Copyright (c),  2006 - 2014, 河南蓝信科技有限公司 版权所有.
 #
 #
 #     修改历史:   none
 #
 # ----------------------------------------------------------------------------------
 ##

set(SRC
    BoardBusOp.cpp
    DataHub.cpp
    DataSwitchApp.cpp
    main.cpp
    JruService.cpp
    SwitchService.cpp
)

set(EXEC
    datahub
)

add_executable(${EXEC} ${SRC})
target_link_libraries(${EXEC} pthread rt tinyxml helium z)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    get_target_property(exec_location ${EXEC} LOCATION)
    add_custom_command(TARGET ${EXEC} POST_BUILD COMMAND ${CMAKE_STRIP} "${exec_location}")
endif()
