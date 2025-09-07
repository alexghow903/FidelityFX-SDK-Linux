	# This file is part of the FidelityFX SDK.
#
# Copyright (C) 2024 Advanced Micro Devices, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 3.23)

if(POLICY CMP0147)
	# parallel custom command builds (used for shader compilation)
	cmake_policy(SET CMP0147 NEW)
endif()

# Clean a bit before
unset(FFX_GDK CACHE)

message(STATUS "FFX_API_BACKEND is ${FFX_API_BACKEND}")

if(FFX_API_BACKEND STREQUAL VK)
	set(FFX_CUSTOM_API OFF)
else()
	set(FFX_CUSTOM_API ON)
endif()

# Set configuration types based on what we are building for
if (NOT FFX_CUSTOM_API)

	# Pull in tool chain definitions
	include(toolchain.cmake)

	# Queue up DX12/VK solution for building
	message(STATUS "Building FidelityFX SDK with VK backend")
endif()

# Pre-compile shaders
set(FFX_AUTO_COMPILE_SHADERS ON CACHE BOOL "Compile shaders automatically as a prebuild step.")

#if(CMAKE_GENERATOR STREQUAL "Ninja")
#    set(USE_DEPFILE TRUE)
#else()
#    set(USE_DEPFILE FALSE)
#endif()

# Set configuration types based on what we are building for
if (NOT FFX_CUSTOM_API)

	# Embed PDBs in the debug versions of the libs
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Z7 /Od")
    
	# Write debug, profile, and release versions of the static libs to the /bin folder as they are uniquely named	
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)


	if (FFX_BUILD_AS_DLL)
		# Write both debug and release versions of the dlls to the /bin folder as they are uniquely named
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
        set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS FFX_BUILD_AS_DLL)
	endif()
else()

	# Ouput the static libs to the /bin folder as they are uniquely named
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT}/ffx_sdk)
	foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
		string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
		set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BIN_OUTPUT}/ffx_sdk/ )
		if (FFX_BUILD_AS_DLL)
			# Write both debug and release versions of the dlls to the /bin folder as they are uniquely named
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
			set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_HOME_DIRECTORY}/bin/ffx_sdk/)
		endif()
	endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

endif()

if (FFX_FSR3)
	option(FFX_FSR3UPSCALER "Build FFX FSR3 Upscaler API" ON)
	option(FFX_FI "Build FFX Frame Interpolation API" ON)
	option(FFX_OF "Build FFX Optical Flow API" ON)
endif()

if (FFX_SSSR)
	option(FFX_DENOISER "Build FFX Denoiser API" ON)
endif()

# Add requred compile definitions
add_compile_definitions(_UNICODE)
add_compile_definitions(UNICODE)
add_compile_definitions(_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR)

# Setup common variables
set(FFX_SC_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/tools/binary_store/FidelityFX_SC.exe)
set(FFX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(FFX_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(FFX_BIN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/ffx_sdk)
set(FFX_SHARED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/shared)
set(FFX_HOST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/FidelityFX/host)
set(FFX_GPU_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/FidelityFX/gpu)
set(FFX_COMPONENTS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/components)
set(FFX_HOST_BACKENDS_PATH ${FFX_HOST_PATH}/backends)
set(FFX_SRC_BACKENDS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/backends)

# ------------------------------------------------------
# Define common source groupings used by all effects

# Set shared and global public sources
file(GLOB FFX_SHARED_SOURCES
    "${FFX_SHARED_PATH}/*.cpp"
    "${FFX_SHARED_PATH}/*.h")

set(FFX_PUBLIC_SOURCES
    "${FFX_HOST_PATH}/ffx_assert.h"
	"${FFX_HOST_PATH}/ffx_message.h"
    "${FFX_HOST_PATH}/ffx_error.h"
	"${FFX_HOST_PATH}/ffx_fx.h"
	"${FFX_HOST_PATH}/ffx_interface.h"
    "${FFX_HOST_PATH}/ffx_types.h"
    "${FFX_HOST_PATH}/ffx_util.h")

# Init shader sources (for easy viewing)
set(FFX_PUBLIC_SHADER_SOURCES
    "${FFX_GPU_PATH}/ffx_common_types.h"
    "${FFX_GPU_PATH}/ffx_core.h"
    "${FFX_GPU_PATH}/ffx_core_cpu.h"
    "${FFX_GPU_PATH}/ffx_core_glsl.h"
    "${FFX_GPU_PATH}/ffx_core_gpu_common.h"
    "${FFX_GPU_PATH}/ffx_core_gpu_common_half.h"
    "${FFX_GPU_PATH}/ffx_core_hlsl.h"
    "${FFX_GPU_PATH}/ffx_core_portability.h")

if (FFX_API_BACKEND STREQUAL VK)
	message(STATUS "Creating project FidelityFX-SDK_VK")
	project(FidelityFX-SDK_VK)
else()
	# This is likely a custom include
	project (FidelityFX-SDK)
endif()

# Components
add_subdirectory(${FFX_COMPONENTS_PATH}/opticalflow)
add_subdirectory(${FFX_COMPONENTS_PATH}/frameinterpolation)
add_subdirectory(${FFX_COMPONENTS_PATH}/fsr3)
add_subdirectory(${FFX_COMPONENTS_PATH}/fsr3upscaler)
add_subdirectory(${FFX_COMPONENTS_PATH}/fsr2)
add_subdirectory(${FFX_COMPONENTS_PATH}/fsr1)
add_subdirectory(${FFX_COMPONENTS_PATH}/spd)
add_subdirectory(${FFX_COMPONENTS_PATH}/cacao)
add_subdirectory(${FFX_COMPONENTS_PATH}/lpm)
add_subdirectory(${FFX_COMPONENTS_PATH}/blur)
add_subdirectory(${FFX_COMPONENTS_PATH}/vrs)
add_subdirectory(${FFX_COMPONENTS_PATH}/cas)
add_subdirectory(${FFX_COMPONENTS_PATH}/dof)
add_subdirectory(${FFX_COMPONENTS_PATH}/lens)
add_subdirectory(${FFX_COMPONENTS_PATH}/parallelsort)
add_subdirectory(${FFX_COMPONENTS_PATH}/denoiser)
add_subdirectory(${FFX_COMPONENTS_PATH}/sssr)
add_subdirectory(${FFX_COMPONENTS_PATH}/brixelizer)
add_subdirectory(${FFX_COMPONENTS_PATH}/brixelizergi)
add_subdirectory(${FFX_COMPONENTS_PATH}/classifier)
add_subdirectory(${FFX_COMPONENTS_PATH}/breadcrumbs)

if (FFX_API_BACKEND STREQUAL VK)
	add_subdirectory(${FFX_SRC_BACKENDS_PATH}/vk)
endif()
