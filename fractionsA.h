/*fractionsA.h */
/*
 * Copyright ©2026 王家祺(Jacky Velarde Z.) <3594929067@qq.com>
 *
 * This file is released under the GPLv2 (or later, at your option).
 *
 * All Rights Reserved.
 *
 */

//Note: THIS WORK USED AI FOR SOME PARTS BECAUSE I AM NOT FAMILIAR WITH ARDUINIO
//THE MAIN VERSION ： https://github.com/CodingIsFun1242/Fractions-lib-for-C-plus-plus
//AI ASSISTED PORTIONS: ARDUINO COMPACTIBILITY

#ifndef FRACTIONS_H
#define FRACTIONS_H

#ifdef ARDUINO
#include <Arduino.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#undef abs
#undef min
#undef max
namespace std {
	inline long abs(long x) 
	{
		return x < 0 ? -x : x; 
	}
	inline long long  abs(long long x)
	{
		return x < 0 ? -x : x;
	}
	using ::abs;
	using ::pow;
	using ::sqrtf;
}
#else
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <string>
#include <stdexcept>
#endif

template <typename T>
static inline T gcd(T a, T b)
{
	a = std::abs(a);
	b = std::abs(b);
	while (b != 0)
	{
		T temp = b;
		b = a % b;
		a = temp;

	}
	return a;
}

inline static int Decimal_count(double in) { // This should do it.
#ifdef ARDUINO
	char buf[64];
	dtostrf(in, 1, 6, buf);
	char* dot = strchr(buf, '.');
	if (dot == NULL) return 0;
	int len = strlen(buf);
	int end = len - 1;
	while (end >= 0 && buf[end] == '0') {
		end--;
	}
	if (end < 0 || buf[end] == '.') return 0;
	return end - (dot - buf);
#else
	std::string s = std::to_string(in);
	const size_t pos = s.find('.');
	if (pos == std::string::npos) return 0;
	const size_t end = s.find_last_not_of('0');
	if (end == std::string::npos || s[end] == '.') return 0;
	return static_cast<int>(end - pos);
#endif
}

#ifdef ARDUINO
#define THROW_INVALID_ARGUMENT(msg) do { Serial.println(msg); while(1); } while(0)
#else
#define THROW_INVALID_ARGUMENT(msg) throw std::invalid_argument(msg)
#endif

