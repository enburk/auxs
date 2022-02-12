#pragma once
#include "aux_aux.h"
namespace aux
{
    template<class T> inline T min () { return std::numeric_limits<T>::lowest(); } // !!!
    template<class T> inline T max () { return std::numeric_limits<T>::max(); }
    template<class T> inline T min (T a, T b) { return (a < b ? a : b); }
    template<class T> inline T max (T a, T b) { return (a < b ? b : a); }
    template<class T> inline T min (T a, T b, T c) { return min(a, min(b, c)); }
    template<class T> inline T max (T a, T b, T c) { return max(a, max(b, c)); }
    template<class T> inline T min (T a, T b, T c, T d) { return min(a, min(b, min(c, d))); }
    template<class T> inline T max (T a, T b, T c, T d) { return max(a, max(b, max(c, d))); }

    template<class T, class U>
    T clamp (U value, T Min = min<T>(), T Max = max<T>()) {
        return
            value <= Min ? Min :
            value >= Max ? Max :
            std::clamp(T(value), Min, Max);
    }

    template <typename Int = int>
    Int random (Int l = min<Int>(), Int u = max<Int>())
    {
        thread_local std::random_device seed;
        thread_local std::mt19937 generator(seed());
        return std::uniform_int_distribution(l,u)(generator);
    }

    auto sum (auto... xs) { return (xs + ...); }

    template<class T, int D> struct vector_data { std::array<T, D> data{}; void resize(int) {} }; // value-initialized
    template<class T> struct vector_data<T, 1> { union { std::array<T, 1> data{}; struct { T x; }; }; void resize(int) {} };
    template<class T> struct vector_data<T, 2> { union { std::array<T, 2> data{}; struct { T x, y; }; }; void resize(int) {} };
    template<class T> struct vector_data<T, 3> { union { std::array<T, 3> data{}; struct { T x, y, z; }; }; void resize(int) {} };
    template<class T> struct vector_data<T, 4> { union { std::array<T, 4> data{}; struct { T x, y, z, w; }; }; void resize(int) {} };
    template<class T> struct vector_data<T, 0> { std::vector<T> data; void resize(int n) { data.resize(n); } };

    template<int D, class T = float> struct vector : vector_data<T, D>
    {
        using vector_data_ = vector_data<T, D>;
        using vector_data_::data;

        vector() = default;
        template<int N, class U> vector(const U(&list)[N]) {
            static_assert(D == 0 || D == N, "wrong initializer list size");
            vector_data_::resize(N); // does nothing in case of std::array
            int i = 0; for (auto value : list) data[i++] = clamp<T>(value);
        }

        T  operator () (int n) const { return data[n]; }
        T& operator () (int n) /***/ { return data[n]; }

        template<class scalar> vector& operator += (scalar p) { for (auto & value : data) value = clamp<T>(value + p); return *this; }
        template<class scalar> vector& operator -= (scalar p) { for (auto & value : data) value = clamp<T>(value - p); return *this; }
        template<class scalar> vector& operator *= (scalar p) { for (auto & value : data) value = clamp<T>(value * p); return *this; }
        template<class scalar> vector& operator /= (scalar p) { for (auto & value : data) value = clamp<T>(value / p); return *this; }

        void operator += (const vector & v) { for (int i = 0; i < (int)data.size(); i++) (*this)(i) = clamp<T>((*this)(i) + v(i)); }
        void operator -= (const vector & v) { for (int i = 0; i < (int)data.size(); i++) (*this)(i) = clamp<T>((*this)(i) - v(i)); }

        bool operator == (const vector & v) const { return data == v.data; }
        bool operator != (const vector & v) const { return data != v.data; }

        double  norm() const { return sqrt(double(norm2())); }
        auto    norm2() const { return (*this) * (*this); }
        vector& normalize() { *this /= norm(); return *this; }
        vector  normalized() const { return (*this) / norm(); }
    };

    template<int D, class T> inline double distance(const vector<D, T> & v1, const vector<D, T> & v2) { return (v2 - v1).norm(); }

    template<int D, class T> inline vector<D, T> operator * (vector<D, T> v, double n) { v *= n; return v; };
    template<int D, class T> inline vector<D, T> operator / (vector<D, T> v, double n) { v /= n; return v; };
    template<int D, class T> inline vector<D, T> operator * (double n, vector<D, T> v) { v *= n; return v; };

    template<int D, class T> inline vector<D, T> operator - (vector<D, T> v) { for (auto & value : v.data) value = -value; return v; } // unary -
    template<int D, class T> inline vector<D, T> operator + (vector<D, T> v) { return v; }; // unary +

    template<int D, class T> inline vector<D, T> operator + (vector<D, T> v1, const vector<D, T> & v2) { v1 += v2; return v1; }
    template<int D, class T> inline vector<D, T> operator - (vector<D, T> v1, const vector<D, T> & v2) { v1 -= v2; return v1; }
    template<int D, class T> inline auto operator * (const vector<D, T> & v1, const vector<D, T> & v2)
    {
        return std::inner_product(v1.data.begin(), v1.data.end(), v2.data.begin(), decltype(v1.norm())());
    }
}
