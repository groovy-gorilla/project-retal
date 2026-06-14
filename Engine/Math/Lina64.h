#pragma once

#include "immintrin.h"
#include <cmath>

constexpr float  PI_F      = 3.14159265358979323846f;
constexpr float  HALF_PI_F = PI_F * 0.5f;
constexpr float  TWO_PI_F  = PI_F * 2.0f;

constexpr double PI_D      = 3.1415926535897932384626433832795;
constexpr double HALF_PI_D = PI_D * 0.5;
constexpr double TWO_PI_D  = PI_D * 2.0;

namespace lina {

    union vec3;
    union fvec3;
    union mat4;
    union fmat4;
    inline mat4 Translate(double x, double y, double z);
    inline mat4 Translate(const vec3& t);
    inline fmat4 Translate(float x, float y, float z);
    inline fmat4 Translate(const fvec3& t);
    inline fmat4 Scale(float x, float y, float z);
    inline fmat4 Scale(const fvec3& s);
    inline mat4 Scale(double x, double y, double z);
    inline mat4 Scale(const vec3& s);
    inline fmat4 RotateX(float angle);
    inline mat4 RotateX(double angle);
    inline fmat4 RotateY(float angle);
    inline mat4 RotateY(double angle);
    inline fmat4 RotateZ(float angle);
    inline mat4 RotateZ(double angle);
    inline fmat4 RotateXYZ(float pitch, float yaw, float roll);
    inline mat4 RotateXYZ(double pitch, double yaw, double roll);




    // FLOAT VECTORS
    union alignas(16) fvec2 {

        __m128 v;

        struct {
            float x;
            float y;
        };

        fvec2() : v(_mm_setzero_ps()) {}
        explicit fvec2(float scalar) : v(_mm_set_ps(0.0f, 0.0f, scalar, scalar)) {}
        fvec2(float x, float y) : v(_mm_set_ps(0.0f, 0.0f, y, x)) {}

        fvec2 operator+(const fvec2& rhs) const {
            fvec2 result;
            result.v = _mm_add_ps(v, rhs.v);
            return result;
        }

        fvec2 operator-(const fvec2& rhs) const {
            fvec2 result;
            result.v = _mm_sub_ps(v, rhs.v);
            return result;
        }

        fvec2 operator*(float scalar) const {
            fvec2 result;
            result.v = _mm_mul_ps(v, _mm_set1_ps(scalar));
            return result;
        }

        fvec2 operator/(float scalar) const {
            fvec2 result;
            result.v = _mm_div_ps(v, _mm_set1_ps(scalar));
            return result;
        }

        fvec2& operator+=(const fvec2& rhs) {
            v = _mm_add_ps(v, rhs.v);
            return *this;
        }

        fvec2& operator-=(const fvec2& rhs) {
            v = _mm_sub_ps(v, rhs.v);
            return *this;
        }

        fvec2& operator*=(float scalar) {
            v = _mm_mul_ps(v, _mm_set1_ps(scalar));
            return *this;
        }

        fvec2& operator/=(float scalar) {
            v = _mm_div_ps(v, _mm_set1_ps(scalar));
            return *this;
        }

        fvec2 operator-() const {
            fvec2 result;
            result.v = _mm_sub_ps(_mm_setzero_ps(), v);
            return result;
        }

        bool operator==(const fvec2& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        bool operator!=(const fvec2& rhs) const {
            return !(*this == rhs);
        }

    };

    union alignas(16) fvec3 {

        __m128 v;

        struct {
            float x;
            float y;
            float z;
        };

        fvec3() : v(_mm_setzero_ps()) {}
        explicit fvec3(float scalar) : v(_mm_set_ps(0.0f, scalar, scalar, scalar)) {}
        fvec3(float x, float y, float z) : v(_mm_set_ps(0.0f, z, y, x)) {}

        fvec3 operator+(const fvec3& rhs) const {
            fvec3 result;
            result.v = _mm_add_ps(v, rhs.v);
            return result;
        }

        fvec3 operator-(const fvec3& rhs) const {
            fvec3 result;
            result.v = _mm_sub_ps(v, rhs.v);
            return result;
        }

        fvec3 operator*(float scalar) const {
            fvec3 result;
            result.v = _mm_mul_ps(v, _mm_set1_ps(scalar));
            return result;
        }

        fvec3 operator/(float scalar) const {
            fvec3 result;
            result.v = _mm_div_ps(v, _mm_set1_ps(scalar));
            return result;
        }

        fvec3& operator+=(const fvec3& rhs) {
            v = _mm_add_ps(v, rhs.v);
            return *this;
        }

