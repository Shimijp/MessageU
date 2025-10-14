cmake_minimum_required(VERSION 3.29)
project(Client LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mrdrnd")


add_executable(Client
        ReadPort.cpp
        MUClient.cpp
        ReadInfo.cpp
        AESWrapper.cpp
        Base64Wrapper.cpp
        RSAWrapper.cpp
        ReadPort.h
        ClientD.h
        ReadInfo.h
        AESWrapper.h
        Base64Wrapper.h
        RSAWrapper.h
        ClientD.cpp
        RWClientD.cpp
        RWClientD.h
        Register.cpp
        Register.h
        header.cpp
        header.h
        header.cpp
        utils.h
)

# ---- Include Paths ----

target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


target_include_directories(Client PRIVATE "C:/msys64/mingw64/include/cryptopp")

# ---- Libraries ----
target_link_libraries(Client PRIVATE ws2_32)

target_link_libraries(Client PRIVATE "C:/msys64/mingw64/lib/libcryptopp.a")

if (MSVC)
    target_compile_options(Client PRIVATE /W4)
else()
    target_compile_options(Client PRIVATE -Wall -Wextra -Wpedantic)
endif()
