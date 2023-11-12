#pragma once
#include "aux_abc.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    void test_array () try
    {
        test("array.append");
        {
            array<int> a, b, c;
            oops( a += 1;            out(a) ) { "1" };
            oops( b += a;            out(b) ) { "1" };
            oops( b += b;            out(b) ) { "1, 1" };
            oops( c += std::move(b); out(c) ) { "1, 1" };
//          oops( b += 2;            out(b) ) { "2" };
        }
        test("array.ranges");
        {
            array<int> a = {1, 2, 3};
            oops( out(a.from(0)) ) { "1, 2, 3" };
            oops( out(a.from(1)) ) { "2, 3" };
            oops( out(a.from(2)) ) { "3" };
            oops( out(a.from(3)) ) { "" };
            oops( out(a.from(0).upto(0)) ) { "" };
            oops( out(a.from(0).upto(1)) ) { "1" };
            oops( out(a.from(0).upto(2)) ) { "1, 2" };
            oops( out(a.from(0).upto(3)) ) { "1, 2, 3" };
            oops( out(a.from(1).upto(1)) ) { "" };
            oops( out(a.from(1).upto(2)) ) { "2" };
            oops( out(a.from(1).upto(3)) ) { "2, 3" };
            oops( out(a.from(2).upto(2)) ) { "" };
            oops( out(a.from(2).upto(3)) ) { "3" };
            oops( out(a.from(3).upto(3)) ) { "" };
            oops( out(a(0, 0)) ) { "" };
            oops( out(a(0, 1)) ) { "1" };
            oops( out(a(0, 2)) ) { "1, 2" };
            oops( out(a(0, 3)) ) { "1, 2, 3" };
            oops( out(a(1, 1)) ) { "" };
            oops( out(a(1, 2)) ) { "2" };
            oops( out(a(1, 3)) ) { "2, 3" };
            oops( out(a(2, 2)) ) { "" };
            oops( out(a(2, 3)) ) { "3" };
            oops( out(a(3, 3)) ) { "" };
        }
        test("array.compare");
        {
            array<int> a = {1, 2, 3};
            array<int> b = {2, 3, 4};
            oops( out(a == a) ) { "1" };
            oops( out(a != b) ) { "1" };
            oops( out(a <= b) ) { "1" };
            oops( out(a != b.upto(3)) ) { "1" };
            oops( out(a <= b.upto(3)) ) { "1" };
            oops( out(a.from(0) != b) ) { "1" };
            oops( out(a.from(0) <= b) ) { "1" };
            oops( out(a.from(0) != b.upto(3)) ) { "1" };
            oops( out(a.from(0) <= b.upto(3)) ) { "1" };
            oops( out(a.from(1) == b.upto(2)) ) { "1" };
            oops( out(a.from(1).starts_with(b)) ) { "0" };
            oops( out(a.from(1).starts_with(b.upto(2))) ) { "1" };
            oops( out(a.from(0).ends_with(b.upto(3))) ) { "0" };
            oops( out(a.from(0).ends_with(b.upto(2))) ) { "1" };
        }
        test("array.erase");
        {
            array<int> a = {1, 2, 3};
            oops( a.from(0).upto(0).erase(); out(a) ) { "1, 2, 3" };
            oops( a.from(0).upto(1).erase(); out(a) ) { "2, 3" };
            oops( a.from(0).upto(2).erase(); out(a) ) { "" };
            oops( a.from(0).upto(2).erase(); out(a) ) { "" };
        }
        test("array.strip");
        {
            array<int> a = {1, 2, 3, 3, 2, 1};
            oops( a.strip(2); out(a) ) { "1, 2, 3, 3, 2, 1" };
            oops( a.strip(1); out(a) ) { "2, 3, 3, 2" };
            oops( a.triml(2); out(a) ) { "3, 3, 2" };
            oops( a.trimr(2); out(a) ) { "3, 3" };
            oops( a.trimr(3); out(a) ) { "" };
        }
        test("array.insert");
        {
            array<int> a = {};
            oops( a.insert(a.begin()+0, 2); out(a) ) { "2" };
            oops( a.insert(a.begin()+1, 3); out(a) ) { "2, 3" };
            oops( a.insert(a.begin()+0, 1); out(a) ) { "1, 2, 3" };
        }
        test("array.replace");
        {
            array<int> a = {1, 2, 3};
            array<int> b = {4, 5, 6};
            oops( a.upto(0).replace_by(b.from(3)); out(a) ) { "1, 2, 3" };
            oops( a.upto(1).replace_by(b.upto(1)); out(a) ) { "4, 2, 3" };
            oops( a.upto(1).replace_by(b.from(1)); out(a) ) { "5, 6, 2, 3" };
            oops( a.upto(2).replace_by(b.from(2)); out(a) ) { "6, 2, 3" };
            oops( a.upto(3).replace_by(b.upto(3)); out(a) ) { "4, 5, 6" };
            oops( a.upto(0).replace_by(b.from(0)); out(a) ) { "4, 5, 6, 4, 5, 6" };
        }
        test("array.performance");
        {
            using std::vector;
            using std::string;
            using e = element;
            using V = vector<e>;
            using A = array <e>;

            vector<string> cd   = { "ctor: a", "dtor: a", };
            vector<string> ccdd = { "ctor: a", "copy: a", "dtor: a", "dtor: a", };
            vector<string> cmdd = { "ctor: a", "move: a", "dtor: " , "dtor: a", };

            oops([]{ V a(1, 'a'); }();) {ccdd};
            oops([]{ V a = {'a'}; }();) {ccdd};
            oops([]{ V a(1, e('a')); }();) {ccdd};
            oops([]{ V a = {e('a')}; }();) {ccdd};
            oops([]{ V a; a.emplace_back('a'); }();) {cd};
            oops([]{ V a; a.push_back('a'); }();) {cmdd};

            oops([]{ A a('a'); }();) {cmdd};
            oops([]{ A a; a.push_back('a'); }();) {cmdd};
            oops([]{ array<A> a(A('a')); }();) {cmdd};
            oops([]{ auto a = A('a'); }();) {cmdd};

            vector<string> cmdmdd = { "ctor: a",
                "move: a", "dtor: " ,
                "move: a", "dtor: " ,
                "dtor: a", };

            oops([]{ V a; a.push_back('a'); a.reserve(9); }();) {cmdmdd};
            oops([]{ A a; a.push_back('a'); a.reserve(9); }();) {cmdmdd};

            oops([]{ vector<V> a = {{}}; a[0].push_back('a'); a.reserve(9); }();) {cmdd};
            oops([]{ array <A> a = {{}}; a[0].push_back('a'); a.reserve(9); }();) {cmdd};

            oops([]{ auto a = 'a' + A('b'); }();) 
            {
                "ctor: b", "move: b", 
                "ctor: a", "move: a", 
                "move: b", "dtor: " , 
                "dtor: " , "dtor: " , 
                "dtor: b", "dtor: a"
            };
            oops([]{ auto a = A('a') + 'b'; }();)
            {
                "ctor: b", "ctor: a", 
                "move: a", "move: b", 
                "move: a", "dtor: " , 
                "dtor: " , "dtor: " , 
                "dtor: a", "dtor: b"
            };
            oops([]{ auto a = A('a') * A('b'); }();)
            {
                "ctor: b", "move: b", 
                "ctor: a", "move: a",
                "move: a", "dtor: " , "move: b",  
                "dtor: " , "dtor: " , "dtor: " , 
                "dtor: a", "dtor: b"
            };
        }
        test("array.unexpected");
        {
            // law of exclusivity
            oops( array<int> a = {0,0,1,0,1,2}; a.erase_all(a[0]); out(a) ) { "1, 1, 2" };
            oops( array<int> a = {0,0,1,0,1,2}; std::erase(a,a[0]); out(a) ) { "1, 0, 2" };
        }
    }
    catch(assertion_failed){}
}
