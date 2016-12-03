#ifndef PHI_UTILS_TYPES_H
#define PHI_UTILS_TYPES_H
#include <boost/range/any_range.hpp>

namespace phi {

template <typename T>
using AnyRange = boost::any_range<T, boost::forward_traversal_tag>;

} // namespace phi

#endif
