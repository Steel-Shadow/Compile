cmake --build cmake-build-debug

cd bin

rm *.pdb, *.ilk
.\Compiler.exe | Select-String -Pattern "error"

mkdir output -f >nul
java -jar mars.jar nc mips.txt > output\marsOutput.txt

Copy-Item -Path testfile.txt -Destination temp.c
gcc temp.c -o temp.exe 2> nul # 2>nul 隐藏错误信息

.\temp.exe > output\gccOutput.txt
rm temp.c, temp.exe

# compare gccOutput.txt and marsOutput.txt
$diff = Compare-Object (Get-Content output\gccOutput.txt) (Get-Content output\marsOutput.txt)

# 检查比较结果
if ($diff.Count -eq 0)
{
    echo (Get-Content output\marsOutput.txt)
    Write-Host "Good"
}
else
{
    Write-Host "Bad`n<= ans => myOutput"
    # 输出不同的部分
    foreach ($item in $diff)
    {
        $info = $item | Select-Object InputObject, SideIndicator
        Write-Host "$( $info.SideIndicator ) $( $info.InputObject )"
    }
}

cd ..

# windows Compress-Archive use \ as seperators for file, incompatible with linux
7z a srcCode.zip src\ CMakeLists.txt > nul
