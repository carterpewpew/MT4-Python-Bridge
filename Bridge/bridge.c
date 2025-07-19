// bridge.c
#include <windows.h>
#include <string.h>

#define SHM_NAME "Local\\MT4PythonBridge"
#define EVENT_NAME "Global\\MT4TickEvent"

struct SharedData {
    double ask;
    double bid;
    char signal[32];
};

static HANDLE hMapFile = NULL;
static struct SharedData* data = NULL;
static HANDLE hEvent = NULL;

__declspec(dllexport) void __stdcall InitBridge() {
    if (data != NULL) return;  // Already initialized

    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(struct SharedData),
        SHM_NAME
    );

    if (hMapFile != NULL) {
        data = (struct SharedData*) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(struct SharedData));
        if (data != NULL) {
            data->ask = 0.0;
            data->bid = 0.0;
            strcpy(data->signal, "none");
        }
    }

    if (hEvent == NULL) {
        hEvent = CreateEventA(NULL, FALSE, FALSE, EVENT_NAME);
    }
}

__declspec(dllexport) void __stdcall SendTick(double ask, double bid) {
    if (data != NULL) {
        data->ask = ask;
        data->bid = bid;
        if (hEvent != NULL) SetEvent(hEvent);
    }
}

__declspec(dllexport) void __stdcall CopySignal(char* out, int maxLen) {
    if (data != NULL && out != NULL) {
        strncpy(out, data->signal, maxLen - 1);
        out[maxLen - 1] = '\0';
    }
}

__declspec(dllexport) void __stdcall CleanupBridge() {
    if (data != NULL) {
        UnmapViewOfFile(data);
        data = NULL;
    }
    if (hMapFile != NULL) {
        CloseHandle(hMapFile);
        hMapFile = NULL;
    }
    if (hEvent != NULL) {
        CloseHandle(hEvent);
        hEvent = NULL;
    }
}
