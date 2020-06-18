$ErrorActionPreference = "Stop"
New-Item -ItemType Directory -Force -Path build                 
cd build
cmake ..
cmake --build . --config RELEASE
cp Release/Tsuka.exe ..
cd ..