namespace mfc { //mfc, aka.Math for C++.
	using type = long; //Change int to whatever you need
	class fraction {
	private:
		type num = 0;//numerator
		type den = 1;//denominator
	public:
		template<typename TypesForSolve = float>
		TypesForSolve solve() const
		{
			if (den == 0)
			{
				THROW_INVALID_ARGUMENT("Denominator cannot be zero");
			}
			else
			{
				return static_cast<TypesForSolve>(num) / den;
			}
		}
		fraction(type n, type d) : num(n), den(d) {
			if (d == 0) {
				THROW_INVALID_ARGUMENT("Denominator cannot be zero");
			}
			if (den < 0) {
				num = -num;
				den = -den;
			}
		}
		fraction() : num(0), den(1) {}
		fraction(type n) : num(n), den(1) {}
		fraction(double n) : num(0), den(1)
		{
			fraction temp1(n * std::pow(10, Decimal_count(n)), std::pow(10, Decimal_count(n)));
			temp1 = temp1.simplify();
			num = temp1.num;
			den = temp1.den;
		}
		fraction(long double n) : num(0), den(1)
		{
			fraction temp1(n * std::pow(10, Decimal_count(n)), std::pow(10, Decimal_count(n)));
			temp1 = temp1.simplify();
			num = temp1.num;
			den = temp1.den;
		}
		fraction pow_int(int a) const;
		fraction rec() const;//Basically makes the fraction upside down.
		fraction& abs(); //This modifys the fraction.
		fraction rtrn_abs() const; //This returns the result instead of modifying the fraction itself.
		fraction sqrt() const
		{
			if (num < 0)
			{
				THROW_INVALID_ARGUMENT("this is not gonna work");
			}
			else
			{
				return fraction(std::sqrtf(num * den)) / den;
			}
		}
		fraction nroot(int n) const
		{
			if (n == 0)
				THROW_INVALID_ARGUMENT("N can't be 0");
			if (num < 0 && n % 2 != 0)
				return -fraction(std::pow(-num / static_cast<double>(den), 1.0 / n));
			return fraction(std::pow(num / static_cast<double>(den), 1.0 / n));
		}
		fraction simplify() const
		{
			type g = gcd(std::abs(num), std::abs(den));
			return fraction(num / g, den / g);
		}
		void swap(fraction& other) noexcept
		{
#ifdef ARDUINO
			type temp_num = num;
			num = other.num;
			other.num = temp_num;
			type temp_den = den;
			den = other.den;
			other.den = temp_den;
#else
			std::swap(num, other.num);
			std::swap(den, other.den);
#endif
		}
		fraction& operator = (fraction other) noexcept
		{
			swap(other);
			return *this;
		}
		fraction operator % (const fraction& other) const noexcept
		{
			type lcm = den / gcd(den, other.den) * other.den;
			fraction Temp((this->num * (lcm / this->den)) % (other.num * (lcm / other.den)), lcm);
			return Temp.simplify();

		}
		fraction operator - () const noexcept
		{
			fraction Temp(-num, den);
			return Temp;
		}
		fraction operator + (const fraction& other) const noexcept
		{
			type lcm = den / gcd(den, other.den) * other.den;
			type new_num = num * (lcm / den) + other.num * (lcm / other.den);
			fraction result(new_num, lcm);
			return result.simplify();
		}
		fraction operator - (const fraction& other) const noexcept
		{
			type lcm = den / gcd(den, other.den) * other.den;
			type new_num = num * (lcm / den) - other.num * (lcm / other.den);
			fraction result(new_num, lcm);
			return result.simplify();
		}
		fraction operator * (const fraction& other) const
		{
			fraction result(num * other.num, den * other.den);
			return result.simplify();
		}
		fraction operator / (const fraction& other) const
		{
			fraction result(num * other.den, den * other.num);
			return result.simplify();
		}
		fraction operator / (const type& other) const
		{
			if (other == 0)
			{
				THROW_INVALID_ARGUMENT("Divisor can't be zero");
			}
			else {
				fraction result(num, den * other);
				return result.simplify();
			}
		}
		//For the other side
		friend fraction operator + (const double& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp + b;
		}
		friend fraction operator - (const double& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp - b;
		}
		friend fraction operator * (const double& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp * b;
		}
		friend fraction operator / (const double& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp / b;
		}
		friend fraction operator + (const type& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp + b;
		}
		friend fraction operator - (const type& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp - b;
		}
		friend fraction operator * (const type& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp * b;
		}
		friend fraction operator / (const type& a, const fraction& b) noexcept
		{
			fraction Temp(a);
			return Temp / b;
		}
		fraction operator + (const type& other) const noexcept
		{
			fraction Temp(other);
			return *this + Temp;
		}
		fraction operator - (const type& other) const noexcept
		{
			fraction Temp(other);
			return *this - Temp;
		}
		fraction operator * (const type& other) const noexcept
		{
			fraction Temp(other);
			return *this * Temp;
		}
		fraction& operator+=(const fraction& other) noexcept {
			*this = *this + other;
			return *this;
		}
		fraction& operator-=(const fraction& other) noexcept {
			*this = *this - other;
			return *this;
		}
		fraction& operator*=(const fraction& other) noexcept {
			*this = *this * other;
			return *this;
		}
		fraction& operator/=(const fraction& other) noexcept {
			*this = *this / other;
			return *this;
		}
		fraction& operator++() noexcept { num += den; return *this; }
		fraction& operator--() noexcept { num -= den; return *this; }
		fraction operator++(int) noexcept {
			fraction temp = *this;
			num += den;
			return temp;
		}
		fraction operator--(int) noexcept {
			fraction temp = *this;
			num -= den;
			return temp;
		}

#ifndef ARDUINO
		friend std::ostream& operator<<(std::ostream& os, const fraction& a) noexcept
		{
			if (a.num % a.den == 0)
			{
				os << a.num / a.den;
			}
			else
			{
				os << a.num << "/" << a.den;
			}
			return os;
		}
		friend std::istream& operator>>(std::istream& is, fraction& a) noexcept
		{
			std::cout << "Num.:";
			is >> a.num;
			std::cout << "Den.:";
			is >> a.den;
			return is;
		}
#endif

