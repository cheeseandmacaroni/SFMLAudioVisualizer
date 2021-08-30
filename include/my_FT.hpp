#pragma once
#include <SFML/Audio.hpp>
#define Pi 3.141592653

class Complex
{
public:
	double re;
	double im;
	Complex();
	Complex(int k, int N);
	void setWnk(double k, double N, double n);
	inline void operator=(const Complex &a);
	inline void operator+=(Complex &a);
	inline void operator-=(Complex &a);
	inline void operator*=(Complex &a);
	inline void operator*=(float mult);
};

class FurierTransform
{
private:
	void initBitReverse(int samples);
	std::vector<Complex> buffer;
	std::vector<double> magnitudes;
	std::vector<double> phase;
	std::vector<double> freq;
	double eps = 0.001;
	void HanningWindow();
	int samples;
	void calculateMagnitude();//also calculates frequences and phases
public:
	void copyToMagnitudeBuffer(const sf::Int16 * pos, int channels);
	FurierTransform(int samples);
	void processTheBuffer();
	int getSamples() { return samples; }
	const std::vector<double>& getMagnitudes(){ return magnitudes; }
	const std::vector<double>& getPhases() { return phase; }
	const std::vector<double>& getFreq() { return freq; }
};
void FFT(std::vector<Complex>& src, int size);

inline Complex operator*(Complex &a, Complex &b);
inline Complex operator+(const Complex &a, const Complex &b);
inline Complex operator-(const Complex &a, const Complex &b);
