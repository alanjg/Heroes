#pragma once

struct Spline
{
	int degree;
	int dimension;
	std::vector<float> knots;
	std::vector<float> controls;
	void Read(std::istream& in, int dim);
	void ReadBinary(std::istream& in, int dim);
	void Evaluate(float t, float* result);
private:
	void EvaluateDegree2(float t, float* result);
};