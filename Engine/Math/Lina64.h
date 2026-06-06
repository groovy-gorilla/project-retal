#pragma once

#include "immintrin.h"
#include <cmath>

namespace lina {

    typedef __m128 simd_fvec;
    typedef __m256d simd_vec;

    struct simd_fmat {
        simd_fvec r[4];
    };

    struct simd_mat {
        simd_vec r[4];
    };

    // FORWARD DECLARATION
    struct fvec2;
    struct fvec3;
    struct fvec4;
    struct fmat4;
    struct vec2;
    struct vec3;
    struct vec4;
    struct mat4;

    simd_fvec Load(const fvec2&);
    simd_fvec Load(const fvec3&);
    simd_fvec Load(const fvec4&);
    simd_fmat Load(const fmat4&);

    simd_vec Load(const vec2&);
    simd_vec Load(const vec3&);
    simd_vec Load(const vec4&);
    simd_mat Load(const mat4&);

    fvec2 Store2(const simd_fvec&);
    fvec3 Store3(const simd_fvec&);
    fvec4 Store4(const simd_fvec&);
    fmat4 Store(const simd_fmat&);

    vec2 Store2(const simd_vec&);
    vec3 Store3(const simd_vec&);
    vec4 Store4(const simd_vec&);
    mat4 Store(const simd_mat&);

    constexpr double PI = 3.14159265358979323846;

    // FLOAT VEC2 STRUCTURE
    struct fvec2 {
        float x;
        float y;
        fvec2() : x(0.0f), y(0.0f) {}
        fvec2(float x, float y) : x(x), y(y) {}
    };

    // FLOAT VEC3 STRUCTURE
    struct fvec3 {
        float x;
        float y;
        float z;
        fvec3() : x(0.0f), y(0.0f), z(0.0f) {}
        fvec3(float x, float y, float z) : x(x), y(y), z(z) {}
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

    // FLOAT MAT4 STRUCTURE
    struct fmat4 {
        fvec4 r[4];
    };

    // DOUBLE VEC2 STRUCTURE
    struct vec2 {
        double x;
        double y;
        vec2() : x(0.0), y(0.0) {}
        vec2(double x, double y) : x(x), y(y) {}

        vec2 operator+(const vec2& other) const {
            return vec2(this->x + other.x, this->y + other.y);
        }

        vec2 operator-(const vec2& other) const {
            return vec2(this->x - other.x, this->y - other.y);
        }

        vec2 operator*(const vec2& other) const {
            return vec2(this->x * other.x, this->y * other.y);
        }

        vec2 operator*(double scalar) const {
            return vec2(this->x * scalar, this->y * scalar);
        }
    };

    // DOUBLE VEC3 STRUCTURE
    struct vec3 {
        double x;
        double y;
        double z;
        vec3() : x(0.0), y(0.0), z(0.0) {}
        vec3(double x, double y, double z) : x(x), y(y), z(z) {}

        vec3 operator+(const vec3& other) const {
            return vec3(this->x + other.x, this->y + other.y, this->z + other.z);
        }

        vec3 operator-(const vec3& other) const {
            return vec3(this->x - other.x, this->y - other.y, this->z - other.z);
        }

        vec3 operator*(const vec3& other) const {
            return vec3(this->x * other.x, this->y * other.y, this->z * other.z);
        }

        vec3 operator*(double scalar) const {
            return vec3(this->x * scalar, this->y * scalar, this->z * scalar);
        }
    };

    // DOUBLE VEC4 STRUCTURE
    struct vec4 {
        double x;
        double y;
        double z;
        double w;
        vec4() : x(0.0), y(0.0), z(0.0), w(0.0) {}
        vec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

        vec4 operator+(const vec4& other) const {
            return vec4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
        }

        vec4 operator-(const vec4& other) const {
            return vec4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
        }

        vec4 operator*(const vec4& other) const {
            return vec4(this->x * other.x, this->y * other.y, this->z * other.z, this->w * other.w);
        }

        vec4 operator*(double scalar) const {
            return vec4(this->x * scalar, this->y * scalar, this->z * scalar, this->w * scalar);
        }
    };

    // DOUBLE MAT4 STRUCTURE
    struct mat4 {

        vec4 r[4]{};

        mat4() {}
        explicit mat4(const vec4 *pArray) {
            r[0] = pArray[0]; r[1] = pArray[1]; r[2] = pArray[2]; r[3] = pArray[3];
        }

        static mat4 Identity() {
            mat4 result{};
            result.r[0].x = 1.0;
            result.r[1].y = 1.0;
            result.r[2].z = 1.0;
            result.r[3].w = 1.0;
            return result;
        }

        mat4 operator+(const mat4& other) const {
            vec4 v[4];
            for (int i=0; i<4; ++i) v[i] = this->r[i] + other.r[i];
            return mat4(v);
        }

