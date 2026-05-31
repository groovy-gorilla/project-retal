#pragma once

#include "immintrin.h"
#include <cmath>



namespace lina {

    // FORWARD DECLARATION
    struct fvec2;
    struct fvec3;
    struct fvec4;
    struct fmat4;
    struct vec2;
    struct vec3;
    struct vec4;
    struct mat4;
    struct simd_fvec2;
    struct simd_fvec3;
    struct simd_fvec4;
    struct simd_fmat4;
    struct simd_vec2;
    struct simd_vec3;
    struct simd_vec4;
    struct simd_mat4;
    simd_fvec2 Load(const fvec2&);
    simd_fvec3 Load(const fvec3&);
    simd_fvec4 Load(const fvec4&);
    simd_fmat4 Load(const fmat4&);
    simd_vec2 Load(const vec2&);
    simd_vec3 Load(const vec3&);
    simd_vec4 Load(const vec4&);
    simd_mat4 Load(const mat4&);
    fvec2 Store(const simd_fvec2&);
    fvec3 Store(const simd_fvec3&);
    fvec4 Store(const simd_fvec4&);
    fmat4 Store(const simd_fmat4&);
    vec2 Store(const simd_vec2&);
    vec3 Store(const simd_vec3&);
    vec4 Store(const simd_vec4&);
    mat4 Store(const simd_mat4&);

    constexpr double PI = 3.14159265358979323846;

    // FLOAT VEC2 STRUCTURE
    struct fvec2 {
        float x;
        float y;
        fvec2() : x(0.0f), y(0.0f) {}
        fvec2(float x, float y) : x(x), y(y) {}
    };

    struct simd_fvec2 {
        __m128 v;
    };

    // FLOAT VEC3 STRUCTURE
    struct fvec3 {
        float x;
        float y;
        float z;
        fvec3() : x(0.0f), y(0.0f), z(0.0f) {}
        fvec3(float x, float y, float z) : x(x), y(y), z(z) {}
    };

    struct simd_fvec3 {
        __m128 v;
    };

    // FLOAT VEC4 STRUCTURE
    struct fvec4 {
        float x;
        float y;
        float z;
        float w;
        fvec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        fvec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    };

    struct simd_fvec4 {
        __m128 v;
    };

    // FLOAT MAT4 STRUCTURE
    struct fmat4 {
        fvec4 r[4];
    };

    struct simd_fmat4 {
        __m128 r[4];
    };

    // DOUBLE VEC2 STRUCTURE
    struct simd_vec2 {
        __m256d v;
    };

    struct vec2 {
        double x;
        double y;
        vec2() : x(0.0), y(0.0) {}
        vec2(double x, double y) : x(x), y(y) {}

        vec2 operator+(const vec2& other) const {
            simd_vec2 a = Load(*this);
            simd_vec2 b = Load(other);
            simd_vec2 r;
            r.v = _mm256_add_pd(a.v, b.v);
            return Store(r);
        }

        vec2 operator-(const vec2& other) const {
            simd_vec2 a = Load(*this);
            simd_vec2 b = Load(other);
            simd_vec2 r;
            r.v = _mm256_sub_pd(a.v, b.v);
            return Store(r);
        }

        vec2 operator*(double scalar) const {
            simd_vec2 a = Load(*this);
            simd_vec2 r;
            r.v = _mm256_mul_pd(a.v, _mm256_set1_pd(scalar));
            return Store(r);
        }
    };

    // DOUBLE VEC3 STRUCTURE
    struct simd_vec3 {
        __m256d v;
    };

    struct vec3 {
        double x;
        double y;
        double z;
        vec3() : x(0.0), y(0.0), z(0.0) {}
        vec3(double x, double y, double z) : x(x), y(y), z(z) {}

        vec3 operator+(const vec3& other) const {
            simd_vec3 a = Load(*this);
            simd_vec3 b = Load(other);
            simd_vec3 r;
            r.v = _mm256_add_pd(a.v, b.v);
            return Store(r);
        }

        vec3 operator-(const vec3& other) const {
            simd_vec3 a = Load(*this);
            simd_vec3 b = Load(other);
            simd_vec3 r;
            r.v = _mm256_sub_pd(a.v, b.v);
            return Store(r);
        }

