// Gr2Converter.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"


#include "granny.h"
#pragma comment(lib, "granny2.lib")

#include "Converter.h"
using namespace std;

bool triangleComp(SkinnedModelTriangle& t1, SkinnedModelTriangle& t2)
{
	return t1.material < t2.material;
}

bool ConvertSMD(std::string source, std::string dest)
{
	string file = source;
	ifstream infile(file);
	ofstream outfile(dest, ios::binary);
	unordered_map<int, int> boneIdToIndex;
	
	string temp;
	int number;
	infile >> temp >> number;
	string version = "version";
	if (temp != version)
	{
		return false;
	}
	infile >> temp;
	if (temp != "nodes")
	{
		return false;
	}
	int globalBone = -1;
	vector<int> ids, parentIds;
	vector<string> names;
	string line;
	getline(infile, line);
	while (line != "end")
	{
		if (line.length() > 0)
		{
			istringstream in(line);
			int id;

			string name;
			int parentId;
			in >> id;
			getline(in, name, '\"');
			getline(in, name, '\"');
			in >> parentId;

			boneIdToIndex[id] = ids.size();
			ids.push_back(id);
			parentIds.push_back(parentId);
			names.push_back(name);

		}
		getline(infile, line);
	}

	int count = ids.size();
	outfile.write((char*) &count, sizeof(count));
	for (unsigned int i = 0; i < ids.size(); i++)
	{
		outfile.write((char*) &ids[i], sizeof(ids[i]));
		int nameLen = names[i].size();
		outfile.write((char*) &nameLen, sizeof(nameLen));
		outfile.write((char*) &names[i][0], sizeof(char)*nameLen);
		outfile.write((char*) &parentIds[i], sizeof(parentIds[i]));
	}
	infile >> temp;
	if (temp != "skeleton")
	{
		return false;
	}
	int time;
	infile >> temp >> time;
	if (temp != "time")
	{
		return false;
	}
	if (time != 0)
	{
		return false;
	}
	getline(infile, line);
	while (line != "end")
	{
		if (line.length() > 0)
		{
			istringstream in(line);
			int id;
			in >> id;
			float bx, by, bz, brx, bry, brz;
			// not used - we read these values from skeleton.txt instead.
			// this looks wrong in the file, stomped by skeleton.txt
			in >> bx >> by >> bz >> brx >> bry >> brz;
		}
		getline(infile, line);
	}

	infile >> temp;
	if (temp != "triangles")
	{
		return false;
	}

	int maxBones = 0;
	getline(infile, line);
	unordered_set<int> linksSeen;
	unordered_map<string, vector<SkinnedModelTriangle>> trianglesMap;
	while (line != "end")
	{
		if (line.length() > 0)
		{
			SkinnedModelTriangle t;
			t.material = line;
			for (int i = 0; i < 3; i++)
			{
				getline(infile, line);
				istringstream in(line);
				SkinnedModelVertex& v = t.v[i];
				in >> v.globalLink;

				v.globalLink = boneIdToIndex[v.globalLink];
				in >> v.x >> v.y >> v.z >>
					v.nx >> v.ny >> v.nz >>
					v.u >> v.v;

				int links;
				in >> links;
				double global = 1.0;
				maxBones = max(maxBones, links);
				for (int j = 0; j < links; j++)
				{
					int linkId;
					float linkWeight;
					in >> linkId >> linkWeight;
					
					linkId = boneIdToIndex[linkId];

					linksSeen.insert(linkId);
					
					global -= linkWeight;
					v.linkIds.push_back(linkId);
					v.linkWeights.push_back(linkWeight);
				}
				v.globalWeight = global;
				
			}
			trianglesMap[t.material].push_back(t);		
		}
		getline(infile, line);
	}
	if (!infile) return false;

	int mapCount = trianglesMap.size();

	outfile.write((char*) &mapCount, sizeof(mapCount));
	for each(auto p in trianglesMap)
	{
		int materialLen = p.first.size();
		outfile.write((char*) &materialLen, sizeof(materialLen));
		outfile.write((char*) &p.first[0], sizeof(char)*p.first.size());

		int triangleCount = p.second.size();
		outfile.write((char*) &triangleCount, sizeof(triangleCount));

		vector<SkinnedModelTriangle>& triangles = p.second;
		SkinnedMeshVertex* vertices = new SkinnedMeshVertex[triangleCount * 3];
		for (int i = 0; i < triangleCount; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				vertices[i * 3 + j].v.x = triangles[i].v[j].x;
				vertices[i * 3 + j].v.y = triangles[i].v[j].y;
				vertices[i * 3 + j].v.z = triangles[i].v[j].z;

				float w[4] = { 0, 0, 0, 0 };
				int ind[4] = { 0, 0, 0, 0 };

				for (unsigned int k = 0; k < triangles[i].v[j].linkIds.size(); k++)
				{
					w[k] = triangles[i].v[j].linkWeights[k];
					ind[k] = triangles[i].v[j].linkIds[k];
				}

				vertices[i * 3 + j].weights.x = w[0];
				vertices[i * 3 + j].weights.y = w[1];
				vertices[i * 3 + j].weights.z = w[2];
				vertices[i * 3 + j].weights.w = w[3];

				vertices[i * 3 + j].MatrixIndices.x = ind[0];
				vertices[i * 3 + j].MatrixIndices.y = ind[1];
				vertices[i * 3 + j].MatrixIndices.z = ind[2];
				vertices[i * 3 + j].MatrixIndices.w = ind[3];

				vertices[i * 3 + j].n.x = triangles[i].v[j].nx;
				vertices[i * 3 + j].n.y = triangles[i].v[j].ny;
				vertices[i * 3 + j].n.z = triangles[i].v[j].nz;

				vertices[i * 3 + j].Tex.x = triangles[i].v[j].u;
				vertices[i * 3 + j].Tex.y = 1 - triangles[i].v[j].v;
			}
		}
		outfile.write((char*) vertices, sizeof(SkinnedMeshVertex)*triangleCount * 3);
		/*
		for (int i = 0; i < triangleCount; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				outfile.write((char*) &p.second[i].v[j].x, sizeof(p.second[i].v[j].x));
				outfile.write((char*) &p.second[i].v[j].y, sizeof(p.second[i].v[j].y));
				outfile.write((char*) &p.second[i].v[j].z, sizeof(p.second[i].v[j].z));
				outfile.write((char*) &p.second[i].v[j].nx, sizeof(p.second[i].v[j].nz));
				outfile.write((char*) &p.second[i].v[j].ny, sizeof(p.second[i].v[j].ny));
				outfile.write((char*) &p.second[i].v[j].nz, sizeof(p.second[i].v[j].nz));
				outfile.write((char*) &p.second[i].v[j].u, sizeof(p.second[i].v[j].u));
				outfile.write((char*) &p.second[i].v[j].v, sizeof(p.second[i].v[j].v));

				int linkCount = p.second[i].v[j].linkIds.size();
				outfile.write((char*) &linkCount, sizeof(linkCount));
				outfile.write((char*) &p.second[i].v[j].linkIds[0], sizeof(p.second[i].v[j].linkIds[0])*linkCount);
				
				int linkWeightCount = p.second[i].v[j].linkWeights.size();
				outfile.write((char*) &linkWeightCount, sizeof(linkWeightCount));
				outfile.write((char*) &p.second[i].v[j].linkWeights[0], sizeof(p.second[i].v[j].linkWeights[0])*linkWeightCount);

				outfile.write((char*) &p.second[i].v[j].globalWeight, sizeof(p.second[i].v[j].globalWeight));
				outfile.write((char*) &p.second[i].v[j].globalLink, sizeof(p.second[i].v[j].globalLink));					
			}
		}*/	
	}

	return true;
}

