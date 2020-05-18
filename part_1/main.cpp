#include <iostream>
#include <windows.h>

HANDLE gh_mutex;

int main()
{
    gh_mutex = CreateMutex(NULL, FALSE, TEXT("lab_05_part_1_mutex"));

    // Request ownership of mutex.
    DWORD dw_wait_result = WaitForSingleObject(
        gh_mutex,    // handle to mutex
        0
    );

    if (dw_wait_result != WAIT_OBJECT_0)
    {
        std::cout << "Error! Cannot start another copy of the program." << std::endl;
        std::cin.get();

        return 0;
    }
    else
    {
        std::cout << "Is running..." << std::endl;
        std::cin.get();
    }

    return 0;
}
