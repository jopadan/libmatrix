//
// Copyright (c) 2010 Linaro Limited
//
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the MIT License which accompanies
// this distribution, and is available at
// http://www.opensource.org/licenses/mit-license.php
//
// Contributors:
//     Jesse Barker - original implementation.
//
#ifndef MAT_H_
#define MAT_H_

#include <iostream>
#include <iomanip>
#include "vec.h"

namespace LibMatrix
{
// Proxy class for providing the functionality of a doubly-dimensioned array
// representation of matrices.  Each matrix class defines its operator[]
// to return an ArrayProxy.  The ArrayProxy then returns the appropriate item
// from its operator[].
template<typename T, unsigned int dimension>
class ArrayProxy
{
public:
    ArrayProxy(T* data) { data_ = data; }
    ~ArrayProxy() { data_ = 0; }
    T& operator[](int index)
    {
        return data_[index * dimension];
    }
    const T& operator[](int index) const
    {
        return data_[index * dimension];
    }
private:
    T* data_;
};


// Programming interfaces to all matrix objects are represented row-centric 
// (i.e. C/C++ style references to the data appear as matrix[row][column]).  
// However, the internal data representation is column-major, so when using 
// the raw data access member to treat the data as a singly-dimensioned array,
// it does not have to be transposed.
template<typename T>
class tmat2
{
public:
    tmat2()
    {
        setIdentity();
    }
    tmat2(const tmat2& m)
    {
        m_[0] = m.m_[0];
        m_[1] = m.m_[1];
        m_[2] = m.m_[2];
        m_[3] = m.m_[3];
    }
    ~tmat2() {}

    void setIdentity()
    {
        m_[0] = 1;
        m_[1] = 0;
        m_[2] = 0;
        m_[3] = 1;
    }

    tmat2& transpose()
    {
        T tmp_val = m_[1];
        m_[1] = m_[2];
        m_[2] = tmp_val;
        return *this;
    }

    void print() const
    {
        static const int precision(6);
        // row 0
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][1];
        std::cout << " |" << std::endl;
        // row 1
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][1];
        std::cout << " |" << std::endl;
    }

    operator const T*() const { return &m_[0];}

    tmat2& operator=(const tmat2& rhs)
    {
        if (this != &rhs)
        {
            m_[0] = rhs.m_[0];
            m_[1] = rhs.m_[1];
            m_[2] = rhs.m_[2];
            m_[3] = rhs.m_[3];
        }
        return *this;
    }

    tmat2& operator+=(const tmat2& rhs)
    {
        m_[0] += rhs.m_[0];
        m_[1] += rhs.m_[1];
        m_[2] += rhs.m_[2];
        m_[3] += rhs.m_[3];
        return *this;
    }

    const tmat2 operator+(const tmat2& rhs)
    {
        return tmat2(*this) += rhs;
    }

    tmat2& operator-=(const tmat2& rhs)
    {
        m_[0] -= rhs.m_[0];
        m_[1] -= rhs.m_[1];
        m_[2] -= rhs.m_[2];
        m_[3] -= rhs.m_[3];
        return *this;
    }

    const tmat2 operator-(const tmat2& rhs)
    {
        return tmat2(*this) += rhs;
    }

    tmat2& operator*=(const tmat2& rhs)
    {
        T c0r0((m_[0] * rhs.m_[0]) + (m_[2] * rhs.m_[1]));
        T c0r1((m_[1] * rhs.m_[0]) + (m_[3] * rhs.m_[1]));
        T c1r0((m_[0] * rhs.m_[2]) + (m_[2] * rhs.m_[3]));
        T c1r1((m_[1] * rhs.m_[2]) + (m_[3] * rhs.m_[3]));
        m_[0] = c0r0;
        m_[1] = c0r1;
        m_[2] = c1r0;
        m_[3] = c1r1;
        return *this;
    }

    const tmat2 operator*(const tmat2& rhs)
    {
        return tmat2(*this) *= rhs;
    }

    tmat2& operator*=(const T& rhs)
    {
        m_[0] *= rhs;
        m_[1] *= rhs;
        m_[2] *= rhs;
        m_[3] *= rhs;
        return *this;
    }

    const tmat2 operator*(const T& rhs)
    {
        return tmat2(*this) *= rhs;
    }

    tmat2& operator/=(const T& rhs)
    {
        m_[0] /= rhs;
        m_[1] /= rhs;
        m_[2] /= rhs;
        m_[3] /= rhs;
        return *this;
    }

    const tmat2 operator/(const T& rhs)
    {
        return tmat2(*this) /= rhs;
    }

    ArrayProxy<T, 2> operator[](int index)
    {
        return ArrayProxy<T, 2>(&m_[index]);
    }
    const ArrayProxy<T, 2> operator[](int index) const
    {
        return ArrayProxy<T, 2>(const_cast<T*>(&m_[index]));
    }

