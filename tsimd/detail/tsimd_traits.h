// ========================================================================== //
// The MIT License (MIT)                                                      //
//                                                                            //
// Copyright (c) 2017 Jefferson Amstutz                                       //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// in all copies or substantial portions of the Software.                     //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
// ========================================================================== //

#pragma once

#include "config.h"
#include "bool_t.h"

namespace tsimd {
  namespace traits {

    // Defined a struct to get around type conversion problems ////////////////

    struct undefined_type
    {
    };

    // C++14 traits for C++11 /////////////////////////////////////////////////

    template <bool B, class T = void>
    using enable_if_t = typename std::enable_if<B, T>::type;

    // If a single type is convertable to another /////////////////////////////

    template <typename FROM, typename TO>
    using can_convert = enable_if_t<std::is_convertible<TO, FROM>::value>;

    // Is a pack<> with given width ///////////////////////////////////////////

    // TODO: verify if it is indeed a pack! this trait fails if given anything
    //       other than T = pack<something, something>!

    template <typename T, int W>
    struct is_pack_of_width
    {
      static const bool value = (T::static_size == W);
    };

    template <typename T, int W>
    using is_pack_of_width_t = enable_if_t<is_pack_of_width<T, W>::value>;

    // If a given T is a valid type for use in a pack<> ///////////////////////

    template <typename T>
    struct valid_type_for_pack
    {
      static const bool value = std::is_same<T, float>::value ||
                                std::is_same<T, int>::value ||
                                std::is_same<T, bool32_t>::value ||
                                std::is_same<T, double>::value ||
                                std::is_same<T, long long>::value ||
                                std::is_same<T, bool64_t>::value;
    };

    template <typename T>
    using valid_type_for_pack_t = enable_if_t<valid_type_for_pack<T>::value>;

    // If given type is a bool32_t or bool64_t ////////////////////////////////

    template <typename T>
    struct is_bool
    {
      static const bool value = std::is_same<T, bool32_t>::value ||
                                std::is_same<T, bool64_t>::value;
    };

    template <typename T>
    using is_bool_t = enable_if_t<is_bool<T>::value>;

    // If given pack is a mask (vbool) or not /////////////////////////////////

    template <typename MASK_T>
    struct is_mask
    {
      using value_t = typename MASK_T::value_t;
      static const bool value = std::is_same<value_t, bool32_t>::value ||
                                std::is_same<value_t, bool64_t>::value;
    };

    template <typename MASK_T>
    using is_mask_t = enable_if_t<is_mask<MASK_T>::value>;

    // Provide intrinsic type given a SIMD width //////////////////////////////

    template <typename T, int W>
    struct simd_type
    {
      using type = undefined_type;
    };

    // 1-wide //

    template <>
    struct simd_type<float, 1>
    {
      using type = float;
    };

    template <>
    struct simd_type<int, 1>
    {
      using type = int;
    };

    template <>
    struct simd_type<bool32_t, 1>
    {
      using type = simd_type<float, 1>::type;
    };

    template <>
    struct simd_type<double, 1>
    {
      using type = double;
    };

    template <>
    struct simd_type<long long, 1>
    {
      using type = long long;
    };

    template <>
    struct simd_type<bool64_t, 1>
    {
      using type = simd_type<double, 1>::type;
    };

    // 4-wide //

    template <>
    struct simd_type<float, 4>
    {
#if defined(__SSE__)
      using type = __m128;
#else
      using type = float[4];
#endif
    };

    template <>
    struct simd_type<int, 4>
    {
#if defined(__SSE__)
      using type = __m128i;
#else
      using type = int[4];
#endif
    };

    template <>
    struct simd_type<bool32_t, 4>
    {
#if 0//defined(__SSE__)
      using type = __mmask8;
#else
      using type = simd_type<float, 4>::type;
#endif
    };

    template <>
    struct simd_type<double, 4>
    {
#if defined(__AVX512F__) || defined(__AVX2__) || defined(__AVX__)
      using type = __m256d;
#else
      using type = double[4];
#endif
    };

    template <>
    struct simd_type<long long, 4>
    {
#if defined(__AVX512F__) || defined(__AVX2__)
      using type = __m256i;
#else
      using type = long long[4];
#endif
    };

    template <>
    struct simd_type<bool64_t, 4>
    {
      using type = simd_type<double, 4>::type;
    };

    // 8-wide //

    template <>
    struct simd_type<float, 8>
    {
#if defined(__AVX512F__) || defined(__AVX2__) || defined(__AVX__)
      using type = __m256;
#else
      using type = float[8];
#endif
    };

