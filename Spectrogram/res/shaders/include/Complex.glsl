struct Complex
{
    float real;
    float imag;
};

// Convertes from Polar to Cartesian Coordinates
Complex ComplexFromPolarCoordinates(float magnitude, float angle)
{
    return Complex(magnitude * cos(angle), magnitude * sin(angle));
}

Complex GetNThRootsOfUnity(uint k, uint n)
{
    const float PI = 3.14159265;
    return ComplexFromPolarCoordinates(1.0, 2.0 * PI * k / n);
}

float ComplexGetMagnitude(Complex c)
{
    return sqrt(c.real * c.real + c.imag * c.imag);
}

Complex ComplexAdd(Complex left, float right)
{
    return Complex(left.real + right, left.imag);
}

Complex ComplexAdd(Complex left, Complex right)
{
    return Complex(left.real + right.real, left.imag + right.imag);
}

Complex ComplexSubtract(Complex left, Complex right)
{
    return Complex(left.real - right.real, left.imag - right.imag);
}

Complex ComplexMultiply(Complex left, float right)
{
    return Complex(left.real * right, left.imag * right);
}

Complex ComplexMultiply(Complex left, Complex right)
{
    //   (a + bi)  * (c + di)
    // = (ac - bd) + (bc + ad)i
    const float real = (left.real * right.real) - (left.imag * right.imag);
    const float imag = (left.imag * right.real) + (left.real * right.imag);
    
    return Complex(real, imag);
}

Complex ComplexExp(Complex c)
{
    const float expReal = exp(c.real);
    return ComplexFromPolarCoordinates(expReal, c.imag);
}

