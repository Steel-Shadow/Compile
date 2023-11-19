cmake --build .\cmake-build-debug\
cd bin
.\Compiler.exe
java -jar .\mars.jar nc mips.txt
cd ..
7z a srcCode.zip src\ CMakeLists.txt > nul