        vec3 operator*(double scalar) const {
            simd_vec3 a = Load(*this);
            simd_vec3 r;
            r.v = _mm256_mul_pd(a.v, _mm256_set1_pd(scalar));
            return Store(r);
        }
    };

    // DOUBLE VEC4 STRUCTURE
    struct simd_vec4 {
        __m256d v;
    };

    struct vec4 {
        double x;
        double y;
        double z;
        double w;
        vec4() : x(0.0), y(0.0), z(0.0), w(0.0) {}
        vec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

        vec4 operator+(const vec4& other) const {
            simd_vec4 a = Load(*this);
            simd_vec4 b = Load(other);
            simd_vec4 r;
            r.v = _mm256_add_pd(a.v, b.v);
            return Store(r);
        }

        vec4 operator-(const vec4& other) const {
            simd_vec4 a = Load(*this);
            simd_vec4 b = Load(other);
            simd_vec4 r;
            r.v = _mm256_sub_pd(a.v, b.v);
            return Store(r);
        }

        vec4 operator*(double scalar) const {
            simd_vec4 a = Load(*this);
            simd_vec4 r;
            r.v = _mm256_mul_pd(a.v, _mm256_set1_pd(scalar));
            return Store(r);
        }
    };

    // DOUBLE MAT4 STRUCTURE
    struct simd_mat4 {
        __m256d r[4];
    };

    struct mat4 {

        vec4 r[4]{};

        static mat4 Identity() {
            mat4 result;
            result.r[0].x = 1.0;
            result.r[1].y = 1.0;
            result.r[2].z = 1.0;
            result.r[3].w = 1.0;
            return result;
        }

        mat4 operator+(const mat4& other) const {
            simd_mat4 A = Load(*this);
            simd_mat4 B = Load(other);
            simd_mat4 R;
            R.r[0] = _mm256_add_pd(A.r[0], B.r[0]);
            R.r[1] = _mm256_add_pd(A.r[1], B.r[1]);
            R.r[2] = _mm256_add_pd(A.r[2], B.r[2]);
            R.r[3] = _mm256_add_pd(A.r[3], B.r[3]);
            return Store(R);
        }

        mat4 operator-(const mat4& other) const {
            simd_mat4 A = Load(*this);
            simd_mat4 B = Load(other);
            simd_mat4 R;
            R.r[0] = _mm256_sub_pd(A.r[0], B.r[0]);
            R.r[1] = _mm256_sub_pd(A.r[1], B.r[1]);
            R.r[2] = _mm256_sub_pd(A.r[2], B.r[2]);
            R.r[3] = _mm256_sub_pd(A.r[3], B.r[3]);
            return Store(R);
        }

        mat4 operator*(double scalar) const {
            simd_mat4 A = Load(*this);
            simd_mat4 R;
            __m256d s = _mm256_set1_pd(scalar);
            R.r[0] = _mm256_mul_pd(A.r[0], s);
            R.r[1] = _mm256_mul_pd(A.r[1], s);
            R.r[2] = _mm256_mul_pd(A.r[2], s);
            R.r[3] = _mm256_mul_pd(A.r[3], s);
            return Store(R);
        }

