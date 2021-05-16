#pragma once

#include <set>
#include <string>
#include <string_view>
#include <vector>

#if defined _WIN32
#define RESHA_OS_WINDOWS 1
#elif defined __linux__
#define RESHA_OS_LINUX 1
#else
#error "Not supported OS"
#endif

template <typename F> struct privDefer
{
    F f;
    privDefer(F f) : f(f)
    {
    }
    ~privDefer()
    {
        f();
    }
};
template <typename F> privDefer<F> defer_func(F f)
{
    return privDefer<F>(f);
}
#define DEFER_1(x, y) x##y
#define DEFER_2(x, y) DEFER_1(x, y)
#define DEFER_3(x) DEFER_2(x, __COUNTER__)
#define defer(code) auto DEFER_3(_defer_) = defer_func([&]() { code; })

//------------------Math----------------------------//
const double PI = 3.14159265358979323846264338327950288;

union Vec2d
{
    struct
    {
        double x, y;
    };
    double data[2];
};

union Vec3d
{
    struct
    {
        double x, y, z;
    };
    double data[3];
};

union Vec2f
{
    struct
    {
        float x, y;
    };
    float data[2];
};

union Vec3f
{
    struct
    {
        float x, y, z;
    };
    float data[3];
};

// the matrices are column major.
union Mat3
{
    double elements[3][3];
    double data[9];
};

union Mat4
{
    double elements[4][4];
    double data[16];
};

struct BBox
{
    Vec3d min{ DBL_MAX, DBL_MAX, DBL_MAX };
    Vec3d max{ -DBL_MAX, -DBL_MAX, -DBL_MAX };
};

Vec2f operator+(const Vec2f& a, const Vec2f& b);
Vec2f operator-(const Vec2f& a, const Vec2f& b);
Vec2f operator*(const Vec2f& a, const Vec2f& b);
Vec2f operator*(const Vec2f& a, float x);
Vec2f operator*(float x, const Vec2f& a);
bool operator==(const Vec2f& a, const Vec2f& b);
bool operator!=(const Vec2f& a, const Vec2f& b);

Vec3f operator+(const Vec3f& a, const Vec3f& b);
Vec3f operator-(const Vec3f& a, const Vec3f& b);
Vec3f operator*(const Vec3f& a, const Vec3f& b);
Vec3f operator*(const Vec3f& a, float x);
Vec3f operator*(float x, const Vec3f& a);
bool operator==(const Vec3f& a, const Vec3f& b);
bool operator!=(const Vec3f& a, const Vec3f& b);

Vec2d operator+(const Vec2d& a, const Vec2d& b);
Vec2d operator-(const Vec2d& a, const Vec2d& b);
Vec2d operator*(const Vec2d& a, const Vec2d& b);
Vec2d operator*(const Vec2d& a, double x);
Vec2d operator*(double x, const Vec2d& a);
bool operator==(const Vec2d& a, const Vec2d& b);
bool operator!=(const Vec2d& a, const Vec2d& b);

Vec3d operator+(const Vec3d& a, const Vec3d& b);
Vec3d operator-(const Vec3d& a, const Vec3d& b);
Vec3d operator*(const Vec3d& a, const Vec3d& b);
Vec3d operator*(const Vec3d& a, double x);
Vec3d operator*(double x, const Vec3d& a);
bool operator==(const Vec3d& a, const Vec3d& b);
bool operator!=(const Vec3d& a, const Vec3d& b);

Vec3d operator*(const Mat3& a, const Vec3d& v);

Mat4 operator*(const Mat4& left, const Mat4& right);
Mat4 operator*(const Mat4& left, double x);

Vec3d CrossProduct(const Vec3d& a, const Vec3d& b);
Vec3f CrossProduct(const Vec3f& a, const Vec3f& b);

double DotProduct(const Vec3f& a, const Vec3f& b);
double DotProduct(const Vec2f& a, const Vec2f& b);
double DotProduct(const Vec3d& a, const Vec3d& b);
double DotProduct(const Vec2d& a, const Vec2d& b);
double DotProduct(const double a[4], const double b[4]);

double Length(const Vec3f& v);
double Length(const Vec2f& v);
double Length(const Vec3d& v);
double Length(const Vec2d& v);

void Normalise(Vec3f& v);
void Normalise(Vec2f& v);
void Normalise(Vec3d& v);
void Normalise(Vec2d& v);

Vec3f Normalised(const Vec3f& v);
Vec2f Normalised(const Vec2f& v);
Vec3d Normalised(const Vec3d& v);
Vec2d Normalised(const Vec2d& v);

Mat3 Transpose(Mat3 m);
Mat4 Transpose(Mat4 m);
Mat4 Identity();
Mat4 Translate(Mat4 m, const Vec3d& translation);
Mat4 Rotate(const Mat4& m, double angle, const Vec3d& v);
Mat4 Perspective(double fovy, double aspect, double zNear, double zFar);
Mat4 LookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up);
double Deg2Rad(double v);
double Rad2Deg(double v);

BBox Merge(const BBox& a, const BBox& b);
bool IsBBoxValid(const BBox& b);
Vec3d CalculateBBoxCenter(const BBox& b);
void CalculateBBoxCorners(const BBox& b, Vec3d corners[8]);
//--------------------------------------------------//

//------------------Strings-------------------------//
// extract the file name from file path.
std::string ExtractFileName(const char* path);
bool EndsWith(const std::string_view str, const std::string_view suffix);
//--------------------------------------------//

