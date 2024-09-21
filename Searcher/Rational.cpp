#include "Rational.h"

Rational::Rational(int numerator)
    : numerator_(numerator)
    , denominator_(1) {}

Rational::Rational(int numerator, int denominator)
    : numerator_(numerator)
    , denominator_(denominator)
{
    if (!denominator_) {
        throw std::domain_error("The zero denominator is provided");
    }
    Normalize();
}

int Rational::Numerator() const {
    return numerator_;
}

int Rational::Denominator() const {
    return denominator_;
}

Rational& Rational::operator +=(Rational r) {
    int denumerator = denominator_ * r.Denominator();
    numerator_ = (numerator_ * r.Denominator()) + (r.Numerator() * denominator_);
    denominator_ = denumerator;
    Normalize();
    return *this;
}

Rational& Rational::operator -=(Rational r) {
    int denumerator = denominator_ * r.Denominator();
    numerator_ = (numerator_ * r.Denominator()) - (r.Numerator() * denominator_);
    denominator_ = denumerator;
    Normalize();
    return *this;
}

Rational& Rational::operator *=(Rational r) {
    denominator_ = denominator_ * r.Denominator();
    numerator_ = numerator_ * r.Numerator();
    Normalize();
    return *this;
}

Rational& Rational::operator /=(Rational r) {
    if (!r.Numerator()) {
        throw std::invalid_argument("the rational cannot be devided on 0/1 rational");
    }
    denominator_ = denominator_ * r.Numerator();
    numerator_ = numerator_ * r.Denominator();
    Normalize();
    return *this;
}

void Rational::Normalize() {
    if (denominator_ < 0) {
        numerator_ = -numerator_;
        denominator_ = -denominator_;
    }
    int n = std::gcd(numerator_, denominator_);
    numerator_ /= n;
    denominator_ /= n;
}

std::ostream& operator<<(std::ostream& output, Rational rational) {
    output << rational.Numerator() << '/' << rational.Denominator();
    return output;
}

std::istream& operator>>(std::istream& input, Rational& rational) {
    int numerator, denominator;
    char slash;
    input >> numerator >> slash >> denominator;
    rational = Rational{ numerator, denominator };
    return input;
}

Rational operator +(Rational l, Rational r) {
    return l += r;
}

Rational operator -(Rational l, Rational r) {
    return l -= r;
}

Rational operator *(Rational l, Rational r) {
    return l *= r;
}

Rational operator /(Rational l, Rational r) {
    return l /= r;
}

Rational operator +(Rational r) {
    return r;
}

Rational operator -(Rational r) {
    return { -r.Numerator(), r.Denominator() };
}

bool operator==(Rational left, Rational right) {
    return left.Numerator() == right.Numerator() &&
        left.Denominator() == right.Denominator();
}

bool operator!=(Rational left, Rational right) {
    return !(left == right);
}

bool operator<(Rational left, Rational right) {
    return (left.Numerator() * right.Denominator()) < (right.Numerator() * left.Denominator());
}

bool operator<=(Rational left, Rational right) {
    return (left.Numerator() * right.Denominator()) <= (right.Numerator() * left.Denominator());
}

bool operator > (Rational left, Rational right) {
    return (left.Numerator() * right.Denominator()) > (right.Numerator() * left.Denominator());
}

bool operator >= (Rational left, Rational right) {
    return (left.Numerator() * right.Denominator()) >= (right.Numerator() * left.Denominator());
}