bool ConvertStaticSMD(std::string source, std::string dest)
{
	string file = source;
	ifstream infile(file);
	ofstream outfile(dest, ios::binary);
	unordered_map<int, int> boneIdToIndex;

	string temp;
	int number;
	infile >> temp >> number;
	string version = "version";
	if (temp != version)
	{
		return false;
	}
	infile >> temp;
	if (temp != "nodes")
	{
		return false;
	}
	
	string line;
	getline(infile, line);
	while (line != "end")
	{
		getline(infile, line);
	}

	infile >> temp;
	if (temp != "skeleton")
	{
		return false;
	}
	int time;
	infile >> temp >> time;
	if (temp != "time")
	{
		return false;
	}
	if (time != 0)
	{
		return false;
	}
	getline(infile, line);
	while (line != "end")
	{
		getline(infile, line);
	}

	infile >> temp;
	if (temp != "triangles")
	{
		return false;
	}

	int maxBones = 0;
	getline(infile, line);
	unordered_map<string, vector<SkinnedModelTriangle>> trianglesMap;
	while (line != "end")
	{
		if (line.length() > 0)
		{
			SkinnedModelTriangle t;
			t.material = line;
			for (int i = 0; i < 3; i++)
			{
				getline(infile, line);
				istringstream in(line);
				SkinnedModelVertex& v = t.v[i];
				in >> v.globalLink;

				
				in >> v.x >> v.y >> v.z >>
					v.nx >> v.ny >> v.nz >>
					v.u >> v.v;
			}
			trianglesMap[t.material].push_back(t);
		}
		getline(infile, line);
	}
	if (!infile) return false;

	int mapCount = trianglesMap.size();

	outfile.write((char*) &mapCount, sizeof(mapCount));
	for each(auto p in trianglesMap)
	{
		int materialLen = p.first.size();
		outfile.write((char*) &materialLen, sizeof(materialLen));
		outfile.write((char*) &p.first[0], sizeof(char)*p.first.size());

		int triangleCount = p.second.size();
		outfile.write((char*) &triangleCount, sizeof(triangleCount));

		vector<SkinnedModelTriangle>& triangles = p.second;
		StaticMeshVertex* vertices = new StaticMeshVertex[triangleCount * 3];
		for (int i = 0; i < triangleCount; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				vertices[i * 3 + j].v.x = triangles[i].v[j].x;
				vertices[i * 3 + j].v.y = triangles[i].v[j].y;
				vertices[i * 3 + j].v.z = triangles[i].v[j].z;

				vertices[i * 3 + j].n.x = triangles[i].v[j].nx;
				vertices[i * 3 + j].n.y = triangles[i].v[j].ny;
				vertices[i * 3 + j].n.z = triangles[i].v[j].nz;

				vertices[i * 3 + j].Tex.x = triangles[i].v[j].u;
				vertices[i * 3 + j].Tex.y = 1 - triangles[i].v[j].v;
			}
		}
		outfile.write((char*) vertices, sizeof(StaticMeshVertex)*triangleCount * 3);
	}

	return true;
}

