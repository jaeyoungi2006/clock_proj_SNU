long gcd(long a, long b){
    if(a % b == 0) return b;
    return gcd(b, a % b);
}

long lcm(long a, long b){
    long c = gcd(a, b);
    long a_ = a / c;
    long b_ = b / c;
    return a_ * b_ * c;
}

class Fraction{
    public:
        long numerator;
        long denominator;
        Fraction(long n, long d = 1){
            long g = gcd(n, d);
            numerator = n / g;
            denominator = d / g;
            if(denominator < 0){
                numerator = -numerator;
                denominator = -denominator;
            }
        }

        // Addition
        Fraction operator+(const Fraction & other) const{
            long num = numerator * other.denominator + other.numerator * denominator;
            long den = denominator * other.denominator;
            return Fraction(num, den);
        }

        Fraction operator+(long other) const{
            return *this + Fraction(other);
        }

        friend Fraction operator+(long lhs, const Fraction& rhs){
            return rhs + lhs;
        }

        // Negation
        Fraction operator-() const {
            return Fraction(-numerator, denominator);
        }

        // Subtraction
        Fraction operator-(const Fraction& other) const {
            return *this + (-other);
        }

        Fraction operator-(long other) const {
            return *this - Fraction(other);
        }

        friend Fraction operator-(long lhs, const Fraction& rhs) {
            return -rhs + lhs;
        }

        // Multiplication
    Fraction operator*(const Fraction& other) const {
        long num = numerator * other.numerator;
        long den = denominator * other.denominator;
        return Fraction(num, den);
    }

    Fraction operator*(long other) const {
        return *this * Fraction(other);
    }

    friend Fraction operator*(long lhs, const Fraction& rhs) {
        return rhs * lhs;
    }

    // Division
    Fraction operator/(const Fraction& other) const {
        return *this * Fraction(other.denominator, other.numerator);
    }

    Fraction operator/(long other) const {
        return Fraction(numerator, denominator * other);
    }

    friend Fraction operator/(long lhs, const Fraction& rhs) {
        return Fraction(lhs) / rhs;
    }

    // Equality
    bool operator==(const Fraction& other) const {
        return numerator == other.numerator && denominator == other.denominator;
    }

    bool operator==(long other) const {
        return *this == Fraction(other);
    }

    friend bool operator==(long lhs, const Fraction& rhs) {
        return rhs == lhs;
    }

    // Inequality
    bool operator!=(const Fraction& other) const {
        return !(*this == other);
    }

    bool operator!=(long other) const {
        return !(*this == other);
    }

    friend bool operator!=(long lhs, const Fraction& rhs) {
        return !(rhs == lhs);
    }

    // Less than
    bool operator<(const Fraction& other) const {
        return numerator * other.denominator < denominator * other.numerator;
    }

    bool operator<(long other) const {
        return *this < Fraction(other);
    }

    friend bool operator<(long lhs, const Fraction& rhs) {
        return Fraction(lhs) < rhs;
    }

    // Less than or equal
    bool operator<=(const Fraction& other) const {
        return *this == other || *this < other;
    }

    bool operator<=(long other) const {
        return *this <= Fraction(other);
    }

    friend bool operator<=(long lhs, const Fraction& rhs) {
        return Fraction(lhs) <= rhs;
    }

    // Greater than
    bool operator>(const Fraction& other) const {
        return !(*this <= other);
    }

    bool operator>(long other) const {
        return *this > Fraction(other);
    }

    friend bool operator>(long lhs, const Fraction& rhs) {
        return Fraction(lhs) > rhs;
    }

    // Greater than or equal
    bool operator>=(const Fraction& other) const {
        return !(*this < other);
    }

    bool operator>=(long other) const {
        return *this >= Fraction(other);
    }

    friend bool operator>=(long lhs, const Fraction& rhs) {
        return Fraction(lhs) >= rhs;
    }

    double to_double() const {
        return (double) numerator / denominator;
    }

    void serial_print() const{
      Serial.print((int) numerator);
      if(denominator != 1){
        Serial.print("/");
        Serial.print((int) denominator);
      }
    }
};