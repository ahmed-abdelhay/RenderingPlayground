#include "Resha.h"
#include <math.h>

Vec2f operator+(const Vec2f& a, const Vec2f& b)
{
    return Vec2f{ a.x + b.x, a.y + b.y };
}
Vec2f operator-(const Vec2f& a, const Vec2f& b)
{
    return Vec2f{ a.x - b.x, a.y - b.y };
}
Vec2f operator*(const Vec2f& a, const Vec2f& b)
{
    return Vec2f{ a.x * b.x, a.y * b.y };
}
Vec2f operator*(const Vec2f& a, float x)
{
    return Vec2f{ a.x * x, a.y * x };
}
Vec2f operator*(float x, const Vec2f& a)
{
    return Vec2f{ a.x * x, a.y * x };
}

Vec3f operator+(const Vec3f& a, const Vec3f& b)
{
    return Vec3f{ a.x + b.x, a.y + b.y, a.z + b.z };
}
Vec3f operator-(const Vec3f& a, const Vec3f& b)
{
    return Vec3f{ a.x - b.x, a.y - b.y, a.z - b.z };
}
Vec3f operator*(const Vec3f& a, const Vec3f& b)
{
    return Vec3f{ a.x * b.x, a.y * b.y, a.z * b.z };
}
Vec3f operator*(const Vec3f& a, float x)
{
    return Vec3f{ a.x * x, a.y * x, a.z * x };
}
Vec3f operator*(float x, const Vec3f& a)
{
    return Vec3f{ a.x * x, a.y * x, a.z * x };
}

Vec2d operator+(const Vec2d& a, const Vec2d& b)
{
    return Vec2d{ a.x + b.x, a.y + b.y };
}
Vec2d operator-(const Vec2d& a, const Vec2d& b)
{
    return Vec2d{ a.x - b.x, a.y - b.y };
}
Vec2d operator*(const Vec2d& a, const Vec2d& b)
{
    return Vec2d{ a.x * b.x, a.y * b.y };
}
Vec2d operator*(const Vec2d& a, double x)
{
    return Vec2d{ a.x * x, a.y * x };
}
Vec2d operator*(double x, const Vec2d& a)
{
    return Vec2d{ a.x * x, a.y * x };
}

Vec3d operator+(const Vec3d& a, const Vec3d& b)
{
    return Vec3d{ a.x + b.x, a.y + b.y, a.z + b.z };
}
Vec3d operator-(const Vec3d& a, const Vec3d& b)
{
    return Vec3d{ a.x - b.x, a.y - b.y, a.z - b.z };
}
Vec3d operator*(const Vec3d& a, const Vec3d& b)
{
    return Vec3d{ a.x * b.x, a.y * b.y, a.z * b.z };
}
Vec3d operator*(const Vec3d& a, double x)
{
    return Vec3d{ a.x * x, a.y * x, a.z * x };
}
Vec3d operator*(double x, const Vec3d& a)
{
    return Vec3d{ a.x * x, a.y * x, a.z * x };
}

double DotProduct(const Vec3d& a, const Vec3d& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
double DotProduct(const Vec2d& a, const Vec2d& b)
{
    return a.x * b.x + a.y * b.y;
}
double DotProduct(const Vec3f& a, const Vec3f& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
double DotProduct(const Vec2f& a, const Vec2f& b)
{
    return a.x * b.x + a.y * b.y;
}

double DotProduct(const double a[4], const double b[4])
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
}

Mat4 operator*(const Mat4& left, double x)
{
    Mat4 r = left;
    for (int i = 0; i < 16; ++i)
    {
        r.data[i] *= x;
    }
    return r;
}

Mat4 operator*(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int i = 0; i < 4; ++i)
    {
        r.elements[0][i] = a.elements[0][i] * b.elements[0][0] +
            a.elements[1][i] * b.elements[0][1] +
            a.elements[2][i] * b.elements[0][2] +
            a.elements[3][i] * b.elements[0][3];
        r.elements[1][i] = a.elements[0][i] * b.elements[1][0] +
            a.elements[1][i] * b.elements[1][1] +
            a.elements[2][i] * b.elements[1][2] +
            a.elements[3][i] * b.elements[1][3];
        r.elements[2][i] = a.elements[0][i] * b.elements[2][0] +
            a.elements[1][i] * b.elements[2][1] +
            a.elements[2][i] * b.elements[2][2] +
            a.elements[3][i] * b.elements[2][3];
        r.elements[3][i] = a.elements[0][i] * b.elements[3][0] +
            a.elements[1][i] * b.elements[3][1] +
            a.elements[2][i] * b.elements[3][2] +
            a.elements[3][i] * b.elements[3][3];
    }
    return r;
}

Vec3d operator*(const Mat3& a, const Vec3d& v)
{
    const Vec3d v0{ a.elements[0][0], a.elements[0][1], a.elements[0][2] };
    const Vec3d v1{ a.elements[1][0], a.elements[1][1], a.elements[1][2] };
    const Vec3d v2{ a.elements[2][0], a.elements[2][1], a.elements[2][2] };
    return Vec3d{ DotProduct(v0, v), DotProduct(v1, v), DotProduct(v2, v) };
}

