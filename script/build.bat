cd ..
git submodule update --init
rd /s /q build
md build
cd .\build
cmake ..
cmake --build .