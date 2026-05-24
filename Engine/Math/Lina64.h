#pragma once

#include "immintrin.h"
#include <cmath>

namespace lina {

    constexpr double PI = 3.14159265358979323846;

    // FLOAT VEC2 STRUCTURE
    struct fvec2 {

        union {
            __m128 v;
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };

        fvec2() : v(_mm_setzero_ps()) {}
        fvec2(float x, float y) : v(_mm_set_ps(0.0f, 0.0f, y, x)) {}
        fvec2(__m128 value) : v(value) {}

    };

    // FLOAT VEC3 STRUCTURE
    struct fvec3 {

        union {
            __m128 v;
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };

        fvec3() : v(_mm_setzero_ps()) {}
        fvec3(float x, float y, float z) : v(_mm_set_ps(0.0f, z, y, x)) {}
        fvec3(__m128 value) : v(value) {}

    };

    // FLOAT VEC4 STRUCTURE
    struct fvec4 {

        union {
            __m128 v;
            struct {
                float x;
                float y;
                float z;
                float w;
            };
        };

        fvec4() : v(_mm_setzero_ps()) {}
        fvec4(float x, float y, float z, float w) : v(_mm_set_ps(w, z, y, x)) {}
        fvec4(__m128 value) : v(value) {}

    };

    // DOUBLE VEC2 STRUCTURE
    struct alignas(32) vec2 {

        union {
            __m256d v;
            struct {
                double x;
                double y;
                double z;
                double w;
            };
        };

        vec2() : v(_mm256_setzero_pd()) {}
        vec2(double x, double y) : v(_mm256_set_pd(0.0, 0.0, y, x)) {}
        vec2(__m256d value) : v(value) {}

        vec2 operator+(const vec2& other) const {
            return _mm256_add_pd(v, other.v);
        }

        vec2 operator-(const vec2& other) const {
            return _mm256_sub_pd(v, other.v);
        }

        vec2 operator*(double scalar) const {
            __m256d s = _mm256_set1_pd(scalar);
            return _mm256_mul_pd(v, s);
        }

    };

    // DOUBLE VEC3 STRUCTURE
    struct alignas(32) vec3 {

        union {
            __m256d v;
            struct {
                double x;
                double y;
                double z;
                double w;
            };
        };

        vec3() : v(_mm256_setzero_pd()) {}
        vec3(double x, double y, double z) : v(_mm256_set_pd(0.0, z, y, x)) {}
        vec3(__m256d value) : v(value) {}

        vec3 operator+(const vec3& other) const {
            return _mm256_add_pd(v, other.v);
        }

        vec3 operator-(const vec3& other) const {
            return _mm256_sub_pd(v, other.v);
        }

        vec3 operator*(double scalar) const {
            __m256d s = _mm256_set1_pd(scalar);
            return _mm256_mul_pd(v, s);
        }

    };

    // DOUBLE VEC4 STRUCTURE
    struct alignas(32) vec4 {

        union {
            __m256d v;
            struct {
                double x;
                double y;
                double z;
                double w;
            };
        };

        vec4() : v(_mm256_setzero_pd()) {}
        vec4(double x, double y, double z, double w) : v(_mm256_set_pd(w, z, y, x)) {}
        vec4(__m256d value) : v(value) {}

        vec4 operator+(const vec4& other) const {
            return _mm256_add_pd(v, other.v);
        }

        vec4 operator-(const vec4& other) const {
            return _mm256_sub_pd(v, other.v);
        }

        vec4 operator*(double scalar) const {
            __m256d s = _mm256_set1_pd(scalar);
            return _mm256_mul_pd(v, s);
        }

    };

    // DOUBLE MAT4 STRUCTURE
    struct mat4 {

        __m256d rows[4];

        mat4() {
            rows[0] = _mm256_setzero_pd();
            rows[1] = _mm256_setzero_pd();
            rows[2] = _mm256_setzero_pd();
            rows[3] = _mm256_setzero_pd();
        }

        static mat4 Identity() {
            mat4 result;
            result.rows[0] = _mm256_set_pd(0.0, 0.0, 0.0, 1.0);
            result.rows[1] = _mm256_set_pd(0.0, 0.0, 1.0, 0.0);
            result.rows[2] = _mm256_set_pd(0.0, 1.0, 0.0, 0.0);
            result.rows[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);
            return result;
        }

