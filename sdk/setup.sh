if [ ! -d "$(pwd)/build" ]; then 
	mkdir build;
fi

cd build

if [[ $(pwd) == */FidelityFX-SDK-Linux/sdk/build ]]; then
	rm -r ./*
	cmake .. -DFFX_API_BACKEND=VK -DFFX_FSR3=ON -DFFX_AUTO_COMPILE_SHADERS=1
	cmake --build ./ --config Release
fi