		bool operator < (const fraction& other) const noexcept
		{
			if (this->num / this->den != other.num / other.den)
			{
				return this->num / this->den < other.num / other.den;
			}
			else {
				return (this->num % this->den) * other.den < (other.num % other.den) * this->den;
			}
		}
		bool operator <= (const fraction& other) const noexcept
		{
			return *this < other or *this == other;
		}
		bool operator != (const fraction& other) const noexcept
		{
			if (this->num / this->den != other.num / other.den)
			{
				return true;
			}
			else {
				return (this->num % this->den) * other.den != (other.num % other.den) * this->den;
			}
		}
		bool operator > (const fraction& other) const noexcept
		{
			return other < *this;
		}
		bool operator >= (const fraction& other) const noexcept
		{
			return other < *this or other == *this;;
		}
		bool operator < (const double& other) const noexcept
		{
			if (this->num / this->den != static_cast<int>(other))
			{
				return this->num / this->den < static_cast<int>(other);
			}
			else {
				return this->num % this->den < (other - static_cast<int>(other)) * this->den;
			}
		}
		bool operator != (const double& other) const noexcept
		{
			if (this->num / this->den != static_cast<int>(other))
			{
				return true;
			}
			else {
				return this->num % this->den != (other - static_cast<int>(other)) * this->den;
			}
		}
		bool operator <= (const double& other) const noexcept
		{
			if (this->num / this->den != static_cast<int>(other))
			{
				return this->num / this->den <= static_cast<int>(other);
			}
			else {
				return this->num % this->den <= (other - static_cast<int>(other)) * this->den;
			}
		}
		bool operator > (const double& other) const noexcept
		{
			if (this->num / this->den != static_cast<int>(other))
			{
				return this->num / this->den > static_cast<int>(other);
			}
			else {
				return this->num % this->den > (other - static_cast<int>(other)) * this->den;
			};
		}
		bool operator >= (const double& other) const noexcept
		{
			if (this->num / this->den != static_cast<int>(other))
			{
				return this->num / this->den >= static_cast<int>(other);
			}
			else {
				return this->num % this->den >= (other - static_cast<int>(other)) * this->den;
			};
		}
		bool operator == (const fraction& other) const noexcept
		{
			if (this->num / this->den != other.num / other.den)
			{
				return this->num / this->den == other.num / other.den;
			}
			else {
				return (this->num % this->den) * other.den == (other.num % other.den) * this->den;
			}
		}
		bool operator == (const double& other) const noexcept
		{
			if (this->num / this->den != static_cast<int>(other))
			{
				return this->num / this->den == static_cast<int>(other);
			}
			else {
				return this->num % this->den == (other - static_cast<int>(other)) * this->den;
			};
		}
		//The other side
		friend bool operator <  (const double& a, const fraction& b) noexcept { return b > a; }
		friend bool operator <= (const double& a, const fraction& b) noexcept { return b >= a; }
		friend bool operator >  (const double& a, const fraction& b) noexcept { return b < a; }
		friend bool operator >= (const double& a, const fraction& b) noexcept { return b <= a; }
		friend bool operator == (const double& a, const fraction& b) noexcept { return b == a; }
		friend bool operator != (const double& a, const fraction& b) noexcept { return b != a; }
		friend bool operator <  (const type& a, const fraction& b) noexcept { return fraction(a) < b; }
		friend bool operator <= (const type& a, const fraction& b) noexcept { return fraction(a) <= b; }
		friend bool operator >  (const type& a, const fraction& b) noexcept { return fraction(a) > b; }
		friend bool operator >= (const type& a, const fraction& b) noexcept { return fraction(a) >= b; }
		friend bool operator == (const type& a, const fraction& b) noexcept { return fraction(a) == b; }
		friend bool operator != (const type& a, const fraction& b) noexcept { return fraction(a) != b; }
	};
	fraction fraction::rec() const
	{
		if (num == 0)
		{
			THROW_INVALID_ARGUMENT("Denominator cannot be zero");
		}
		else {
			fraction temp(den, num);
			return temp.simplify();
		}
	}
	fraction fraction::pow_int(int a) const
	{
		if (a > 0)
		{
			fraction temp(static_cast<int>(std::pow(num, a)), static_cast<int>(std::pow(den, a)));
			return temp.simplify();
		}
		if (a == 0)
		{
			fraction b(1, 1);
			return b;
		}
		else
		{
			fraction Ntemp = this->rec();
			return Ntemp.pow_int(-a);
		}
	}
	fraction& fraction::abs()
	{
		if (num < 0)
		{
			this->num = -num;
		}
		return *this;
	}
	fraction fraction::rtrn_abs() const
	{
		if (num < 0)
		{
			fraction temp(-num, den);
			return temp;
		}
		else
		{
			fraction temp2(num, den);
			return temp2;
		}
	}
	const inline  fraction f(type a, type b) noexcept//Easier Fraction Maker
	{
		return fraction(a, b);
	}
}

#endif


//===========================================================================
//THIS VERSION IS THE VERSION OF FRACTIONS.H FOR ARDUINO UNO
// IF YOU ARE NOT USING ARDUINO:
// I RECOMMEND USING THIS:
// https://github.com/CodingIsFun1242/Fractions-lib-for-C-plus-plus
// Version : 4.0.0 FOR ARDUINO
//============================================================================