private:
    T m_[4];
};

template<typename T>
const tmat2<T> operator*(const T& lhs, const tmat2<T>& rhs)
{
    return tmat2<T>(rhs) * lhs;
}

template<typename T>
const tvec2<T> operator*(const tvec2<T>& lhs, const tmat2<T>& rhs)
{
    T x((lhs.x() * rhs[0][0]) + (lhs.y() * rhs[1][0]));
    T y((lhs.x() * rhs[0][1]) + (lhs.y() * rhs[1][1]));
    return tvec2<T>(x,y);
}

template<typename T>
const tvec2<T> operator*(const tmat2<T>& lhs, const tvec2<T>& rhs)
{
    T x((lhs[0][0] * rhs.x()) + (lhs[0][1] * rhs.y()));
    T y((lhs[1][0] * rhs.x()) + (lhs[1][1] * rhs.y()));
    return tvec2<T>(x, y);
}

template<typename T>
const tmat2<T> outer(const tvec2<T>& a, const tvec2<T>& b)
{
    tmat2<T> product;
    product[0][0] = a.x() * b.x();
    product[0][1] = a.x() * b.y();
    product[1][0] = a.y() * b.x();
    product[1][1] = a.y() * b.y();
    return product;
}

template<typename T>
class tmat3
{
public:
    tmat3()
    {
        setIdentity();
    }
    tmat3(const tmat3& m)
    {
        m_[0] = m.m_[0];
        m_[1] = m.m_[1];
        m_[2] = m.m_[2];
        m_[3] = m.m_[3];
        m_[4] = m.m_[4];
        m_[5] = m.m_[5];
        m_[6] = m.m_[6];
        m_[7] = m.m_[7];
        m_[8] = m.m_[8];
    }
    ~tmat3() {}

    void setIdentity()
    {
        m_[0] = 1;
        m_[1] = 0;
        m_[2] = 0;
        m_[3] = 0;
        m_[4] = 1;
        m_[5] = 0;
        m_[6] = 0;
        m_[7] = 0;
        m_[8] = 1;
    }

    tmat3& transpose()
    {
        T tmp_val = m_[1];
        m_[1] = m_[3];
        m_[3] = tmp_val;
        tmp_val = m_[2];
        m_[2] = m_[6];
        m_[6] = tmp_val;
        tmp_val = m_[5];
        m_[5] = m_[7];
        m_[7] = tmp_val;
        return *this;
    }

