#ifndef PYTHONIC_NUMPY_SETDIFF1D_HPP
#define PYTHONIC_NUMPY_SETDIFF1D_HPP

#include "pythonic/include/numpy/setdiff1d.hpp"

#include "pythonic/utils/functor.hpp"
#include "pythonic/types/ndarray.hpp"
#include "pythonic/numpy/asarray.hpp"

#include <set>
#include <algorithm>

PYTHONIC_NS_BEGIN

namespace numpy
{
  namespace impl
  {

    template <typename InputIterator1, typename InputIterator2,
              typename OutputIterator>
    OutputIterator
    set_difference_unique(InputIterator1 first1, InputIterator1 last1,
                          InputIterator2 first2, InputIterator2 last2,
                          OutputIterator result)
    {
      while (first1 != last1 && first2 != last2) {
        auto const t1 = *first1;
        auto const t2 = *first2;
        if (t1 < t2) {
          *result = t1;
          while (*++first1 == t1)
            ;
          ++result;
        } else if (t2 < t1)
          while (*++first2 == t2)
            ;
        else {
          while (*++first1 == t1)
            ;
          while (*++first2 == t2)
            ;
        }
      }
      while (first1 != last1) {
        auto const t1 = *first1;
        *result = t1;
        while (*++first1 == t1)
          ;
        ++result;
      }
      return result;
    }
  } // namespace impl

  template <class T, class U>
  types::ndarray<typename __combined<typename types::dtype_of<T>::type,
                                     typename types::dtype_of<U>::type>::type,
                 types::pshape<long>>
  setdiff1d(T const &ar1, U const &ar2, bool assume_unique)
  {
    using dtype = typename __combined<typename types::dtype_of<T>::type,
                                      typename types::dtype_of<U>::type>::type;
    auto far1 = numpy::functor::array{}(ar1);
    auto far2 = numpy::functor::array{}(ar2);
    if (assume_unique) {
      std::sort(far1.fbegin(), far1.fend());
      std::sort(far2.fbegin(), far2.fend());
      dtype *out =
          (dtype *)malloc(far1.flat_size() * far2.flat_size() * sizeof(dtype));
      dtype *out_last = std::set_difference(far1.fbegin(), far1.fend(),
                                            far2.fbegin(), far2.fend(), out);
      auto size = out_last - out;
      out = (dtype *)realloc(out, size * sizeof(dtype));
      return {out, types::pshape<long>(size), types::ownership::owned};
    } else {
      std::sort(far1.fbegin(), far1.fend());
      std::sort(far2.fbegin(), far2.fend());
      dtype *out =
          (dtype *)malloc(far1.flat_size() * far2.flat_size() * sizeof(dtype));
      dtype *out_last = impl::set_difference_unique(
          far1.fbegin(), far1.fend(), far2.fbegin(), far2.fend(), out);
      auto size = out_last - out;
      out = (dtype *)realloc(out, size * sizeof(dtype));
      return {out, types::pshape<long>(size), types::ownership::owned};
    }
  }
}
PYTHONIC_NS_END

#endif