void PrintFileStatistics(granny_file* GrannyFile)
{
	assert(GrannyFile);
	printf("granny_file\n-----------\n");
	granny_grn_section* SectionArray = GrannyGetGRNSectionArray(GrannyFile->Header);
	assert(SectionArray);

	printf("File contains: %d sections.\n", GrannyFile->SectionCount);
	{for (granny_int32x Section = 0; Section < GrannyFile->SectionCount; ++Section)
	{
		if (GrannyFile->Sections[Section])
		{
			printf("  Section %d: present", Section);

			// DataSize is the sizeof the data on disk, ExpandedDataSize is the size in memory.  If
			// they match, this section was written without Oodle1 compression.
			if (SectionArray[Section].DataSize ==
				SectionArray[Section].ExpandedDataSize)
			{
				printf(" (uncompressed)\n");
			}
			else
			{
				printf(" (compressed)\n");
			}
		}
		else
		{
			// This will likely never happen, since we haven't called GrannyFreeFileSection
			printf("  Section %d: already freed (or empty)\n", Section);
		}
	}}

	printf("\n");
}

void PrintFileInfoStatistics(granny_file_info* GrannyFileInfo)
{
	assert(GrannyFileInfo);
	printf("granny_file_info\n----------------\n");

	if (GrannyFileInfo->ArtToolInfo)
	{
		granny_art_tool_info* ToolInfo = GrannyFileInfo->ArtToolInfo;

		printf("File created by %s (%d.%d)\n",
			ToolInfo->FromArtToolName,
			ToolInfo->ArtToolMajorRevision,
			ToolInfo->ArtToolMinorRevision);
		printf("  Coordinate system\n"
			"    Units per meter: %f\n"
			"       Right Vector: (%f %f %f)\n"
			"          Up Vector: (%f %f %f)\n"
			"        Back Vector: (%f %f %f)\n\n",
			ToolInfo->UnitsPerMeter,
			ToolInfo->RightVector[0], ToolInfo->RightVector[1], ToolInfo->RightVector[2],
			ToolInfo->UpVector[0], ToolInfo->UpVector[1], ToolInfo->UpVector[2],
			ToolInfo->BackVector[0], ToolInfo->BackVector[1], ToolInfo->BackVector[2]);
	}
	else
	{
		printf("No Art tool information, GrannyComputeBasisConversion won't work on this file.\n");
	}
	if (GrannyFileInfo->ExporterInfo)
	{
		printf("File created with version %d.%d.%d.%d of Granny.\n",
			GrannyFileInfo->ExporterInfo->ExporterMajorRevision,
			GrannyFileInfo->ExporterInfo->ExporterMinorRevision,
			GrannyFileInfo->ExporterInfo->ExporterCustomization,
			GrannyFileInfo->ExporterInfo->ExporterBuildNumber);
	}
	if (GrannyFileInfo->FromFileName)
	{
		printf("Source file: %s\n\n", GrannyFileInfo->FromFileName);
	}
	printf("Number of    Textures: %d\n"
		"            Materials: %d\n"
		"               Meshes: %d\n"
		"               Models: %d\n"
		"           Animations: %d\n",
		GrannyFileInfo->TextureCount,
		GrannyFileInfo->MaterialCount,
		GrannyFileInfo->MeshCount,
		GrannyFileInfo->ModelCount,
		GrannyFileInfo->AnimationCount);
	printf("\n");
}

