#pragma once
#include <stack> 
#include <string> 
#include <variant> 
#include <stdexcept> 
#include <functional>
#include <filesystem>
#include "aux_array.h"
namespace aux
{
    using array_of_chars =
        array<char, std::string>;

    struct str : array_of_chars
    {
        using base = array_of_chars;
        using base:: array;

        constexpr str (const char * s ) : base(s) {}
        constexpr str (const char8_t * s ) : base((char*)s) {}
        constexpr str (char c, int n) { if (n > 0) *this = std::string(n,c); }
        constexpr str (std::string_view s) : base(std::string(s)) {}
        constexpr str (const_iterator f, const_iterator l) : base(std::string(f, l)) {}
        constexpr str (const char *   f, const char *   l) : base(std::string(f, l)) {}
        explicit  str (std::filesystem::path p) : str(p.u8string().c_str()) {}

        explicit  str (int                n) : base(std::to_string(n)) {}
        explicit  str (long               n) : base(std::to_string(n)) {}
        explicit  str (long long          n) : base(std::to_string(n)) {}
        explicit  str (unsigned           n) : base(std::to_string(n)) {}
        explicit  str (unsigned long      n) : base(std::to_string(n)) {}
        explicit  str (unsigned long long n) : base(std::to_string(n)) {}
        explicit  str (float              n) : base(std::to_string(n)) {}
        explicit  str (double             n) : base(std::to_string(n)) {}
        explicit  str (long double        n) : base(std::to_string(n)) {}

        explicit  str (array<char> text) { *this = std::string(text.data(), text.data()+text.size()); }
        explicit  str (array<str> lines, str delimiter = "\n") {
            for (auto line : lines) {
                *this += line;
                *this += delimiter; }
            if (size() > 0) resize(size() - delimiter.size());
        }

        static str list(array<str> lines, str delimiter = ", ", str last_delimiter = ", ") {
            if (lines.size() == 0) return "";
            if (lines.size() == 1) return lines[0];
            str s = lines[0];
            for (int i=1; i<lines.size()-1; i++) {
                s += delimiter;
                s += lines[i];
            }
            s += last_delimiter;
            s += lines.back();
            return s;
        }

        auto stoi() { return std::stoi(stripped()); }
        auto stod() { return std::stod(stripped()); }

        void operator += (      char   c) { std::string::operator+=(c); }
        void operator += (const char * s) { std::string::operator+=(s); }
        void operator += (const str  & s) { std::string::operator+=(s); }
        void operator += (      str && s) { std::string::operator+=(std::move(s)); }

        using base::first;
        using base::last;
        //auto first (const char* s) const { return first(str(s)); }
        //auto first (const char* s)       { return first(str(s)); }
        //auto last  (const char* s) const { return last(str(s)); }
        //auto last  (const char* s)       { return last(str(s)); }

        str upto_first (str s) { return upto(first(s).offset()); }
        str from_first (str s) { return from(first(s).offset()); }

        /////////////////////////////////////////////////

        using base::insert;
        auto insert(int i, const char* s) { return std::string::insert(i, s); }

        void triml (const str & chars = " "){
            auto r = first(one_not_of{chars});
            if (r.empty()) clear(); else
                upto(r.offset()).erase();
        }
        void trimr (const str & chars = " "){
            auto r = last(one_not_of{chars});
            if (r.empty()) clear(); else
                from(r.offset()+1).erase();
        }
        void strip (const str & chars = " "){
             trimr(chars);
             triml(chars);
        }
        str stripped () const { str s = *this; s.strip(); return s; }

        void align_left   (int n, char padding = ' '); // defined in aux_unicode.h
        void align_right  (int n, char padding = ' '); // defined in aux_unicode.h
        str left_aligned  (int n, char padding = ' ') { str s = *this; s.align_left (n, padding); return s; }
        str right_aligned (int n, char padding = ' ') { str s = *this; s.align_right(n, padding); return s; }

        using base::replace_by;
        void replace_by (const char * r) {
                return replace_by(str(r)); }

        int replace_all (str what, str with) { return base::
            replace_all (what, with); }

        str replaced (str what, str with) {
            str s = *this; s.replace_all(what, with);
            return s; }

        int replace_all (char c, int ccc, str to) // c repeated exactly ccc times
        {
            int pos = 0, nn = 0;
            while (true) {
                auto r = from(pos).first(str(c, ccc)); if (not r) break;
                pos = r.offset();
                if (pos+ccc >= size() || at(pos+ccc) != c) {
                    range(r).replace_by(to);
                    pos += to.size();
                    nn++;
                }
                else {
                    auto r = from(pos).first(one_not_of{str(c, 1)}); if (not r) break;
                    pos = r.offset();
                }
            }
            return nn;
        }