        mat4 operator*(const mat4& other) const {
            simd_mat4 A = Load(*this);
            simd_mat4 B = Load(other);

            simd_mat4 R;

            __m256d ymm0, ymm1, ymm2, ymm3;

            // row 0
            ymm0 = _mm256_permute4x64_pd(A.r[0], _MM_SHUFFLE(0,0,0,0));
            ymm1 = _mm256_permute4x64_pd(A.r[0], _MM_SHUFFLE(1,1,1,1));
            ymm2 = _mm256_permute4x64_pd(A.r[0], _MM_SHUFFLE(2,2,2,2));
            ymm3 = _mm256_permute4x64_pd(A.r[0], _MM_SHUFFLE(3,3,3,3));

            ymm0 = _mm256_mul_pd(ymm0, B.r[0]);
            ymm1 = _mm256_mul_pd(ymm1, B.r[1]);
            ymm2 = _mm256_mul_pd(ymm2, B.r[2]);
            ymm3 = _mm256_mul_pd(ymm3, B.r[3]);

            ymm0 = _mm256_add_pd(ymm0, ymm1);
            ymm2 = _mm256_add_pd(ymm2, ymm3);

            R.r[0] = _mm256_add_pd(ymm0, ymm2);

            // row 1
            ymm0 = _mm256_permute4x64_pd(A.r[1], _MM_SHUFFLE(0,0,0,0));
            ymm1 = _mm256_permute4x64_pd(A.r[1], _MM_SHUFFLE(1,1,1,1));
            ymm2 = _mm256_permute4x64_pd(A.r[1], _MM_SHUFFLE(2,2,2,2));
            ymm3 = _mm256_permute4x64_pd(A.r[1], _MM_SHUFFLE(3,3,3,3));

            ymm0 = _mm256_mul_pd(ymm0, B.r[0]);
            ymm1 = _mm256_mul_pd(ymm1, B.r[1]);
            ymm2 = _mm256_mul_pd(ymm2, B.r[2]);
            ymm3 = _mm256_mul_pd(ymm3, B.r[3]);

            ymm0 = _mm256_add_pd(ymm0, ymm1);
            ymm2 = _mm256_add_pd(ymm2, ymm3);

            R.r[1] = _mm256_add_pd(ymm0, ymm2);

            // row 2
            ymm0 = _mm256_permute4x64_pd(A.r[2], _MM_SHUFFLE(0,0,0,0));
            ymm1 = _mm256_permute4x64_pd(A.r[2], _MM_SHUFFLE(1,1,1,1));
            ymm2 = _mm256_permute4x64_pd(A.r[2], _MM_SHUFFLE(2,2,2,2));
            ymm3 = _mm256_permute4x64_pd(A.r[2], _MM_SHUFFLE(3,3,3,3));

            ymm0 = _mm256_mul_pd(ymm0, B.r[0]);
            ymm1 = _mm256_mul_pd(ymm1, B.r[1]);
            ymm2 = _mm256_mul_pd(ymm2, B.r[2]);
            ymm3 = _mm256_mul_pd(ymm3, B.r[3]);

            ymm0 = _mm256_add_pd(ymm0, ymm1);
            ymm2 = _mm256_add_pd(ymm2, ymm3);

            R.r[2] = _mm256_add_pd(ymm0, ymm2);

            // row 3
            ymm0 = _mm256_permute4x64_pd(A.r[3], _MM_SHUFFLE(0,0,0,0));
            ymm1 = _mm256_permute4x64_pd(A.r[3], _MM_SHUFFLE(1,1,1,1));
            ymm2 = _mm256_permute4x64_pd(A.r[3], _MM_SHUFFLE(2,2,2,2));
            ymm3 = _mm256_permute4x64_pd(A.r[3], _MM_SHUFFLE(3,3,3,3));

            ymm0 = _mm256_mul_pd(ymm0, B.r[0]);
            ymm1 = _mm256_mul_pd(ymm1, B.r[1]);
            ymm2 = _mm256_mul_pd(ymm2, B.r[2]);
            ymm3 = _mm256_mul_pd(ymm3, B.r[3]);

            ymm0 = _mm256_add_pd(ymm0, ymm1);
            ymm2 = _mm256_add_pd(ymm2, ymm3);

            R.r[3] = _mm256_add_pd(ymm0, ymm2);

            return Store(R);
        }

    };