//-----------------UUID-----------------------------//
struct UUId
{
    uint8_t data[16] = {};
};

UUId GenerateUUID();
bool operator==(const UUId& a, const UUId& b);
bool ParseUUID(const std::string_view data, UUId& result);
//--------------------------------------------------//

//--------------------------------File IO---------------------//
enum class PathType
{
    FAILURE, FILE, DIRECTORY
};

// File names are utf8 encoded.
bool DoesFileExist(const char* fileName);
int64_t GetFileSize(const char* fileName);
bool ReadFile(const char* fileName, std::vector<uint8_t>& data);
bool WriteFile(const char* fileName, const uint8_t* data, size_t size);
PathType GetPathType(const char* path);
//------------------------------------------------------------//

//-----------------------Time  -------------------------------//
#define TIME_BLOCK(BlockName)                                                  \
  StopWatch _t;                                                                \
  StartStopWatch(_t);                                                          \
  defer({                                                                      \
    StopStopWatch(_t);                                                         \
    printf("Time spent in (%s): %f seconds.\n", BlockName,                     \
           ElapsedSeconds(_t));                                                \
  });

struct StopWatch
{
    int64_t start = 0;
    int64_t end = 0;
};
void StartStopWatch(StopWatch& in);
void StopStopWatch(StopWatch& in);
double ElapsedSeconds(const StopWatch& in);
//------------------------------------------------------------//

//------------------------------------------------------------//

enum class IOStatus
{
    OK, FAILURE, FILE_DOESNT_EXIST, INVALID_EXTENSION
};

struct Color
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;
};

struct Triangle
{
    uint32_t idx[3];
};

struct VertexNeighbours
{
    size_t vertexIndex;
    std::set<size_t> adjacentFaces;
};

struct Connectivity
{
    std::vector<VertexNeighbours> pointCells;
};

struct SurfaceMesh
{
    std::vector<Vec3d> vertices;
    std::vector<Triangle> faces;
    std::string name;
    Color color;
    UUId id;
    bool visible = true;
};


std::vector<Vec3d> CalculateFacesNormals(const SurfaceMesh& mesh);
std::vector<Vec3d> CalculateVertexNormals(const SurfaceMesh& mesh,
                                          const Connectivity& connectivity);
BBox CalculateBoundingBox(const SurfaceMesh& mesh);
Connectivity BuildConnectivity(const SurfaceMesh& mesh);

IOStatus ReadMesh(const char* fileName, SurfaceMesh& result);
bool WriteStl(const SurfaceMesh& mesh, const char* fileName);

Color GenerateColor();

// Graphics 
enum class ShaderType
{
    GEOMETRY, FRAGMENT, VERTEX
};

struct Program
{
    bool valid = false;

    int32_t id = -1;
    std::string geometryShader;
    std::string fragmentShader;
    std::string vertexShader;
};

struct RenderBuffer
{
    size_t width = 0;
    size_t height = 0;
    uint32_t frameBufferId;
    uint32_t renderBufferId;
    uint32_t textureId; // id of the texture used to store the 3d render pipeline of the 3D view.
};

struct MeshRenderInfo
{
    uint32_t vertexBufferObject;
    uint32_t vertexBufferId;
    uint32_t elementBufferId;
    size_t facesCount;
    size_t verticesCount;
    BBox box;
    UUId id;
};


RenderBuffer CreateRenderBuffer(size_t width, size_t height);
void ReizeRenderBuffer(RenderBuffer& buffer, size_t width, size_t heigth);
void ClearRenderBuffer(const RenderBuffer& buffer, Color c);

// geometry shader pointers can be null, in that case it won't be added to the output program.
Program CreateProgram(const char* geometryShader,
                      const char* vertexShader,
                      const char* fragmentShader,
                      std::string& log);

// returns -1 on failure and the shader id on success.
int32_t CompileShader(const char* shader, ShaderType type, std::string& log);

void SetProgramUniformV3f(const Program& program, const char* name, const float data[3]);
void SetProgramUniformM4x4f(const Program& program, const char* name, const float data[16]);

uint32_t GenerateTexture();
void UpdateTexture(uint32_t textureId, size_t width, size_t height, Color* rgbaData);

MeshRenderInfo CreateSurfaceMeshRenderInfo(SurfaceMesh& mesh);
void RenderMesh(const RenderBuffer& buffer, const Program& program, const MeshRenderInfo& info);

// 3D Camera
struct Camera
{
    static constexpr double DEFAULT_NEAR_CLIP = 0.005;
    static constexpr double DEFAULT_FAR_CLIP = 20.0;
    static constexpr double DEFAULT_FOV = 45.0;

    Mat4 viewMatrix = Identity();
    double lengthScale = 1.0f;
    Vec3d center = { 0 };
    double fov = DEFAULT_FOV;
    double nearClipRatio = DEFAULT_NEAR_CLIP;
    double farClipRatio = DEFAULT_FAR_CLIP;
};

Mat4 CameraGetViewMatrix(const Camera& c);
Mat4 CameraGetProjectionMatrix(const Camera& c, size_t width, size_t height);
void CameraGetFrame(const Camera& c, Vec3d& look, Vec3d& up, Vec3d& right);
void CameraFitBBox(Camera& c, const BBox& box);
void CameraProcessZoom(Camera& c, double amount);
void CameraProcessRotate(Camera& c, Vec2d start, Vec2d end);
void CameraProcessTranslate(Camera& c, Vec2d delta);
