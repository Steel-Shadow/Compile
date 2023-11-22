cmake --build .\cmake-build-debug\

cd bin

.\Compiler.exe | Select-String -Pattern "error"

rm marsOutput.txt
java -jar .\mars.jar nc mips.txt > marsOutput.txt

Copy-Item -Path .\testfile.txt -Destination .\temp.c
gcc temp.c -o temp.exe 2> nul # 2>nul 隐藏错误信息

rm gccOutput.txt
.\temp.exe > gccOutput.txt
rm .\temp.exe, .\temp.c

# compare gccOutput.txt and marsOutput.txt
$diff = Compare-Object (Get-Content .\gccOutput.txt) (Get-Content .\marsOutput.txt)

# 检查比较结果
if ($diff.Count -eq 0)
{
    echo (Get-Content marsOutput.txt)
    Write-Host "Good"
}
else
{
    Write-Host "Bad`n<= ans >= myOutput"
    # 输出不同的部分
    foreach ($item in $diff)
    {
        $info = $item | Select-Object InputObject, SideIndicator
        Write-Host "$( $info.SideIndicator ) $( $info.InputObject )"
    }
}

cd ..
7z a srcCode.zip src\ CMakeLists.txt > nul
