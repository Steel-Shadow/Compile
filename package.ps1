#Set-ExecutionPolicy -ExecutionPolicy Unrestricted

cmake --build cmake-build-debug
# windows Compress-Archive use \ as seperators for file, incompatible with linux
7z a srcCode.zip src\ CMakeLists.txt package.ps1 config.json > $null

cd bin

rm *.pdb, *.ilk
.\Compiler.exe | Select-String -Pattern "error"

#### GCC ####
# 添加头文件和 getint()
$contentToAdd = "#include <stdio.h>`nint getint(){`n    int x=0;`n    scanf(`"%d`",&x);    return x;`n}`n"
$existingContent = Get-Content testfile.txt
$newContent = $contentToAdd + $existingContent
$newContent | Set-Content temp.c
gcc temp.c -o temp.exe # 2> $null

#### Mars ####
mkdir output -f > $null
$input_file_path = "in.txt"
if (Test-Path $input_file_path)
{
    cat $input_file_path | .\temp.exe  > output\gccOutput.txt
    cat $input_file_path | java -jar mars.jar nc mips.txt > output\marsOutput.txt
}
else
{
    .\temp.exe  > output\gccOutput.txt
    java -jar mars.jar nc mips.txt > output\marsOutput.txt
}

rm temp.c, temp.exe

#### compare gccOutput.txt and marsOutput.txt ####
$diff = Compare-Object (Get-Content output\gccOutput.txt) (Get-Content output\marsOutput.txt)

#### 检查比较结果 ####
if ($diff.Count -eq 0)
{
    cat output\marsOutput.txt
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
