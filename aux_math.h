#pragma once
#include <numbers>
#include "aux_aux.h"
namespace aux
{
    template<class T> T min () { return std::numeric_limits<T>::lowest(); } // !!!
    template<class T> T max () { return std::numeric_limits<T>::max(); }
    template<class T> T min (T a, T b) { return (a < b ? a : b); }
    template<class T> T max (T a, T b) { return (a < b ? b : a); }
    template<class T> T min (T a, T b, T c) { return min(a, min(b, c)); }
    template<class T> T max (T a, T b, T c) { return max(a, max(b, c)); }
    template<class T> T min (T a, T b, T c, T d) { return min(a, min(b, min(c, d))); }
    template<class T> T max (T a, T b, T c, T d) { return max(a, max(b, max(c, d))); }

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

    // vector

    namespace vector_data {
    template<class T, int D> struct s { std::array<T,D> data{}; }; // value-initialized
    template<class T> struct s<T,1> { union { std::array<T,1> data{}; struct { T x; }; }; };
    template<class T> struct s<T,2> { union { std::array<T,2> data{}; struct { T x,y; }; }; };
    template<class T> struct s<T,3> { union { std::array<T,3> data{}; struct { T x,y,z; }; }; };
    template<class T> struct s<T,4> { union { std::array<T,4> data{}; struct { T x,y,z,w; }; }; };
    template<class T> struct s<T,0> { std::vector<T> data; }; }

    template<int D, class T=double> struct vector : vector_data::s<T,D>
    {
        using data_type = vector_data::s<T,D>;
        using data_type::data;

        vector() = default;
        constexpr
        vector(auto... xs) requires (D>0)
            : data_type{clamp<T>(xs)...} {
            static_assert(D == sizeof...(xs),
                "not enough initializers"); }
        constexpr
        vector(auto... xs) requires (D==0) {
            data.reserve(sizeof...(xs));
            data.insert(data.end(),
                {clamp<T>(xs)...}); }

        int size () const { return (int)(data.size()); }
        T  operator [] (int n) const { return data[n]; }
        T& operator [] (int n) /***/ { return data[n]; }

        vector& operator += (auto scalar) { for (auto& x: data) x = clamp<T>(x + scalar); return *this; }
        vector& operator -= (auto scalar) { for (auto& x: data) x = clamp<T>(x - scalar); return *this; }
        vector& operator *= (auto scalar) { for (auto& x: data) x = clamp<T>(x * scalar); return *this; }
        vector& operator /= (auto scalar) { for (auto& x: data) x = clamp<T>(x / scalar); return *this; }

        void operator += (vector const& v) { int i=0; for (auto& x: data) x = clamp<T>(x + v[i++]); }
        void operator -= (vector const& v) { int i=0; for (auto& x: data) x = clamp<T>(x - v[i++]); }

        bool operator == (vector const& v) const { return data == v.data; }
        bool operator != (vector const& v) const { return data != v.data; }

        double  norm() const { return sqrt(double(norm2())); }
        auto    norm2() const { return (*this) * (*this); }
        vector& normalize() { *this /= norm(); return *this; }
        vector  normalized() const { return (*this) / norm(); }

        vector rotated (auto a) const requires (D==2) {
        return vector { // rotation is from x-axis toward y-axis:
            cos(a)*data[0] - sin(a)*data[1], // if x:right y:up then counterclockwise
            sin(a)*data[0] + cos(a)*data[1]  // if x:right y:down then clockwise
        };}
    };
    template<class T> using Vector = vector<0,T>;

    #define template template<int D, class T>
    #define vector vector<D,T>
    template auto operator * (vector v, double n) { v *= n; return v; };
    template auto operator / (vector v, double n) { v /= n; return v; };
    template auto operator * (double n, vector v) { v *= n; return v; };
    template auto operator + (vector v) { return v; }; // unary +
    template auto operator - (vector v) // unary -
    {
        for (auto& value: v.data)
        value = -value;
        return v;
    }
    template auto operator + (vector v1, vector const& v2) { v1 += v2; return v1; }
    template auto operator - (vector v1, vector const& v2) { v1 -= v2; return v1; }
    template auto operator * (vector const& v1, vector const& v2)
    {
        return std::inner_product(
            v1.data.begin(), v1.data.end(),
            v2.data.begin(), decltype(v1.norm())());
    }
    template auto distance (vector const& v1, vector const& v2)
    {
        return (v2 - v1).norm();
    }
    #undef template
    #undef vector

    // matrix

