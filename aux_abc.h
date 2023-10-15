#pragma once
#include "aux_array.h"
#include "aux_string.h"
#include "aux_registry.h"
#include "aux_unicode.h"
#include "aux_timing.h"
#include "aux_graph.h"
#include "aux_coro.h"
#include "aux_hask.h"

using aux::max;
using aux::min;
using aux::clamp;
using aux::polymorphic;
using aux::overloaded;
using aux::expected;
using aux::nothing;
using aux::deque;
using aux::array;
using aux::str;
using aux::one_of;
using aux::one_not_of;
using aux::enumerate;
using aux::task;
using aux::timing;
using aux::format;

using std::ranges::views::reverse;
using std::numbers::pi;
using std::sin;
using std::cos;

template <class... xx> using hashmap = std::unordered_map<xx...>;
template <class... xx> using hashset = std::unordered_set<xx...>;

