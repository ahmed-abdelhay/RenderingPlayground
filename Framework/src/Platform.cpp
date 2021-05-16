#include "Resha.h"
#include <assert.h>

#if defined RESHA_OS_WINDOWS
#define UNICODE
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define WIN32_MEAN_AND_LEAN
#define VC_EXTRALEAN
#pragma comment(lib, "rpcrt4.lib") // UuidCreate - Minimum supported OS Win 2000
#include <Rpc.h>
#include <windows.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#undef WIN32_MEAN_AND_LEAN
#undef VC_EXTRALEAN
#elif defined RESHA_OS_LINUX
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <uuid/uuid.h> // user will have to link against libuuid.
#else
#error "Not supported OS"
#endif

//--------------------------------File IO---------------------//
#if defined RESHA_OS_WINDOWS
static std::wstring UTF8ToUTF16(const char* txt)
{
    const size_t len = strlen(txt);
    const size_t wlen =
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, txt, len, NULL, 0);
    std::wstring result;
    result.resize(wlen);
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, txt, len, &result[0],
                        result.size());
    return result;
}

static HANDLE GetFileHandle(const char* fileName, bool read)
{
    if (!fileName)
    {
        return NULL;
    }
    std::wstring string = UTF8ToUTF16(fileName);
    HANDLE handle = NULL;
    if (read)
    {
        handle =
            CreateFile(string.c_str(),  // file to open
                       GENERIC_READ,    // open for reading
                       FILE_SHARE_READ, // share for reading
                       NULL,            // default security
                       OPEN_EXISTING,   // existing file only
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // normal file
                       NULL);
    }
    else
    {
        handle = CreateFile(string.c_str(),        // name of the write
                            GENERIC_WRITE,         // open for writing
                            0,                     // do not share
                            NULL,                  // default security
                            CREATE_NEW,            // create new file only
                            FILE_ATTRIBUTE_NORMAL, // normal file
                            NULL);
    }
    return handle;
}

bool ReadFile(const char* fileName, std::vector<uint8_t>& data)
{
    const int64_t fileSize = GetFileSize(fileName);
    if (fileSize < 0)
    {
        return false;
    }
    data.resize(fileSize);
    HANDLE handle = GetFileHandle(fileName, true);
    defer(CloseHandle(handle));
    OVERLAPPED overlapped = {};
    const bool success =
        ReadFileEx(handle, data.data(), data.size(), &overlapped, NULL);
    assert(success);
    return true;
}

bool WriteFile(const char* fileName, const uint8_t* data, size_t size)
{
    HANDLE handle = GetFileHandle(fileName, false);
    if (handle)
    {
        defer(CloseHandle(handle));
        OVERLAPPED overlapped = {};
        return WriteFileEx(handle, data, size - 1, &overlapped, NULL);
    }
    return false;
}

bool DoesFileExist(const char* fileName)
{
    HANDLE handle = GetFileHandle(fileName, true);
    const bool found = handle != INVALID_HANDLE_VALUE;
    if (found)
    {
        CloseHandle(handle);
    }
    return found;
}

int64_t GetFileSize(const char* fileName)
{
    HANDLE handle = GetFileHandle(fileName, true);
    const bool found = handle != INVALID_HANDLE_VALUE;
    if (found)
    {
        LARGE_INTEGER size;
        GetFileSizeEx(handle, &size);
        CloseHandle(handle);
        return size.QuadPart;
    }
    return -1;
}

PathType GetPathType(const char* path)
{
    HANDLE handle = GetFileHandle(path, true);
    const bool found = handle != INVALID_HANDLE_VALUE;
    if (found)
    {
        defer(CloseHandle(handle));
        FILE_BASIC_INFO basicInfo;
        GetFileInformationByHandleEx(handle, FileBasicInfo, &basicInfo,
                                     sizeof(basicInfo));
        return (basicInfo.FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            ? PathType::DIRECTORY
            : PathType::FILE;
    }
    return PathType::FAILURE;
}

#elif defined RESHA_OS_LINUX
bool ReadFile(const char* fileName, std::vector<uint8_t>& data)
{
    FILE* fp = fopen(fileName, "r");
    MemoryBlock result;
    if (!fp)
    {
        return false;
    }
    fseek(fp, 0, SEEK_END);
    const int64_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    result.resize(size);
    fread(result.data(), 1, size, fp);
    fclose(fp);
    return true;
}

bool WriteFile(const char* fileName, const uint8_t* data, size_t size)
{
    FILE* fp = fopen(fileName, "w");
    if (fp)
    {
        fwrite(data, 1, size, fp);
        fclose(fp);
        return true;
    }
    return false;
}

bool DoesFileExist(const char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    if (fp)
    {
        fclose(fp);
        return true;
    }
    return false;
}

int64_t GetFileSize(const char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        fclose(fp);
        const int64_t size = ftell(fp);
        return size;
    }
    return -1;
}