        fvec3& operator-=(const fvec3& rhs) {
            v = _mm_sub_ps(v, rhs.v);
            return *this;
        }

        fvec3& operator*=(float scalar) {
            v = _mm_mul_ps(v, _mm_set1_ps(scalar));
            return *this;
        }

        fvec3& operator/=(float scalar) {
            v = _mm_div_ps(v, _mm_set1_ps(scalar));
            return *this;
        }

        fvec3 operator-() const {
            fvec3 result;
            result.v = _mm_sub_ps(_mm_setzero_ps(), v);
            return result;
        }

        bool operator==(const fvec3& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        bool operator!=(const fvec3& rhs) const {
            return !(*this == rhs);
        }

    };

    union alignas(16) fvec4 {

        __m128 v;

        struct {
            float x;
            float y;
            float z;
            float w;
        };

        fvec4() : v(_mm_setzero_ps()) {}
        explicit fvec4(float scalar) : v(_mm_set_ps(scalar, scalar, scalar, scalar)) {}
        fvec4(float x, float y, float z, float w) : v(_mm_set_ps(w, z, y, x)) {}

        fvec4 operator+(const fvec4& rhs) const {
            fvec4 result;
            result.v = _mm_add_ps(v, rhs.v);
            return result;
        }

        fvec4 operator-(const fvec4& rhs) const {
            fvec4 result;
            result.v = _mm_sub_ps(v, rhs.v);
            return result;
        }

        fvec4 operator*(float scalar) const {
            fvec4 result;
            result.v = _mm_mul_ps(v, _mm_set1_ps(scalar));
            return result;
        }

        fvec4 operator/(float scalar) const {
            fvec4 result;
            result.v = _mm_div_ps(v, _mm_set1_ps(scalar));
            return result;
        }

        fvec4& operator+=(const fvec4& rhs) {
            v = _mm_add_ps(v, rhs.v);
            return *this;
        }

        fvec4& operator-=(const fvec4& rhs) {
            v = _mm_sub_ps(v, rhs.v);
            return *this;
        }

        fvec4& operator*=(float scalar) {
            v = _mm_mul_ps(v, _mm_set1_ps(scalar));
            return *this;
        }

        fvec4& operator/=(float scalar) {
            v = _mm_div_ps(v, _mm_set1_ps(scalar));
            return *this;
        }

        fvec4 operator-() const {
            fvec4 result;
            result.v = _mm_sub_ps(_mm_setzero_ps(), v);
            return result;
        }

        bool operator==(const fvec4& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        bool operator!=(const fvec4& rhs) const {
            return !(*this == rhs);
        }

    };

    // DOUBLE VECTORS
    union alignas(32) vec2 {

        __m256d v;

        struct {
            double x;
            double y;
        };

        vec2() : v(_mm256_setzero_pd()) {}
        explicit vec2(double scalar) : v(_mm256_set_pd(0.0, 0.0, scalar, scalar)) {}
        vec2(double x, double y) : v(_mm256_set_pd(0.0, 0.0, y, x)) {}

        vec2 operator+(const vec2& rhs) const {
            vec2 result;
            result.v = _mm256_add_pd(v, rhs.v);
            return result;
        }

        vec2 operator-(const vec2& rhs) const {
            vec2 result;
            result.v = _mm256_sub_pd(v, rhs.v);
            return result;
        }

        vec2 operator*(float scalar) const {
            vec2 result;
            result.v = _mm256_mul_pd(v, _mm256_set1_pd(scalar));
            return result;
        }

        vec2 operator/(float scalar) const {
            vec2 result;
            result.v = _mm256_div_pd(v, _mm256_set1_pd(scalar));
            return result;
        }

        vec2& operator+=(const vec2& rhs) {
            v = _mm256_add_pd(v, rhs.v);
            return *this;
        }

        vec2& operator-=(const vec2& rhs) {
            v = _mm256_sub_pd(v, rhs.v);
            return *this;
        }

        vec2& operator*=(float scalar) {
            v = _mm256_mul_pd(v, _mm256_set1_pd(scalar));
            return *this;
        }

        vec2& operator/=(float scalar) {
            v = _mm256_div_pd(v, _mm256_set1_pd(scalar));
            return *this;
        }

        vec2 operator-() const {
            vec2 result;
            result.v = _mm256_sub_pd(_mm256_setzero_pd(), v);
            return result;
        }

        bool operator==(const vec2& rhs) const {
            return x == rhs.x && y == rhs.y;
        }

        bool operator!=(const vec2& rhs) const {
            return !(*this == rhs);
        }

    };

