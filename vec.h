//
// Copyright (c) 2010-2011 Linaro Limited
//
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the MIT License which accompanies
// this distribution, and is available at
// http://www.opensource.org/licenses/mit-license.php
//
// Contributors:
//     Jesse Barker - original implementation.
//
#ifndef VEC_H_
#define VEC_H_

#include <iostream> // only needed for print() functions...
#include <math.h>
#include <array>
#include <bit>
#include <numeric>
#include <utility>

namespace LibMatrix
{
enum class align : size_t
{
	none     = 0,
	element  = 1,
	vector   = 2,
	adaptive = 3
};

// A template class for creating, managing and operating on a 2-element vector
// of any type you like (intended for built-in types, but as long as it 
// supports the basic arithmetic and assignment operators, any type should
// work).
template<typename T, size_t N = 3, enum align A = align::adaptive, size_t N_POW2 = std::bit_ceil<size_t>(N)>
struct alignas(((N == N_POW2 && A != align::element) || A == align::vector) ? N_POW2 * sizeof(T) : sizeof(T)) tvec : std::array<T,N>
{
    tvec() : std::array<T,N>{} {}
    tvec(const T t) { (*this).fill((T)1); }
    template<typename... I> requires(sizeof...(I) > 1)
    tvec(const I... args) : std::array<T,N>{{ (T)args... }} {}

    template<size_t N_LHS, enum align A_RHS = align::adaptive> 
    operator tvec<T,N_LHS,A_RHS>() { return *reinterpret_cast<tvec<T,N_LHS,A_RHS>*>(this); }
    template<size_t N_LHS, enum align A_RHS = align::adaptive> 
    operator const tvec<T,N_LHS,A_RHS>() const { return *reinterpret_cast<const tvec<T,N_LHS,A_RHS>*>(this); }

    // Print the elements of the vector to standard out.
    // Really only useful for debug and test.
    void print() const
    {
    	std::cout << "| ";
    	for(T& i : (*this))
		std::cout << i << " ";
	std::cout << "|" << std::endl;
    }

    // Allow raw data access for API calls and the like.
    // For example, it is valid to pass a tvec2<float> into a call to
    // the OpenGL command "glUniform2fv()".
    operator const T*() const { return (*this).data(); }

    // Get and set access members for the individual elements.
    const T x() const { return (*this)[0]; }
    const T y() const { return (*this)[1]; }
    const T z() const { return (*this)[2]; }
    const T w() const { return (*this)[3]; }

    void x(const T& val) { (*this)[0] = val; }
    void y(const T& val) { (*this)[1] = val; }
    void z(const T& val) { (*this)[2] = val; }
    void w(const T& val) { (*this)[3] = val; }