    void print() const
    {
        static const int precision(6);
        // row 0
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][1];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][2];
        std::cout << " |" << std::endl;
        // row 1
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][1];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][2];
        std::cout << " |" << std::endl;
        // row 2
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[2][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[2][1];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[2][2];
        std::cout << " |" << std::endl;
    }

    operator const T*() const { return &m_[0];}

    tmat3& operator=(const tmat3& rhs)
    {
        if (this != &rhs)
        {
            m_[0] = rhs.m_[0];
            m_[1] = rhs.m_[1];
            m_[2] = rhs.m_[2];
            m_[3] = rhs.m_[3];
            m_[4] = rhs.m_[4];
            m_[5] = rhs.m_[5];
            m_[6] = rhs.m_[6];
            m_[7] = rhs.m_[7];
            m_[8] = rhs.m_[8];
        }
        return *this;
    }

    tmat3& operator+=(const tmat3& rhs)
    {
        m_[0] += rhs.m_[0];
        m_[1] += rhs.m_[1];
        m_[2] += rhs.m_[2];
        m_[3] += rhs.m_[3];
        m_[4] += rhs.m_[4];
        m_[5] += rhs.m_[5];
        m_[6] += rhs.m_[6];
        m_[7] += rhs.m_[7];
        m_[8] += rhs.m_[8];
        return *this;
    }

    const tmat3 operator+(const tmat3& rhs)
    {
        return tmat3(*this) += rhs;
    }

    tmat3& operator-=(const tmat3& rhs)
    {
        m_[0] -= rhs.m_[0];
        m_[1] -= rhs.m_[1];
        m_[2] -= rhs.m_[2];
        m_[3] -= rhs.m_[3];
        m_[4] -= rhs.m_[4];
        m_[5] -= rhs.m_[5];
        m_[6] -= rhs.m_[6];
        m_[7] -= rhs.m_[7];
        m_[8] -= rhs.m_[8];
        return *this;
    }

    const tmat3 operator-(const tmat3& rhs)
    {
        return tmat3(*this) -= rhs;
    }

    tmat3& operator*=(const tmat3& rhs)
    {
        T c0r0((m_[0] * rhs.m_[0]) + (m_[3] * rhs.m_[1]) + (m_[6] * rhs.m_[2]));
        T c0r1((m_[1] * rhs.m_[0]) + (m_[4] * rhs.m_[1]) + (m_[7] * rhs.m_[2]));
        T c0r2((m_[2] * rhs.m_[0]) + (m_[5] * rhs.m_[1]) + (m_[8] * rhs.m_[2]));
        T c1r0((m_[0] * rhs.m_[3]) + (m_[3] * rhs.m_[4]) + (m_[6] * rhs.m_[5]));
        T c1r1((m_[1] * rhs.m_[3]) + (m_[4] * rhs.m_[4]) + (m_[7] * rhs.m_[5]));
        T c1r2((m_[2] * rhs.m_[3]) + (m_[5] * rhs.m_[4]) + (m_[8] * rhs.m_[5]));
        T c2r0((m_[0] * rhs.m_[6]) + (m_[3] * rhs.m_[7]) + (m_[6] * rhs.m_[8]));
        T c2r1((m_[1] * rhs.m_[6]) + (m_[4] * rhs.m_[7]) + (m_[7] * rhs.m_[8]));
        T c2r2((m_[2] * rhs.m_[6]) + (m_[5] * rhs.m_[7]) + (m_[8] * rhs.m_[8]));
        m_[0] = c0r0;
        m_[1] = c0r1;
        m_[2] = c0r2;
        m_[3] = c1r0;
        m_[4] = c1r1;
        m_[5] = c1r2;
        m_[6] = c2r0;
        m_[7] = c2r1;
        m_[8] = c2r2;
        return *this;
    }

    const tmat3 operator*(const tmat3& rhs)
    {
        return tmat3(*this) *= rhs;
    }

    tmat3& operator*=(const T& rhs)
    {
        m_[0] *= rhs;
        m_[1] *= rhs;
        m_[2] *= rhs;
        m_[3] *= rhs;
        m_[4] *= rhs;
        m_[5] *= rhs;
        m_[6] *= rhs;
        m_[7] *= rhs;
        m_[8] *= rhs;
        return *this;
    }

    const tmat3 operator*(const T& rhs)
    {
        return tmat3(*this) *= rhs;
    }

    tmat3& operator/=(const T& rhs)
    {
        m_[0] /= rhs;
        m_[1] /= rhs;
        m_[2] /= rhs;
        m_[3] /= rhs;
        m_[4] /= rhs;
        m_[5] /= rhs;
        m_[6] /= rhs;
        m_[7] /= rhs;
        m_[8] /= rhs;
        return *this;
    }

    const tmat3 operator/(const T& rhs)
    {
        return tmat3(*this) /= rhs;
    }

    ArrayProxy<T, 3> operator[](int index)
    {
        return ArrayProxy<T, 3>(&m_[index]);
    }
    const ArrayProxy<T, 3> operator[](int index) const
    {
        return ArrayProxy<T, 3>(const_cast<T*>(&m_[index]));
    }

private:
    T m_[9];
};

template<typename T>
const tmat3<T> operator*(const T& lhs, const tmat3<T>& rhs)
{
    return tmat3<T>(rhs) * lhs;
}