        mat4 operator-(const mat4& other) const {
            vec4 v[4];
            for (int i=0; i<4; ++i) v[i] = this->r[i] - other.r[i];
            return mat4(v);
        }

        mat4 operator*(double scalar) const {
            vec4 v[4];
            for (int i=0; i<4; ++i) v[i] = this->r[i] * scalar;
            return mat4(v);
        }

        mat4 operator*(const mat4& other) const {
            simd_mat A = Load(*this);
            simd_mat B = Load(other);

            simd_mat R;

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
        simd_vec sa = Load(a);
        simd_vec sb = Load(b);
        __m256d mul = _mm256_mul_pd(sa, sb);
        alignas(32) double temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1];
    }

    inline double Dot(const vec3& a, const vec3& b) {
        simd_vec sa = Load(a);
        simd_vec sb = Load(b);
        __m256d mul = _mm256_mul_pd(sa, sb);
        alignas(32) double_t temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1] + temp[2];
    }

    inline double Dot(const vec4& a, const vec4& b) {
        simd_vec sa = Load(a);
        simd_vec sb = Load(b);
        __m256d mul = _mm256_mul_pd(sa, sb);
        alignas(32) double temp[4];
        _mm256_store_pd(temp, mul);
        return temp[0] + temp[1] + temp[2] + temp[3];
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
    inline vec4 operator*(const vec4& v, const mat4& m) {
        simd_vec V = Load(v);
        simd_mat M = Load(m);

        __m256d xxxx = _mm256_permute4x64_pd(V, _MM_SHUFFLE(0,0,0,0));
        __m256d yyyy = _mm256_permute4x64_pd(V, _MM_SHUFFLE(1,1,1,1));
        __m256d zzzz = _mm256_permute4x64_pd(V, _MM_SHUFFLE(2,2,2,2));
        __m256d wwww = _mm256_permute4x64_pd(V, _MM_SHUFFLE(3,3,3,3));

        simd_vec R =
            _mm256_add_pd(
                _mm256_add_pd(
                    _mm256_mul_pd(xxxx, M.r[0]),
                    _mm256_mul_pd(yyyy, M.r[1])),
                _mm256_add_pd(
                    _mm256_mul_pd(zzzz, M.r[2]),
                    _mm256_mul_pd(wwww, M.r[3]))
            );

        return Store4(R);
    }

    inline mat4 Transpose(const mat4& m) {
        simd_mat M = Load(m);

        __m256d t0 = _mm256_unpacklo_pd(M.r[0], M.r[1]);
        __m256d t1 = _mm256_unpackhi_pd(M.r[0], M.r[1]);
        __m256d t2 = _mm256_unpacklo_pd(M.r[2], M.r[3]);
        __m256d t3 = _mm256_unpackhi_pd(M.r[2], M.r[3]);

        simd_mat R;

        R.r[0] = _mm256_permute2f128_pd(t0, t2, 0x20);
        R.r[1] = _mm256_permute2f128_pd(t1, t3, 0x20);
        R.r[2] = _mm256_permute2f128_pd(t0, t2, 0x31);
        R.r[3] = _mm256_permute2f128_pd(t1, t3, 0x31);

        return Store(R);
    }

    inline mat4 Translate(const vec3& translation) {
        mat4 result = mat4::Identity();

        result.r[3].x = translation.x;
        result.r[3].y = translation.y;
        result.r[3].z = translation.z;

        return result;
    }

    inline mat4 Scale(const vec3& scale)
    {
        simd_mat R;

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

        simd_mat R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, 1.0);
        R.r[1] = _mm256_set_pd(0.0,  s,   c, 0.0);
        R.r[2] = _mm256_set_pd(0.0,  c,  -s, 0.0);
        R.r[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 RotateY(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);

        simd_mat R;

        R.r[0] = _mm256_set_pd(0.0,  -s, 0.0,  c);
        R.r[1] = _mm256_set_pd(0.0, 0.0, 1.0, 0.0);
        R.r[2] = _mm256_set_pd(0.0,  c, 0.0, s);
        R.r[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 RotateZ(double angle) {
        double c = std::cos(angle);
        double s = std::sin(angle);

        simd_mat R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, s,  c);
        R.r[1] = _mm256_set_pd(0.0, 0.0,  c,  -s);
        R.r[2] = _mm256_set_pd(0.0, 1.0, 0.0, 0.0);
        R.r[3] = _mm256_set_pd(1.0, 0.0, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 RotateXYZ(double pitch, double yaw, double roll) {

        return RotateX(pitch) * RotateY(yaw) * RotateZ(roll);

    }

    inline mat4 Perspective(double fov, double aspect, double nearPlane, double farPlane) {
        double f = 1.0 / std::tan(fov * 0.5);

        simd_mat R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, f / aspect);
        R.r[1] = _mm256_set_pd(0.0, 0.0, f, 0.0);
        R.r[2] = _mm256_set_pd(-1.0, farPlane / (nearPlane - farPlane), 0.0, 0.0);
        R.r[3] = _mm256_set_pd(0.0, (nearPlane * farPlane) / (nearPlane - farPlane), 0.0, 0.0);

        return Store(R);
    }

    inline mat4 PerspectiveReverseZInfinite(double fov, double aspect, double nearPlane) {
        double f = 1.0 / std::tan(fov * 0.5);

        simd_mat R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, f / aspect);
        R.r[1] = _mm256_set_pd(0.0, 0.0, -f, 0.0);
        R.r[2] = _mm256_set_pd(-1.0, 0.0, 0.0, 0.0);
        R.r[3] = _mm256_set_pd(0.0, nearPlane, 0.0, 0.0);

        return Store(R);
    }

    inline mat4 Orthographic(double left, double right, double bottom, double top, double nearPlane, double farPlane) {
        double rl = right - left;
        double tb = bottom - top;
        double fn = farPlane - nearPlane;

        simd_mat R;

        R.r[0] = _mm256_set_pd(0.0, 0.0, 0.0, 2.0 / rl);
        R.r[1] = _mm256_set_pd(0.0, 0.0, 2.0 / tb, 0.0);
        R.r[2] = _mm256_set_pd(0.0, 1.0 / fn, 0.0, 0.0);
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
    inline simd_fvec Load(const fvec2& v) {
        return _mm_set_ps(0.0f, 0.0f, v.y, v.x);
    }

    inline simd_fvec Load(const fvec3& v) {
        return _mm_set_ps(0.0f, v.z, v.y, v.x);
    }

    inline simd_fvec Load(const fvec4& v) {
        return _mm_set_ps(v.w, v.z, v.y, v.x);
    }

    inline simd_fmat Load(const fmat4& m) {
        simd_fmat result;
        result.r[0] = _mm_loadu_ps(&m.r[0].x);
        result.r[1] = _mm_loadu_ps(&m.r[1].x);
        result.r[2] = _mm_loadu_ps(&m.r[2].x);
        result.r[3] = _mm_loadu_ps(&m.r[3].x);
        return result;
    }

    inline simd_vec Load(const vec2& v) {
        return {
            _mm256_set_pd(0.0f, 0.0f, v.y, v.x)
        };
    }

    inline simd_vec Load(const vec3& v) {
        return _mm256_set_pd(0.0f, v.z, v.y, v.x);
    }

    inline simd_vec Load(const vec4& v) {
        return {
            _mm256_set_pd(v.w, v.z, v.y, v.x)
        };
    }

    inline simd_mat Load(const mat4& m) {
        simd_mat result;
        result.r[0] = _mm256_loadu_pd(&m.r[0].x);
        result.r[1] = _mm256_loadu_pd(&m.r[1].x);
        result.r[2] = _mm256_loadu_pd(&m.r[2].x);
        result.r[3] = _mm256_loadu_pd(&m.r[3].x);
        return result;
    }

    // STORE
    inline fvec2 Store2(const simd_fvec& v) {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, v);
        return { tmp[0], tmp[1] };
    }

    inline fvec3 Store3(const simd_fvec& v) {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, v);
        return { tmp[0], tmp[1], tmp[2] };
    }

    inline fvec4 Store4(const simd_fvec& v) {
        alignas(16) float tmp[4];
        _mm_store_ps(tmp, v);
        return { tmp[0], tmp[1], tmp[2], tmp[3] };
    }

    inline fmat4 Store(const simd_fmat& m) {
        fmat4 result;
        result.r[0] = Store4(simd_fvec{ m.r[0] });
        result.r[1] = Store4(simd_fvec{ m.r[1] });
        result.r[2] = Store4(simd_fvec{ m.r[2] });
        result.r[3] = Store4(simd_fvec{ m.r[3] });
        return result;
    }

    inline vec2 Store2(const simd_vec& v) {
        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, v);
        return { tmp[0], tmp[1] };
    }

    inline vec3 Store3(const simd_vec& v) {
        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, v);
        return { tmp[0], tmp[1], tmp[2] };
    }

    inline vec4 Store4(const simd_vec& v) {
        alignas(32) double tmp[4];
        _mm256_store_pd(tmp, v);
        return { tmp[0], tmp[1], tmp[2], tmp[3] };
    }

    inline mat4 Store(const simd_mat& m) {
        mat4 result;
        result.r[0] = Store4(simd_vec{ m.r[0] });
        result.r[1] = Store4(simd_vec{ m.r[1] });
        result.r[2] = Store4(simd_vec{ m.r[2] });
        result.r[3] = Store4(simd_vec{ m.r[3] });
        return result;
    }

}



