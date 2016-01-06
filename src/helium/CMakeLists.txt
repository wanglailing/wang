##
 # ----------------------------------------------------------------------------------
 #     系统名称: 列控设备动态监测系统 
 #       文件名: CMakeLists.txt
 #
 #       版本号: 1.0
 #     创建日期: 2014年08月20日 10时13分40秒
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
    App.cpp
    AppConfig.cpp
    Base64Coder.cpp
    Buffer.cpp
    CharStuff.cpp
    DataCoder.cpp
    DataSubService.cpp
    FdSet.cpp
    GprsPackage.cpp
    InetAddress.cpp
    Logger.cpp
    LoggerImplement.cpp
    MemDump.cpp
    Reactor.cpp
    PacketManager.cpp
    SlaveDevManager.cpp
    Socket.cpp
    StdoutLogger.cpp
    RemoteLogger.cpp
    Packet.cpp
    TcpSocket.cpp
    Thread.cpp
    TimerQueue.cpp
    TimeVal.cpp
    UdpSocket.cpp
    UnixAddress.cpp
    ZlibStream.cpp
)

set(LIB
    helium
)

add_library(${LIB} ${SRC})
set_target_properties(${LIB} PROPERTIES VERSION 2.0.3.1013 SOVERSION 2)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    get_target_property(lib_location ${LIB} LOCATION)
    add_custom_command(TARGET ${LIB} POST_BUILD COMMAND ${CMAKE_STRIP} "${lib_location}")
endif()
