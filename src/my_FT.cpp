#include "My_FT.hpp"

std::vector<unsigned short> reverse;

inline void Complex::operator=(const Complex &c)
{
	re = c.re;
	im = c.im;
}
inline void Complex::operator+=(Complex &a)
{
	re += a.re;
	im += a.im;
}
inline void Complex::operator-=(Complex &a)
{
	re -= a.re;
	im -= a.im;
}
inline void Complex::operator*=(Complex &a)
{
	long double tmp = re;
	re = tmp * a.re - im * a.im;
	im = tmp * a.im + im * a.re;
}
inline Complex operator+(const Complex &a, const Complex &b)
{
	Complex res;
	res.re = a.re + b.re;
	res.im = a.im + b.im;
	return res;
}
inline Complex operator-(const Complex &a, const Complex &b)
{
	Complex res;
	res.re = a.re - b.re;
	res.im = a.im - b.im;
	return res;
}

inline Complex operator*(Complex &a, Complex &b)
{
	Complex res;
	res.re = a.re * b.re - a.im * b.im;
	res.im = a.re * b.im + a.im * b.re;
	return res;
}

inline void Complex::operator*=(float mult)
{
	re *= mult;
	im *= mult;
}

Complex::Complex(int k, int N)
{
	re = cos(2 * Pi*k / N);
	im = sin(-2 * Pi*k / N);
}
void Complex::setWnk(double k, double N, double n)
{
	if (n != 1)
	{
		re = cos(-2 * Pi * k * (n / N));
		im = sin(-2 * Pi * k * (n / N));
	}
	else
	{
		re = cos(-2 * Pi * k / N);
		im = sin(-2 * Pi * k / N);
	}
}
Complex::Complex()
{
	re = 0;
	im = 0;
}

int amountOfIterations(int size)//returns log2(size). Size MUST be power of 2
{
	int tmp = 1;
	int iterations = 0;
	while (tmp != size)
	{
		tmp *= 2;
		iterations++;
	}
	return iterations;
}

unsigned short bitReverse(int bits, unsigned short num)//reverses bits in a number depending on amount of bits (e.g. 00011100(28) -> 00111000(56) if there are 8 bits)
{
	unsigned short res = 0;
	for (int i = 0; i < bits; i++)
	{
		unsigned short tmp = num >> i;
		tmp = (tmp << 15);
		tmp = (tmp >> i);
		res = res | tmp;
	}
	res = res >> (16 - bits);
	return res;
}

void FurierTransform::initBitReverse(int samples)//initializes array of reversed bits depending on the amount of bits (e.g. reverse[12] = 24 if there are 7 bits: 0001100 -> 0011000)
{
	reverse.resize(samples);
	int tmp = 1;
	int bits = 0;
	bits = amountOfIterations(samples);
	for (int i = 0; i < samples; i++)
	{
		reverse[i] = bitReverse(bits, i);
	}
}

void FFT(std::vector<Complex>& src, int size)
{

	Complex *res = new Complex[size];
	int iterations = amountOfIterations(size);

	for (int i = 0; i < size; i++)
	{
		res[reverse[i]].re = src[i].re;
	}
	int subsequenceSize = 1;

	for (int i = 0; i < iterations; i++)
	{
		Complex *subRes = new Complex[subsequenceSize * 2];
		for (int j = 0; j < (size / (2 * subsequenceSize)); j++)
		{
			for (int t = 0; t < subsequenceSize; t++)
			{
				Complex w_kN(t, subsequenceSize * 2);

				subRes[t] = res[subsequenceSize * 2 * j + t] + (w_kN * res[subsequenceSize * 2 * j + subsequenceSize + t]);
				subRes[t + subsequenceSize] = res[subsequenceSize * 2 * j + t] - (w_kN * res[subsequenceSize * 2 * j + subsequenceSize + t]);
			}
			int t = 0;
			for (int l = subsequenceSize * 2 * j; l < subsequenceSize * 2 * j + subsequenceSize * 2; l++)
			{
				res[l] = subRes[t++];
			}
		}
		delete[] subRes;
		subsequenceSize *= 2;
	}
	for (int i = 0; i < size; i++)
	{
		src[i] = res[i];
	}
	delete[] res;
}

void FurierTransform::processTheBuffer()
{
	FFT(buffer, samples);
	//HanningWindow();
	calculateMagnitude();
}

void FurierTransform::copyToMagnitudeBuffer(const sf::Int16 * pos, int channels)
{
	for (int i = 0; i < samples; ++i)
		buffer[i].re = pos[i * channels] / 32767.;
}

FurierTransform::FurierTransform(int samples)
{
	this->samples = samples;
	initBitReverse(samples);
	buffer.resize(samples);
	magnitudes.resize(samples / 2);
	phase.resize(samples / 2);
	freq.resize(samples / 2);
}

void FurierTransform::HanningWindow()
{
	for (int i = 0; i < samples / 2; i++)
	{
		double multiplier = 0.5 * (1 - cos(2 * Pi*i / (samples / 2 - 1)));
		buffer[i].re *= multiplier;
	}
}

void FurierTransform::calculateMagnitude()
{
	for (int i = 0; i < samples / 2; i++) //get rid of the mirror effect
	{
		double amp = 2 * sqrt(buffer[i].re * buffer[i].re + buffer[i].im * buffer[i].im) / samples;
		if (amp < eps)
			magnitudes[i] = 0;
		else
		{
			phase[i] = atan2(buffer[i].im, buffer[i].re);
			phase[i] += Pi / 2;
			magnitudes[i] = amp;
		}
	}
}