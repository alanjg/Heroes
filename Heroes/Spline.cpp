#include "stdafx.h"
#include "Spline.h"

void Spline::Read(std::istream& in, int dim)
{
	dimension = dim;
	in >> degree;

	int knotCount;
	in >> knotCount;
	knots.reserve(knotCount);
	controls.reserve(knotCount * 3);
	for (int i = 0; i < knotCount; i++)
	{
		float knot;
		in >> knot;
		knots.push_back(knot);
	}

	for (int i = 0; i < knotCount * dimension; i++)
	{
		float control;
		in >> control;
		controls.push_back(control);
	}
}

void Spline::ReadBinary(std::istream& in, int dim)
{
	dimension = dim;
	in.read((char*)&degree, sizeof(degree));

	int knotCount;
	in.read((char*) &knotCount, sizeof(knotCount));
	knots.resize(knotCount);
	controls.resize(knotCount * dim);
	in.read((char*) &knots[0], sizeof(float)*knotCount);
	in.read((char*) &controls[0], sizeof(float)*knotCount*dimension);
}

float GetControlPointClamped(std::vector<float>& controls, int knotCount, int index, int dimensionIndex, int dimensions)
{
	if (index < 0)
	{
		return controls[dimensionIndex];
	}
	if (index >= knotCount)
	{
		return controls[(knotCount - 1)*dimensions + dimensionIndex];
	}
	return controls[index*dimensions + dimensionIndex];
}

float GetKnotClamped(std::vector<float>& knots, int knotCount, int index)
{
	if (index < 0)
	{
		return knots[0];
	}
	if (index >= knotCount)
	{
		return knots[knotCount - 1];
	}
	return knots[index];
}

void Spline::Evaluate(float t, float* result)
{
	int knotCount = knots.size();
	
	int uindex = 0;
	if (t < knots[0])
	{
		for (int m = 0; m < dimension; m++)
		{
			result[m] = controls[m];
		}
		return;
	}
	if (t >= knots[knotCount - 1])
	{
		for (int m = 0; m < dimension; m++)
		{
			result[m] = controls[(knotCount - 1) * dimension + m];
		}
		return;
	}

	if (degree == 2)
	{
		EvaluateDegree2(t, result);
		return;
	}

	while (uindex < knotCount)
	{
		if (t < knots[uindex])
		{
			break;
		}
		uindex++;
	}
	uindex--;

	std::vector<std::vector<float>> N;
	int limit = degree + 1;
	N.resize(limit);
	int startIndex = uindex - degree;
	for (int j = uindex - degree; j <= uindex + degree; j++)
	{
		N[0].push_back((GetKnotClamped(knots, knotCount, j) <= t &&
			t < GetKnotClamped(knots, knotCount, j + 1)) ? 1.0f : 0.0f);
	}

	// ti = uindex
	for (int j = 1; j <= degree; j++)
	{
		std::vector<float>& Nj = N[j];
		for (unsigned int i = 0; i + 1 < N[j - 1].size(); i++)
		{
			float ti = GetKnotClamped(knots, knotCount, startIndex + i);
			float tij = GetKnotClamped(knots, knotCount, startIndex + i + j);
			float tij1 = GetKnotClamped(knots, knotCount, startIndex + i + j + 1);
			float ti1 = GetKnotClamped(knots, knotCount, startIndex + i + 1);
			float Nij1 = N[j - 1][i];
			float Ni1j1 = N[j - 1][i + 1];
			float Nij = 0;
			if (Nij1 != 0)
			{
				Nij += (t - ti) / (tij - ti) * Nij1;
			}
			if (Ni1j1 != 0)
			{
				Nij += (tij1 - t) / (tij1 - ti1) * Ni1j1;
			}
			N[j].push_back(Nij);
		}
	}

	for (int j = 0; j < dimension; j++)
	{
		result[j] = 0.0;
	}

	for (int i = 0; i <= degree; i++)
	{
		for (int j = 0; j < dimension; j++)
		{
			float c = GetControlPointClamped(controls, knotCount, uindex - degree + i + 1, j, dimension);
			result[j] += c * N[degree][i];
		}
	}
}

void Spline::EvaluateDegree2(float t, float* result)
{
	int knotCount = knots.size();
	int uindex = 0;
	int low = 0, high = knotCount - 1;
	do
	{
		// k: 1, 2, 3, 4, 5
		// t: 3
		// i = 3
		int mid = (low + high) / 2;
		float k = knots[mid];
		if (t < k)
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
	} while(low != high);
	uindex = low - 1;

	float N[3][5]; // 5, 4, 3
	int size[3];
	
	int limit = 3;
	int startIndex = uindex - degree;
	int c = 0;
	size[0] = 5;
	N[0][0] = 0.0f;
	N[0][1] = 0.0f;
	N[0][2] = 1.0f;
	N[0][3] = 0.0f;
	N[0][4] = 0.0f;
	/*
	size[1] = 4;
	N[0][0] = 0.0f;
	N[0][1] = 0.0f;
	N[0][2] = 1.0f;
	N[0][3] = 0.0f;
	N[0][4] = 0.0f;


	size[2] = 3;
	N[0][0] = 0.0f;
	N[0][1] = 0.0f;
	N[0][2] = 1.0f;
	N[0][3] = 0.0f;
	N[0][4] = 0.0f;*/

	/*
	for (int j = uindex - degree; j <= uindex + degree; j++)
	{
		N[0][c] = ((GetKnotClamped(knots, knotCount, j) <= t &&	t < GetKnotClamped(knots, knotCount, j + 1)) ? 1.0f : 0.0f);
		size[0]++;
		c++;
	}
	*/

	// ti = uindex
	for (int j = 1; j <= degree; j++)
	{
		float* Nj = N[j];
		int c = 0;
		size[j] = 0;
		for (unsigned int i = 0; i + 1 < size[j - 1]; i++)
		{
			float ti = GetKnotClamped(knots, knotCount, startIndex + i);
			float tij = GetKnotClamped(knots, knotCount, startIndex + i + j);
			float tij1 = GetKnotClamped(knots, knotCount, startIndex + i + j + 1);
			float ti1 = GetKnotClamped(knots, knotCount, startIndex + i + 1);
			float Nij1 = N[j - 1][i];
			float Ni1j1 = N[j - 1][i + 1];
			float Nij = 0;
			if (Nij1 != 0)
			{
				Nij += (t - ti) / (tij - ti) * Nij1;
			}
			if (Ni1j1 != 0)
			{
				Nij += (tij1 - t) / (tij1 - ti1) * Ni1j1;
			}
			N[j][c] = Nij;
			size[j]++;
			c++;
		}
	}

	for (int j = 0; j < dimension; j++)
	{
		result[j] = 0.0;
	}

	for (int i = 0; i <= degree; i++)
	{
		for (int j = 0; j < dimension; j++)
		{
			if (N[degree][i] != 0)
			{
				float c = GetControlPointClamped(controls, knotCount, uindex - degree + i + 1, j, dimension);
				result[j] += c * N[degree][i];
			}
		}
	}
}