Vec3d CrossProduct(const Vec3d& a, const Vec3d& b)
{
    return Vec3d{ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                 a.x * b.y - a.y * b.x };
}

Vec3f CrossProduct(const Vec3f& a, const Vec3f& b)
{
    return Vec3f{ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                 a.x * b.y - a.y * b.x };
}

double Length(const Vec2d& v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}
double Length(const Vec3d& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

double Length(const Vec2f& v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}
double Length(const Vec3f& v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

void Normalise(Vec2d& v)
{
    const double length = Length(v);
    v.x /= length;
    v.y /= length;
}

Vec2d Normalised(const Vec2d& v)
{
    Vec2d result = v;
    Normalise(result);
    return result;
}

void Normalise(Vec3d& v)
{
    const double length = Length(v);
    v.x /= length;
    v.y /= length;
    v.z /= length;
}

Vec3d Normalised(const Vec3d& v)
{
    Vec3d result = v;
    Normalise(result);
    return result;
}

void Normalise(Vec2f& v)
{
    const double length = Length(v);
    v.x /= length;
    v.y /= length;
}

void Normalise(Vec3f& v)
{
    const double length = Length(v);
    v.x /= length;
    v.y /= length;
    v.z /= length;
}

Vec3f Normalised(const Vec3f& v)
{
    Vec3f result = v;
    Normalise(result);
    return result;
}

Vec2f Normalised(const Vec2f& v)
{
    Vec2f result = v;
    Normalise(result);
    return result;
}

Mat4 Transpose(Mat4 m)
{
    std::swap(m.elements[0][1], m.elements[1][0]);
    std::swap(m.elements[0][2], m.elements[2][0]);
    std::swap(m.elements[0][3], m.elements[3][0]);
    std::swap(m.elements[1][2], m.elements[2][1]);
    std::swap(m.elements[1][3], m.elements[3][1]);
    std::swap(m.elements[2][3], m.elements[3][2]);
    return m;
}

Mat3 Transpose(Mat3 m)
{
    std::swap(m.elements[0][1], m.elements[1][0]);
    std::swap(m.elements[0][2], m.elements[2][0]);
    std::swap(m.elements[1][2], m.elements[2][1]);
    return m;
}

Mat4 Translate(Mat4 m, const Vec3d& v)
{
    // m[3] = m[0] * v + m[1] * v + m[2] * v + m[3];
    for (int i = 0; i < 4; ++i)
    {
        m.elements[3][i] =
            m.elements[0][i] * v.data[0] +
            m.elements[1][i] * v.data[1] +
            m.elements[2][i] * v.data[2] +
            m.elements[3][i];
    }
    return m;
}

Mat4 Rotate(const Mat4& m, double angle, const Vec3d& v)
{
    const double c = cos(angle);
    const double s = sin(angle);

    const Vec3d axis = Normalised(v);
    const Vec3d temp = axis * (1. - c);

    Mat4 rotate;
    rotate.elements[0][0] = c + temp.data[0] * axis.data[0];
    rotate.elements[0][1] = temp.data[0] * axis.data[1] + s * axis.data[2];
    rotate.elements[0][2] = temp.data[0] * axis.data[2] - s * axis.data[1];

    rotate.elements[1][0] = temp.data[1] * axis.data[0] - s * axis.data[2];
    rotate.elements[1][1] = c + temp.data[1] * axis.data[1];
    rotate.elements[1][2] = temp.data[1] * axis.data[2] + s * axis.data[0];

    rotate.elements[2][0] = temp.data[2] * axis.data[0] + s * axis.data[1];
    rotate.elements[2][1] = temp.data[2] * axis.data[1] - s * axis.data[0];
    rotate.elements[2][2] = c + temp.data[2] * axis.data[2];

    Mat4 result = m;
    for (int i = 0; i < 4; ++i)
    {
        result.elements[0][i] = m.elements[0][i] * rotate.elements[0][0] +
            m.elements[1][i] * rotate.elements[0][1] +
            m.elements[2][i] * rotate.elements[0][2];
        result.elements[1][i] = m.elements[0][i] * rotate.elements[1][0] +
            m.elements[1][i] * rotate.elements[1][1] +
            m.elements[2][i] * rotate.elements[1][2];
        result.elements[2][i] = m.elements[0][i] * rotate.elements[2][0] +
            m.elements[1][i] * rotate.elements[2][1] +
            m.elements[2][i] * rotate.elements[2][2];
    }
    return result;
}

Mat4 Ortho(double left, double right, double bottom, double top, double nearVal,
           double farVal)
{
    const double rl = 1.0f / (right - left);
    const double tb = 1.0f / (top - bottom);
    const double fn = -1.0f / (farVal - nearVal);

    Mat4 dest = { 0 };
    dest.elements[0][0] = 2.0f * rl;
    dest.elements[1][1] = 2.0f * tb;
    dest.elements[2][2] = 2.0f * fn;
    dest.elements[3][0] = -(right + left) * rl;
    dest.elements[3][1] = -(top + bottom) * tb;
    dest.elements[3][2] = (farVal + nearVal) * fn;
    dest.elements[3][3] = 1.0f;

    return dest;
}

Mat4 Identity()
{
    Mat4 m = { 0 };
    m.elements[0][0] = m.elements[1][1] = m.elements[2][2] = m.elements[3][3] =
        1.0f;
    return m;
}

Mat4 LookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up)
{
    const Vec3d f = Normalised(center - eye);
    const Vec3d s = Normalised(CrossProduct(f, up));
    const Vec3d u = CrossProduct(s, f);

    Mat4 dest = { 0 };
    dest.elements[0][0] = s.data[0];
    dest.elements[0][1] = u.data[0];
    dest.elements[0][2] = -f.data[0];
    dest.elements[1][0] = s.data[1];
    dest.elements[1][1] = u.data[1];
    dest.elements[1][2] = -f.data[1];
    dest.elements[2][0] = s.data[2];
    dest.elements[2][1] = u.data[2];
    dest.elements[2][2] = -f.data[2];
    dest.elements[3][0] = -DotProduct(s, eye);
    dest.elements[3][1] = -DotProduct(u, eye);
    dest.elements[3][2] = DotProduct(f, eye);
    dest.elements[3][3] = 1.0f;
    return dest;
}

Mat4 Perspective(double fovy, double aspect, double zNear, double zFar)
{
    const double f = 1.0f / tan(fovy * 0.5f);
    const double fn = 1.0f / (zNear - zFar);
    Mat4 dest = { 0 };
    dest.elements[0][0] = f / aspect;
    dest.elements[1][1] = f;
    dest.elements[2][2] = (zNear + zFar) * fn;
    dest.elements[2][3] = -1.0f;
    dest.elements[3][2] = 2.0f * zNear * zFar * fn;
    return dest;
}

BBox Merge(const BBox& a, const BBox& b)
{
    BBox r = a;
    r.min.x = std::min(b.min.x, r.min.x);
    r.min.y = std::min(b.min.y, r.min.y);
    r.min.z = std::min(b.min.z, r.min.z);
    r.max.x = std::max(b.max.x, r.max.x);
    r.max.y = std::max(b.max.y, r.max.y);
    r.max.z = std::max(b.max.z, r.max.z);
    return r;
}

Vec3d CalculateBBoxCenter(const BBox& b)
{
    return Vec3d{ (b.min.x + b.max.x) / 2, (b.min.y + b.max.y) / 2,
                 (b.min.z + b.max.z) / 2 };
}

bool IsBBoxValid(const BBox& b)
{
    return b.min.x <= b.max.x && b.min.y <= b.max.y && b.min.z <= b.max.z;
}

void CalculateBBoxCorners(const BBox& b, Vec3d corners[8])
{
    corners[0] = Vec3d{ b.min.x, b.min.y, b.min.z };
    corners[1] = Vec3d{ b.min.x, b.min.y, b.max.z };
    corners[2] = Vec3d{ b.min.x, b.max.y, b.min.z };
    corners[3] = Vec3d{ b.min.x, b.max.y, b.max.z };
    corners[4] = Vec3d{ b.max.x, b.min.y, b.min.z };
    corners[5] = Vec3d{ b.max.x, b.min.y, b.max.z };
    corners[6] = Vec3d{ b.max.x, b.max.y, b.min.z };
    corners[7] = Vec3d{ b.max.x, b.max.y, b.max.z };
}

bool operator==(const Vec2d& a, const Vec2d& b)
{
    return a.x == b.x && a.y == b.y;
}
bool operator!=(const Vec2d& a, const Vec2d& b)
{
    return !(a == b);
}
bool operator==(const Vec3d& a, const Vec3d& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
bool operator!=(const Vec3d& a, const Vec3d& b)
{
    return !(a == b);
}
bool operator==(const Vec2f& a, const Vec2f& b)
{
    return a.x == b.x && a.y == b.y;
}
bool operator!=(const Vec2f& a, const Vec2f& b)
{
    return !(a == b);
}
bool operator==(const Vec3f& a, const Vec3f& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
bool operator!=(const Vec3f& a, const Vec3f& b)
{
    return !(a == b);
}

double Deg2Rad(double v)
{
    return v * (PI / 180);
}

double Rad2Deg(double v)
{
    return v * (180 / PI);
}

Color GenerateColor()
{
    static Color pregenerateList[]
    {
        Color{255, 0, 0, 255},
        Color{255, 255, 0, 255},
        Color{255, 0, 255, 255},
        Color{0, 255, 255, 255},
        Color{0, 0, 255, 255},
        Color{0, 255, 0, 255},
    };

    static const size_t listSize = sizeof(pregenerateList) / sizeof(Color);

    static size_t counter = 0;

    const Color result = pregenerateList[counter];
    counter = (counter + 1) % listSize;
    return result;
}