void writeSkeleton(std::string fileName, granny_skeleton* skeleton)
{
	ofstream sout(fileName + ".txt");
	ofstream bsout(fileName + ".bin", ios::binary);
	cout << " bones" << endl;
	sout << skeleton->BoneCount << endl;
	bsout.write((char*) &skeleton->BoneCount, sizeof(skeleton->BoneCount));
	for (int i = 0; i < skeleton->BoneCount; i++)
	{
		granny_bone& bone = skeleton->Bones[i];
		cout << bone.Name << " " << bone.ParentIndex << " " << endl;
		granny_real32 tx[4][4];

		GrannyBuildCompositeTransform4x4(&bone.LocalTransform, (granny_real32*) tx);
		sout << bone.Name << " ";
		int nameLen = strlen(bone.Name);

		bsout.write((char*) &nameLen, sizeof(nameLen));
		bsout.write((char*) bone.Name, sizeof(char)*nameLen);

		sout << bone.LocalTransform.Position[0] << " " << bone.LocalTransform.Position[1] << " " << bone.LocalTransform.Position[2] << endl;
		bsout.write((char*) &bone.LocalTransform.Position[0], sizeof(float) * 3);

		sout << bone.LocalTransform.Orientation[0] << " " << bone.LocalTransform.Orientation[1] << " " << bone.LocalTransform.Orientation[2] << " " << bone.LocalTransform.Orientation[3] << endl;
		bsout.write((char*) &bone.LocalTransform.Orientation[0], sizeof(float) * 4);

		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				sout << bone.LocalTransform.ScaleShear[j][k] << " ";
				bsout.write((char*) &bone.LocalTransform.ScaleShear[j][k], sizeof(float));
			}
		}
		//bsout.write((char*) &bone.LocalTransform.ScaleShear[0][0], sizeof(float) * 9);

		sout << endl;

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				sout << tx[j][k] << " ";
				bsout.write((char*) &tx[j][k], sizeof(float));
			}
		}
		sout << endl;
		//bsout.write((char*) &tx[0][0], sizeof(tx[0][0]) * 16);

		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				sout << bone.InverseWorld4x4[j][k] << " ";
				bsout.write((char*) &bone.InverseWorld4x4[j][k], sizeof(float));
			}
		}
		sout << endl;
		//bsout.write((char*) &bone.InverseWorld4x4[0][0], sizeof(float) * 16);
	}
}

float GetControlPointClamped(float* controls, int knotCount, int index, int dimensionIndex, int dimensions)
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

float GetKnotClamped(float* knots, int knotCount, int index)
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

void writeSpline(granny_curve2* curve, float* identity, int dim, ostream& out, ostream& outb, bool cancelYTranslation, float replacementY)
{
	out << "spline" << endl;
	int code = 2;
	outb.write((char*) &code, sizeof(code));

	int knotCount = GrannyCurveGetKnotCount(curve);
	int degree = GrannyCurveGetDegree(curve);

	float* knots = new float[knotCount];
	float* controls = new float[knotCount * dim];
	float* result = new float[dim];
	GrannyCurveExtractKnotValues(curve, 0, knotCount, knots, controls, identity);

	out << degree << endl;
	out << knotCount << endl;
	outb.write((char*) &degree, sizeof(degree));
	outb.write((char*) &knotCount, sizeof(knotCount));

	for (int i = 0; i < knotCount; i++)
	{
		out << knots[i] << " ";
	}
	outb.write((char*) knots, sizeof(float)*knotCount);
	out << endl;
	for (int i = 0; i < knotCount; i++)
	{
		if (cancelYTranslation)
		{
			controls[i*dim + 1] = replacementY;
		}
	}
	for (int i = 0; i < knotCount; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			out << controls[i*dim + j] << " ";
		}
	}
	outb.write((char*) controls, sizeof(float)*knotCount*dim);
	out << endl;
}

