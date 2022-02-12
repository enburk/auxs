#pragma once
#include <map>
#include "doc_text_text.h"
namespace doc::html::lexica
{
	using text::token;

    generator<token> tokenize (const text::text & text)
    {
        token t;

        for (auto [n, line] : enumerate(text.lines))
        {
            for (auto [offset, glyph] : enumerate(line, ""))
            {
				if (glyph.letter() or
					glyph.digit() or
					glyph == "_")
					t += glyph;
				else
                {
                    if (t.text != "")
						co_yield t;

					// each and every symbol is a token
                    co_yield token {glyph, "", "", range{
                        {n, offset},
                        {n, offset+1}}
                    };

                    t = token {"", "", "", range{
                        {n, offset+1},
                        {n, offset+1}}
                    };
                }
            }
        }
    }

    generator<token> parse (const text::text & text)
    {
        token last; str kind = "text";

		for (token t : tokenize(text))
        {
            if (last.kind == "text"
            &&  last.text.starts_with("&")
            && !last.text.ends_with(";")) {
                last.text += t.text;
                last.range.upto = t.range.upto;
                continue;
            }

            if (last.kind == "tag"
            &&  last.text.starts_with("\""))
            if (last.text.size() == 1
            || !last.text.ends_with("\"")) {
                last.text += t.text;
                last.range.upto = t.range.upto;
                continue;
            }

            if (last.kind == "tag"
            &&  last.text == "#") {
                last.text += t.text;
                last.range.upto = t.range.upto;
                continue;
            }

            if (t.text == "<") kind = "tag";

			if (last.text != "")
				co_yield last;

            last = t;
			last.kind = kind;

            if (t.text == ">") kind = "text";
        }

		if (last.text != "")
			co_yield last;
    }