        bool rebracket (str opening, str closing, auto lambda, bool despace = false)
        {
            int pos = 0;
            
            std::stack<int> lefts; 

            while (pos < size())
            {
                if (from(pos).starts_with(opening))
                {
                    lefts.push(pos);
                    pos += opening.size();
                }
                else
                if (from(pos).starts_with(closing))
                {
                    if (lefts.empty()) return false;

                    int b = lefts.top(); lefts.pop();
                    int e = pos + closing.size();
                    str s = lambda(from(b +
                        opening.size()).
                        upto(pos));

                    if (despace
                    and b > 0 and
                    at (b-1) == ' ')
                        b--;

                    from(b).upto(e).replace_by(s);

                    pos = b + s.size();
                }
                else pos++;
            }
            return lefts.size() == 0;
        }
        void rebracket (str opening, str closing, str before, str after)
        {
             rebracket (opening, closing, [=](str s){ return before + s + after; });
        }
        void debracket (str opening, str closing)
        {
             rebracket (opening, closing, [](str s){ return ""; }, true);
        }
        str rebracketed (str o, str c, str b, str a)
        {
            str s = *this; s.rebracket(o, c, b, a); return s;
        }
        str debracketed (str o, str c)
        {
            str s = *this; s.debracket(o, c); return s;
        }
    
        enum class delimiter { exclude, to_the_left, to_the_right };

        bool split_by (str pattern, str& str1, str& str2, delimiter delimiter = delimiter::exclude) const
        {
            int n = (int)container::find (pattern); int m = n == container::npos ? 0 : pattern.size();
            str s1 = m > 0 ? str(upto (n     + (delimiter == delimiter::to_the_left  ? m : 0))) : *this;
            str s2 = m > 0 ? str(from (n + m - (delimiter == delimiter::to_the_right ? m : 0))) : str{};
            str1 = std::move(s1); // could be this
            str2 = std::move(s2); // could be this
            return m > 0;
        }
        auto split_by (str pattern) const -> generator<str>
        {
            array<str> result;
            if (size() == 0) co_return;
            int start = 0; while (true)
            {
                auto next = (int)container::find(pattern, start);
                bool found = next != std::string::npos;
                if (not found) next = size();
                co_yield from(start).upto(next);
                start = next + pattern.size();
                if (start >= size()) {
                if (found) co_yield "";
                break; }
            }
        }

        auto lines () const { return split_by("\n"); }

        auto split_strip_by (str pattern) const -> generator<str>
        {
            for (str s: split_by(pattern)) {
                s.strip(); co_yield s; }
        }

        auto extract_from (str pattern)
        {
            str s;
            split_by(pattern, *this, s);
            trimr(); s.triml();
            return s;
        }
        auto extract_upto (str pattern)
        {
            str s;
            split_by(pattern, s, *this);
            s.trimr(); triml();
            return s;
        }

        void canonicalize ()
        {
            for (char & c : *this) if( c == '\t' || c  == '\r' || c == '\n' )  c = ' ';
            base::erase(std::unique(begin(), end(), [](char c1, char c2){ return c1 == ' ' && c2 == ' '; }), end());
            strip();
        }

        ////////////////////////////////////////////////////////////////////////////

        bool digital () const {
            bool digital = false;
            for (char c : *this)
                if(c < '0' || '9' < c)
                    return false; else
                        digital = true;
            return digital;
        }

        bool ascii_isalpha () const {
            for (char c : *this)
                if((c < 'A') || ('Z' < c &&
                    c < 'a') || ('z' < c))
                    return false;
            return true;
        }

        bool ascii_isalnum () const {
            for (char c : *this)
                if((c < '0') || ('9' < c &&
                    c < 'A') || ('Z' < c &&
                    c < 'a') || ('z' < c))
                    return false;
            return true;
        }

        static char ascii_tolower (char c) { return 'A' <= c && c <= 'Z' ? c - 'A' + 'a' : c; }
        static char ascii_toupper (char c) { return 'a' <= c && c <= 'z' ? c - 'a' + 'A' : c; }

        str ascii_lowercased () const { str s = *this; std::transform(s.begin(), s.end(), s.begin(), ascii_tolower); return s; }
        str ascii_uppercased () const { str s = *this; std::transform(s.begin(), s.end(), s.begin(), ascii_toupper); return s; }

        bool is_ascii_lowercased () const { for (char c : *this) if ('A' <= c and c <= 'Z') return false; return true; }
        bool is_ascii_uppercased () const { for (char c : *this) if ('a' <= c and c <= 'z') return false; return true; }

        void capitalize (bool on = true)
        {
            if (not empty())  (*this)[0] = on ?
                ascii_toupper((*this)[0]):
                ascii_tolower((*this)[0]);
        }
        str capitalized (bool on = true)
        {
            str s= *this; s.capitalize(on); return s;
        }
    };

    one_of     (const char*) -> one_of     <str>;
    one_not_of (const char*) -> one_not_of <str>;
}
namespace std
{
    template <> struct hash<aux::str>
    {
        std::size_t operator()(const aux::str & s) const
        {
            using std::size_t;
            using std::hash;
            using std::string;
            return hash<string>()(s);
        }
    };
}