        mat4 operator+(const mat4& other) const {
            mat4 result;
            result.rows[0] = _mm256_add_pd(rows[0], other.rows[0]);
            result.rows[1] = _mm256_add_pd(rows[1], other.rows[1]);
            result.rows[2] = _mm256_add_pd(rows[2], other.rows[2]);
            result.rows[3] = _mm256_add_pd(rows[3], other.rows[3]);
            return result;
        }

        mat4 operator-(const mat4& other) const {
            mat4 result;
            result.rows[0] = _mm256_sub_pd(rows[0], other.rows[0]);
            result.rows[1] = _mm256_sub_pd(rows[1], other.rows[1]);
            result.rows[2] = _mm256_sub_pd(rows[2], other.rows[2]);
            result.rows[3] = _mm256_sub_pd(rows[3], other.rows[3]);
            return result;
        }

        mat4 operator*(double scalar) const {
            mat4 result;
            __m256d s = _mm256_set1_pd(scalar);
            result.rows[0] = _mm256_mul_pd(rows[0], s);
            result.rows[1] = _mm256_mul_pd(rows[1], s);
            result.rows[2] = _mm256_mul_pd(rows[2], s);
            result.rows[3] = _mm256_mul_pd(rows[3], s);
            return result;
        }

        mat4 operator*(const mat4& other) {
            mat4 R;
	        __m256d ymm0, ymm1, ymm2, ymm3;
	        // first row
	        ymm0 = _mm256_permute4x64_pd(rows[0], _MM_SHUFFLE(0, 0, 0, 0));
	        ymm1 = _mm256_permute4x64_pd(rows[0], _MM_SHUFFLE(1, 1, 1, 1));
	        ymm2 = _mm256_permute4x64_pd(rows[0], _MM_SHUFFLE(2, 2, 2, 2));
	        ymm3 = _mm256_permute4x64_pd(rows[0], _MM_SHUFFLE(3, 3, 3, 3));
	        ymm0 = _mm256_mul_pd(ymm0, other.rows[0]);
	        ymm1 = _mm256_mul_pd(ymm1, other.rows[1]);
	        ymm2 = _mm256_mul_pd(ymm2, other.rows[2]);
	        ymm3 = _mm256_mul_pd(ymm3, other.rows[3]);
	        ymm0 = _mm256_add_pd(ymm0, ymm1);
	        ymm2 = _mm256_add_pd(ymm2, ymm3);
	        ymm3 = _mm256_add_pd(ymm0, ymm2);
	        R.rows[0] = ymm3;
	        // second row
	        ymm0 = _mm256_permute4x64_pd(rows[1], _MM_SHUFFLE(0, 0, 0, 0));
	        ymm1 = _mm256_permute4x64_pd(rows[1], _MM_SHUFFLE(1, 1, 1, 1));
	        ymm2 = _mm256_permute4x64_pd(rows[1], _MM_SHUFFLE(2, 2, 2, 2));
	        ymm3 = _mm256_permute4x64_pd(rows[1], _MM_SHUFFLE(3, 3, 3, 3));
	        ymm0 = _mm256_mul_pd(ymm0, other.rows[0]);
	        ymm1 = _mm256_mul_pd(ymm1, other.rows[1]);
	        ymm2 = _mm256_mul_pd(ymm2, other.rows[2]);
	        ymm3 = _mm256_mul_pd(ymm3, other.rows[3]);
	        ymm0 = _mm256_add_pd(ymm0, ymm1);
	        ymm2 = _mm256_add_pd(ymm2, ymm3);
	        ymm3 = _mm256_add_pd(ymm0, ymm2);
	        R.rows[1] = ymm3;
	        // third row
	        ymm0 = _mm256_permute4x64_pd(rows[2], _MM_SHUFFLE(0, 0, 0, 0));
	        ymm1 = _mm256_permute4x64_pd(rows[2], _MM_SHUFFLE(1, 1, 1, 1));
	        ymm2 = _mm256_permute4x64_pd(rows[2], _MM_SHUFFLE(2, 2, 2, 2));
	        ymm3 = _mm256_permute4x64_pd(rows[2], _MM_SHUFFLE(3, 3, 3, 3));
	        ymm0 = _mm256_mul_pd(ymm0, other.rows[0]);
	        ymm1 = _mm256_mul_pd(ymm1, other.rows[1]);
	        ymm2 = _mm256_mul_pd(ymm2, other.rows[2]);
	        ymm3 = _mm256_mul_pd(ymm3, other.rows[3]);
	        ymm0 = _mm256_add_pd(ymm0, ymm1);
	        ymm2 = _mm256_add_pd(ymm2, ymm3);
	        ymm3 = _mm256_add_pd(ymm0, ymm2);
            R.rows[2] = ymm3;
	        // fourth row
	        ymm0 = _mm256_permute4x64_pd(rows[3], _MM_SHUFFLE(0, 0, 0, 0));
	        ymm1 = _mm256_permute4x64_pd(rows[3], _MM_SHUFFLE(1, 1, 1, 1));
	        ymm2 = _mm256_permute4x64_pd(rows[3], _MM_SHUFFLE(2, 2, 2, 2));
	        ymm3 = _mm256_permute4x64_pd(rows[3], _MM_SHUFFLE(3, 3, 3, 3));
	        ymm0 = _mm256_mul_pd(ymm0, other.rows[0]);
	        ymm1 = _mm256_mul_pd(ymm1, other.rows[1]);
	        ymm2 = _mm256_mul_pd(ymm2, other.rows[2]);
	        ymm3 = _mm256_mul_pd(ymm3, other.rows[3]);
	        ymm0 = _mm256_add_pd(ymm0, ymm1);
	        ymm2 = _mm256_add_pd(ymm2, ymm3);
	        ymm3 = _mm256_add_pd(ymm0, ymm2);
	        R.rows[3] = ymm3;
	        return R;
        }

    };

