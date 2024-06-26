﻿#pragma once
#include "aux_abc.h"
#include "aux_unittest.h"
namespace aux::unittest
{
    void test_string () try
    {
        test("string.substr");
        {
            ouch( out(str("").from(0).span(0)) ) { "" };
            ouch( out(str("").from(0).span(1)) ) { "" };
            ouch( out(str("").from(1).span(1)) ) { "" };

            oops( out(str("abc").from(1).span(0)) ) { "" };
            oops( out(str("abc").from(1).span(1)) ) { "b" };
            oops( out(str("abc").from(1).span(2)) ) { "bc" };
            oops( out(str("abc").from(1).span(3)) ) { "bc" };
            oops( out(str("abc").from(2)) ) { "c" };
            oops( out(str("abc").upto(2)) ) { "ab" };
        }
        test("string.compare");
        {
            oops( out(str("abc") == str("abc")) ) { "1" };
            oops( out(str("abc") == "abc") ) { "1" };
            oops( out(str("abc") != "abz") ) { "1" };
            oops( out(str("abc") <= "abz") ) { "1" };
            oops( out("abc" == str("abc")) ) { "1" };
            oops( out("abz" != str("abc")) ) { "1" };
            oops( out("abz" >= str("abc")) ) { "1" };

            oops( out(str("abc") != str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc") <= str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc").from(0) != str("bcd")) ) { "1" };
            oops( out(str("abc").from(0) <= str("bcd")) ) { "1" };
            oops( out(str("abc").from(0) != str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc").from(0) <= str("bcd").upto(3)) ) { "1" };
            oops( out(str("abc").from(1) == str("bcd").upto(2)) ) { "1" };
        }
        test("string.insert");
        {
            oops( str s = "abc"; s.insert(0, "_"); out(s) ) { "_abc" };
            oops( str s = "abc"; s.insert(1, "_"); out(s) ) { "a_bc" };
            oops( str s = "abc"; s.insert(2, "_"); out(s) ) { "ab_c" };
            oops( str s = "abc"; s.insert(3, "_"); out(s) ) { "abc_" };
        }
        test("string.search");
        {
            oops( out(str("").first("").size()) ) { "0" };
            oops( out(str(" ").last("").size()) ) { "0" };
            oops( out(str("").first(one_of("")).size()) ) { "0" };
            oops( out(str(" ").last(one_of("")).size()) ) { "0" };
            oops( out(str("").first(one_not_of{""}).size()) ) { "0" };
            oops( out(str(" ").last(one_not_of{""}).size()) ) { "1" };

            oops( str s = ""; s.first().erase(); out(s) ) { "" };
            oops( str s = ""; s.last ().erase(); out(s) ) { "" };
            oops( str s = "abc"; s.first().erase(); out(s) ) { "bc" };
            oops( str s = "abc"; s.last ().erase(); out(s) ) { "ab" };

            oops( str s = "abcba"; s.first("a").erase(); out(s) ) { "bcba" };
            oops( str s = "abcba"; s.first("b").erase(); out(s) ) { "acba" };
            oops( str s = "abcba"; s.first("c").erase(); out(s) ) { "abba" };
            oops( str s = "abcba"; s.first("d").erase(); out(s) ) { "abcba" };
            oops( str s = "abcba"; s.last ("d").erase(); out(s) ) { "abcba" };
            oops( str s = "abcba"; s.last ("c").erase(); out(s) ) { "abba" };
            oops( str s = "abcba"; s.last ("b").erase(); out(s) ) { "abca" };
            oops( str s = "abcba"; s.last ("a").erase(); out(s) ) { "abcb" };

            oops( str s = "abcba"; s.first(one_of("da")).erase(); out(s) ) { "bcba" };
            oops( str s = "abcba"; s.first(one_of("db")).erase(); out(s) ) { "acba" };
            oops( str s = "abcba"; s.first(one_of("dc")).erase(); out(s) ) { "abba" };
            oops( str s = "abcba"; s.first(one_of("dd")).erase(); out(s) ) { "abcba" };
            oops( str s = "abcba"; s.last (one_of("dd")).erase(); out(s) ) { "abcba" };
            oops( str s = "abcba"; s.last (one_of("dc")).erase(); out(s) ) { "abba" };
            oops( str s = "abcba"; s.last (one_of("db")).erase(); out(s) ) { "abca" };
            oops( str s = "abcba"; s.last (one_of("da")).erase(); out(s) ) { "abcb" };

            oops( str s = "abcba"; s.first(one_not_of("bc")).erase(); out(s) ) { "bcba" };
            oops( str s = "abcba"; s.first(one_not_of("ac")).erase(); out(s) ) { "acba" };
            oops( str s = "abcba"; s.first(one_not_of("ab")).erase(); out(s) ) { "abba" };
            oops( str s = "abcba"; s.first(one_not_of("abc")).erase(); out(s) ) { "abcba" };
            oops( str s = "abcba"; s.last (one_not_of("abc")).erase(); out(s) ) { "abcba" };
            oops( str s = "abcba"; s.last (one_not_of("ab")).erase(); out(s) ) { "abba" };
            oops( str s = "abcba"; s.last (one_not_of("ac")).erase(); out(s) ) { "abca" };
            oops( str s = "abcba"; s.last (one_not_of("bc")).erase(); out(s) ) { "abcb" };

        }
        test("string.contains");
        {
            oops( out(str("abcabc").contains("abc")) ) { "1" };
            oops( out(str("abcabc").contains("abz")) ) { "0" };
            oops( out(str("abccba").contains(one_of("abc"))) ) { "1" };
            oops( out(str("abccba").contains(one_of("def"))) ) { "0" };
            oops( out(str("abccba").contains(one_not_of("abz"))) ) { "1" };
            oops( out(str("abccba").contains(one_not_of("abc"))) ) { "0" };

            oops( out(str("abcabc").contains_only("abcabc")) ) { "1" };
            oops( out(str("abccba").contains_only("abcabc")) ) { "0" };
            oops( out(str("abccba").contains_only(one_of("abc"))) ) { "1" };
            oops( out(str("abccba").contains_only(one_of("abz"))) ) { "0" };
            oops( out(str("abccba").contains_only(one_not_of("def"))) ) { "1" };
            oops( out(str("abccba").contains_only(one_not_of("abc"))) ) { "0" };
        }
        test("string.fragment");
        {
            oops( out(str("abc").starts_with("")) ) { "1" };
            oops( out(str("abc").starts_with("a")) ) { "1" };
            oops( out(str("abc").starts_with("ab")) ) { "1" };
            oops( out(str("abc").starts_with("abc")) ) { "1" };
            oops( out(str("abc").starts_with("abcd")) ) { "0" };
            oops( out(str("abc").starts_with("abd")) ) { "0" };
            oops( out(str("abc").starts_with("ad")) ) { "0" };
            oops( out(str("abc").starts_with("d")) ) { "0" };

            oops( out(str("abc").ends_with("")) ) { "1" };
            oops( out(str("abc").ends_with("c")) ) { "1" };
            oops( out(str("abc").ends_with("bc")) ) { "1" };
            oops( out(str("abc").ends_with("abc")) ) { "1" };
            oops( out(str("abc").ends_with("aabc")) ) { "0" };
            oops( out(str("abc").ends_with("aab")) ) { "0" };
            oops( out(str("abc").ends_with("aa")) ) { "0" };
            oops( out(str("abc").ends_with("a")) ) { "0" };
        }
        test("string.replace");
        {
            oops( str s = "abc"; s.replace_by("def"); out(s) ) { "def" };
            oops( str s = "abc"; s.upto(2).replace_by("def"); out(s) ) { "defc" };
            oops( str s = "abc"; s.from(1).replace_by("def"); out(s) ) { "adef" };
            oops( str s = "abcab"; s.replace_all("a", "de"); out(s) ) { "debcdeb" };
            oops( str s = "abcab"; s.replace_all("ab", "d"); out(s) ) { "dcd" };
            oops( str s = "ababa"; s.replace_all("ab", "d"); out(s) ) { "dda" };
            oops( str s = "ababa"; s.replace_all("aba","d"); out(s) ) { "dba" };
            oops( str s = "..a..b.."; s.replace_all('.', 2, ":"); out(s) ) { ":a:b:" };
            oops( str s = "...a..b.."; s.replace_all('.', 2, ":"); out(s) ) { "...a:b:" };
            oops( str s = "..a...b.."; s.replace_all('.', 2, ":"); out(s) ) { ":a...b:" };
            oops( str s = "..a..b..."; s.replace_all('.', 2, ":"); out(s) ) { ":a:b..." };
            oops( str s = ".a..b.."; s.replace_all('.', 2, ":"); out(s) ) { ".a:b:" };
            oops( str s = "..a.b.."; s.replace_all('.', 2, ":"); out(s) ) { ":a.b:" };
            oops( str s = "..a..b."; s.replace_all('.', 2, ":"); out(s) ) { ":a:b." };
        }
        test("string.strip");
        {
            oops( str s = "ababa"; s.triml("bc"); out(s) ) { "ababa" };
            oops( str s = "ababa"; s.trimr("bc"); out(s) ) { "ababa" };
            oops( str s = "ababa"; s.triml("ac"); out(s) ) { "baba" };
            oops( str s = "ababa"; s.trimr("ca"); out(s) ) { "abab" };
            oops( str s = "ababa"; s.triml("ba"); out(s) ) { "" };
            oops( str s = "ababa"; s.trimr("ab"); out(s) ) { "" };
            oops( str s = "abcba"; s.triml("ab"); out(s) ) { "cba" };
            oops( str s = "abcba"; s.trimr("ba"); out(s) ) { "abc" };
            oops( str s = "abcba"; s.strip("ba"); out(s) ) { "c" };
        }
        test("string.split");
        {
            oops( out(str(""   ).split_by("|"), ",") ) { "" };
            oops( out(str("a"  ).split_by("|"), ",") ) { "a" };
            oops( out(str("|"  ).split_by("|"), ",") ) { "," };
            oops( out(str("a|" ).split_by("|"), ",") ) { "a," };
            oops( out(str("|b" ).split_by("|"), ",") ) { ",b" };
            oops( out(str("a|b").split_by("|"), ",") ) { "a,b" };

            str  a,b;
            str("a|b").split_by("|", a, b, str::delimiter::to_the_left);  oops( out(a) ) { "a|" };
            str("a|b").split_by("|", a, b, str::delimiter::to_the_left);  oops( out(b) ) { "b"  };
            str("a|b").split_by("|", a, b, str::delimiter::to_the_right); oops( out(a) ) { "a"  };
            str("a|b").split_by("|", a, b, str::delimiter::to_the_right); oops( out(b) ) { "|b" };
        }
        test("string.unicode");
        {
            str apostrophe = u8"’"; // e2 80 99
            str creme_brulee = u8"crème brûlée";
            str matreska = u8"матрёшка";
            str paprika = u8"па́прика"; // cc 81 // COMBINING ACUTE ACCENT
            str T = u8"Ͳ"; // cd b2 // GREEK CAPITAL LETTER ARCHAIC SAMPI
            oops( out(unicode::length(""          )) ) {  "0" };
            oops( out(unicode::length("abc"       )) ) {  "3" };
            oops( out(unicode::length(apostrophe  )) ) {  "1" };
            oops( out(unicode::length(creme_brulee)) ) { "12" };
            oops( out(unicode::length(matreska    )) ) {  "8" };
            oops( out(unicode::length(paprika     )) ) {  "7" };
            oops( out(unicode::length(T           )) ) {  "1" };
            oops( out(str(""          ). left_aligned(8)) ) {       "        "  };
            oops( out(str("abc"       ). left_aligned(8)) ) {       "abc     "  };
            oops( out(str(apostrophe  ). left_aligned(8)) ) { str(u8"’       ") };
            oops( out(str(creme_brulee). left_aligned(8)) ) { str(u8"crème brûlée") };
            oops( out(str(matreska    ).right_aligned(8)) ) { str(u8"матрёшка") };
            oops( out(str(paprika     ).right_aligned(8)) ) { str(u8" па́прика") };
            oops( out(str(T           ).right_aligned(8)) ) { str(u8"       Ͳ") };
        }
        test("string.brackets");
        {
            oops( out(str(""         ).debracketed("{","}")) ) { "" };
            oops( out(str("{"        ).debracketed("{","}")) ) { "{" };
            oops( out(str("}"        ).debracketed("{","}")) ) { "}" };
            oops( out(str("{}"       ).debracketed("{","}")) ) { "" };
            oops( out(str(" {}"      ).debracketed("{","}")) ) { "" };
            oops( out(str("{} "      ).debracketed("{","}")) ) { " " };
            oops( out(str("a(b)"     ).debracketed("(",")")) ) { "a" };
            oops( out(str("a(b)c"    ).debracketed("(",")")) ) { "ac" };
            oops( out(str("a (b)"    ).debracketed("(",")")) ) { "a" };
            oops( out(str("a (b) c"  ).debracketed("(",")")) ) { "a c" };
            oops( out(str("a (b), c" ).debracketed("(",")")) ) { "a, c" };
            oops( out(str("a (bb)"   ).debracketed("(",")")) ) { "a" };
            oops( out(str("a (bb) c" ).debracketed("(",")")) ) { "a c" };
            oops( out(str("a (bb), c").debracketed("(",")")) ) { "a, c" };
            oops( out(str("a [[bb]]"   ).debracketed("[[","]]")) ) { "a" };
            oops( out(str("a [[bb]] c" ).debracketed("[[","]]")) ) { "a c" };
            oops( out(str("a [[bb]], c").debracketed("[[","]]")) ) { "a, c" };
            oops( out(str("a (bb)"   ).rebracketed("(",")","<i>","</i>")) ) { "a <i>bb</i>" };
            oops( out(str("a (bb) c" ).rebracketed("(",")","<i>","</i>")) ) { "a <i>bb</i> c" };
            oops( out(str("a (bb), c").rebracketed("(",")","<i>","</i>")) ) { "a <i>bb</i>, c" };

            struct result
            {
                str s;
                array<str> ss;
                bool ok = true;
            };
            auto pass = [](str s)
            {
                result r;
                r.s  = s;
                r.ok = r.s.rebracket("{","}",
                [&](str x){ r.ss += x; return x; });
                return r;
            };

            oops( out(pass(""    ).ok) ) { "1" };
            oops( out(pass("{}"  ).ok) ) { "1" };
            oops( out(pass("{{}}").ok) ) { "1" };
            oops( out(pass("{{}" ).ok) ) { "0" };
            oops( out(pass("{}}" ).ok) ) { "0" };
            oops( out(pass("{"   ).ok) ) { "0" };
            oops( out(pass("}"   ).ok) ) { "0" };

            oops( out(pass("a{b{c}d}e").s) ) { "abcde" };
            oops( out(pass("a{{bc}d}e").s) ) { "abcde" };
            oops( out(pass("a{{b}}cde").s) ) { "abcde" };

            oops( out(str::list(pass("a{b{c}d}e").ss)) ) { "c, bcd" };
            oops( out(str::list(pass("a{{bc}d}e").ss)) ) { "bc, bcd" };
            oops( out(str::list(pass("a{{b}}cde").ss)) ) { "b, b" };
        }
    }
    catch(assertion_failed){}
}
