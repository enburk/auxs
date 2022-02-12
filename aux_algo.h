template<class binary_predicate>
auto lower_bound (value_type const& e,
    binary_predicate order = std::less<>{}) const
{
    return std::lower_bound(begin(), end(), e, order);
}
template<class binary_predicate>
auto upper_bound (value_type const& e,
    binary_predicate order = std::less<>{}) const
{
    return std::upper_bound(begin(), end(), e, order);
}
template<class binary_predicate>
auto equal_range (value_type const& e,
    binary_predicate order = std::less<>{}) const
{
    auto r = std::equal_range (begin(), end(), e, order);
    return range(r.first, r.second);
}
template<class binary_predicate>
auto equal_range (value_type const& e,
    binary_predicate order = std::less<>{})
{
    auto r = std::equal_range (begin(), end(), e, order);
    return range(r.first, r.second);
}


auto first () { return range(begin(), begin() + min(1, size())); }
auto last  () { return range(end() - min(1, size()), end()); }

template<class T>
auto first (T r)
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::search(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + r.size());
}
template<class T>
auto first (T r) const
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::search(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + r.size());
}
template<class T>
auto last (T r)
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::find_end(begin(), end(), r.begin(), r.end());
    return it == end() ? range(begin(), begin()) :
        range(it, it + r.size());
}
template<class T>
auto last (T r) const
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::find_end(begin(), end(), r.begin(), r.end());
    return it == end() ? range(begin(), begin()) :
        range(it, it + r.size());
}
auto first (value_type const& e) const {
    auto it = std::find(begin(), end(), e);
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
auto first (value_type const& e) {
    auto it = std::find(begin(), end(), e);
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
auto last (value_type const& e) const {
    auto it = std::find(rbegin(), rend(), e);
    return it == rend() ? range(begin(), begin()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}
auto last (value_type const& e) {
    auto it = std::find(rbegin(), rend(), e);
    return it == rend() ? range(begin(), begin()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}
auto first (const char* s) const
    requires std::same_as<value_type, char> {
        return first(std::string_view(s));
}
auto first (const char* s)
    requires std::same_as<value_type, char> {
        return first(std::string_view(s));
}
auto last (const char* s) const
    requires std::same_as<value_type, char> {
        return last(std::string_view(s));
}
auto last (const char* s)
    requires std::same_as<value_type, char> {
        return last(std::string_view(s));
}


template<class T>
auto first (one_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto first (one_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto last (one_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(rbegin(), rend(), r.begin(), r.end());
    return it == rend() ? range(begin(), begin()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}
template<class T>
auto last (one_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(rbegin(), rend(), r.begin(), r.end());
    return it == rend() ? range(begin(), begin()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}

template<class T>
auto first (one_not_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(begin(), end(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto first (one_not_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(begin(), end(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto last (one_not_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(rbegin(), rend(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == rend() ? range(begin(), begin()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}
template<class T>
auto last (one_not_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(rbegin(), rend(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == rend() ? range(begin(), begin()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}

bool contains (auto what) const
{
    return first(what).size() != 0;
}

template<class T>
bool contains_only (T r) const
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    return std::equal(begin(), end(),
        r.begin(), r.end());
}
bool contains_only (const char* s) const
    requires std::same_as<value_type, char> {
        return contains_only(std::string(s));
}
template<class T>
bool contains_only (one_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    return not contains(one_not_of{r});
}
template<class T>
bool contains_only (one_not_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    return not contains(one_of(r));
}
