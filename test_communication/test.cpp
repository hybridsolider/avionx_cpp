#include <iostream>
#include <sys/wait.h>
#include <cstdlib>
int main()
{
    std::cout << "C++ -> python connection\n";
    int status = std::system("python main.py hello 1 2");
    std::cout << WEXITSTATUS(status) << '\n';
    return 0;
}