    template<class T, int Y, int X> struct matrix_data
    {
        std::array<T,Y*X> data{};
        enum {
            yy = Y,
            xx = X,
            nn = Y*X };
    };
    template<class T> struct matrix_data<T,0,0>
    {
        std::vector<T> data;
        int yy = 0,
            xx = 0,
            nn = 0;
    };
    template<int Y, int X, class T=double> struct matrix : matrix_data<T,Y,X>
    {
        using data_type = matrix_data<T,Y,X>;
        using data_type::data;
        using data_type::yy;
        using data_type::xx;
        using data_type::nn;

        matrix() = default;
        constexpr
        matrix(auto... xs) requires (Y>0 and X>0)
            : data_type{clamp<T>(xs)...}
            { static_assert(Y*X == sizeof...(xs),
                "not enough initializers"); }
        constexpr
        matrix(int yy, int xx) requires (Y==0 and X==0)
            : yy(yy), xx(xx), nn(yy*xx)
            { data.resize(yy*xx); }

        const T* operator [] (int y) const { return data.data() + y*yy; }
        /***/ T* operator [] (int y) /***/ { return data.data() + y*yy; }

        matrix& operator += (auto scalar) { for (auto& x: data) x = clamp<T>(x + scalar); return *this; }
        matrix& operator -= (auto scalar) { for (auto& x: data) x = clamp<T>(x - scalar); return *this; }
        matrix& operator *= (auto scalar) { for (auto& x: data) x = clamp<T>(x * scalar); return *this; }
        matrix& operator /= (auto scalar) { for (auto& x: data) x = clamp<T>(x / scalar); return *this; }

        void operator += (matrix const& m) { for (int i=0; i<nn; i++) data[i] = clamp<T>(data[i] + m.data[i]); }
        void operator -= (matrix const& m) { for (int i=0; i<nn; i++) data[i] = clamp<T>(data[i] - m.data[i]); }

        bool operator == (matrix const& m) const { return data == m.data; }
        bool operator != (matrix const& m) const { return data != m.data; }

        vector<X,T> row (int y) const { vector<X,T> v; for (int i=0; i<X; i++) v[i] = (*this)[y][i]; return v; }
        vector<Y,T> col (int x) const { vector<Y,T> v; for (int i=0; i<Y; i++) v[i] = (*this)[i][x]; return v; }

        static inline constexpr 
        matrix identity () requires (Y == X) {
        matrix identity;
            for (int i=0; i<yy; i++)
            identity[i][i] = 1;
            return identity; }

        matrix& transpose () requires (Y == X) {
            for (int i=0;   i<yy; i++)
            for (int j=i+1; j<xx; j++)
            std::swap((*this)[j][i], (*this)[i][j]);
            return *this; }

        matrix<X,Y,T> transposed () const {
        matrix<X,Y,T> transposed;
            for (int i=0; i<yy; i++)
            for (int j=0; j<xx; j++)
            transposed[j][i] = (*this)[i][j];
            return transposed; }

        matrix<Y-1,X-1,T> minor (int y, int x) const {
        matrix<Y-1,X-1,T> minor;
            for (int i=0; i<yy-1; i++)
            for (int j=0; j<xx-1; j++)
                minor[j][i] = (*this)
                [i<y ? i : i+1]
                [j<x ? j : j+1];
                return minor; }
    };
    template<class T> using Matrix = matrix<0,0,T>;

    #define template template<int Y, int X, class T>
    #define matrix matrix<Y,X,T>
    template auto operator * (matrix m, double n) { m *= n; return m; };
    template auto operator / (matrix m, double n) { m /= n; return m; };
    template auto operator * (double n, matrix m) { m *= n; return m; };
    template auto operator + (matrix m) { return m; }; // unary +
    template auto operator - (matrix m) // unary -
    {
        for (auto& value: m.data)
        value = -value;
        return m;
    }
    template auto operator + (matrix m1, matrix const& m2) { m1 += m2; return m1; }
    template auto operator - (matrix m1, matrix const& m2) { m1 -= m2; return m1; }
    #undef template
    #undef matrix

    template<int Y, int X, class T>
    vector<X,T> operator * (vector<Y,T> const& v, matrix<Y,X,T> const& m) {
    vector<X,T> r; for (int i=0; i<X; i++) r[i] = v * m.col(i); return r; }

    template<int Y, int X, class T>
    vector<Y,T> operator * (matrix<Y,X,T> const& m, vector<X,T> const& v) {
    vector<Y,T> r; for (int i=0; i<Y; i++) r[i] = m.row(i) * v; return r; }

    template<int N1, int N2, int N3, class T>
    matrix<N1,N3,T> operator * (
    matrix<N1,N2,T> const& m1,
    matrix<N2,N3,T> const& m2) {
    matrix<N1,N3,T> m;
        for (int i=0; i<N1; i++)
        for( int j=0; j<N3; j++)
        m[i][j] = clamp<T>(m1.row(i) * m2.col(j));
        return m; }

    // homogeneous coordinates
    // right-handed counterclockwise

    auto rotation_x (double a) { double c = cos(a), s = sin(a); return matrix<4,4>
    {
        1,  0,  0,  0,
        0,  c, -s,  0,
        0,  s,  c,  0,
        0,  0,  0,  1
    };}
    auto rotation_y (double a) { double c = cos(a), s = sin(a); return matrix<4,4>
    {
        c,  0,  s,  0,
        0,  1,  0,  0,
       -s,  0,  c,  0,
        0,  0,  0,  1
    };}
    auto rotation_z (double a) { double c = cos(a), s = sin(a); return matrix<4,4>
    {
        c, -s,  0,  0,
        s,  c,  0,  0,
        0,  0,  1,  0,
        0,  0,  0,  1
    };}
    auto translation (double x, double y, double z) { return matrix<4,4>
    {
        1,  0,  0,  x,
        0,  1,  0,  y,
        0,  0,  1,  z,
        0,  0,  0,  1
    };}
    auto scaling (double x, double y, double z) { return matrix<4,4>
    {
        x,  0,  0,  0,
        0,  y,  0,  0,
        0,  0,  z,  0,
        0,  0,  0,  1
    };}
}