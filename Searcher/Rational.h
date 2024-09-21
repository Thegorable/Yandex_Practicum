#pragma once
#include<numeric>
#include<iostream>
#include<stdexcept>

class Rational {
public:
    Rational() = default;

    Rational(int numerator);

    Rational(int numerator, int denominator);

    int Numerator() const;

    int Denominator() const;

    Rational& operator +=(Rational r);
    Rational& operator -=(Rational r);
    Rational& operator *=(Rational r);
    Rational& operator /=(Rational r);

private:
    void Normalize();

    int numerator_ = 0;
    int denominator_ = 1;
};

std::ostream& operator<<(std::ostream& output, Rational rational);

std::istream& operator>>(std::istream& input, Rational& rational);

Rational operator +(Rational l, Rational r);

Rational operator -(Rational l, Rational r);

Rational operator *(Rational l, Rational r);

Rational operator /(Rational l, Rational r);

Rational operator +(Rational r);

Rational operator -(Rational r);

bool operator==(Rational left, Rational right);

bool operator!=(Rational left, Rational right);

bool operator<(Rational left, Rational right);

bool operator<=(Rational left, Rational right);

bool operator > (Rational left, Rational right);

bool operator >= (Rational left, Rational right);