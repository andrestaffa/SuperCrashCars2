INSTRUCTIONS:

	STEP 1: Clone the vcpkg repo
		- git clone https://github.com/Microsoft/vcpkg.git
			- Make sure to do this in the folder you want this to be installed
		
	STEP 2: Run the bootstrap script to build vcpkg	
		- .\vcpkg\bootstrap-vcpkg.bat
		
	STEP 3: Install all necessary libraries 

		GLEW:
			- .\vcpkg.exe install glew:x64-windows
				- glew 2.2.0
		
		GLFW3:
			- .\vcpkg.exe install glfw3:x64-windows
				- glfw3 3.3.6
				
		GLM:
			- .\vcpkg.exe install glm:x64-windows
				- glm 0.9.9.8#1
				
		FMT:
			- .\vcpkg.exe install fmt:x64-windows
				- fmt 8.1.0
				
		ARGH:
			- .\vcpkg.exe install argh:x64-windows
				- argh 2018-12-18#3
				
		STB:
			- .\vcpkg.exe install stb:x64-windows
				- stb 2020-09-14#1
				
		PHYSX:
			- .\vcpkg.exe install physx:x64-windows
				- physx 4.1.2
				
		
	STEP 4: Run this command
		- vcpkg integrate install
		
	
Done :)
	