    union alignas(32) vec3 {

        __m256d v;

        struct {
            double x;
            double y;
            double z;
        };

        vec3() : v(_mm256_setzero_pd()) {}
        explicit vec3(double scalar) : v(_mm256_set_pd(0.0, scalar, scalar, scalar)) {}
        vec3(double x, double y, double z) : v(_mm256_set_pd(0.0, z, y, x)) {}

        vec3 operator+(const vec3& rhs) const {
            vec3 result;
            result.v = _mm256_add_pd(v, rhs.v);
            return result;
        }

        vec3 operator-(const vec3& rhs) const {
            vec3 result;
            result.v = _mm256_sub_pd(v, rhs.v);
            return result;
        }

        vec3 operator*(float scalar) const {
            vec3 result;
            result.v = _mm256_mul_pd(v, _mm256_set1_pd(scalar));
            return result;
        }

        vec3 operator/(float scalar) const {
            vec3 result;
            result.v = _mm256_div_pd(v, _mm256_set1_pd(scalar));
            return result;
        }

        vec3& operator+=(const vec3& rhs) {
            v = _mm256_add_pd(v, rhs.v);
            return *this;
        }

        vec3& operator-=(const vec3& rhs) {
            v = _mm256_sub_pd(v, rhs.v);
            return *this;
        }

        vec3& operator*=(float scalar) {
            v = _mm256_mul_pd(v, _mm256_set1_pd(scalar));
            return *this;
        }

        vec3& operator/=(float scalar) {
            v = _mm256_div_pd(v, _mm256_set1_pd(scalar));
            return *this;
        }

        vec3 operator-() const {
            vec3 result;
            result.v = _mm256_sub_pd(_mm256_setzero_pd(), v);
            return result;
        }

        bool operator==(const vec3& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }

        bool operator!=(const vec3& rhs) const {
            return !(*this == rhs);
        }

    };

    union alignas(32) vec4 {

        __m256d v;

        struct {
            double x;
            double y;
            double z;
            double w;
        };

        vec4() : v(_mm256_setzero_pd()) {}
        explicit vec4(double scalar) : v(_mm256_set_pd(scalar, scalar, scalar, scalar)) {}
        vec4(double x, double y, double z, double w) : v(_mm256_set_pd(w, z, y, x)) {}

        vec4 operator+(const vec4& rhs) const {
            vec4 result;
            result.v = _mm256_add_pd(v, rhs.v);
            return result;
        }

        vec4 operator-(const vec4& rhs) const {
            vec4 result;
            result.v = _mm256_sub_pd(v, rhs.v);
            return result;
        }

        vec4 operator*(float scalar) const {
            vec4 result;
            result.v = _mm256_mul_pd(v, _mm256_set1_pd(scalar));
            return result;
        }

        vec4 operator/(float scalar) const {
            vec4 result;
            result.v = _mm256_div_pd(v, _mm256_set1_pd(scalar));
            return result;
        }

        vec4& operator+=(const vec4& rhs) {
            v = _mm256_add_pd(v, rhs.v);
            return *this;
        }

        vec4& operator-=(const vec4& rhs) {
            v = _mm256_sub_pd(v, rhs.v);
            return *this;
        }

        vec4& operator*=(float scalar) {
            v = _mm256_mul_pd(v, _mm256_set1_pd(scalar));
            return *this;
        }

        vec4& operator/=(float scalar) {
            v = _mm256_div_pd(v, _mm256_set1_pd(scalar));
            return *this;
        }

        vec4 operator-() const {
            vec4 result;
            result.v = _mm256_sub_pd(_mm256_setzero_pd(), v);
            return result;
        }

        bool operator==(const vec4& rhs) const {
            return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
        }

        bool operator!=(const vec4& rhs) const {
            return !(*this == rhs);
        }

    };

    // FLOAT MATRIX
    union alignas(16) fmat4 {

        fvec4 col[4];

        fmat4() : col {
            fvec4(0.0f),
            fvec4(0.0f),
            fvec4(0.0f),
            fvec4(0.0f)
        } {}

        explicit fmat4(float diagonal) : col {
            fvec4(diagonal, 0.0f, 0.0f, 0.0f),
            fvec4(0.0f, diagonal, 0.0f, 0.0f),
            fvec4(0.0f, 0.0f, diagonal, 0.0f),
            fvec4(0.0f, 0.0f, 0.0f, diagonal)
        } {}

        fmat4(const fvec4& c0, const fvec4& c1, const fvec4& c2, const fvec4& c3) : col { c0, c1, c2, c3 } {}