PathType GetPathType(const char* path)
{
    struct stat s;
    if (stat(path, &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            return PathType::DIRECTORY;
        }
        else if (s.st_mode & S_IFREG)
        {
            return PathType::FILE;
        }
    }
    return PathType::FAILURE;
}
#endif
//--------------------------------------------------------------------//

//--------------------UUID------------------------------------//
#if defined(RESHA_OS_WINDOWS)
UUId GenerateUUID()
{
    GUID uuid;
    UuidCreate(&uuid);

    UUId result{ (uint8_t)((uuid.Data1 >> 24) & 0xFF),
                (uint8_t)((uuid.Data1 >> 16) & 0xFF),
                (uint8_t)((uuid.Data1 >> 8) & 0xFF),
                (uint8_t)((uuid.Data1) & 0xff),

                (uint8_t)((uuid.Data2 >> 8) & 0xFF),
                (uint8_t)((uuid.Data2) & 0xff),

                (uint8_t)((uuid.Data3 >> 8) & 0xFF),
                (uint8_t)((uuid.Data3) & 0xFF),

                (uint8_t)uuid.Data4[0],
                (uint8_t)uuid.Data4[1],
                (uint8_t)uuid.Data4[2],
                (uint8_t)uuid.Data4[3],
                (uint8_t)uuid.Data4[4],
                (uint8_t)uuid.Data4[5],
                (uint8_t)uuid.Data4[6],
                (uint8_t)uuid.Data4[7] };
    return result;
}

#elif defined(RESHA_OS_LINUX)
UUId GenerateUUID()
{
    UUId result;
    uuid_generate(result.data);
    return result;
}
#endif
bool operator==(const UUId& a, const UUId& b)
{
    for (size_t i = 0; i < 16; ++i)
    {
        if (a.data[i] != b.data[i])
        {
            return false;
        }
    }
    return true;
}

bool ParseUUID(const std::string_view data, UUId& result)
{
    // example: 0537256b-c6ba-4ab2-be23-fd7158c94883
    if (data.size() != 36 || data[8] != '-' || data[13] != '-' ||
        data[18] != '-')
    {
        return false;
    }
    const char* pos = data.data();
    for (size_t count = 0; count < 16; count++)
    {
        sscanf(pos, "%2hhx", &result.data[count]);
        pos += 2;
        if (*pos == '-')
        {
            pos++;
        }
    }
    return true;
}
//--------------------------------------------------------------------//

//------------------------Time-------------------------------//
#if defined(RESHA_OS_WINDOWS)
static double GetPerformanceFrequency()
{
    LARGE_INTEGER PerfCountFrequencyResult;
    QueryPerformanceFrequency(&PerfCountFrequencyResult);
    return PerfCountFrequencyResult.QuadPart;
}

void StartStopWatch(StopWatch& in)
{
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    in.start = t.QuadPart;
}

void StopStopWatch(StopWatch& in)
{
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    in.end = t.QuadPart;
}

double ElapsedSeconds(const StopWatch& in)
{
    const static double frequency = GetPerformanceFrequency();
    return (in.end - in.start) / frequency;
}
#elif defined(RESHA_OS_LINUX)
void StartStopWatch(StopWatch& in)
{
    time_t t;
    time(&t);
    in.start = t;
}

void StopStopWatch(StopWatch& in)
{
    time_t t;
    time(&t);
    in.end = t;
}

double ElapsedSeconds(const StopWatch& in)
{
    return difftime(in.end, in.start);
}
#endif
//----------------------------------------------------------//