    // VECTORS DOT PRODUCT (ILOCZYN SKALARNY WEKTORÓW)
    inline double Dot(const vec2& a, const vec2& b) {
        simd_vec2 sa = Load(a);
        simd_vec2 sb = Load(b);
        __m256d mul = _mm256_mul_pd(sa.v, sb.v);
        alignas(32) double temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1];
    }

    inline double Dot(const vec3& a, const vec3& b) {
        simd_vec3 sa = Load(a);
        simd_vec3 sb = Load(b);
        __m256d mul = _mm256_mul_pd(sa.v, sb.v);
        alignas(32) double temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1];
    }

    inline double Dot(const vec4& a, const vec4& b) {
        simd_vec4 sa = Load(a);
        simd_vec4 sb = Load(b);
        __m256d mul = _mm256_mul_pd(sa.v, sb.v);
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
        simd_mat4 M = Load(m);
        simd_vec4 V = Load(v);

        __m256d xxxx = _mm256_permute4x64_pd(V.v, _MM_SHUFFLE(0, 0, 0, 0));
        __m256d yyyy = _mm256_permute4x64_pd(V.v, _MM_SHUFFLE(1, 1, 1, 1));
        __m256d zzzz = _mm256_permute4x64_pd(V.v, _MM_SHUFFLE(2, 2, 2, 2));
        __m256d wwww = _mm256_permute4x64_pd(V.v, _MM_SHUFFLE(3, 3, 3, 3));

        __m256d r0 = _mm256_mul_pd(M.r[0], xxxx);
        __m256d r1 = _mm256_mul_pd(M.r[1], yyyy);
        __m256d r2 = _mm256_mul_pd(M.r[2], zzzz);
        __m256d r3 = _mm256_mul_pd(M.r[3], wwww);

        __m256d sum0 = _mm256_add_pd(r0, r1);
        __m256d sum1 = _mm256_add_pd(r2, r3);

        simd_vec4 result;
        result.v = _mm256_add_pd(sum0, sum1);

        return Store(result);
    }

    inline mat4 Transpose(const mat4& m) {
        simd_mat4 M = Load(m);

        __m256d t0 = _mm256_unpacklo_pd(M.r[0], M.r[1]);
        __m256d t1 = _mm256_unpackhi_pd(M.r[0], M.r[1]);
        __m256d t2 = _mm256_unpacklo_pd(M.r[2], M.r[3]);
        __m256d t3 = _mm256_unpackhi_pd(M.r[2], M.r[3]);

        simd_mat4 R;

        R.r[0] = _mm256_permute2f128_pd(t0, t2, 0x20);
        R.r[1] = _mm256_permute2f128_pd(t1, t3, 0x20);
        R.r[2] = _mm256_permute2f128_pd(t0, t2, 0x31);
        R.r[3] = _mm256_permute2f128_pd(t1, t3, 0x31);

        return Store(R);
    }

    inline mat4 Translate(const vec3& translation) {
        mat4 result = mat4::Identity();

        simd_vec3 t = Load(translation);

        alignas(32) double temp[4];
        _mm256_store_pd(temp, t.v);

        result.r[3] = {
            temp[0],
            temp[1],
            temp[2],
            1.0
        };

        return result;
    }

    inline mat4 Scale(const vec3& scale)
    {
        simd_mat4 R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, scale.x);
        R.r[1] = _mm256_set_pd(0.0, 0.0, scale.y, 0.0);
        R.r[2] = _mm256_set_pd(0.0, scale.z, 0.0, 0.0);
        R.r[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 Scale(const double scale) {
        return Scale(vec3(scale, scale, scale));
    }

    inline mat4 RotateX(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);

        simd_mat4 R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, 1.0);
        R.r[1] = _mm256_set_pd(0.0, -s,  c, 0.0);
        R.r[2] = _mm256_set_pd(0.0,  c,  s, 0.0);
        R.r[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 RotateY(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);

        simd_mat4 R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0,  c);
        R.r[1] = _mm256_set_pd(0.0, 0.0, 1.0, 0.0);
        R.r[2] = _mm256_set_pd(0.0, 0.0, 0.0, -s);
        R.r[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 RotateZ(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);

        simd_mat4 R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, 1.0);
        R.r[1] = _mm256_set_pd(0.0, 0.0, -s, c);
        R.r[2] = _mm256_set_pd(0.0, 0.0,  c, s);
        R.r[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 RotateXYZ(double pitch, double yaw, double roll) {

        return RotateX(pitch) * RotateY(yaw) * RotateZ(roll);

    }

    inline mat4 Perspective(double fov, double aspect, double nearPlane, double farPlane) {
        double f = 1.0 / std::tan(fov * 0.5);
        simd_mat4 R;
        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, f / aspect);
        R.r[1] = _mm256_set_pd(0.0, 0.0, -f, 0.0);
        R.r[2] = _mm256_set_pd(-1.0, farPlane / (nearPlane - farPlane), 0.0, 0.0);
        R.r[3] = _mm256_set_pd(0.0, (nearPlane * farPlane) / (nearPlane - farPlane), 0.0, 0.0);
        return Store(R);
    }

    inline mat4 Orthographic(double left, double right, double bottom, double top, double nearPlane, double farPlane) {
        double rl = right - left;
        double tb = top - bottom;
        double fn = farPlane - nearPlane;
        simd_mat4 R;
        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, 2.0 / rl);
        R.r[1] = _mm256_set_pd(0.0, 0.0, 2.0 / tb, 0.0);
        R.r[2] = _mm256_set_pd(0.0, -1.0 / fn, 0.0, 0.0);
        R.r[3] = _mm256_set_pd(1.0, -nearPlane / fn, -(top + bottom) / tb, -(right + left) / rl);
        return Store(R);
    }





    // HELPERS
    inline double ToRadians(double degrees) {
        return degrees * (PI / 180.0);
    }

    inline fvec4 ToFloat(const vec4& v) {
        return {
            static_cast<float>(v.x),
            static_cast<float>(v.y),
            static_cast<float>(v.z),
            static_cast<float>(v.w)
        };
    }

    inline fmat4 ToFloat(const mat4& m) {
        return {
            ToFloat(m.r[0]),
            ToFloat(m.r[1]),
            ToFloat(m.r[2]),
            ToFloat(m.r[3])
        };
    }




    // LOAD
    inline simd_fvec2 Load(const fvec2& v) {
        return {
            _mm_set_ps(0.0f, 0.0f, v.y, v.x)
        };
    }

    inline simd_fvec3 Load(const fvec3& v) {
        return {
            _mm_set_ps(0.0f, v.z, v.y, v.x)
        };
    }

    inline simd_fvec4 Load(const fvec4& v) {
        return {
            _mm_set_ps(v.w, v.z, v.y, v.x)
        };
    }

    inline simd_fmat4 Load(const fmat4& m) {
        simd_fmat4 result;
        result.r[0] = _mm_load_ps(&m.r[0].x);
        result.r[1] = _mm_load_ps(&m.r[1].x);
        result.r[2] = _mm_load_ps(&m.r[2].x);
        result.r[3] = _mm_load_ps(&m.r[3].x);
        return result;
    }

    inline simd_vec2 Load(const vec2& v) {
        return {
            _mm256_set_pd(0.0f, 0.0f, v.y, v.x)
        };
    }

    inline simd_vec3 Load(const vec3& v) {
        return {
            _mm256_set_pd(0.0f, v.z, v.y, v.x)
        };
    }

    inline simd_vec4 Load(const vec4& v) {
        return {
            _mm256_set_pd(v.w, v.z, v.y, v.x)
        };
    }

    inline simd_mat4 Load(const mat4& m) {
        simd_mat4 result;
        result.r[0] = _mm256_load_pd(&m.r[0].x);
        result.r[1] = _mm256_load_pd(&m.r[1].x);
        result.r[2] = _mm256_load_pd(&m.r[2].x);
        result.r[3] = _mm256_load_pd(&m.r[3].x);
        return result;
    }

    // STORE
    inline fvec2 Store(const simd_fvec2& v) {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, v.v);
        return { tmp[0], tmp[1] };
    }

    inline fvec3 Store(const simd_fvec3& v) {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, v.v);
        return { tmp[0], tmp[1], tmp[2] };
    }

    inline fvec4 Store(const simd_fvec4& v) {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, v.v);
        return { tmp[0], tmp[1], tmp[2], tmp[3] };
    }

    inline fmat4 Store(const simd_fmat4& m) {
        fmat4 result;
        result.r[0] = Store(simd_fvec4{ m.r[0] });
        result.r[1] = Store(simd_fvec4{ m.r[1] });
        result.r[2] = Store(simd_fvec4{ m.r[2] });
        result.r[3] = Store(simd_fvec4{ m.r[3] });
        return result;
    }

    inline vec2 Store(const simd_vec2& v) {
        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, v.v);
        return { tmp[0], tmp[1] };
    }

    inline vec3 Store(const simd_vec3& v) {
        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, v.v);
        return { tmp[0], tmp[1], tmp[2] };
    }

    inline vec4 Store(const simd_vec4& v) {
        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, v.v);
        return { tmp[0], tmp[1], tmp[2], tmp[3] };
    }

    inline mat4 Store(const simd_mat4& m) {
        mat4 result;
        result.r[0] = Store(simd_vec4{ m.r[0] });
        result.r[1] = Store(simd_vec4{ m.r[1] });
        result.r[2] = Store(simd_vec4{ m.r[2] });
        result.r[3] = Store(simd_vec4{ m.r[3] });
        return result;
    }



    static_assert(sizeof(fvec2) == 8);
    static_assert(sizeof(fvec3) == 12);
    static_assert(sizeof(fvec4) == 16);
    static_assert(sizeof(fmat4) == 64);

}