        fvec4 operator*(const fvec4& rhs) const {

            __m128 xxxx = _mm_set1_ps(rhs.x);
            __m128 yyyy = _mm_set1_ps(rhs.y);
            __m128 zzzz = _mm_set1_ps(rhs.z);
            __m128 wwww = _mm_set1_ps(rhs.w);

            fvec4 result;

            result.v =
                _mm_add_ps(
                    _mm_add_ps(
                        _mm_mul_ps(col[0].v, xxxx),
                        _mm_mul_ps(col[1].v, yyyy)
                    ),
                    _mm_add_ps(
                        _mm_mul_ps(col[2].v, zzzz),
                        _mm_mul_ps(col[3].v, wwww)
                    )
                );

            return result;
        }

        fmat4 operator*(const fmat4& rhs) const {

            fmat4 result;

            result.col[0] = (*this) * rhs.col[0];
            result.col[1] = (*this) * rhs.col[1];
            result.col[2] = (*this) * rhs.col[2];
            result.col[3] = (*this) * rhs.col[3];

            return result;
        }

        fmat4& operator*=(const fmat4& rhs) {

            *this = *this * rhs;
            return *this;
        }

        fmat4 operator*(float scalar) const {

            __m128 s = _mm_set1_ps(scalar);

            fmat4 result;

            result.col[0].v = _mm_mul_ps(col[0].v, s);
            result.col[1].v = _mm_mul_ps(col[1].v, s);
            result.col[2].v = _mm_mul_ps(col[2].v, s);
            result.col[3].v = _mm_mul_ps(col[3].v, s);

            return result;
        }

        fmat4& operator*=(float scalar) {

            __m128 s = _mm_set1_ps(scalar);

            col[0].v = _mm_mul_ps(col[0].v, s);
            col[1].v = _mm_mul_ps(col[1].v, s);
            col[2].v = _mm_mul_ps(col[2].v, s);
            col[3].v = _mm_mul_ps(col[3].v, s);

            return *this;
        }



    };

    // DOUBLE MATRIX
    union alignas(32) mat4 {

        vec4 col[4];

        mat4() : col {
            vec4(0.0),
            vec4(0.0),
            vec4(0.0),
            vec4(0.0)
        } {}

        explicit mat4(double diagonal) : col {
            vec4(diagonal, 0.0, 0.0, 0.0),
            vec4(0.0, diagonal, 0.0, 0.0),
            vec4(0.0, 0.0, diagonal, 0.0),
            vec4(0.0, 0.0, 0.0, diagonal)
        } {}

        mat4(const vec4& c0, const vec4& c1, const vec4& c2, const vec4& c3) : col { c0, c1, c2, c3 } {}

        vec4 operator*(const vec4& rhs) const {

            __m256d xxxx = _mm256_set1_pd(rhs.x);
            __m256d yyyy = _mm256_set1_pd(rhs.y);
            __m256d zzzz = _mm256_set1_pd(rhs.z);
            __m256d wwww = _mm256_set1_pd(rhs.w);

            vec4 result;

            result.v =
                _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_mul_pd(col[0].v, xxxx),
                        _mm256_mul_pd(col[1].v, yyyy)
                    ),
                    _mm256_add_pd(
                        _mm256_mul_pd(col[2].v, zzzz),
                        _mm256_mul_pd(col[3].v, wwww)
                    )
                );

            return result;
        }

        mat4 operator*(const mat4& rhs) const {

            mat4 result;

            result.col[0] = (*this) * rhs.col[0];
            result.col[1] = (*this) * rhs.col[1];
            result.col[2] = (*this) * rhs.col[2];
            result.col[3] = (*this) * rhs.col[3];

            return result;
        }

        mat4& operator*=(const mat4& rhs) {

            *this = *this * rhs;
            return *this;
        }

        mat4 operator*(double scalar) const {

            __m256d s = _mm256_set1_pd(scalar);

            mat4 result;

            result.col[0].v = _mm256_mul_pd(col[0].v, s);
            result.col[1].v = _mm256_mul_pd(col[1].v, s);
            result.col[2].v = _mm256_mul_pd(col[2].v, s);
            result.col[3].v = _mm256_mul_pd(col[3].v, s);

            return result;
        }

        mat4& operator*=(double scalar) {

            __m256d s = _mm256_set1_pd(scalar);

            col[0].v = _mm256_mul_pd(col[0].v, s);
            col[1].v = _mm256_mul_pd(col[1].v, s);
            col[2].v = _mm256_mul_pd(col[2].v, s);
            col[3].v = _mm256_mul_pd(col[3].v, s);

            return *this;
        }

    };