	inline std::map<str, const char8_t*> symbols
	{
		{ "AElig", u8"Æ" },
		{ "Aacute", u8"Á" },
		{ "Acirc", u8"Â" },
		{ "Agrave", u8"À" },
		{ "Alpha", u8"Α" },
		{ "Aring", u8"Å" },
		{ "Atilde", u8"Ã" },
		{ "Auml", u8"Ä" },
		{ "Beta", u8"Β" },
		{ "Ccedil", u8"Ç" },
		{ "Chi", u8"Χ" },
		{ "Dagger", u8"‡" },
		{ "Delta", u8"Δ" },
		{ "ETH", u8"Ð" },
		{ "Eacute", u8"É" },
		{ "Ecirc", u8"Ê" },
		{ "Egrave", u8"È" },
		{ "Epsilon", u8"Ε" },
		{ "Eta", u8"Η" },
		{ "Euml", u8"Ë" },
		{ "Gamma", u8"Γ" },
		{ "Iacute", u8"Í" },
		{ "Icirc", u8"Î" },
		{ "Igrave", u8"Ì" },
		{ "Iota", u8"Ι" },
		{ "Iuml", u8"Ï" },
		{ "Kappa", u8"Κ" },
		{ "Lambda", u8"Λ" },
		{ "Mu", u8"Μ" },
		{ "Ntilde", u8"Ñ" },
		{ "Nu", u8"Ν" },
		{ "OElig", u8"Œ" },
		{ "Oacute", u8"Ó" },
		{ "Ocirc", u8"Ô" },
		{ "Ograve", u8"Ò" },
		{ "Omega", u8"Ω" },
		{ "Omicron", u8"Ο" },
		{ "Oslash", u8"Ø" },
		{ "Otilde", u8"Õ" },
		{ "Ouml", u8"Ö" },
		{ "Phi", u8"Φ" },
		{ "Pi", u8"Π" },
		{ "Prime", u8"″" },
		{ "Psi", u8"Ψ" },
		{ "Rho", u8"Ρ" },
		{ "Scaron", u8"Š" },
		{ "Sigma", u8"Σ" },
		{ "THORN", u8"Þ" },
		{ "Tau", u8"Τ" },
		{ "Theta", u8"Θ" },
		{ "Uacute", u8"Ú" },
		{ "Ucirc", u8"Û" },
		{ "Ugrave", u8"Ù" },
		{ "Upsilon", u8"Υ" },
		{ "Uuml", u8"Ü" },
		{ "Xi", u8"Ξ" },
		{ "Yacute", u8"Ý" },
		{ "Yuml", u8"Ÿ" },
		{ "Zeta", u8"Ζ" },
		{ "aacute", u8"á" },
		{ "acirc", u8"â" },
		{ "acute", u8"´" },
		{ "aelig", u8"æ" },
		{ "agrave", u8"à" },
		{ "alefsym", u8"ℵ" },
		{ "alpha", u8"α" },
		{ "amp", u8"&" },
		{ "and", u8"∧" },
		{ "ang", u8"∠" },
		{ "apos", u8"'" },
		{ "aring", u8"å" },
		{ "asymp", u8"≈" },
		{ "atilde", u8"ã" },
		{ "auml", u8"ä" },
		{ "bdquo", u8"„" },
		{ "beta", u8"β" },
		{ "brvbar", u8"¦" },
		{ "bull", u8"•" },
		{ "cap", u8"∩" },
		{ "ccedil", u8"ç" },
		{ "cedil", u8"¸" },
		{ "cent", u8"¢" },
		{ "chi", u8"χ" },
		{ "circ", u8"ˆ" },
		{ "clubs", u8"♣" },
		{ "cong", u8"≅" },
		{ "copy", u8"©" },
		{ "crarr", u8"↵" },
		{ "cup", u8"∪" },
		{ "curren", u8"¤" },
		{ "dArr", u8"⇓" },
		{ "dagger", u8"†" },
		{ "darr", u8"↓" },
		{ "deg", u8"°" },
		{ "delta", u8"δ" },
		{ "diams", u8"♦" },
		{ "divide", u8"÷" },
		{ "eacute", u8"é" },
		{ "ecirc", u8"ê" },
		{ "egrave", u8"è" },
		{ "empty", u8"∅" },
		{ "emsp", u8"\u2003" }, // u8"\xE2\x80\x83" },
		{ "ensp", u8"\u2002" }, // u8"\xE2\x80\x82" },
		{ "epsilon", u8"ε" },
		{ "equiv", u8"≡" },
		{ "eta", u8"η" },
		{ "eth", u8"ð" },
		{ "euml", u8"ë" },
		{ "euro", u8"€" },
		{ "exist", u8"∃" },
		{ "fnof", u8"ƒ" },
		{ "forall", u8"∀" },
		{ "frac12", u8"½" },
		{ "frac14", u8"¼" },
		{ "frac34", u8"¾" },
		{ "frasl", u8"⁄" },
		{ "gamma", u8"γ" },
		{ "ge", u8"≥" },
		{ "gt", u8">" },
		{ "hArr", u8"⇔" },
		{ "harr", u8"↔" },
		{ "hearts", u8"♥" },
		{ "hellip", u8"…" },
		{ "iacute", u8"í" },
		{ "icirc", u8"î" },
		{ "iexcl", u8"¡" },
		{ "igrave", u8"ì" },
		{ "image", u8"ℑ" },
		{ "infin", u8"∞" },
		{ "int", u8"∫" },
		{ "iota", u8"ι" },
		{ "iquest", u8"¿" },
		{ "isin", u8"∈" },
		{ "iuml", u8"ï" },
		{ "kappa", u8"κ" },
		{ "lArr", u8"⇐" },
		{ "lambda", u8"λ" },
		{ "lang", u8"〈" },
		{ "laquo", u8"«" },
		{ "larr", u8"←" },
		{ "lceil", u8"⌈" },
		{ "ldquo", u8"“" },
		{ "le", u8"≤" },
		{ "lfloor", u8"⌊" },
		{ "lowast", u8"∗" },
		{ "loz", u8"◊" },
		{ "lrm", u8"\u200E" }, // u8"\xE2\x80\x8E" },
		{ "lsaquo", u8"‹" },
		{ "lsquo", u8"‘" },
		{ "lt", u8"<" },
		{ "macr", u8"¯" },
		{ "mdash", u8"—" },
		{ "micro", u8"µ" },
		{ "middot", u8"·" },
		{ "minus", u8"−" },
		{ "mu", u8"μ" },
		{ "nabla", u8"∇" },
		{ "nbsp", u8"\u00A0" }, // u8"\xC2\xA0" },
		{ "ndash", u8"–" },
		{ "ne", u8"≠" },
		{ "ni", u8"∋" },
		{ "not", u8"¬" },
		{ "notin", u8"∉" },
		{ "nsub", u8"⊄" },
		{ "ntilde", u8"ñ" },
		{ "nu", u8"ν" },
		{ "oacute", u8"ó" },
		{ "ocirc", u8"ô" },
		{ "oelig", u8"œ" },
		{ "ograve", u8"ò" },
		{ "oline", u8"‾" },
		{ "omega", u8"ω" },
		{ "omicron", u8"ο" },
		{ "oplus", u8"⊕" },
		{ "or", u8"∨" },
		{ "ordf", u8"ª" },
		{ "ordm", u8"º" },
		{ "oslash", u8"ø" },
		{ "otilde", u8"õ" },
		{ "otimes", u8"⊗" },
		{ "ouml", u8"ö" },
		{ "para", u8"¶" },
		{ "part", u8"∂" },
		{ "permil", u8"‰" },
		{ "perp", u8"⊥" },
		{ "phi", u8"φ" },
		{ "pi", u8"π" },
		{ "piv", u8"ϖ" },
		{ "plusmn", u8"±" },
		{ "pound", u8"£" },
		{ "prime", u8"′" },
		{ "prod", u8"∏" },
		{ "prop", u8"∝" },
		{ "psi", u8"ψ" },
		{ "quot", u8"\"" },
		{ "rArr", u8"⇒" },
		{ "radic", u8"√" },
		{ "rang", u8"〉" },
		{ "raquo", u8"»" },
		{ "rarr", u8"→" },
		{ "rceil", u8"⌉" },
		{ "rdquo", u8"”" },
		{ "real", u8"ℜ" },
		{ "reg", u8"®" },
		{ "rfloor", u8"⌋" },
		{ "rho", u8"ρ" },
		{ "rlm", u8"\u200F" }, // u8"\xE2\x80\x8F" },
		{ "rsaquo", u8"›" },
		{ "rsquo", u8"’" },
		{ "sbquo", u8"‚" },
		{ "scaron", u8"š" },
		{ "sdot", u8"⋅" },
		{ "sect", u8"§" },
		{ "shy", u8"\u00AD" }, // u8"\xC2\xAD" },
		{ "sigma", u8"σ" },
		{ "sigmaf", u8"ς" },
		{ "sim", u8"∼" },
		{ "spades", u8"♠" },
		{ "sub", u8"⊂" },
		{ "sube", u8"⊆" },
		{ "sum", u8"∑" },
		{ "sup1", u8"¹" },
		{ "sup2", u8"²" },
		{ "sup3", u8"³" },
		{ "sup", u8"⊃" },
		{ "supe", u8"⊇" },
		{ "szlig", u8"ß" },
		{ "tau", u8"τ" },
		{ "there4", u8"∴" },
		{ "theta", u8"θ" },
		{ "thetasym", u8"ϑ" },
		{ "thinsp", u8"\u2009" }, // u8"\xE2\x80\x89" },
		{ "thorn", u8"þ" },
		{ "tilde", u8"˜" },
		{ "times", u8"×" },
		{ "trade", u8"™" },
		{ "uArr", u8"⇑" },
		{ "uacute", u8"ú" },
		{ "uarr", u8"↑" },
		{ "ucirc", u8"û" },
		{ "ugrave", u8"ù" },
		{ "uml", u8"¨" },
		{ "upsih", u8"ϒ" },
		{ "upsilon", u8"υ" },
		{ "uuml", u8"ü" },
		{ "weierp", u8"℘" },
		{ "xi", u8"ξ" },
		{ "yacute", u8"ý" },
		{ "yen", u8"¥" },
		{ "yuml", u8"ÿ" },
		{ "zeta", u8"ζ" },
		{ "zwj", u8"\u200D" }, // u8"\xE2\x80\x8D" },
		{ "zwnj", u8"\u200C" }, // u8"\xE2\x80\x8C" }
	};
}
 