    template <>
    struct simd_type<int, 8>
    {
#if defined(__AVX512F__) || defined(__AVX2__) || defined(__AVX__)
      using type = __m256i;
#else
      using type = int[8];
#endif
    };

    template <>
    struct simd_type<bool32_t, 8>
    {
      using type = simd_type<float, 8>::type;
    };

    // 16-wide //

    template <>
    struct simd_type<float, 16>
    {
#if defined(__AVX512F__)
      using type = __m512;
#else
      using type = float[16];
#endif
    };

    template <>
    struct simd_type<int, 16>
    {
#if defined(__AVX512F__)
      using type = __m512i;
#else
      using type = int[16];
#endif
    };

    template <>
    struct simd_type<bool32_t, 16>
    {
#if defined(__AVX512F__)
      using type = __mmask32;
#else
      using type = int[16];
#endif
    };

    // Provide a cast (float|int) intrinsic type given a SIMD width ///////

    template <typename T, int W>
    struct cast_simd_type
    {
      using type = undefined_type;
    };

    // 1-wide //

    template <>
    struct cast_simd_type<float, 1>
    {
      using type = simd_type<int, 1>::type;
    };

    template <>
    struct cast_simd_type<int, 1>
    {
      using type = simd_type<float, 1>::type;
    };

    template <>
    struct cast_simd_type<double, 1>
    {
      using type = simd_type<long long, 1>::type;
    };

    template <>
    struct cast_simd_type<long long, 1>
    {
      using type = simd_type<double, 1>::type;
    };

    // 4-wide //

    template <>
    struct cast_simd_type<float, 4>
    {
      using type = simd_type<int, 4>::type;
    };

    template <>
    struct cast_simd_type<int, 4>
    {
      using type = simd_type<float, 4>::type;
    };

    template <>
    struct cast_simd_type<bool32_t, 4>
    {
      using type = simd_type<int, 4>::type;
    };

    // 8-wide //

    template <>
    struct cast_simd_type<float, 8>
    {
      using type = simd_type<int, 8>::type;
    };

    template <>
    struct cast_simd_type<int, 8>
    {
      using type = simd_type<float, 8>::type;
    };

    // 16-wide //

    template <>
    struct cast_simd_type<float, 16>
    {
      using type = simd_type<int, 16>::type;
    };

    template <>
    struct cast_simd_type<int, 16>
    {
      using type = simd_type<float, 16>::type;
    };

    // Provide intrinsic type half the size of given width ////////////////////

    template <typename T, int W>
    struct half_simd_type
    {
      using type = undefined_type;
    };

    // 1-wide //

    template <>
    struct half_simd_type<int, 1>
    {
      using type = short;
    };

    // 4-wide //

    template <>
    struct half_simd_type<float, 4>
    {
      using type = float[2];
    };

    template <>
    struct half_simd_type<int, 4>
    {
      using type = int[2];
    };

    // 8-wide //

    template <>
    struct half_simd_type<float, 8>
    {
#if defined(__AVX512F__) || defined(__AVX2__) || defined(__AVX__)
      using type = __m128;
#else
      using type = float[4];
#endif
    };

    template <>
    struct half_simd_type<int, 8>
    {
#if defined(__AVX512F__) || defined(__AVX2__) || defined(__AVX__)
      using type = __m128i;
#else
      using type = int[4];
#endif
    };

    template <>
    struct half_simd_type<bool32_t, 8>
    {
      using type = half_simd_type<float, 8>::type;
    };

    // 16-wide //

    template <>
    struct half_simd_type<float, 16>
    {
#if defined(__AVX512F__)
      using type = __m256;
#else
      using type = float[8];
#endif
    };

    template <>
    struct half_simd_type<int, 16>
    {
#if defined(__AVX512F__)
      using type = __m256i;
#else
      using type = int[8];
#endif
    };

    template <>
    struct half_simd_type<bool32_t, 16>
    {
      using type = half_simd_type<float, 16>::type;
    };

    // Bool type for given primitive type /////////////////////////////////////

    template <typename T>
    struct bool_type_for
    {
      using type = undefined_type;
    };

    // 32-bit //

    template <>
    struct bool_type_for<float>
    {
      using type = bool32_t;
    };

    template <>
    struct bool_type_for<int>
    {
      using type = bool32_t;
    };

    template <>
    struct bool_type_for<bool32_t>
    {
      using type = bool32_t;
    };

    // 64-bit //

    template <>
    struct bool_type_for<double>
    {
      using type = bool64_t;
    };

    template <>
    struct bool_type_for<long long>
    {
      using type = bool64_t;
    };

    template <>
    struct bool_type_for<bool64_t>
    {
      using type = bool64_t;
    };

  }  // namespace traits
}  // namespace tsimd