    struct Transform {

        vec3 position;
        vec3 rotation;
        vec3 scale;

        Transform() : position(0.0, 0.0, 0.0), rotation(0.0, 0.0, 0.0), scale(1.0, 1.0, 1.0) {}

        mat4 GetLocalMatrix() const {
            return  Translate(position) * RotateXYZ(rotation.x, rotation.y, rotation.z) * Scale(scale);
        }
    };

    // ***********************************************************************************************************

    // DOT FVEC2/FVEC2
    inline float Dot(const fvec2& a, const fvec2& b) {

        __m128 dot = _mm_dp_ps(a.v, b.v, 0x31);

        return _mm_cvtss_f32(dot);
    }


    // DOT FVEC3/FVEC3
    inline float Dot(const fvec3& a, const fvec3& b) {

        __m128 dot = _mm_dp_ps(a.v, b.v, 0x71);

        return _mm_cvtss_f32(dot);
    }

    // DOT FVEC4/FVEC4
    inline float Dot(const fvec4& a, const fvec4& b) {

        __m128 dot = _mm_dp_ps(a.v, b.v, 0xF1);

        return _mm_cvtss_f32(dot);
    }

    // DOT VEC2/VEC2
    inline double Dot(const vec2& a, const vec2& b) {

        __m256d mul = _mm256_mul_pd(a.v, b.v);

        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, mul);

        return tmp[0] + tmp[1];
    }

    // DOT VEC3/VEC3
    inline double Dot(const vec3& a, const vec3& b) {

        __m256d mul = _mm256_mul_pd(a.v, b.v);

        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, mul);