    // VECTORS DOT PRODUCT (ILOCZYN SKALARNY WEKTORÓW)
    inline double Dot(const vec2& a, const vec2& b) {
        __m256d mul = _mm256_mul_pd(a.v, b.v);
        alignas(32) double temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1];
    }

    inline double Dot(const vec3& a, const vec3& b) {
        __m256d mul = _mm256_mul_pd(a.v, b.v);
        alignas(32) double temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1];
    }

    inline double Dot(const vec4& a, const vec4& b) {
        __m256d mul = _mm256_mul_pd(a.v, b.v);
        alignas(32) double temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1];
    }

    // VECTOR LENGTH (DŁUGOŚĆ WEKTORA)
    inline double Length(const vec2& v) {
        return std::sqrt(Dot(v, v));
    }

    inline double Length(const vec3& v) {
        return std::sqrt(Dot(v, v));
    }

    inline double Length(const vec4& v) {
        return std::sqrt(Dot(v, v));
    }

    // VECTOR NORMALIZE (NORMALIZACJA WEKTORA)
    inline vec2 Normalize(const vec2& v) {
        double len = Length(v);
        if(len == 0.0) {
            return vec2();
        }
        return v * (1.0 / len);
    }

    inline vec3 Normalize(const vec3& v) {
        double len = Length(v);
        if(len == 0.0) {
            return vec3();
        }
        return v * (1.0 / len);
    }

    inline vec4 Normalize(const vec4& v) {
        double len = Length(v);
        if(len == 0.0) {
            return vec4();
        }
        return v * (1.0 / len);
    }

    // MNOŻENIE MACIERZY PRZEZ WEKTOR
    inline vec4 operator*(const mat4& m, const vec4& v) {

        vec4 result;

        __m256d xxxx = _mm256_permute4x64_pd(v.v, _MM_SHUFFLE(0, 0, 0, 0));
        __m256d yyyy = _mm256_permute4x64_pd(v.v, _MM_SHUFFLE(1, 1, 1, 1));
        __m256d zzzz = _mm256_permute4x64_pd(v.v, _MM_SHUFFLE(2, 2, 2, 2));
        __m256d wwww = _mm256_permute4x64_pd(v.v, _MM_SHUFFLE(3, 3, 3, 3));

        __m256d r0 = _mm256_mul_pd(m.rows[0], xxxx);
        __m256d r1 = _mm256_mul_pd(m.rows[1], yyyy);
        __m256d r2 = _mm256_mul_pd(m.rows[2], zzzz);
        __m256d r3 = _mm256_mul_pd(m.rows[3], wwww);

        __m256d sum0 = _mm256_add_pd(r0, r1);
        __m256d sum1 = _mm256_add_pd(r2, r3);

        result.v = _mm256_add_pd(sum0, sum1);

        return result;

    }

    inline mat4 Translate(const vec3& translation) {

        mat4 result = mat4::Identity();

        alignas(32) double temp[4];
        _mm256_store_pd(temp, translation.v);
        result.rows[3] = _mm256_set_pd(1.0, temp[2], temp[1], temp[0]);

        return result;

    }

    inline mat4 Scale(const vec3& scale) {

        alignas(32) double temp[4];

        _mm256_store_pd(temp, scale.v);

        mat4 result;
        result.rows[0] = _mm256_set_pd(0.0, 0.0, 0.0, temp[0]);
        result.rows[1] = _mm256_set_pd(0.0, 0.0, temp[1], 0.0);
        result.rows[2] = _mm256_set_pd(0.0, temp[2], 0.0, 0.0);
        result.rows[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return result;

    }

    inline mat4 Scale(const double scale) {
        return Scale(vec3(scale, scale, scale));
    }

    inline mat4 RotateX(double angle) {

        double c = std::cos(angle);
        double s = std::sin(angle);

        mat4 result = mat4::Identity();
        result.rows[1] = _mm256_set_pd(0.0, -s, c, 0.0);
        result.rows[2] = _mm256_set_pd(0.0, c, s, 0.0);

        return result;

    }

    inline mat4 RotateY(double angle) {

        double c = std::cos(angle);
        double s = std::sin(angle);

        mat4 result = mat4::Identity();
        result.rows[1] = _mm256_set_pd(0.0, s, 0.0, c);
        result.rows[2] = _mm256_set_pd(0.0, c, 0.0, -s);

        return result;

    }

    inline mat4 RotateZ(double angle) {

        double c = std::cos(angle);
        double s = std::sin(angle);

        mat4 result = mat4::Identity();
        result.rows[1] = _mm256_set_pd(0.0, 0.0, -s, c);
        result.rows[2] = _mm256_set_pd(0.0, 0.0, c, s);

        return result;

    }

    inline mat4 RotateXYZ(double pitch, double yaw, double roll) {

        return RotateX(pitch) * RotateY(yaw) * RotateZ(roll);

    }

    inline mat4 Perspective(double fov, double aspect, double nearPlane, double farPlane) {

        double f = 1.0 / std::tan(fov * 0.5);

        mat4 result{};
        result.rows[0] = _mm256_set_pd(0.0, 0.0, 0.0, f / aspect);
        result.rows[1] = _mm256_set_pd(0.0, 0.0, -f, 0.0);
        result.rows[2] = _mm256_set_pd(-1.0, farPlane / (nearPlane - farPlane), 0.0, 0.0);
        result.rows[3] = _mm256_set_pd(0.0, (nearPlane * farPlane) / (nearPlane - farPlane), 0.0, 0.0);

        return result;

    }

    mat4 Orthographic(double left, double right, double bottom, double top, double nearPlane, double farPlane) {

        mat4 result = mat4::Identity();
        double rl = right - left;
        double tb = top - bottom;
        double fn = farPlane - nearPlane;

        result.rows[0] = _mm256_set_pd(0.0, 0.0, 0.0, 2.0 / rl);
        result.rows[1] = _mm256_set_pd(0.0, 0.0, 2.0 / tb, 0.0);
        result.rows[2] = _mm256_set_pd(0.0, -1.0 / fn, 0.0, 0.0);
        result.rows[3] = _mm256_set_pd(1.0, -nearPlane / fn, -(top + bottom) / tb, -(right + left) / rl);

        return result;

    }





    // HELPERS
    double ToRadians(double degrees) {
        return degrees * (PI / 180.0);
    }



}