float* EvalCurve(granny_curve2* curve, float* identity, int dim, float t)
{
	// try to eval. degree 2 b spline is general case
	int knotCount = GrannyCurveGetKnotCount(curve);
	int degree = GrannyCurveGetDegree(curve);

	if (degree == 0) return NULL;

	float* knots = new float[knotCount];
	float* controls = new float[knotCount * dim];
	float* result = new float[dim];
	GrannyCurveExtractKnotValues(curve, 0, knotCount, knots, controls, identity);

	int uindex = 0;
	if (t < knots[0])
	{
		for (int m = 0; m < dim; m++)
		{
			result[m] = controls[m];
		}
		return result;
	}
	if (t >= knots[knotCount - 1])
	{
		for (int m = 0; m < dim; m++)
		{
			result[m] = controls[(knotCount - 1) * dim + m];
		}
		return result;
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

	vector<vector<float>> N;
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
		vector<float>& Nj = N[j];
		for (unsigned int i = 0; i < N[j - 1].size() - 1; i++)
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
			//float Nij = (t - ti) / (tij - ti) * Nij1 + (tij1 - t) / (tij1 - ti1) * Ni1j1;
			N[j].push_back(Nij);
		}
	}

	for (int j = 0; j < dim; j++)
	{
		result[j] = 0.0;
	}

	for (int i = 0; i <= degree; i++)
	{
		for (int j = 0; j < dim; j++)
		{
			float c = GetControlPointClamped(controls, knotCount, uindex - degree + i + 1, j, dim);
			//controls[(i+uindex-degree)*dim + j] 
			result[j] += c * N[degree][i];
		}
	}

	return result;
	// knot values for i, i+1, i+2
	float ui = knots[uindex], uj = knots[uindex + 1], uk = knots[uindex + 2], um = knots[uindex + 3];
	float ni_0 = t >= knots[uindex] ? 1.0f : 0.0f;
	float nj_0 = t >= knots[uindex + 1] ? 1.0f : 0.0f;
	float nk_0 = t >= knots[uindex + 2] ? 1.0f : 0.0f;
	float nm_0 = t >= knots[uindex + 3] ? 1.0f : 0.0f;

	float ni_1 = (t - ui) / (uj - ui) * ni_0 + (uk - t) / (uk - uj) * nj_0 + 0;
	float nj_1 = (t - uj) / (uk - uj) * nj_0 + (um - t) / (um - uk) * nk_0 + 0;

	float ni_2 = (t - ui) / (uk - ui) * ni_1 + (um - t) / (um - uj) * nj_1;

	float t0 = t - ui;
	float t1 = t - uj;
	float t2 = t - uk;

}

void writeResult(ostream& out, ostream& outb, float* array, int dim)
{
	for (int i = 0; i < dim; i++)
	{
		out << array[i] << " ";
	}
	outb.write((char*) array, sizeof(float)*dim);
	out << endl;
}

void writeKeyframes(ostream& out, ostream& outb, int dimension, granny_curve2* curve, float duration, float t, float* result, float* identity)
{
	GrannyEvaluateCurveAtT(dimension, false, false, curve, false, duration, t, result, identity);
	writeResult(out, outb, result, dimension);
}


