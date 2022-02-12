#pragma once
#include <string> 
#include <variant> 
#include <stdexcept> 
#include <functional>
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
        constexpr str (char c, int n=1) { if (n > 0) *this = std::string(n,c); }
        constexpr str (std::string_view s) : base(std::string(s)) {}
        constexpr str (const_iterator f, const_iterator l) : base(std::string(f, l)) {}
        constexpr str (const char *   f, const char *   l) : base(std::string(f, l)) {}

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

        void operator += (      char   c) { std::string::operator+=(c); }
        void operator += (const char * s) { std::string::operator+=(s); }
        void operator += (const str  & s) { std::string::operator+=(s); }
        void operator += (      str && s) { std::string::operator+=(std::move(s)); }

        using base::first;
        using base::last;
        auto first (const char* s) const { return first(str(s)); }
        auto first (const char* s) { return first(str(s)); }
        auto last (const char* s) const { return last(str(s)); }
        auto last (const char* s) { return last(str(s)); }

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

        void align_left (int n, char padding = ' ') {
            if (size() < n) *this += str(padding, n - size());
        }
        void align_right (int n, char padding = ' ') {
            if (size() < n) *this = str(padding, n - size()) + *this;
        }
        str left_aligned  (int n, char padding = ' ') { str s = *this; s.align_left (n, padding); return s; }
        str right_aligned (int n, char padding = ' ') { str s = *this; s.align_right(n, padding); return s; }

        using base::replace_by;
        void replace_by (const char * r) {
                return replace_by(str(r)); }

        using base::replace_all;
        int replace_all (const char * what, const char * with) {
                return replace_all(str(what), str(with)); }

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
        array<str> split_by (str pattern) const
        {
            array<str> result;
            if (size() == 0) return result;
            int start = 0; while (true) {
                auto range = (int)container::find(pattern, start);
                bool found = range != std::string::npos;
                result += from(start).span(found ? range - start : size() - start);
                if (!found) range = size();
                start = range + pattern.size();
                if (start >= size()) { if (found) result += ""; break; }
            }
            return result;
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
