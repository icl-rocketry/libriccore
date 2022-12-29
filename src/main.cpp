#include <iostream>
#include <headers/thread.h>
#include <headers/fs.h>

int fib(int n, int a, int b) {
    if (n == 0) {
        return a;
    }
    return fib(n - 1, a + b, a);
}

int fib(int n) {
    return fib(n, 0, 1);
}

void thread2(void* args) {
    Lock* l = (Lock*) args;
    int x = fib(32);
    std::cout << x << std::endl;

    FS::mkdir("./tmp/");
    File f("./tmp/test.txt", FILE_MODE::WRITE);

    std::string str = std::to_string(x);
    f.write(str.c_str(), str.length() + 1);
    f.close();

    l->release(); 
}

int main() {
    Lock l;
    l.acquire();

    Thread t(&thread2, (void*) &l);

    l.acquire(); // Wait for thread 2 to complete

    std::cout << "Done" << std::endl;
    return 0;
}