void writeAnimation(std::string name, granny_animation* animation)
{
	bool offsetMoveAnimation = true;
	ofstream out(name + ".txt");
	ofstream outb(name + ".bin", ios::binary);
	out << animation->Duration << endl;
	outb.write((char*)&animation->Duration, sizeof(animation->Duration));

	for (int i = 0; i < animation->TrackGroupCount; i++)
	{
		granny_track_group* group = animation->TrackGroups[i];
		/*
		for (int j = 0; j < group->TransformTrackCount; j++)
		{
		granny_transform_track track = group->TransformTracks[j];
		granny_curve2 positionCurve = track.PositionCurve;
		int positionDim = GrannyCurveGetDimension(&positionCurve);
		float* pr = EvalCurve(&positionCurve, GrannyCurveIdentityPosition, positionDim, 0);

		granny_curve2 orientationCurve = track.OrientationCurve;
		int orientationDim = GrannyCurveGetDimension(&orientationCurve);
		float* or = EvalCurve(&orientationCurve, GrannyCurveIdentityOrientation, orientationDim, 0);

		granny_curve2 scaleShearCurve = track.ScaleShearCurve;
		int scaleShearDim = GrannyCurveGetDimension(&scaleShearCurve);
		float* sr = EvalCurve(&scaleShearCurve, GrannyCurveIdentityScaleShear, scaleShearDim, 0);
		}
		*/

		vector<int> translatedBones;
		vector<bool> hasTranslation;
		vector<float> offsets;
		vector<float> startValues;
		float moveOffset = 0;
		for (int j = 0; j < group->TransformTrackCount; j++)
		{
			granny_transform_track track = group->TransformTracks[j];
			granny_curve2 positionCurve = track.PositionCurve;
			int positionDim = GrannyCurveGetDimension(&positionCurve);
			granny_real32* positionResult = new granny_real32[positionDim];
			GrannyEvaluateCurveAtT(positionDim, false, false, &positionCurve, false, animation->Duration, 0, positionResult, GrannyCurveIdentityPosition);
			float startY = positionResult[1];
			GrannyEvaluateCurveAtT(positionDim, false, false, &positionCurve, false, animation->Duration, animation->Duration, positionResult, GrannyCurveIdentityPosition);
			float endY = positionResult[1];
			float tx = endY - startY;
			offsets.push_back(tx);
			startValues.push_back(startY);
			if (abs(tx) > 2 && offsetMoveAnimation)
			{
				hasTranslation.push_back(true);
				translatedBones.push_back(j);
			}
			else
			{
				hasTranslation.push_back(false);
			}
		}

//		cout << "Move translations!" << endl;

		for (unsigned int j = 0; j < translatedBones.size(); j++)
		{
//			cout << translatedBones[j] << " " << group->TransformTracks[translatedBones[j]].Name << " " << offsets[translatedBones[j]] << endl;
		}

//		cout << "End move translations!" << endl;

		outb.write((char*) &group->TransformTrackCount, sizeof(group->TransformTrackCount));
		for (int j = 0; j < group->TransformTrackCount; j++)
		{
			granny_transform_track track = group->TransformTracks[j];

			cout << track.Name << endl;
			out << track.Name << endl;
			
			int len = strlen(track.Name);
			outb.write((char*) &len, sizeof(len));
			outb.write(track.Name, len);

			int nFrames = 60;

			granny_curve2 positionCurve = track.PositionCurve;
			int positionDim = GrannyCurveGetDimension(&positionCurve);
			granny_real32* positionResult = new granny_real32[positionDim];
			if (GrannyCurveIsIdentity(&positionCurve))
			{
				out << "identity" << endl;
				int code = 0;
				outb.write((char*)&code, sizeof(code));
			}
			else if (GrannyCurveIsConstantNotIdentity(&positionCurve))
			{
				out << "constant ";
				int code = 1;
				outb.write((char*) &code, sizeof(code));
				if (positionDim != 3)
				{
					cout << "Error";
					exit(0);
				}
				GrannyEvaluateCurveAtT(positionDim, false, false, &positionCurve, false, animation->Duration, 0, positionResult, GrannyCurveIdentityPosition);
				writeResult(out, outb, positionResult, positionDim);
			}
			else
			{
				if (positionDim != 3)
				{
					cout << "Error";
					exit(0);
				}

				writeSpline(&positionCurve, GrannyCurveIdentityPosition, positionDim, out, outb, hasTranslation[j], startValues[j]);

				//	out << "keyframes" << endl;
				//	out << nFrames << endl;

				for (int k = 0; k < nFrames; k++) //frames
				{
					float t = ((float) k) / (nFrames - 1) * animation->Duration;

					GrannyEvaluateCurveAtT(positionDim, false, false, &positionCurve, false, animation->Duration, t, positionResult, GrannyCurveIdentityPosition);
					if (hasTranslation[j])
					{
						positionResult[1] = startValues[j];
					}
					//		writeResult(out, positionResult, positionDim);

					float* pr = EvalCurve(&positionCurve, GrannyCurveIdentityPosition, positionDim, t);
					for (int m = 0; m < positionDim; m++)
					{
						float diff = pr[m] - positionResult[m];
						if (abs(diff) > 0.0001)
						{
							cout << "Error of " << diff << " at " << t << endl;
						}
					}
				}
			}

			granny_curve2 orientationCurve = track.OrientationCurve;
			int orientationDim = GrannyCurveGetDimension(&orientationCurve);
			granny_real32* orientationResult = new granny_real32[orientationDim];
			if (GrannyCurveIsIdentity(&orientationCurve))
			{
				out << "identity" << endl;
				int code = 0;
				outb.write((char*) &code, sizeof(code));
			}
			else if (GrannyCurveIsConstantNotIdentity(&orientationCurve))
			{
				if (orientationDim != 4)
				{
					cout << "Error";
					exit(0);
				}
				out << "constant ";
				int code = 1;
				outb.write((char*) &code, sizeof(code));
				GrannyEvaluateCurveAtT(orientationDim, false, false, &orientationCurve, false, animation->Duration, 0, orientationResult, GrannyCurveIdentityOrientation);
				writeResult(out, outb, orientationResult, orientationDim);
			}
			else
			{
				if (orientationDim != 4)
				{
					cout << "Error";
					exit(0);
				}

				writeSpline(&orientationCurve, GrannyCurveIdentityOrientation, orientationDim, out, outb, false, 0);

				//		out << "keyframes" << endl;
				//		out << nFrames << endl;
				for (int k = 0; k < nFrames; k++) //frames
				{
					float t = ((float) k) / (nFrames - 1) * animation->Duration;
					GrannyEvaluateCurveAtT(orientationDim, false, false, &orientationCurve, false, animation->Duration, t, orientationResult, GrannyCurveIdentityOrientation);
					//			writeResult(out, orientationResult, orientationDim);

					float* or = EvalCurve(&orientationCurve, GrannyCurveIdentityOrientation, orientationDim, t);
					for (int m = 0; m < orientationDim; m++)
					{
						float diff = or[m] - orientationResult[m];
						if (abs(diff) > 0.0001)
						{
							cout << "Error of " << diff << " at " << t << endl;
						}
					}
				}
			}

			granny_curve2 scaleShearCurve = track.ScaleShearCurve;
			int scaleShearDim = GrannyCurveGetDimension(&scaleShearCurve);
			granny_real32* scaleShearResult = new granny_real32[scaleShearDim];
			if (GrannyCurveIsIdentity(&scaleShearCurve))
			{
				out << "identity" << endl;
				int code = 0;
				outb.write((char*) &code, sizeof(code));
			}
			else if (GrannyCurveIsConstantNotIdentity(&scaleShearCurve))
			{
				if (scaleShearDim != 9)
				{
					cout << "Error";
					exit(0);
				}
				out << "constant ";
				int code = 1;
				outb.write((char*) &code, sizeof(code));
				GrannyEvaluateCurveAtT(scaleShearDim, false, false, &scaleShearCurve, false, animation->Duration, 0, scaleShearResult, GrannyCurveIdentityScaleShear);
				writeResult(out, outb, scaleShearResult, scaleShearDim);
			}
			else
			{
				if (scaleShearDim != 9)
				{
					cout << "Error";
					exit(0);
				}

				writeSpline(&scaleShearCurve, GrannyCurveIdentityScaleShear, scaleShearDim, out, outb, false, 0);

				//		out << "keyframes" << endl;
				//		out << nFrames << endl;
				for (int k = 0; k < nFrames; k++) //frames
				{
					float t = ((float) k) / (nFrames - 1) * animation->Duration;
					GrannyEvaluateCurveAtT(scaleShearDim, false, false, &scaleShearCurve, false, animation->Duration, t, scaleShearResult, GrannyCurveIdentityScaleShear);
					//			writeResult(out, scaleShearResult, scaleShearDim);

					float* sr = EvalCurve(&scaleShearCurve, GrannyCurveIdentityScaleShear, scaleShearDim, t);
					for (int m = 0; m < scaleShearDim; m++)
					{
						float diff = sr[m] - scaleShearResult[m];
						if (abs(diff) > 0.0001)
						{
							cout << "Error of " << diff << " at " << t << endl;
						}
					}
				}
			}
		}
	}
}

