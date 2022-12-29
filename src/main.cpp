#include <iostream>
#include <headers/thread.h>

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
    std::cout << fib(32) << std::endl;
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
