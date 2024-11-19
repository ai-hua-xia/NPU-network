cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(MyProject VERSION 1.0)

set(CMAKE_C_COMPILER "D:/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "D:/mingw64/bin/g++.exe")

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 指定库的路径
set(LIBRARY_PATH D:/mingw64)

# 添加包含目录
include_directories(
    ${LIBRARY_PATH}/include
    D:\\computer_network\\tools\\WpdPack\\Include  # 添加新的包含目录
)

# 添加库目录
link_directories(
    ${LIBRARY_PATH}/lib
    D:\\computer_network\\tools\\WpdPack\\Lib\\x64  # 添加新的库目录
)

# 添加 WinPcapSend 可执行文件，并包含相关的源文件
add_executable(WinPcapSend 
    D:/computer_network/three/WinPcap_ARP_Sender/WinPcap_ARP_Sender/ARP_Cache_Table.cpp
    D:/computer_network/three/WinPcap_ARP_Sender/WinPcap_ARP_Sender/Ethernet.cpp
    D:/computer_network/three/WinPcap_ARP_Sender/WinPcap_ARP_Sender/Global_Variable.cpp
    D:/computer_network/three/WinPcap_ARP_Sender/WinPcap_ARP_Sender/Main.cpp
    D:/computer_network/three/WinPcap_ARP_Sender/WinPcap_ARP_Sender/Network_ARP_recv.cpp
    D:/computer_network/three/WinPcap_ARP_Sender/WinPcap_ARP_Sender/Network_ARP_send.cpp
    D:/computer_network/three/WinPcap_ARP_Sender/WinPcap_ARP_Sender/Network_IPV4_send.cpp
)

# 链接库
target_link_libraries(WinPcapSend
    packet
    wpcap
    ws2_32  # 添加Ws2_32库
)

# 添加 WinPcapReceive 可执行文件，并包含相关的源文件
add_executable(WinPcapReceive 
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/ARP_Cache_Table.cpp
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/Ethernet_recv.cpp
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/Ethernet_send.cpp
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/Global_Variable.cpp
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/Main.cpp
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/Network_ARP_recv.cpp
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/Network_ARP_send.cpp
    D:/computer_network/three/WinPcap_ARP_recv/WinPcap_ARP_recv/Network_IPV4_recv.cpp
)

# 链接库
target_link_libraries(WinPcapReceive
    packet
    wpcap
    ws2_32  # 添加Ws2_32库
)