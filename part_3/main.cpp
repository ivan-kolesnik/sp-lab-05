#include <Windows.h>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

#define ELEMENTS_COUNT 1e7
#define THREAD_COUNT 3

CRITICAL_SECTION critical_section;
vector<int> arr;

void f_sum(int limit)
{
    int sum = 0;
    for (int i = 0; i < limit; i++)
    {
        sum += arr[i];
    }
        
    printf("Sum = %d;\n", sum);
}

void f_avg(int limit)
{
    float avg = 0;
    for (int i = 0; i < limit; i++)
    {
        avg += arr[i];
    }

    printf("Average = %f;\n", (float)(avg / limit));
}

void f_max(int limit)
{
    int v = 0;
    for (size_t i = 0; i < limit; i++)
    {
        if (arr[i] > v)
        {
            v = arr[i];
        }
    }

    printf("Maximum = %d;\n", v);
}

void generate_array(int limit)
{
    for (size_t i = 0; i < limit; i++)
    {
        arr.push_back(rand() % 90 + 10);
    }
}

DWORD WINAPI sum_thread_routine(LPVOID lpv_use_critical_section)
{
    bool use_critical_section = (bool)lpv_use_critical_section;

    if (use_critical_section)
    {
        if (!InitializeCriticalSectionAndSpinCount(&critical_section, 0))
        {
            return 0;
        }

        EnterCriticalSection(&critical_section);
        f_sum(ELEMENTS_COUNT);
        LeaveCriticalSection(&critical_section);
    }
    else
    {
        f_sum(ELEMENTS_COUNT);
    }

    return 0;
}

DWORD WINAPI avg_thread_routine(LPVOID lpv_use_critical_section)
{
    bool use_critical_section = (bool)lpv_use_critical_section;

    if (use_critical_section)
    {
        if (!InitializeCriticalSectionAndSpinCount(&critical_section, 0))
        {
            return 0;
        }

        EnterCriticalSection(&critical_section);
        f_avg(ELEMENTS_COUNT);
        LeaveCriticalSection(&critical_section);
    }
    else
    {
        f_avg(ELEMENTS_COUNT);
    }

    return 0;
}

DWORD WINAPI max_thread_routine(LPVOID lpv_use_critical_section)
{
    bool use_critical_section = (bool)lpv_use_critical_section;

    if (use_critical_section)
    {
        // Initialize a critical section object and
        // set the spin count for the critical section. 
        if (!InitializeCriticalSectionAndSpinCount(&critical_section, 0))
        {
            return 0;
        }

        EnterCriticalSection(&critical_section);
        f_max(ELEMENTS_COUNT);
        LeaveCriticalSection(&critical_section);
    }
    else
    {
        f_max(ELEMENTS_COUNT);
    }

    return 0;
}

void CreateThreadsAndWait(bool use_critical_section)
{
    vector<HANDLE> v_h_thread; //Threads handle array
    vector<DWORD> v_id_thread; //Threads ID array

    clock_t t_start, t_end;

    printf("Is critical section mode?: %d\n", use_critical_section);

    generate_array(ELEMENTS_COUNT);

    t_start = clock();

    for (size_t i = 0; i < THREAD_COUNT; i++)
    {
        v_h_thread.push_back(NULL);
        v_id_thread.push_back(NULL);
    }

    v_h_thread[0] = CreateThread(NULL, 0, sum_thread_routine, (LPVOID)use_critical_section, 0, &v_id_thread[0]);
    v_h_thread[1] = CreateThread(NULL, 0, avg_thread_routine, (LPVOID)use_critical_section, 0, &v_id_thread[1]);
    v_h_thread[2] = CreateThread(NULL, 0, max_thread_routine, (LPVOID)use_critical_section, 0, &v_id_thread[2]);
    WaitForMultipleObjects(THREAD_COUNT, v_h_thread.data(), TRUE, INFINITE);

    t_end = clock();

    printf("Duration: %fsec\n", (float)(t_end - t_start) / CLOCKS_PER_SEC);

    DeleteCriticalSection(&critical_section);

    for (int i = 0; i < THREAD_COUNT; i++) {
        CloseHandle(v_h_thread[i]);
    }
}

int main()
{
    CreateThreadsAndWait(false); // no critical section
    CreateThreadsAndWait(true);  // critical section

    cin.get();
    cin.get();

    return 0;
}