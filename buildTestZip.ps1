cmake --build .\cmake-build-debug\
cd bin
.\Compiler.exe
java -jar .\mars.jar mips.txt
cd ..
Compress-Archive -LiteralPath .\src -DestinationPath .\srcCode.zip -Update