int main(int argc, char** argv)
{
	if (!GrannyVersionsMatch)
	{
		printf("Warning: the Granny DLL currently loaded "
			"doesn't match the .h file used during compilation\n");
		return EXIT_FAILURE;
	}

	if (argc == 4)
	{
		string type = argv[1];
		string input = argv[2];
		string output = argv[3];
		if (type == "skeleton")
		{
			granny_file* GrannyFile = GrannyReadEntireFile(input.c_str());
			if (GrannyFile == 0)
			{
				cout << "Error: unable to load " << input << " as a Granny file." << endl;
				return EXIT_FAILURE;
			}

			granny_file_info* GrannyFileInfo = GrannyGetFileInfo(GrannyFile);
			if (GrannyFileInfo == 0)
			{
				printf("Could not extract a granny_file_info from the file.\n");
				return EXIT_FAILURE;
			}

			for (int modelCount = 0; modelCount < GrannyFileInfo->ModelCount; modelCount++)
			{
				granny_model* model = GrannyFileInfo->Models[modelCount];
				granny_skeleton* skeleton = model->Skeleton;
				writeSkeleton(output, skeleton);
			}
			GrannyFreeFile(GrannyFile);
			return 0;
		}
		else if (type == "animation")
		{
			granny_file* GrannyFile = GrannyReadEntireFile(input.c_str());
			if (GrannyFile == 0)
			{
				cout << "Error: unable to load " << input << " as a Granny file." << endl;
				return EXIT_FAILURE;
			}
			granny_file_info* GrannyFileInfo = GrannyGetFileInfo(GrannyFile);
			if (GrannyFileInfo == 0)
			{
				printf("Could not extract a granny_file_info from the file.\n");
				return EXIT_FAILURE;
			}

			for (int animationCount = 0; animationCount < GrannyFileInfo->AnimationCount; animationCount++)
			{
				granny_animation* animation = GrannyFileInfo->Animations[animationCount];
				writeAnimation(output, animation);
			}
			GrannyFreeFile(GrannyFile);
			return 0;
		}
		else if (type == "skinnedmesh")
		{
			ConvertSMD(input, output);
		}
		else if (type == "staticmesh")
		{
			ConvertStaticSMD(input, output);
		}
		cout << "invalid arguments" << endl;
		return 0;
	}
	//1726DB61-87C2-4441-AE4A-C7DC358E659E
	// char const* GrannyFilename = "d:\\data\\laptop\\Treants\\bin\\animations\\6BB5D561-21F6-42A9-93A0-BBCE5FEFCD70";
	//char const* GrannyFilename = "E:\\ArchMagi\\Files\\ArchMagiAnimation.gr2";
	//char const* GrannyFilename = "E:\\Code\\grannyrendering\\media\\pixo_dance.gr2";
	//char const* GrannyFilename = "E:\\HeroesAnimations\\Angels\\attack0.gr2";
	//char const* GrannySkeletonFilename = "E:\\HeroesAnimations\\sawmill\\skeleton.gr2";
	char const* GrannyAnimationFilename = "E:\\HeroesAnimations\\footman\\attack2.gr2";
	char const* GrannySkeletonFilename = "E:\\HeroesAnimations\\Angels\\skeleton.gr2";
	//char const* GrannyFilename = "E:\\Code\\grannyrendering\\media\\pixo_dance.gr2";
	if (argc == 2)
	{
		GrannyAnimationFilename = argv[1];
	}

	granny_file* GrannyFile = GrannyReadEntireFile(GrannySkeletonFilename);
	if (GrannyFile == 0)
	{
		printf("Error: unable to load %s as a Granny file.\n", GrannySkeletonFilename);
		return EXIT_FAILURE;
	}
	else
	{
		PrintFileStatistics(GrannyFile);
	}
	granny_file_info* GrannyFileInfo = GrannyGetFileInfo(GrannyFile);
	if (GrannyFileInfo == 0)
	{
		printf("Could not extract a granny_file_info from the file.\n");
		return EXIT_FAILURE;
	}
	else
	{
		PrintFileInfoStatistics(GrannyFileInfo);
	}

	cout << "Models:" << endl;
	for (int modelCount = 0; modelCount < GrannyFileInfo->ModelCount; modelCount++)
	{
		granny_model* model = GrannyFileInfo->Models[modelCount];
		cout << model->Name << endl;
		string n(model->Name);
		//if (n != lookFor)
		{
			//	continue;
		}
		cout << "mesh bindings: " << model->MeshBindingCount << endl;
		for (int i = 0; i < model->MeshBindingCount; i++)
		{
			granny_model_mesh_binding binding = model->MeshBindings[i];
			granny_mesh* mesh = binding.Mesh;
			cout << mesh->Name << endl;
		}
		granny_skeleton* skeleton = model->Skeleton;
		ostringstream sFileName;
		sFileName << "skeleton" << modelCount << ".txt";
		writeSkeleton(sFileName.str(), skeleton);
	}

	GrannyFile = GrannyReadEntireFile(GrannyAnimationFilename);
	if (GrannyFile == 0)
	{
		printf("Error: unable to load %s as a Granny file.\n", GrannyAnimationFilename);
		return EXIT_FAILURE;
	}
	else
	{
		PrintFileStatistics(GrannyFile);
	}
	GrannyFileInfo = GrannyGetFileInfo(GrannyFile);
	if (GrannyFileInfo == 0)
	{
		printf("Could not extract a granny_file_info from the file.\n");
		return EXIT_FAILURE;
	}
	else
	{
		PrintFileInfoStatistics(GrannyFileInfo);
	}

	cout << endl << "Animations:" << endl;
	for (int animationCount = 0; animationCount < GrannyFileInfo->AnimationCount; animationCount++)
	{
		granny_animation* animation = GrannyFileInfo->Animations[animationCount];

		ostringstream aFileName;
		aFileName << "animation" << animationCount << ".txt";
		writeAnimation(aFileName.str(), animation);
	}
	GrannyFreeFile(GrannyFile);
	GrannyFileInfo = NULL;
	GrannyFile = NULL;

	return EXIT_SUCCESS;


}