        return tmp[0] + tmp[1] + tmp[2];
    }

    // DOT VEC4/VEC4
    inline double Dot(const vec4& a, const vec4& b) {

        __m256d mul = _mm256_mul_pd(a.v, b.v);

        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, mul);

        return tmp[0] + tmp[1] + tmp[2] + tmp[3];
    }

    // LENGTH FVEC2
    inline float Length(const fvec2& v) {
        return std::sqrt(Dot(v, v));
    }

    // LENGTH FVEC3
    inline float Length(const fvec3& v) {
        return std::sqrt(Dot(v, v));
    }

    // LENGTH FVEC4
    inline float Length(const fvec4& v) {
        return std::sqrt(Dot(v, v));
    }

    // LENGTH VEC2
    inline double Length(const vec2& v) {
        return std::sqrt(Dot(v, v));
    }

    // LENGTH VEC3
    inline double Length(const vec3& v) {
        return std::sqrt(Dot(v, v));
    }

    // LENGTH VEC4
    inline double Length(const vec4& v) {
        return std::sqrt(Dot(v, v));
    }

    // LENGTHSQUARED FVEC2
    inline float LengthSquared(const fvec2& v) {
        return Dot(v, v);
    }

    // LENGTHSQUARED FVEC3
    inline float LengthSquared(const fvec3& v) {
        return Dot(v, v);
    }

    // LENGTHSQUARED FVEC4
    inline float LengthSquared(const fvec4& v) {
        return Dot(v, v);
    }

    // LENGTHSQUARED VEC2
    inline double LengthSquared(const vec2& v) {
        return Dot(v, v);
    }

    // LENGTHSQUARED VEC3
    inline double LengthSquared(const vec3& v) {
        return Dot(v, v);
    }

    // LENGTHSQUARED VEC4
    inline double LengthSquared(const vec4& v) {
        return Dot(v, v);
    }

    // NORMALIZE FVEC2
    inline fvec2 Normalize(const fvec2& v) {

        float len = Length(v);

        if (len == 0.0f)
            return fvec2();

        __m128 invLen = _mm_set1_ps(1.0f / len);

        fvec2 result;
        result.v = _mm_mul_ps(v.v, invLen);

        return result;
    }

    // NORMALIZE FVEC3
    inline fvec3 Normalize(const fvec3& v) {

        float len = Length(v);

        if (len == 0.0f)
            return fvec3();

        __m128 invLen = _mm_set1_ps(1.0f / len);

        fvec3 result;
        result.v = _mm_mul_ps(v.v, invLen);

        return result;
    }

    // NORMALIZE FVEC4
    inline fvec4 Normalize(const fvec4& v) {

        float len = Length(v);

        if (len == 0.0f)
            return fvec4();

        __m128 invLen = _mm_set1_ps(1.0f / len);

        fvec4 result;
        result.v = _mm_mul_ps(v.v, invLen);

        return result;
    }

    // NORMALIZE VEC2
    inline vec2 Normalize(const vec2& v) {

        double len = Length(v);

        if (len == 0.0)
            return vec2();

        __m256d invLen = _mm256_set1_pd(1.0 / len);

        vec2 result;
        result.v = _mm256_mul_pd(v.v, invLen);

        return result;
    }

    // NORMALIZE VEC3
    inline vec3 Normalize(const vec3& v) {

        double len = Length(v);

        if (len == 0.0)
            return vec3();

        __m256d invLen = _mm256_set1_pd(1.0 / len);

        vec3 result;
        result.v = _mm256_mul_pd(v.v, invLen);

        return result;
    }

    // NORMALIZE VEC4
    inline vec4 Normalize(const vec4& v) {

        double len = Length(v);

        if (len == 0.0)
            return vec4();

        __m256d invLen = _mm256_set1_pd(1.0 / len);

        vec4 result;
        result.v = _mm256_mul_pd(v.v, invLen);

        return result;
    }

    // CROSS FVEC3
    inline fvec3 Cross(const fvec3& a, const fvec3& b) {

        return fvec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    // CROSS VEC3
    inline vec3 Cross(const vec3& a, const vec3& b) {

        return vec3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    // TRANSPOSE FMAT4
    inline fmat4 Transpose(const fmat4& m) {

        return fmat4(
            fvec4(
                m.col[0].x,
                m.col[1].x,
                m.col[2].x,
                m.col[3].x
            ),

            fvec4(
                m.col[0].y,
                m.col[1].y,
                m.col[2].y,
                m.col[3].y
            ),

            fvec4(
                m.col[0].z,
                m.col[1].z,
                m.col[2].z,
                m.col[3].z
            ),

            fvec4(
                m.col[0].w,
                m.col[1].w,
                m.col[2].w,
                m.col[3].w
            )
        );
    }

    // TRANSPOSE MAT4
    inline mat4 Transpose(const mat4& m) {

        return mat4(
            vec4(
                m.col[0].x,
                m.col[1].x,
                m.col[2].x,
                m.col[3].x
            ),

            vec4(
                m.col[0].y,
                m.col[1].y,
                m.col[2].y,
                m.col[3].y
            ),

            vec4(
                m.col[0].z,
                m.col[1].z,
                m.col[2].z,
                m.col[3].z
            ),

            vec4(
                m.col[0].w,
                m.col[1].w,
                m.col[2].w,
                m.col[3].w
            )
        );
    }

    // TRANSLATE FLOAT
    inline fmat4 Translate(float x, float y, float z) {

        return fmat4(
            fvec4(1.0f, 0.0f, 0.0f, 0.0f),
            fvec4(0.0f, 1.0f, 0.0f, 0.0f),
            fvec4(0.0f, 0.0f, 1.0f, 0.0f),
            fvec4(x,    y,    z,    1.0f)
        );
    }

    // TRANSLATE FVEC3
    inline fmat4 Translate(const fvec3& t) {

        return Translate(
            t.x,
            t.y,
            t.z
        );
    }

    // TRANSLATE DOUBLE
    inline mat4 Translate(double x, double y, double z) {

        return mat4(
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(x,   y,   z,   1.0)
        );
    }

    // TRANSLATE VEC3
    inline mat4 Translate(const vec3& t) {

        return Translate(
            t.x,
            t.y,
            t.z
        );
    }

    // SCALE FLOAT
    inline fmat4 Scale(float x, float y, float z) {

        return fmat4(
            fvec4(x,    0.0f, 0.0f, 0.0f),
            fvec4(0.0f, y,    0.0f, 0.0f),
            fvec4(0.0f, 0.0f, z,    0.0f),
            fvec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    // SCALE FVEC3
    inline fmat4 Scale(const fvec3& s) {

        return Scale(
            s.x,
            s.y,
            s.z
        );
    }

    // SCALE DOUBLE
    inline mat4 Scale(double x, double y, double z) {

        return mat4(
            vec4(x,   0.0, 0.0, 0.0),
            vec4(0.0, y,   0.0, 0.0),
            vec4(0.0, 0.0, z,   0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );
    }

    // SCALE VEC3
    inline mat4 Scale(const vec3& s) {

        return Scale(
            s.x,
            s.y,
            s.z
        );
    }

    // ROTATE X
    inline fmat4 RotateX(float angle) {

        float c = std::cos(angle);
        float s = std::sin(angle);

        return fmat4(
            fvec4(1.0f, 0.0f, 0.0f, 0.0f),
            fvec4(0.0f, c,    s,    0.0f),
            fvec4(0.0f,-s,    c,    0.0f),
            fvec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    inline mat4 RotateX(double angle) {

        double c = std::cos(angle);
        double s = std::sin(angle);

        return mat4(
            vec4(1.0, 0.0, 0.0, 0.0),
            vec4(0.0, c,   s,   0.0),
            vec4(0.0,-s,   c,   0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );
    }

    // ROTATE Y
    inline fmat4 RotateY(float angle) {

        float c = std::cos(angle);
        float s = std::sin(angle);

        return fmat4(
            fvec4( c,   0.0f, -s,   0.0f),
            fvec4(0.0f, 1.0f, 0.0f, 0.0f),
            fvec4( s,   0.0f,  c,   0.0f),
            fvec4(0.0f, 0.0f, 0.0f, 1.0f)
        );
    }

    inline mat4 RotateY(double angle) {

        double c = std::cos(angle);
        double s = std::sin(angle);

        return mat4(
            vec4( c,  0.0, -s,  0.0),
            vec4(0.0, 1.0, 0.0, 0.0),
            vec4( s,  0.0,  c,  0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );
    }

    // ROTATE Z
    inline fmat4 RotateZ(float angle) {

        float c = std::cos(angle);
        float s = std::sin(angle);

        return fmat4(
            fvec4( c,   s,   0.0f, 0.0f),
            fvec4(-s,   c,   0.0f, 0.0f),
            fvec4(0.0f, 0.0f,1.0f, 0.0f),
            fvec4(0.0f, 0.0f,0.0f, 1.0f)
        );
    }

    inline mat4 RotateZ(double angle) {

        double c = std::cos(angle);
        double s = std::sin(angle);

        return mat4(
            vec4( c,  s,  0.0, 0.0),
            vec4(-s,  c,  0.0, 0.0),
            vec4(0.0, 0.0, 1.0, 0.0),
            vec4(0.0, 0.0, 0.0, 1.0)
        );
    }

    // ROTATE XYZ
    inline fmat4 RotateXYZ(
        float pitch,
        float yaw,
        float roll)
    {
        return
            RotateY(yaw) *
            RotateX(pitch) *
            RotateZ(roll);
    }

    inline mat4 RotateXYZ(
       double pitch,
       double yaw,
       double roll)
    {
        return
            RotateY(yaw) *
            RotateX(pitch) *
            RotateZ(roll);
    }

    // PERSPECTIVE FLOAT
    inline fmat4 Perspective(
        float fov,
        float aspect,
        float nearPlane,
        float farPlane)
    {
        float f = 1.0f / std::tan(fov * 0.5f);

        fmat4 result;

        result.col[0] = fvec4(
            f / aspect,
            0.0f,
            0.0f,
            0.0f
        );

        result.col[1] = fvec4(
            0.0f,
            f,
            0.0f,
            0.0f
        );

        result.col[2] = fvec4(
            0.0f,
            0.0f,
            nearPlane / (farPlane - nearPlane),
            -1.0f
        );

        result.col[3] = fvec4(
            0.0f,
            0.0f,
            (farPlane * nearPlane) / (farPlane - nearPlane),
            0.0f
        );

        return result;
    }

    // PERSPECTIVE DOUBLE
    inline mat4 Perspective(
        double fov,
        double aspect,
        double nearPlane,
        double farPlane)
    {
        double f = 1.0 / std::tan(fov * 0.5);

        mat4 result;

        result.col[0] = vec4(
            f / aspect,
            0.0,
            0.0,
            0.0
        );

        result.col[1] = vec4(
            0.0,
            f,
            0.0,
            0.0
        );

        result.col[2] = vec4(
            0.0,
            0.0,
            nearPlane / (farPlane - nearPlane),
            -1.0
        );

        result.col[3] = vec4(
            0.0,
            0.0,
            (farPlane * nearPlane) / (farPlane - nearPlane),
            0.0
        );

        return result;
    }

    // ORTHOGRAPHIC FLOAT
    inline fmat4 Orthographic(
        float left,
        float right,
        float bottom,
        float top,
        float nearPlane,
        float farPlane)
    {
        fmat4 result(1.0f);

        result.col[0].x = 2.0f / (right - left);

        result.col[1].y = 2.0f / (top - bottom);

        result.col[2].z = 1.0f / (farPlane - nearPlane);

        result.col[3].x = -(right + left) / (right - left);
        result.col[3].y = -(top + bottom) / (top - bottom);
        result.col[3].z = -nearPlane / (farPlane - nearPlane);

        return result;
    }

    // ORTHOGRAPGIC DOUBLE
    inline mat4 Orthographic(
        double left,
        double right,
        double bottom,
        double top,
        double nearPlane,
        double farPlane)
    {
        mat4 result(1.0);

        result.col[0].x = 2.0 / (right - left);

        result.col[1].y = 2.0 / (bottom - top);

        result.col[2].z = 1.0 / (farPlane - nearPlane);

        result.col[3].x = -(right + left) / (right - left);
        result.col[3].y = -(top + bottom) / (bottom - top);
        result.col[3].z = -nearPlane / (farPlane - nearPlane);

        return result;
    }

    // LOOKAT FLOAT
    inline fmat4 LookAt(
        const fvec3& eye,
        const fvec3& target,
        const fvec3& up)
    {
        fvec3 forward = Normalize(target - eye);

        fvec3 right =
            Normalize(
                Cross(up, forward)
            );

        fvec3 cameraUp =
            Cross(right, forward);

        fmat4 result(1.0f);

        result.col[0] = fvec4(
            right.x,
            cameraUp.x,
            -forward.x,
            0.0f
        );

        result.col[1] = fvec4(
            right.y,
            cameraUp.y,
            -forward.y,
            0.0f
        );

        result.col[2] = fvec4(
            right.z,
            cameraUp.z,
            -forward.z,
            0.0f
        );

        result.col[3] = fvec4(
            -Dot(right, eye),
            -Dot(cameraUp, eye),
             Dot(forward, eye),
            1.0f
        );

        return result;
    }

    // LOOKAT DOUBLE
    inline mat4 LookAt(
        const vec3& eye,
        const vec3& target,
        const vec3& up)
    {
        vec3 forward = Normalize(target - eye);

        vec3 right =
            Normalize(
                Cross(up, forward)
            );

        vec3 cameraUp =
            Cross(right, forward);

        mat4 result(1.0);

        result.col[0] = vec4(
            right.x,
            cameraUp.x,
            -forward.x,
            0.0
        );

        result.col[1] = vec4(
            right.y,
            cameraUp.y,
            -forward.y,
            0.0
        );

        result.col[2] = vec4(
            right.z,
            cameraUp.z,
            -forward.z,
            0.0
        );

        result.col[3] = vec4(
            -Dot(right, eye),
            -Dot(cameraUp, eye),
             Dot(forward, eye),
            1.0
        );

        return result;
    }

    // VEC2 -> FVEC2
    inline fvec2 ToFloat(const vec2& v) {
        return fvec2(
            static_cast<float>(v.x),
            static_cast<float>(v.y)
        );
    }

    // VEC3 -> FVEC3
    inline fvec3 ToFloat(const vec3& v) {
        return fvec3(
            static_cast<float>(v.x),
            static_cast<float>(v.y),
            static_cast<float>(v.z)
        );
    }

    // VEC4 -> FVEC4
    inline fvec4 ToFloat(const vec4& v) {
        return fvec4(
            static_cast<float>(v.x),
            static_cast<float>(v.y),
            static_cast<float>(v.z),
            static_cast<float>(v.w)
        );
    }

    // MAT4 -> FMAT4
    inline fmat4 ToFloat(const mat4& m) {

        return fmat4(
            ToFloat(m.col[0]),
            ToFloat(m.col[1]),
            ToFloat(m.col[2]),
            ToFloat(m.col[3])
        );
    }

    inline vec2 ToDouble(const fvec2& v) {
        return vec2(v.x, v.y);
    }

    inline vec3 ToDouble(const fvec3& v) {
        return vec3(v.x, v.y, v.z);
    }

    inline vec4 ToDouble(const fvec4& v) {
        return vec4(v.x, v.y, v.z, v.w);
    }

    inline mat4 ToDouble(const fmat4& m) {

        return mat4(
            ToDouble(m.col[0]),
            ToDouble(m.col[1]),
            ToDouble(m.col[2]),
            ToDouble(m.col[3])
        );
    }

    // TO RADIANS
    inline constexpr float ToRadians(float degrees) {
        return degrees * static_cast<float>(PI_F / 180.0);
    }

    inline constexpr double ToRadians(double degrees) {
        return degrees * (PI_D / 180.0);
    }

    // TO DEGREES
    inline constexpr float ToDegrees(float radians) {
        return radians * static_cast<float>(180.0 / PI_F);
    }

    inline constexpr double ToDegrees(double radians) {
        return radians * (180.0 / PI_D);
    }

    // LERP
    inline float Lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    inline double Lerp(double a, double b, double t) {
        return a + (b - a) * t;
    }


}