template<typename T>
const tvec3<T> operator*(const tvec3<T>& lhs, const tmat3<T>& rhs)
{
    T x((lhs.x() * rhs[0][0]) + (lhs.y() * rhs[1][0]) + (lhs.z() * rhs[2][0]));
    T y((lhs.x() * rhs[0][1]) + (lhs.y() * rhs[1][1]) + (lhs.z() * rhs[2][1]));
    T z((lhs.x() * rhs[0][2]) + (lhs.y() * rhs[1][2]) + (lhs.z() * rhs[2][2]));
    return tvec3<T>(x, y, z);
}

template<typename T>
const tvec3<T> operator*(const tmat3<T>& lhs, const tvec3<T>& rhs)
{
    T x((lhs[0][0] * rhs.x()) + (lhs[0][1] * rhs.y()) + (lhs[0][2] * rhs.z()));
    T y((lhs[1][0] * rhs.x()) + (lhs[1][1] * rhs.y()) + (lhs[1][2] * rhs.z()));
    T z((lhs[2][0] * rhs.x()) + (lhs[2][1] * rhs.y()) + (lhs[2][2] * rhs.z()));
    return tvec3<T>(x, y, z);
}

template<typename T>
const tmat3<T> outer(const tvec3<T>& a, const tvec3<T>& b)
{
    tmat3<T> product;
    product[0][0] = a.x() * b.x();
    product[0][1] = a.x() * b.y();
    product[0][2] = a.x() * b.z();
    product[1][0] = a.y() * b.x();
    product[1][1] = a.y() * b.y();
    product[1][2] = a.y() * b.z();
    product[2][0] = a.z() * b.x();
    product[2][1] = a.z() * b.y();
    product[2][2] = a.z() * b.z();
    return product;
}

template<typename T>
class tmat4
{
public:
    tmat4()
    {
        setIdentity();
    }
    tmat4(const tmat4& m)
    {
        m_[0] = m.m_[0];
        m_[1] = m.m_[1];
        m_[2] = m.m_[2];
        m_[3] = m.m_[3];
        m_[4] = m.m_[4];
        m_[5] = m.m_[5];
        m_[6] = m.m_[6];
        m_[7] = m.m_[7];
        m_[8] = m.m_[8];
        m_[9] = m.m_[9];
        m_[10] = m.m_[10];
        m_[11] = m.m_[11];
        m_[12] = m.m_[12];
        m_[13] = m.m_[13];
        m_[14] = m.m_[14];
        m_[15] = m.m_[15];
    }
    ~tmat4() {}

    void setIdentity()
    {
        m_[0] = 1;
        m_[1] = 0;
        m_[2] = 0;
        m_[3] = 0;
        m_[4] = 0;
        m_[5] = 1;
        m_[6] = 0;
        m_[7] = 0;
        m_[8] = 0;
        m_[9] = 0;
        m_[10] = 1;
        m_[11] = 0;
        m_[12] = 0;
        m_[13] = 0;
        m_[14] = 0;
        m_[15] = 1;
    }

    tmat4& transpose()
    {
        T tmp_val = m_[1];
        m_[1] = m_[4];
        m_[4] = tmp_val;
        tmp_val = m_[2];
        m_[2] = m_[8];
        m_[8] = tmp_val;
        tmp_val = m_[3];
        m_[3] = m_[12];
        m_[12] = tmp_val;
        tmp_val = m_[6];
        m_[6] = m_[9];
        m_[9] = tmp_val;
        tmp_val = m_[7];
        m_[7] = m_[13];
        m_[13] = tmp_val;
        tmp_val = m_[11];
        m_[11] = m_[14];
        m_[14] = tmp_val;
        return *this;
    }