    const tvec<T,3,A> yzx() const requires(N > 2) { return { (*this)[1], (*this)[2], (*this)[0] }; }

    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 / (T_RHS)1)>
    tvec<T_DST,N,A>& operator/=(const tvec<T_RHS,N_RHS,A_RHS>& rhs) { std::transform((*this).cbegin(), (*this).cbegin() + std::min((*this).size(),rhs.size()), rhs.cbegin(), (*this).begin(), std::divides<>{}); return *this; }
    template<typename T_RHS = T>
    tvec<T,N,A>& operator/=(const T_RHS& rhs) { (*this) /= tvec<T,N,A>((T)rhs); return *this; }
    template<typename T_RHS = T, typename T_DST = decltype((T)1 * (T_RHS)1)>
    const tvec<T,N,A> operator/(const T_RHS& rhs) const    { return tvec<T,N,A>(*this) /= (T)rhs; }

    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 / (T_RHS)1)>
    const tvec<T,N,A> operator/(const tvec<T_RHS,N_RHS,A_RHS>& rhs) const { return tvec<T,N,A>(*this) /= rhs; }

    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 * (T_RHS)1)>
    tvec<T_DST,N,A>& operator*=(const tvec<T_RHS,N_RHS,A_RHS>& rhs) { std::transform((*this).cbegin(), (*this).cbegin() + std::min((*this).size(),rhs.size()), rhs.cbegin(), (*this).begin(), std::multiplies<>{}); return *this; }
    template<typename T_RHS = T>
    tvec<T,N,A>& operator*=(const T_RHS& rhs) { (*this) *= tvec<T,N,A>((T)rhs); return *this; }
    template<typename T_RHS = T, typename T_DST = decltype((T)1 * (T_RHS)1)>
    const tvec<T,N,A> operator*(const T_RHS& rhs) const { return tvec<T,N,A>(*this) *= (T)rhs; }
    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 * (T_RHS)1)>
    const tvec<T,N,A> operator*(const tvec<T_RHS,N_RHS,A_RHS>& rhs) const { return tvec<T,N,A>(*this) *= rhs; }

    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 + (T_RHS)1)>
    tvec<T_DST,N,A>& operator+=(const tvec<T_RHS,N_RHS,A_RHS>& rhs) { std::transform((*this).cbegin(), (*this).cbegin() + std::min((*this).size(),rhs.size()), rhs.cbegin(), (*this).begin(), std::plus<>{}); return *this; }
    template<typename T_RHS = T>
    tvec<T,N,A>& operator+=(const T_RHS& rhs) { (*this) += tvec<T,N,A>((T)rhs); return *this; }
    template<typename T_RHS = T, typename T_DST = decltype((T)1 * (T_RHS)1)>
    const tvec<T,N,A> operator+(const T_RHS& rhs) const { return tvec<T,N,A>(*this) += (T)rhs; }
    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 + (T_RHS)1)>
    const tvec<T,N,A> operator+(const tvec<T_RHS,N_RHS,A_RHS>& rhs) const { return tvec<T,N,A>(*this) += rhs; }

    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 - (T_RHS)1)>
    tvec<T_DST,N,A>& operator-=(const tvec<T_RHS,N_RHS,A_RHS>& rhs) { std::transform((*this).cbegin(), (*this).cbegin() + std::min((*this).size(),rhs.size()), rhs.cbegin(), (*this).begin(), std::minus<>{}); return *this; }
    template<typename T_RHS = T>
    tvec<T,N,A>& operator-=(const T_RHS& rhs) { (*this) -= tvec<T,N,A>((T)rhs); return *this; }
    template<typename T_RHS = T, typename T_DST = decltype((T)1 * (T_RHS)1)>
    const tvec<T,N,A> operator-(const T_RHS& rhs) const { return tvec<T,N,A>(*this) -= (T)rhs; }
    template<typename T_RHS = T, size_t N_RHS = N, enum align A_RHS = A, typename T_DST = decltype((T)1 - (T_RHS)1)>
    const tvec<T,N,A> operator-(const tvec<T_RHS,N_RHS,A_RHS>& rhs) const { return tvec<T,N,A>(*this) -= rhs; }

    // Compute the length of this and return it.
    float length() const
    {
        return sqrt(dot(*this, *this));
    }

    // Make this a unit vector.
    void normalize()
    {
        float l = length();
	if(l != 0 && l != 1)
		(*this) /= l;
    }

    // Compute the dot product of two vectors.
    template<typename T_V1 = float, typename T_V2 = float, size_t N_V1 = 3, size_t N_V2 = 3, enum align A_V1 = align::adaptive, enum align A_V2 = align::adaptive, typename T_DST = decltype((T_V1)1*(T_V2)1)>
    static T_DST dot(const tvec<T_V1,N_V1,A_V1>& v1, const tvec<T_V2,N_V2,A_V2>& v2)
    {
    	tvec<T_DST, std::min<size_t>(N_V1,N_V2)> dst(v1 * v2);
        return (T_DST)std::accumulate(dst.begin(), dst.end(), 0); 
    }

    // Compute the cross product of two vectors.
    template<typename T_U = float, typename T_V = float, size_t N_U = 3, size_t N_V = 3, enum align A_U = align::adaptive, enum align A_V = align::adaptive, typename T_DST = decltype((T_U)1*(T_V)1)>
    static tvec<T_DST,3,A> cross(const tvec<T_U,N_U,A_U>& u, const tvec<T_V,N_V,A_V>& v)
    {
    	return (u * v.yzx() - u.yzx() * v).yzx();
    }
};

template<typename T, enum align A = align::adaptive>
using tvec2 = tvec<T, 2, A>;
template<typename T, enum align A = align::adaptive>
using tvec3 = tvec<T, 3, A>;
template<typename T, enum align A = align::adaptive>
using tvec4 = tvec<T, 4, A>;

//
// Convenience typedefs.  These are here to present a homogeneous view of these
// objects with respect to shader source.
//
typedef tvec2<float> vec2;
typedef tvec3<float> vec3;
typedef tvec4<float> vec4;

typedef tvec2<double> dvec2;
typedef tvec3<double> dvec3;
typedef tvec4<double> dvec4;

typedef tvec2<int> ivec2;
typedef tvec3<int> ivec3;
typedef tvec4<int> ivec4;

typedef tvec2<unsigned int> uvec2;
typedef tvec3<unsigned int> uvec3;
typedef tvec4<unsigned int> uvec4;

typedef tvec2<bool> bvec2;
typedef tvec3<bool> bvec3;
typedef tvec4<bool> bvec4;

} // namespace LibMatrix

// Global operators to allow for things like defining a new vector in terms of
// a product of a scalar and a vector
template<typename T>
const LibMatrix::tvec2<T> operator*(const T t, const LibMatrix::tvec2<T>& v)
{
    return v * t;
}

template<typename T>
const LibMatrix::tvec3<T> operator*(const T t, const LibMatrix::tvec3<T>& v)
{
    return v * t;
}

template<typename T>
const LibMatrix::tvec4<T> operator*(const T t, const LibMatrix::tvec4<T>& v)
{
    return v * t;
}

#endif // VEC_H_
