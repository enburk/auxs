#pragma once
#include "aux_abc.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    void test_math1 () try
    {
        using I = int;
        using F = float;
        using D = double;
        using S = signed char;
        using U = uint8_t;
        using aux::vector;
        double epsilon = 0.000001;

        test("clamp");
        {
            oops( out(clamp<S>(1000)) ) { "127" };
            oops( out(clamp<S>(-999)) ) { "-128" };
            oops( out(clamp<U>(1000)) ) { "255" };
            oops( out(clamp<U>(-999)) ) { "0" };
            oops( out(clamp<S>(1000.0)) ) { "127" };
            oops( out(clamp<S>(-999.0)) ) { "-128" };
            oops( out(clamp<U>(1000.0)) ) { "255" };
            oops( out(clamp<U>(-999.0)) ) { "0" };
        }
        test("vector");
        {
            /**/ vector<3> vb{1,2,3}; // OK
            //// vector<3> vc{1,2,3,4}; // shouldn't compile
            //// vector<3> va{1,2}; // shouldn't compile

            oops( out(Vector<F>{1,2,3}[0] == 1.0f) ) { "1" };
            oops( out(Vector<D>{1,2,3}[1] == 2.0) ) { "1" };
            oops( out(Vector<I>{1,2,3}[2] == 3) ) { "1" };
        }
        test("vector.1D");
        {
            oops( out(vector<1,F>{}.x == 0.0f) ) { "1" };
            oops( out(vector<1,D>{}.x == 0.0) ) { "1" };
            oops( out(vector<1,I>{}.x == 0) ) { "1" };

            oops( out(vector<1,S>{1000}.x) ) { "127" };
            oops( out(vector<1,S>{-999}.x) ) { "-128" };
            oops( out(vector<1,U>{1000}.x) ) { "255" };
            oops( out(vector<1,U>{-999}.x) ) { "0" };

            using v = vector<1>;

            oops( out(distance(v{255}, v{255.0}) < epsilon) ) { "1" };
            oops( out(distance(v{255}, v{255.1}) > epsilon) ) { "1" };
            oops( out(distance(v{255}, v{254.9}) > epsilon) ) { "1" };
        }
        test("vector.2D");
        {
            oops( out(vector<2,F>{}.y == 0.0f) ) { "1" };
            oops( out(vector<2,D>{}.y == 0.0) ) { "1" };
            oops( out(vector<2,I>{}.y == 0) ) { "1" };

            oops( out(vector<2,S>{0, 1000}.y) ) { "127" };
            oops( out(vector<2,S>{0, -999}.y) ) { "-128" };
            oops( out(vector<2,U>{0, 1000}.y) ) { "255" };
            oops( out(vector<2,U>{0, -999}.y) ) { "0" };

            using i = vector<2,I>;
            using d = vector<2,D>;

            oops( out(i{0,0} + i{0,0} == i{0,0}) ) { "1" };
            oops( out(i{1,0} + i{1,0} == i{2,0}) ) { "1" };
            oops( out(i{1,2} + i{1,2} == i{2,4}) ) { "1" };

            oops( out(i{0,0} - i{0,0} == i{0,0}) ) { "1" };
            oops( out(i{1,0} - i{1,0} == i{0,0}) ) { "1" };
            oops( out(i{1,2} - i{1,2} == i{0,0}) ) { "1" };

            oops( out(distance(2*d{0,0}, d{0,0}) < epsilon) ) { "1" };
            oops( out(distance(2*d{1,0}, d{2,0}) < epsilon) ) { "1" };
            oops( out(distance(2*d{1,2}, d{2,4}) < epsilon) ) { "1" };

            oops( out(distance(d{0,0}*2, d{0,0}) < epsilon) ) { "1" };
            oops( out(distance(d{1,0}*2, d{2,0}) < epsilon) ) { "1" };
            oops( out(distance(d{1,2}*2, d{2,4}) < epsilon) ) { "1" };

            oops( out(distance(d{0,0}/2, d{0.0, 0.0}) < epsilon) ) { "1" };
            oops( out(distance(d{1,0}/2, d{0.5, 0.0}) < epsilon) ) { "1" };
            oops( out(distance(d{1,2}/2, d{0.5, 1.0}) < epsilon) ) { "1" };

            oops( out(i{0,0} * i{0,0}) ) { "0.000000" };
            oops( out(i{1,0} * i{0,1}) ) { "0.000000" };
            oops( out(i{1,0} * i{1,0}) ) { "1.000000" };
            oops( out(i{1,2} * i{1,2}) ) { "5.000000" };
        }
        test("vector.3D");
        {
            oops( out(vector<3,F>{}.z == 0.0f) ) { "1" };
            oops( out(vector<3,D>{}.z == 0.0) ) { "1" };
            oops( out(vector<3,I>{}.z == 0) ) { "1" };

            oops( out(vector<3,D>{ 1.0, 2.0, 3.0 }.x == 1.0) ) { "1" };
            oops( out(vector<3,D>{ 1.0, 2.0, 3.0 }.y == 2.0) ) { "1" };
            oops( out(vector<3,D>{ 1.0, 2.0, 3.0 }.z == 3.0) ) { "1" };
        }
        test("vector.4D");
        {
            oops( out(vector<4,D>{1,2,3,4}.x == 1.0) ) { "1" };
            oops( out(vector<4,D>{1,2,3,4}.y == 2.0) ) { "1" };
            oops( out(vector<4,D>{1,2,3,4}.z == 3.0) ) { "1" };
            oops( out(vector<4,D>{1,2,3,4}.w == 4.0) ) { "1" };
        }
        test("matrix.1x1");
        {
            /**/ matrix<1,1> ma{}; // OK
            /**/ matrix<1,1> mb{1}; // OK
            //// matrix<1,1> mc{1,2}; // shouldn't compile
            oops( out(matrix<1,1>{1}[0][0] == 1.0f)) { "1" };
        }
        test("matrix.2x2");
        {
            oops( out(matrix<2,2>{1,2,3,4}[0][0] == 1.0f) ) { "1" };
            oops( out(matrix<2,2>{1,2,3,4}[0][1] == 2.0f) ) { "1" };
            oops( out(matrix<2,2>{1,2,3,4}[1][0] == 3.0f) ) { "1" };
            oops( out(matrix<2,2>{1,2,3,4}[1][1] == 4.0f) ) { "1" };
        
            using m = matrix<2,2,I>;
            oops( out(m{1,2,3,4} + m{1,2,3,4} == m{2,4,6,8}) ) { "1" };
            oops( out(m{1,2,3,4} - m{1,2,3,4} == m{0,0,0,0}) ) { "1" };
            oops( out(m{1,2,3,4} * m{1,2,3,4} == m{7,10,15,22}) ) { "1" };
        
            oops( out(m::identity() == m{1,0,0,1}) ) { "1" };
            oops( out(m{1,2,3,4}.transposed() == m{1,3,2,4}) ) { "1" };
            oops( out(m{1,2,3,4}.transpose()[0][1] == 3) ) { "1" };
            oops( out(m{1,2,3,4}.transpose()[1][0] == 2) ) { "1" };
        }
        test("rotation.2D");
        {
            auto ex = vector<2>{1,0};
            auto ey = vector<2>{0,1};
            oops( out(abs(ex.rotated(pi/2).x - 0.0) < epsilon) ) { "1" };
            oops( out(abs(ex.rotated(pi/2).y - 1.0) < epsilon) ) { "1" };
            oops( out(abs(ex.rotated(pi/3).x - 0.5) < epsilon) ) { "1" };
            oops( out(abs(ex.rotated(pi/6).y - 0.5) < epsilon) ) { "1" };
        }
        test("rotation.3D");
        {
            auto ex = vector<4>{1,0,0,1};
            auto ey = vector<4>{0,1,0,1};
            auto ez = vector<4>{0,0,1,1};
            auto x90 = rotation_x(pi/2);
            auto y90 = rotation_y(pi/2);
            auto z90 = rotation_z(pi/2);
            auto x60 = rotation_x(pi/3);
            auto y60 = rotation_y(pi/3);
            auto z60 = rotation_z(pi/3);
            auto x45 = rotation_x(pi/4);
            auto y45 = rotation_y(pi/4);
            auto z45 = rotation_z(pi/4);
            auto s32 = sqrt(3)/2;

            oops( out((z90*ex).x) ) { "0.000000" };
            oops( out((z90*ex).y) ) { "1.000000" };
            oops( out((z90*ex).z) ) { "0.000000" };

            oops( out((y90*ex).x) ) { "0.000000" };
            oops( out((y90*ex).y) ) { "0.000000" };
            oops( out((y90*ex).z) ) {"-1.000000" };

            oops( out((y90*ez).x) ) { "1.000000" };
            oops( out((y90*ez).y) ) { "0.000000" };
            oops( out((y90*ez).z) ) { "0.000000" };

            oops( out(abs((z60*ex).x - 0.5) < epsilon) ) { "1" };
            oops( out(abs((z60*ex).y - s32) < epsilon) ) { "1" };
            oops( out(abs((z60*ex).z - 0.0) < epsilon) ) { "1" };

            oops( out(abs((x90*z90*ex + y90*ex).x) < epsilon) ) { "1" };
            oops( out(abs((x90*z90*ex + y90*ex).y) < epsilon) ) { "1" };
            oops( out(abs((x90*z90*ex + y90*ex).z) < epsilon) ) { "1" };

            oops( out(abs((y90*z45*ex + x45*ez).x) < epsilon) ) { "1" };
            oops( out(abs((y90*z45*ex + x45*ez).y) < epsilon) ) { "1" };
            oops( out(abs((y90*z45*ex + x45*ez).z) < epsilon) ) { "1" };
        }
    }
    catch(assertion_failed){}

    void test_math2 () try
    {
        test("math");
        {
        }
    }
    catch(assertion_failed){}

    void test_math3 () try
    {
        test("math");
        {
        }
    }
    catch(assertion_failed){}
}