    void print() const
    {
        static const int precision(6);
        // row 0
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][1];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][2];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[0][3];
        std::cout << " |" << std::endl;
        // row 1
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][1];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][2];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[1][3];
        std::cout << " |" << std::endl;
        // row 2
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[2][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[2][1];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[2][2];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[2][3];
        std::cout << " |" << std::endl;
        // row 3
        std::cout << "| ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[3][0];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[3][1];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[3][2];
        std::cout << " ";
        std::cout << std::fixed << std::showpoint << std::setprecision(precision) << m_[3][3];
        std::cout << " |" << std::endl;
    }

    operator const T*() const { return &m_[0];}

    tmat4& operator=(const tmat4& rhs)
    {
        if (this != &rhs)
        {
            m_[0] = rhs.m_[0];
            m_[1] = rhs.m_[1];
            m_[2] = rhs.m_[2];
            m_[3] = rhs.m_[3];
            m_[4] = rhs.m_[4];
            m_[5] = rhs.m_[5];
            m_[6] = rhs.m_[6];
            m_[7] = rhs.m_[7];
            m_[8] = rhs.m_[8];
            m_[9] = rhs.m_[9];
            m_[10] = rhs.m_[10];
            m_[11] = rhs.m_[11];
            m_[12] = rhs.m_[12];
            m_[13] = rhs.m_[13];
            m_[14] = rhs.m_[14];
            m_[15] = rhs.m_[15];
        }
        return *this;
    }

    tmat4& operator+=(const tmat4& rhs)
    {
        m_[0] += rhs.m_[0];
        m_[1] += rhs.m_[1];
        m_[2] += rhs.m_[2];
        m_[3] += rhs.m_[3];
        m_[4] += rhs.m_[4];
        m_[5] += rhs.m_[5];
        m_[6] += rhs.m_[6];
        m_[7] += rhs.m_[7];
        m_[8] += rhs.m_[8];
        m_[9] += rhs.m_[9];
        m_[10] += rhs.m_[10];
        m_[11] += rhs.m_[11];
        m_[12] += rhs.m_[12];
        m_[13] += rhs.m_[13];
        m_[14] += rhs.m_[14];
        m_[15] += rhs.m_[15];
        return *this;
    }

    const tmat4 operator+(const tmat4& rhs)
    {
        return tmat4(*this) += rhs;
    }

    tmat4& operator-=(const tmat4& rhs)
    {
        m_[0] -= rhs.m_[0];
        m_[1] -= rhs.m_[1];
        m_[2] -= rhs.m_[2];
        m_[3] -= rhs.m_[3];
        m_[4] -= rhs.m_[4];
        m_[5] -= rhs.m_[5];
        m_[6] -= rhs.m_[6];
        m_[7] -= rhs.m_[7];
        m_[8] -= rhs.m_[8];
        m_[9] -= rhs.m_[9];
        m_[10] -= rhs.m_[10];
        m_[11] -= rhs.m_[11];
        m_[12] -= rhs.m_[12];
        m_[13] -= rhs.m_[13];
        m_[14] -= rhs.m_[14];
        m_[15] -= rhs.m_[15];
        return *this;
    }

    const tmat4 operator-(const tmat4& rhs)
    {
        return tmat4(*this) -= rhs;
    }

    tmat4& operator*=(const tmat4& rhs)
    {
        T c0r0((m_[0] * rhs.m_[0]) + (m_[4] * rhs.m_[1]) + (m_[8] * rhs.m_[2]) + (m_[12] * rhs.m_[3]));
        T c0r1((m_[1] * rhs.m_[0]) + (m_[5] * rhs.m_[1]) + (m_[9] * rhs.m_[2]) + (m_[13] * rhs.m_[3]));
        T c0r2((m_[2] * rhs.m_[0]) + (m_[6] * rhs.m_[1]) + (m_[10] * rhs.m_[2]) + (m_[14] * rhs.m_[3]));
        T c0r3((m_[3] * rhs.m_[0]) + (m_[7] * rhs.m_[1]) + (m_[11] * rhs.m_[2]) + (m_[15] * rhs.m_[3]));
        T c1r0((m_[0] * rhs.m_[4]) + (m_[4] * rhs.m_[5]) + (m_[8] * rhs.m_[6]) + (m_[12] * rhs.m_[7]));
        T c1r1((m_[1] * rhs.m_[4]) + (m_[5] * rhs.m_[5]) + (m_[9] * rhs.m_[6]) + (m_[13] * rhs.m_[7]));
        T c1r2((m_[2] * rhs.m_[4]) + (m_[6] * rhs.m_[5]) + (m_[10] * rhs.m_[6]) + (m_[14] * rhs.m_[7]));
        T c1r3((m_[3] * rhs.m_[4]) + (m_[7] * rhs.m_[5]) + (m_[11] * rhs.m_[6]) + (m_[15] * rhs.m_[7]));
        T c2r0((m_[0] * rhs.m_[8]) + (m_[4] * rhs.m_[9]) + (m_[8] * rhs.m_[10]) + (m_[12] * rhs.m_[11]));
        T c2r1((m_[1] * rhs.m_[8]) + (m_[5] * rhs.m_[9]) + (m_[9] * rhs.m_[10]) + (m_[13] * rhs.m_[11]));
        T c2r2((m_[2] * rhs.m_[8]) + (m_[6] * rhs.m_[9]) + (m_[10] * rhs.m_[10]) + (m_[14] * rhs.m_[11]));
        T c2r3((m_[3] * rhs.m_[8]) + (m_[7] * rhs.m_[9]) + (m_[11] * rhs.m_[10]) + (m_[15] * rhs.m_[11]));
        T c3r0((m_[0] * rhs.m_[12]) + (m_[4] * rhs.m_[13]) + (m_[8] * rhs.m_[14]) + (m_[12] * rhs.m_[15]));
        T c3r1((m_[1] * rhs.m_[12]) + (m_[5] * rhs.m_[13]) + (m_[9] * rhs.m_[14]) + (m_[13] * rhs.m_[15]));
        T c3r2((m_[2] * rhs.m_[12]) + (m_[6] * rhs.m_[13]) + (m_[10] * rhs.m_[14]) + (m_[14] * rhs.m_[15]));
        T c3r3((m_[3] * rhs.m_[12]) + (m_[7] * rhs.m_[13]) + (m_[11] * rhs.m_[14]) + (m_[15] * rhs.m_[15]));
        m_[0] = c0r0;
        m_[1] = c0r1;
        m_[2] = c0r2;
        m_[3] = c0r3;
        m_[4] = c1r0;
        m_[5] = c1r1;
        m_[6] = c1r2;
        m_[7] = c1r3;
        m_[8] = c2r0;
        m_[9] = c2r1;
        m_[10] = c2r2;
        m_[11] = c2r3;
        m_[12] = c3r0;
        m_[13] = c3r1;
        m_[14] = c3r2;
        m_[15] = c3r3;
        return *this;
    }

    const tmat4 operator*(const tmat4& rhs)
    {
        return tmat4(*this) *= rhs;
    }

    tmat4& operator*=(const T& rhs)
    {
        m_[0] *= rhs;
        m_[1] *= rhs;
        m_[2] *= rhs;
        m_[3] *= rhs;
        m_[4] *= rhs;
        m_[5] *= rhs;
        m_[6] *= rhs;
        m_[7] *= rhs;
        m_[8] *= rhs;
        m_[9] *= rhs;
        m_[10] *= rhs;
        m_[11] *= rhs;
        m_[12] *= rhs;
        m_[13] *= rhs;
        m_[14] *= rhs;
        m_[15] *= rhs;
        return *this;
    }

    const tmat4 operator*(const T& rhs)
    {
        return tmat4(*this) *= rhs;
    }

    tmat4& operator/=(const T& rhs)
    {
        m_[0] /= rhs;
        m_[1] /= rhs;
        m_[2] /= rhs;
        m_[3] /= rhs;
        m_[4] /= rhs;
        m_[5] /= rhs;
        m_[6] /= rhs;
        m_[7] /= rhs;
        m_[8] /= rhs;
        m_[9] /= rhs;
        m_[10] /= rhs;
        m_[11] /= rhs;
        m_[12] /= rhs;
        m_[13] /= rhs;
        m_[14] /= rhs;
        m_[15] /= rhs;
        return *this;
    }

    const tmat4 operator/(const T& rhs)
    {
        return tmat4(*this) /= rhs;
    }

    ArrayProxy<T, 4> operator[](int index)
    {
        return ArrayProxy<T, 4>(&m_[index]);
    }
    const ArrayProxy<T, 4> operator[](int index) const
    {
        return ArrayProxy<T, 4>(const_cast<T*>(&m_[index]));
    }

