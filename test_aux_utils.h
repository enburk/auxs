#pragma once
#include "aux_abc.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    void test_utils () try
    {
        test("exceptions");
        {
            try { throw 0; } catch(int x)
            {
                oops(out(x)) { "0" };
                oops(out(std::uncaught_exceptions())) { "0" };
            }

            try { throw 1; } catch(...)
            {
                try
                {
                    try { throw 2; } catch(int x)
                    {
                        oops(out(x)) { "2" };
                    }
                    throw; // may cause terminate() ?
                }
                catch(int y)
                {
                    oops(out(y)) { "1" };
                }
            }

            try { throw 1; } catch(...)
            {
                try
                {
                    try
                    {
                        auto e = std::current_exception();
                        if (e) oops(out("e")) { "e" };
                        if (e) std::rethrow_exception(e);
                    }
                    catch(int x)
                    {
                        oops(out(x)) { "1" };
                    }
                    throw; // may cause terminate() ?
                }
                catch(int y)
                {
                    oops(out(y)) { "1" };
                }
            }
        }
    }
    catch(assertion_failed){}
}
