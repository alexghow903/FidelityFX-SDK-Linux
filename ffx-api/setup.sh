if [ ! -d "$(pwd)/build" ]; then
    mkdir build;
fi

cd build

if [[ $(pwd) == */FidelityFX-SDK-Linux/ffx-api/build ]]; then
	rm -r ./*
    cmake .. -DFFX_API_BACKEND=VK -DFFX_BUILD_AS_DLL=1
    cmake --build ./ --config Release #--parallel 4
fi

cd ..