private:
    T m_[16];
};

template<typename T>
const tmat4<T> operator*(const T& lhs, const tmat4<T>& rhs)
{
    return tmat4<T>(rhs) * lhs;
}

template<typename T>
const tvec4<T> operator*(const tvec4<T>& lhs, const tmat4<T>& rhs)
{
    T x((lhs.x() * rhs[0][0]) + (lhs.y() * rhs[1][0]) + (lhs.z() * rhs[2][0]) + (lhs.w() * rhs[3][0]));
    T y((lhs.x() * rhs[0][1]) + (lhs.y() * rhs[1][1]) + (lhs.z() * rhs[2][1]) + (lhs.w() * rhs[3][1]));
    T z((lhs.x() * rhs[0][2]) + (lhs.y() * rhs[1][2]) + (lhs.z() * rhs[2][2]) + (lhs.w() * rhs[3][2]));
    T w((lhs.x() * rhs[0][3]) + (lhs.y() * rhs[1][3]) + (lhs.z() * rhs[2][3]) + (lhs.w() * rhs[3][3]));
    return tvec4<T>(x, y, z, w);
}

template<typename T>
const tvec4<T> operator*(const tmat4<T>& lhs, const tvec4<T>& rhs)
{
    T x((lhs[0][0] * rhs.x()) + (lhs[0][1] * rhs.y()) + (lhs[0][2] * rhs.z()) + (lhs[0][3] * rhs.w()));
    T y((lhs[1][0] * rhs.x()) + (lhs[1][1] * rhs.y()) + (lhs[1][2] * rhs.z()) + (lhs[1][3] * rhs.w()));
    T z((lhs[2][0] * rhs.x()) + (lhs[2][1] * rhs.y()) + (lhs[2][2] * rhs.z()) + (lhs[2][3] * rhs.w()));
    T w((lhs[3][0] * rhs.x()) + (lhs[3][1] * rhs.y()) + (lhs[3][2] * rhs.z()) + (lhs[3][3] * rhs.w()));
    return tvec4<T>(x, y, z, w);
}

