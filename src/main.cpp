#include <iostream>
#include <fstream>

extern "C" {
#include "libsysy.h"
}

using namespace std;

int main() {
    // 打开文件以进行读取
    ifstream infile("testfile.txt");

    // 检查文件是否成功打开
    if (!infile) {
        cerr << "Read testfile.txt fail!" << endl;
//        return 1;
    }

    // 读取文件内容
    string line;
    while (getline(infile, line)) {
        cout << line << endl;
    }


    // 打开一个文件以进行写入
    ofstream outfile("output.txt");

    // 关闭文件
    infile.close();
    outfile.close();

    return 0;
}
