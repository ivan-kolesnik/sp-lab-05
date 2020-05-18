#include <iostream>
#include <ctime>
#include <vector>
#include <windows.h>

using namespace std;

HANDLE h_semaphore;

void failure_exit(const char * lpsz_message)
{
    fprintf(stderr, "%s\n", lpsz_message);
    ExitProcess(0);
}

DWORD WINAPI thread_routine(LPVOID lpParam)
{
    DWORD bytes_written;
    HANDLE file;
    clock_t t_start, t_end;

    LPSTR buf = new char[20];
    bool running = true;

    t_start = clock();
    DWORD dw_wait_result = WaitForSingleObject(h_semaphore, INFINITE);

    while (running)
    {
        if (dw_wait_result == WAIT_OBJECT_0)
        {
            printf("Thread %d waiting...\n", GetCurrentThreadId());
            Sleep(1000 * (rand() % 3 + 1));

            printf("Thread %d writing to file...\n", GetCurrentThreadId());
            file = CreateFileA(
                "work_time.txt",
                FILE_GENERIC_WRITE,
                FILE_SHARE_WRITE,
                NULL,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );

            t_end = clock();

            SetFilePointer(file, 0, NULL, FILE_END);
            sprintf(buf, "- %f (sec) -", (float)(t_end - t_start) / CLOCKS_PER_SEC);
            WriteFile(file, buf, sizeof(char) * strlen(buf), &bytes_written, NULL);

            if (file == INVALID_HANDLE_VALUE)
            {
                printf("File handle error: %d\n", GetLastError());
            }

            running = false;

            Sleep(1000 * (rand() % 3 + 1));
            CloseHandle(file);

            printf("Thread %d ended its work.\n", GetCurrentThreadId());

            if (!ReleaseSemaphore(h_semaphore, 1, NULL)) {
                printf("Cannot release semaphore: %d\n", GetLastError());
            }
        }
    }

    return 0;
}

int main()
{
    int thread_count;

    cout << "Enter number of threads: ";
    cin >> thread_count;

    HANDLE *h_threads = new HANDLE[thread_count];

    h_semaphore = CreateSemaphore(
        NULL,		            // Pointer to a SECURITY_ATTRIBUTES structure
        2,	                    // Initial count for the semaphore object
        2,	                    // Maximum count for the semaphore object
        TEXT("lab_05_part_2_semaphore")   // Name of the semaphore object
    );

    if (h_semaphore == NULL)
    {
        printf("CreateMutex error: %d\n", GetLastError());
        return 1;
    }

    for (size_t i = 0; i < thread_count; i++)
    {
        HANDLE thread = CreateThread(
            NULL,
            0,
            (LPTHREAD_START_ROUTINE)thread_routine,
            NULL,
            NULL,
            NULL
        );

        // Check the return value for success
        if (h_threads[i] == NULL)
        {
            failure_exit("CreateThread error\n");
        }

        h_threads[i] = thread;
    }

    WaitForMultipleObjects(thread_count, h_threads, TRUE, INFINITE);

    for (size_t i = 0; i < thread_count; i++)
    {
        CloseHandle(h_threads[i]);
    }

    CloseHandle(h_semaphore);

    return 0;
}