template<typename T>
const tmat4<T> outer(const tvec4<T>& a, const tvec4<T>& b)
{
    tmat4<T> product;
    product[0][0] = a.x() * b.x();
    product[0][1] = a.x() * b.y();
    product[0][2] = a.x() * b.z();
    product[0][3] = a.x() * b.w();
    product[1][0] = a.y() * b.x();
    product[1][1] = a.y() * b.y();
    product[1][2] = a.y() * b.z();
    product[1][3] = a.y() * b.w();
    product[2][0] = a.z() * b.x();
    product[2][1] = a.z() * b.y();
    product[2][2] = a.z() * b.z();
    product[2][3] = a.z() * b.w();
    product[3][0] = a.w() * b.x();
    product[3][1] = a.w() * b.y();
    product[3][2] = a.w() * b.z();
    product[3][3] = a.w() * b.w();
    return product;
}

//
// Convenience typedefs.  These are here to present a homogeneous view of these
// objects with respect to shader source.
//
typedef tmat2<float> mat2;
typedef tmat3<float> mat3;
typedef tmat4<float> mat4;

typedef tmat2<double> dmat2;
typedef tmat3<double> dmat3;
typedef tmat4<double> dmat4;

typedef tmat2<int> imat2;
typedef tmat3<int> imat3;
typedef tmat4<int> imat4;

typedef tmat2<unsigned int> umat2;
typedef tmat3<unsigned int> umat3;
typedef tmat4<unsigned int> umat4;

typedef tmat2<bool> bmat2;
typedef tmat3<bool> bmat3;
typedef tmat4<bool> bmat4;

namespace Mat4
{

//
// Some functions to generate transformation matrices that used to be provided
// by OpenGL.
//
mat4 translate(float x, float y, float z);
mat4 scale(float x, float y, float z);
mat4 rotate(float angle, float x, float y, float z);
mat4 frustum(float left, float right, float bottom, float top, float near, float far);
mat4 ortho(float left, float right, float bottom, float top, float near, float far);
mat4 perspective(float fovy, float aspect, float zNear, float zFar);
mat4 lookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

} // namespace Mat4
} // namespace LibMatrix
#endif // MAT_H_
