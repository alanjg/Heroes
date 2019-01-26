#include "stdafx.h"
#include "SkinnedMesh.h"
#include "Renderer.h"
#include "Camera.h"
#include "File.h"
using namespace std;

SkinnedMesh::SkinnedMesh(Renderer* r) : renderer(r) 
{
}

SkinnedMesh::~SkinnedMesh()
{
}

HRESULT SkinnedMesh::LoadMesh(istream& infile, const std::string& directoryRoot)
{
	SkinnedMesh& mesh = *this;
	vector<SkinnedModelBone>& bones = mesh.bones;
	unordered_map<int, int> boneIdToIndex;
	unordered_map<string, vector<SkinnedModelTriangle>> trianglesMap;
	string temp;
	int number;
	infile >> temp >> number;
	string version = "version";
	if (temp != version)
	{
		return E_FAIL;
	}
	infile >> temp;
	if (temp != "nodes")
	{
		return E_FAIL;
	}
	int globalBone = -1;
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
			SkinnedModelBone b;
			b.id = id;
			b.name = name;
			b.parentId = parentId;
			boneIdToIndex[id] = bones.size();
			bones.push_back(b);
			if (parentId == -1) globalBone = id;
		}
		getline(infile, line);
	}
	if (bones.size() > MAX_BONES)
	{
		return E_FAIL;
	}
	infile >> temp;
	if (temp != "skeleton")
	{
		return E_FAIL;
	}
	int time;
	infile >> temp >> time;
	if (temp != "time")
	{
		return E_FAIL;
	}
	if (time != 0)
	{
		return E_FAIL;
	}
	getline(infile, line);
	while (line != "end")
	{
		if (line.length() > 0)
		{
			istringstream in(line);
			int id;
			in >> id;
			SkinnedModelBone& b = bones[boneIdToIndex[id]];

			// not used - we read these values from skeleton.txt instead.
			float bx, by, bz, brx, bry, brz;
			in >> bx >> by >> bz >> brx >> bry >> brz;
		}
		getline(infile, line);
	}

	infile >> temp;
	if (temp != "triangles")
	{
		return E_FAIL;
	}

	int maxBones = 0;
	getline(infile, line);
	unordered_set<int> linksSeen;
	while (line != "end")
	{
		if (line.length() > 0)
		{
			string material = line;
			SkinnedModelTriangle t;
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
					if (linkId < 0 || linkId >= bones.size() || linkId >= mesh.boneList.size())
					{
						int what = 52;
					}
					linkId = boneIdToIndex[linkId];

					linksSeen.insert(linkId);
					int parent = bones[linkId].parentId;
					global -= linkWeight;
					v.linkIds.push_back(linkId);
					v.linkWeights.push_back(linkWeight);
				}
				v.globalWeight = global;
			}
			trianglesMap[material].push_back(t);
		}
		getline(infile, line);
	}
	if (!infile) return E_FAIL;
	
	meshParts.resize(trianglesMap.size());
	int i = 0;
	for each(pair<string, vector<SkinnedModelTriangle>> p in trianglesMap)
	{
		MeshPart& meshPart = meshParts[i++];
		meshPart.material = p.first;

		meshPart.texture = LoadTexture(meshPart, directoryRoot, p.first);
		LoadVertexBuffer(p.second, meshPart);
		
	}
	
	return S_OK;
}

HRESULT SkinnedMesh::LoadBinaryMesh(istream& infile, const std::string& directoryRoot)
{
	SkinnedMesh& mesh = *this;
	vector<SkinnedModelBone>& bones = mesh.bones;
	unordered_map<int, int> boneIdToIndex;
	
	string temp;
	
	int count;
	infile.read((char*) &count, sizeof(count));
	for (int i = 0; i < count; i++)
	{
		int id;
		int nameLen;
		string name;
		int parentId;

		infile.read((char*) &id, sizeof(id));
		infile.read((char*) &nameLen, sizeof(nameLen));
		name.resize(nameLen);
		infile.read((char*) &name[0], sizeof(char)*nameLen);
		infile.read((char*) &parentId, sizeof(parentId));

		SkinnedModelBone b;
		b.id = id;
		b.name = name;
		b.parentId = parentId;
		boneIdToIndex[id] = bones.size();
		bones.push_back(b);
	}
	if (bones.size() > MAX_BONES)
	{
		return E_FAIL;
	}

	int materialCount;
	infile.read((char*) &materialCount, sizeof(materialCount));

	meshParts.resize(materialCount);
	for (int i = 0; i < materialCount; i++)
	{
		MeshPart& meshPart = meshParts[i];
		int materialLen;
		infile.read((char*) &materialLen, sizeof(materialLen));
		string material;
		material.resize(materialLen);
		infile.read(&material[0], sizeof(char)*materialLen);

		meshPart.texture = LoadTexture(meshPart, directoryRoot, material);
		meshPart.material = material;
		
		int triangleCount;
		infile.read((char*) &triangleCount, sizeof(triangleCount));
		meshPart.triangles.resize(triangleCount * 3);

		infile.read((char*) &meshPart.triangles[0], sizeof(SkinnedMeshVertex)*triangleCount * 3);

		meshPart.vertexBuffer = LoadVertexBuffer2(&meshPart.triangles[0], meshPart.triangles.size());
	}

	if (!infile) return E_FAIL;
	return S_OK;
}

void SkinnedMesh::LoadSkeleton(istream& skel, std::vector<SkinnedModelBone>& bones)
{
	SkinnedMesh& mesh = *this;
	int totalBones = 0;
	skel >> totalBones;
	mesh.boneList.resize(totalBones, Bone::Identity());
	int skeletonBoneCount = 0;
	for (int i = 0; i < totalBones; i++)
	{
		string name;
		skel >> name;

		unsigned int pipeIndex = name.find_last_of('|');
		if (pipeIndex != -1)
		{
			name = name.substr(pipeIndex + 1);
		}
		int which = -1;
		for (unsigned int j = 0; j < bones.size(); j++)
		{
			if (bones[j].name == name)
			{
				which = j;
				skeletonBoneCount++;
				break;
			}
		}

		if (which == -1)
		{
			int what = 52;
			//	which = 0;
		}

		XMVECTOR position = XMVectorZero(), orientation = XMQuaternionIdentity();
		XMMATRIX scaleShear = XMMatrixIdentity();
		skel >> position.m128_f32[0] >> position.m128_f32[1] >> position.m128_f32[2];

		float rx, ry, rz, rq;
		skel >> rx >> ry >> rz >> rq;
		float test = rx*rx + ry*ry + rz*rz + rq*rq;
		float len = sqrtf(test);
		if (abs(len - 1.0f) > 0.01)
		{
			int what = 3;
		}
		rx /= len;
		ry /= len;
		rz /= len;
		rq /= len;
		orientation.m128_f32[0] = rx;
		orientation.m128_f32[1] = ry;
		orientation.m128_f32[2] = rz;
		orientation.m128_f32[3] = rq;

		for (int j = 0; j < 3; j++) for (int k = 0; k < 3; k++) skel >> scaleShear.r[j].m128_f32[k];
		if (which != -1)
		{
			XMStoreFloat4(&mesh.boneList[which].position, position);
			XMStoreFloat4(&mesh.boneList[which].orientation, orientation);
			XMStoreFloat4x4(&mesh.boneList[which].scaleShear, scaleShear);
		}
		XMFLOAT4X4 tx;
		XMFLOAT4X4 iw;
		for (int j = 0; j < 4; j++) for (int k = 0; k < 4; k++) skel >> tx.m[j][k];
		for (int j = 0; j < 4; j++) for (int k = 0; k < 4; k++) skel >> iw.m[j][k];
		if (which != -1)
		{
			mesh.boneList[which].matrix = tx;
			mesh.boneList[which].inverseWorld = iw;
		}
	}
	if (skeletonBoneCount != bones.size())
	{
		int wharf = 22;
	}
}

void SkinnedMesh::LoadBinarySkeleton(istream& skel, std::vector<SkinnedModelBone>& bones)
{
	SkinnedMesh& mesh = *this;
	int totalBones = 0;
	skel.read((char*) &totalBones, sizeof(totalBones));
	
	mesh.boneList.resize(totalBones, Bone::Identity());
	int skeletonBoneCount = 0;
	for (int i = 0; i < totalBones; i++)
	{
		int nameLen;
		skel.read((char*) &nameLen, sizeof(nameLen));
		string name;
		name.resize(nameLen);
		skel.read(&name[0], sizeof(char)*nameLen);
		
		unsigned int pipeIndex = name.find_last_of('|');
		if (pipeIndex != -1)
		{
			name = name.substr(pipeIndex + 1);
		}
		int which = -1;
		for (unsigned int j = 0; j < bones.size(); j++)
		{
			if (bones[j].name == name)
			{
				which = j;
				skeletonBoneCount++;
				break;
			}
		}

		if (which == -1)
		{
			int what = 52;
			//	which = 0;
		}

		XMVECTOR position = XMVectorZero(), orientation = XMQuaternionIdentity();
		XMMATRIX scaleShear = XMMatrixIdentity();
		
		float t[3];
		skel.read((char*) &t[0], sizeof(float) * 3);
		
		position.m128_f32[0] = t[0];
		position.m128_f32[1] = t[1];
		position.m128_f32[2] = t[2];

		float r[4];
		skel.read((char*) &r[0], sizeof(float) * 4);

		float rx, ry, rz, rq;
		rx = r[0];
		ry = r[1];
		rz = r[2];
		rq = r[3];
		float test = rx*rx + ry*ry + rz*rz + rq*rq;
		float len = sqrtf(test);
		if (abs(len - 1.0f) > 0.01)
		{
			int what = 3;
		}
		rx /= len;
		ry /= len;
		rz /= len;
		rq /= len;
		orientation.m128_f32[0] = rx;
		orientation.m128_f32[1] = ry;
		orientation.m128_f32[2] = rz;
		orientation.m128_f32[3] = rq;

		float ss[3][3];
		skel.read((char*) &ss[0][0], sizeof(float) * 9);
		
		for (int j = 0; j < 3; j++) for (int k = 0; k < 3; k++) scaleShear.r[j].m128_f32[k] = ss[j][k];
		if (which != -1)
		{
			XMStoreFloat4(&mesh.boneList[which].position, position);
			XMStoreFloat4(&mesh.boneList[which].orientation, orientation);
			XMStoreFloat4x4(&mesh.boneList[which].scaleShear, scaleShear);
		}
		XMFLOAT4X4 tx;
		XMFLOAT4X4 iw;
		skel.read((char*) &tx.m[0][0], sizeof(float) * 16);
		skel.read((char*) &iw.m[0][0], sizeof(float) * 16);
		
		if (which != -1)
		{
			mesh.boneList[which].matrix = tx;
			mesh.boneList[which].inverseWorld = iw;
		}
	}
	if (skeletonBoneCount != bones.size())
	{
		int wharf = 22;
	}
}

void SkinnedMesh::LoadAnimation(Animation& animation, std::istream& anim, std::vector<SkinnedModelBone>& bones)
{
	anim >> animation.duration;

	string name;
	int total = 0;
	while (anim >> name)
	{
		int pipeIndex = name.find_last_of('|');
		if (pipeIndex != -1)
		{
			name = name.substr(pipeIndex + 1);
		}

		bool hit = false;
		int whichMesh = -1;
		for (unsigned int i = 0; i < bones.size(); i++)
		{
			if (bones[i].name == name)
			{
				whichMesh = i;
				hit = true;
				total++;
				break;
			}
		}
		if (!hit)
		{
			int what = 22;
			//return E_FAIL;
		}

		if (whichMesh == -1) continue;

		int pdim = 0;
		float px = 0, py = 0, pz = 0;
		int rdim = 0;
		float rx, ry, rz, rq;

		BoneAnimation& boneAnimation = animation.boneAnimations[whichMesh];

		string type;
		anim >> type;

		if (type == "identity")
		{
			boneAnimation.positionAnimationType = AnimationIdentity;
		}
		else if (type == "constant")
		{
			boneAnimation.positionAnimationType = AnimationConstant;
			anim >> px >> py >> pz;
			boneAnimation.positionConstant = new XMFLOAT4(px, py, pz, 0);
		}
		else if (type == "spline")
		{
			boneAnimation.positionAnimationType = AnimationSpline;
			boneAnimation.positionSpline = new Spline();
			boneAnimation.positionSpline->Read(anim, 3);
		}
		else if (type == "keyframes")
		{

			int frames;
			anim >> frames;
			boneAnimation.positionAnimationType = AnimationKeyframes;
			boneAnimation.positionKeyframes = new PositionKeyframes();
			boneAnimation.positionKeyframes->keyframes.resize(frames);

			for (int f = 0; f < frames; f++)
			{
				XMVECTOR translateVector = XMVectorZero();

				anim >> px >> py >> pz;

				translateVector.m128_f32[0] = px;
				translateVector.m128_f32[1] = py;
				translateVector.m128_f32[2] = pz;
				XMStoreFloat4(&boneAnimation.positionKeyframes->keyframes[f], translateVector);
			}
		}

		anim >> type;
		if (type == "identity")
		{
			boneAnimation.orientationAnimationType = AnimationIdentity;
		}
		else if (type == "constant")
		{
			boneAnimation.orientationAnimationType = AnimationConstant;
			anim >> rx >> ry >> rz >> rq;
			boneAnimation.orientationConstant = new XMFLOAT4(rx, ry, rz, rq);
		}
		else if (type == "spline")
		{
			boneAnimation.orientationAnimationType = AnimationSpline;
			boneAnimation.orientationSpline = new Spline();
			boneAnimation.orientationSpline->Read(anim, 4);
		}
		else if (type == "keyframes")
		{
			int frames;
			anim >> frames;
			boneAnimation.orientationAnimationType = AnimationKeyframes;
			boneAnimation.orientationKeyframes = new OrientationKeyframes();
			boneAnimation.orientationKeyframes->keyframes.resize(frames);

			for (int f = 0; f < frames; f++)
			{
				XMVECTOR orientationVector = XMQuaternionIdentity();
				anim >> rx >> ry >> rz >> rq;
				float test = rx*rx + ry*ry + rz*rz + rq*rq;
				float len = sqrtf(test);

				rx /= len;
				ry /= len;
				rz /= len;
				rq /= len;
				XMVECTOR v;
				v.m128_f32[0] = rx;
				v.m128_f32[1] = ry;
				v.m128_f32[2] = rz;
				v.m128_f32[3] = rq;


				orientationVector.m128_f32[0] = rx;
				orientationVector.m128_f32[1] = ry;
				orientationVector.m128_f32[2] = rz;
				orientationVector.m128_f32[3] = rq;

				test = rx*rx + ry*ry + rz*rz + rq*rq;
				if (fabs(test - 1) > 0.01)
				{
					int what = 54;
				}
				XMStoreFloat4(&boneAnimation.orientationKeyframes->keyframes[f], orientationVector);
			}
		}

		anim >> type;
		if (type == "identity")
		{
			boneAnimation.scaleShearAnimationType = AnimationIdentity;
		}
		else if (type == "constant")
		{
			boneAnimation.scaleShearAnimationType = AnimationConstant;
			boneAnimation.scaleShearConstant = new XMFLOAT4X4();
			float f[9];
			for (int i = 0; i < 9; i++)
			{
				anim >> f[i];
				boneAnimation.scaleShearConstant->m[i / 3][i % 3] = f[i];
			}
			boneAnimation.scaleShearConstant->m[0][3] = 0.0;
			boneAnimation.scaleShearConstant->m[1][3] = 0.0;
			boneAnimation.scaleShearConstant->m[2][3] = 0.0;
			boneAnimation.scaleShearConstant->m[3][3] = 1.0;
			boneAnimation.scaleShearConstant->m[3][0] = 0.0;
			boneAnimation.scaleShearConstant->m[3][1] = 0.0;
			boneAnimation.scaleShearConstant->m[3][2] = 0.0;
		}
		else if (type == "spline")
		{
			boneAnimation.scaleShearAnimationType = AnimationSpline;
			boneAnimation.scaleShearSpline = new Spline();
			boneAnimation.scaleShearSpline->Read(anim, 9);
		}
		else if (type == "keyframes")
		{
			int frames;
			anim >> frames;
			boneAnimation.scaleShearAnimationType = AnimationKeyframes;
			boneAnimation.scaleShearKeyframes = new ScaleShearKeyframes();
			boneAnimation.scaleShearKeyframes->keyframes.resize(frames);

			for (int frame = 0; frame < frames; frame++)
			{
				XMMATRIX scale = XMMatrixIdentity();

				float f[9];
				for (int i = 0; i < 9; i++)
				{
					anim >> f[i];
					scale.r[i / 3].m128_f32[i % 3];
				}
				scale.r[3].m128_f32[3] = 1.0;
				XMStoreFloat4x4(&boneAnimation.scaleShearKeyframes->keyframes[frame], scale);
			}
		}
	}

	if (total != bones.size())
	{
		int weha = 11;
	}
}

void SkinnedMesh::LoadBinaryAnimation(Animation& animation, std::istream& anim, std::vector<SkinnedModelBone>& bones)
{
	anim.read((char*)&animation.duration, sizeof(float));

	int count = 0;
	anim.read((char*) &count, sizeof(count));

	int total = 0;
	for (int j = 0; j < count; j++)
	{
		int nameLen = 0;
		anim.read((char*) &nameLen, sizeof(nameLen));
		string name;
		name.resize(nameLen);
		anim.read(&name[0], nameLen);
		int pipeIndex = name.find_last_of('|');
		if (pipeIndex != -1)
		{
			name = name.substr(pipeIndex + 1);
		}

		bool hit = false;
		int whichMesh = -1;
		for (unsigned int i = 0; i < bones.size(); i++)
		{
			if (bones[i].name == name)
			{
				whichMesh = i;
				hit = true;
				total++;
				break;
			}
		}
		if (!hit)
		{
			int what = 22;
			//return E_FAIL;
		}

		//if (whichMesh == -1) continue;

		int pdim = 0;
		float px = 0, py = 0, pz = 0;
		float p[3];
		int rdim = 0;
		float r[4];

		BoneAnimation empty;
		BoneAnimation& boneAnimation = whichMesh == -1 ? empty : animation.boneAnimations[whichMesh];

		int type;
		anim.read((char*) &type, sizeof(type));

		if (type == 0) //"identity"
		{
			boneAnimation.positionAnimationType = AnimationIdentity;
		}
		else if (type == 1) //"constant"
		{
			boneAnimation.positionAnimationType = AnimationConstant;
			anim.read((char*) &p[0], sizeof(float) * 3);
			
			boneAnimation.positionConstant = new XMFLOAT4(p[0], p[1], p[2], 0);
		}
		else if (type == 2) //"spline"
		{
			boneAnimation.positionAnimationType = AnimationSpline;
			boneAnimation.positionSpline = new Spline();
			boneAnimation.positionSpline->ReadBinary(anim, 3);
		}
		else if (type == 3) //"keyframes"
		{
			/* Not yet implemented
			int frames;
			anim >> frames;
			boneAnimation.positionAnimationType = AnimationKeyframes;
			boneAnimation.positionKeyframes = new PositionKeyframes();
			boneAnimation.positionKeyframes->keyframes.resize(frames);

			for (int f = 0; f < frames; f++)
			{
				XMVECTOR translateVector = XMVectorZero();

				anim >> px >> py >> pz;

				translateVector.m128_f32[0] = px;
				translateVector.m128_f32[1] = py;
				translateVector.m128_f32[2] = pz;
				XMStoreFloat4(&boneAnimation.positionKeyframes->keyframes[f], translateVector);
			}
			*/
		}

		anim.read((char*) &type, sizeof(type));
		if (type == 0)// "identity"
		{
			boneAnimation.orientationAnimationType = AnimationIdentity;
		}
		else if (type == 1) //"constant"
		{
			boneAnimation.orientationAnimationType = AnimationConstant;
			anim.read((char*) &r[0], sizeof(float) * 4);
			boneAnimation.orientationConstant = new XMFLOAT4(r[0], r[1], r[2], r[3]);
		}
		else if (type == 2) //"spline"
		{
			boneAnimation.orientationAnimationType = AnimationSpline;
			boneAnimation.orientationSpline = new Spline();
			boneAnimation.orientationSpline->ReadBinary(anim, 4);
		}
		else if (type == 3) //"keyframes"
		{
			/* Not yet implemented
			int frames;
			anim >> frames;
			boneAnimation.orientationAnimationType = AnimationKeyframes;
			boneAnimation.orientationKeyframes = new OrientationKeyframes();
			boneAnimation.orientationKeyframes->keyframes.resize(frames);

			for (int f = 0; f < frames; f++)
			{
				XMVECTOR orientationVector = XMQuaternionIdentity();
				anim >> rx >> ry >> rz >> rq;
				float test = rx*rx + ry*ry + rz*rz + rq*rq;
				float len = sqrtf(test);

				rx /= len;
				ry /= len;
				rz /= len;
				rq /= len;
				XMVECTOR v;
				v.m128_f32[0] = rx;
				v.m128_f32[1] = ry;
				v.m128_f32[2] = rz;
				v.m128_f32[3] = rq;


				orientationVector.m128_f32[0] = rx;
				orientationVector.m128_f32[1] = ry;
				orientationVector.m128_f32[2] = rz;
				orientationVector.m128_f32[3] = rq;

				test = rx*rx + ry*ry + rz*rz + rq*rq;
				if (fabs(test - 1) > 0.01)
				{
					int what = 54;
				}
				XMStoreFloat4(&boneAnimation.orientationKeyframes->keyframes[f], orientationVector);
			}
			*/
		}

		anim.read((char*) &type, sizeof(type));
		if (type == 0) //"identity"
		{
			boneAnimation.scaleShearAnimationType = AnimationIdentity;
		}
		else if (type == 1) //"constant"
		{
			boneAnimation.scaleShearAnimationType = AnimationConstant;
			boneAnimation.scaleShearConstant = new XMFLOAT4X4();
			float f[9];
			anim.read((char*) &f[0], sizeof(float) * 9);
			for (int i = 0; i < 9; i++)
			{
				boneAnimation.scaleShearConstant->m[i / 3][i % 3] = f[i];
			}
			boneAnimation.scaleShearConstant->m[0][3] = 0.0;
			boneAnimation.scaleShearConstant->m[1][3] = 0.0;
			boneAnimation.scaleShearConstant->m[2][3] = 0.0;
			boneAnimation.scaleShearConstant->m[3][3] = 1.0;
			boneAnimation.scaleShearConstant->m[3][0] = 0.0;
			boneAnimation.scaleShearConstant->m[3][1] = 0.0;
			boneAnimation.scaleShearConstant->m[3][2] = 0.0;
		}
		else if (type == 2)//"spline"
		{
			boneAnimation.scaleShearAnimationType = AnimationSpline;
			boneAnimation.scaleShearSpline = new Spline();
			boneAnimation.scaleShearSpline->ReadBinary(anim, 9);
		}
		else if (type == 3)//"keyframes"
		{
			/* Not yet implemented
			int frames;
			anim >> frames;
			boneAnimation.scaleShearAnimationType = AnimationKeyframes;
			boneAnimation.scaleShearKeyframes = new ScaleShearKeyframes();
			boneAnimation.scaleShearKeyframes->keyframes.resize(frames);

			for (int frame = 0; frame < frames; frame++)
			{
				XMMATRIX scale = XMMatrixIdentity();

				float f[9];
				for (int i = 0; i < 9; i++)
				{
					int tx;
					anim >> f[i];
					scale.r[i / 3].m128_f32[i % 3];
				}
				scale.r[3].m128_f32[3] = 1.0;
				XMStoreFloat4x4(&boneAnimation.scaleShearKeyframes->keyframes[frame], scale);
			}
			*/
		}
	}

	if (total != bones.size())
	{
		int weha = 11;
	}
}

HRESULT SkinnedMesh::CreateModelGeometry(const std::string& directoryRoot)
{	
	SkinnedMesh& mesh = *this;
	mesh.directoryRoot = directoryRoot;

	bool useBinaryMesh = true;
	struct stat buffer;
	string binaryMeshFile = directoryRoot + "\\Geometry.skinnedmesh.smd.bin";

	if (useBinaryMesh && stat(binaryMeshFile.c_str(), &buffer) == 0)
	{
		ifstream infile(binaryMeshFile, ios::binary);

		HRESULT hr = LoadBinaryMesh(infile, directoryRoot);
		if (FAILED(hr)) return hr;
	}
	else
	{
		string meshFile = directoryRoot + "\\Geometry.skinnedmesh.smd";
		ifstream infile(meshFile);

		HRESULT hr = LoadMesh(infile, directoryRoot);
		if (FAILED(hr)) return hr;
	}

	bool useBinarySkeleton = false;
	string binarySkeletonFile = directoryRoot + "\\skeleton.bin";
	if (useBinarySkeleton && stat(binarySkeletonFile.c_str(), &buffer) == 0)
	{		
		ifstream skel(binarySkeletonFile, ios::binary);
		LoadBinarySkeleton(skel, bones);
	}
	else
	{
		string file = directoryRoot + "\\skeleton.txt";
		ifstream skel(file);
		LoadSkeleton(skel, bones);
	}
	
	string directory = directoryRoot + "\\animations\\";

	bool useBinary = true;
	vector<string> files = GetFilesInDirectory(directory);
	for each (string file in files)
	{
		bool isBinary = file[file.size() - 3] == 'b' && file[file.size() - 2] == 'i' && file[file.size() - 1] == 'n';
		if (useBinary != isBinary) continue;

		Animation animation;
		animation.boneAnimations.resize(mesh.boneList.size());
		int dotIndex = file.find_last_of('.');
		int nameStart = dotIndex - 1;
		while (file[nameStart] != '/' && file[nameStart] != '\\')
		{
			nameStart--;
		}
		nameStart++;

		animation.name = file.substr(nameStart, dotIndex - nameStart);

		if (useBinary)
		{
			ifstream anim(file, ios::binary);
			LoadBinaryAnimation(animation, anim, bones);
		}
		else
		{
			ifstream anim(file);
			LoadAnimation(animation, anim, bones);
		}

		mesh.animations.push_back(animation);
	}	
	/*
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = sizeof(CBSkinnedMatrices);
	HRESULT hr = renderer->g_pd3dDevice->CreateBuffer(&bd, NULL, &mesh.skinnedModelMatrices);
	if (FAILED(hr))
		return hr;

	bd.ByteWidth = sizeof(WorldTransformConstantBuffer);
	hr = renderer->g_pd3dDevice->CreateBuffer(&bd, NULL, &m_worldTransformConstantBuffer);
	if (FAILED(hr))
		return hr;
		*/
	return S_OK;
}

ID3D11ShaderResourceView* SkinnedMesh::LoadTexture(MeshPart& meshPart, const std::string& directoryRoot, const std::string& material)
{
	ID3D11ShaderResourceView* texture;

	string textureFile = directoryRoot + "\\" + material + ".tga.dds";
	struct stat buffer;
	if (stat(textureFile.c_str(), &buffer) != 0)
	{
		textureFile = directoryRoot + "\\" + material + ".dds";
		if (stat(textureFile.c_str(), &buffer) != 0)
		{
			textureFile = directoryRoot + "\\" + material + ".(Texture).dds";
			if (stat(textureFile.c_str(), &buffer) != 0)
			{
				textureFile = directoryRoot + "\\" + material + ".1.(Texture).dds";
				if (stat(textureFile.c_str(), &buffer) != 0)
				{
					bool bad = true;
				}
			}
		}
	}

	wstring txf(textureFile.begin(), textureFile.end());
	HRESULT hr = CreateDDSTextureFromFile(renderer->g_pd3dDevice, txf.c_str(), NULL, &texture);
	if (FAILED(hr)) return NULL;
	return texture;
}

HRESULT SkinnedMesh::LoadVertexBuffer(vector<SkinnedModelTriangle>& triangles, MeshPart& meshPart)
{
	meshPart.triangles.resize(triangles.size() * 3);
	SkinnedMeshVertex* vertices = &meshPart.triangles[0];
	float minx, miny, minz;
	float maxx, maxy, maxz;
	minx = triangles[0].v[0].x;
	miny = triangles[0].v[0].y;
	minz = triangles[0].v[0].z;
	maxx = triangles[0].v[0].x;
	maxy = triangles[0].v[0].y;
	maxz = triangles[0].v[0].z;

	for (unsigned int i = 0; i < triangles.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vertices[i * 3 + j].v.x = triangles[i].v[j].x;
			vertices[i * 3 + j].v.y = triangles[i].v[j].y;
			vertices[i * 3 + j].v.z = triangles[i].v[j].z;

			minx = min(minx, triangles[i].v[j].x);
			miny = min(miny, triangles[i].v[j].y);
			minz = min(minz, triangles[i].v[j].z);

			maxx = max(maxx, triangles[i].v[j].x);
			maxy = max(maxy, triangles[i].v[j].y);
			maxz = max(maxz, triangles[i].v[j].z);

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

	meshPart.vertexBuffer = LoadVertexBuffer2(vertices, triangles.size()*3);
	return S_OK;
}

ID3D11Buffer* SkinnedMesh::LoadVertexBuffer2(SkinnedMeshVertex* vertices, int vertexCount)
{
	ID3D11Buffer* pBuffer;
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SkinnedMeshVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	hr = renderer->g_pd3dDevice->CreateBuffer(&bd, &InitData, &pBuffer);
	if (FAILED(hr))
		return NULL;

	// Set vertex buffer
	UINT stride = sizeof(SkinnedMeshVertex);
	UINT offset = 0;
	renderer->g_pImmediateContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);

	return pBuffer;
}

void SkinnedMesh::RenderModel(int animation, float t, std::vector<XMFLOAT4X4>& accumulatedBoneTransforms, ID3D11Buffer* skinnedModelMatrices, ID3D11Buffer* worldTransformBuffer, CBSkinnedMatrices* boneMatricesTransformed)
{
	if (animations.size() == 0)
	{
		return;
	}
	animation = animation % animations.size();
	
	// scale t in range
	while (t >= animations[animation].duration)
	{
		t -= animations[animation].duration;
	}
	

	// walks the skeleton tree, composing matrices, and sets them as the transformed matrices
	for (unsigned int i = 0; i < bones.size(); i++)
	{
		int parentBone = bones[i].parentId;
		XMMATRIX currentTransform = parentBone == -1 ? XMMatrixIdentity() : XMLoadFloat4x4(&accumulatedBoneTransforms[parentBone]);
		float splineRead[9];
		XMVECTOR positionAnimation;
		XMVECTOR orientationAnimation;
		XMMATRIX scaleShearAnimation;
		BoneAnimation& boneAnimation = animations[animation].boneAnimations[i];
		
		switch (boneAnimation.positionAnimationType)
		{
		case AnimationNone:
			positionAnimation = XMLoadFloat4(&boneList[i].position);
			break;
		case AnimationIdentity:
			positionAnimation = XMVectorZero();
			break;
		case AnimationConstant:
			positionAnimation = XMLoadFloat4(boneAnimation.positionConstant);
			break;
		case AnimationSpline:
			boneAnimation.positionSpline->Evaluate(t, splineRead);
			positionAnimation.m128_f32[0] = splineRead[0];
			positionAnimation.m128_f32[1] = splineRead[1];
			positionAnimation.m128_f32[2] = splineRead[2];
			positionAnimation.m128_f32[3] = 1.0;
			break;
		case AnimationKeyframes:
			int frame = (int)(t / animations[animation].duration * boneAnimation.positionKeyframes->keyframes.size());
			int currentFrame = frame % boneAnimation.positionKeyframes->keyframes.size();
			positionAnimation = XMLoadFloat4(&boneAnimation.positionKeyframes->keyframes[currentFrame]);
			break;
		}

		switch (boneAnimation.orientationAnimationType)
		{
		case AnimationNone:
			orientationAnimation = XMLoadFloat4(&boneList[i].orientation);
			break;
		case AnimationIdentity:
			orientationAnimation = XMVectorZero();
			break;
		case AnimationConstant:
			orientationAnimation = XMLoadFloat4(boneAnimation.orientationConstant);
			break;
		case AnimationSpline:
			boneAnimation.orientationSpline->Evaluate(t, splineRead);
			orientationAnimation.m128_f32[0] = splineRead[0];
			orientationAnimation.m128_f32[1] = splineRead[1];
			orientationAnimation.m128_f32[2] = splineRead[2];
			orientationAnimation.m128_f32[3] = splineRead[3];
			break;
		case AnimationKeyframes:
			int frame = (int)(t / animations[animation].duration * boneAnimation.orientationKeyframes->keyframes.size());
			int currentFrame = frame % boneAnimation.orientationKeyframes->keyframes.size();
			orientationAnimation = XMLoadFloat4(&boneAnimation.orientationKeyframes->keyframes[currentFrame]);
			break;
		}

		switch (boneAnimation.scaleShearAnimationType)
		{
		case AnimationNone:
			scaleShearAnimation = XMLoadFloat4x4(&boneList[i].scaleShear);
			break;
		case AnimationIdentity:
			scaleShearAnimation = XMMatrixIdentity();
			break;
		case AnimationConstant:
			scaleShearAnimation = XMLoadFloat4x4(boneAnimation.scaleShearConstant);
			break;
		case AnimationSpline:
			boneAnimation.scaleShearSpline->Evaluate(t, splineRead);
			scaleShearAnimation = XMMatrixIdentity();

			scaleShearAnimation.r[0].m128_f32[0] = splineRead[0];
			scaleShearAnimation.r[0].m128_f32[1] = splineRead[1];
			scaleShearAnimation.r[0].m128_f32[2] = splineRead[2];
			
			scaleShearAnimation.r[1].m128_f32[0] = splineRead[3];
			scaleShearAnimation.r[1].m128_f32[1] = splineRead[4];
			scaleShearAnimation.r[1].m128_f32[2] = splineRead[5];
			
			scaleShearAnimation.r[2].m128_f32[0] = splineRead[6];
			scaleShearAnimation.r[2].m128_f32[1] = splineRead[7];
			scaleShearAnimation.r[2].m128_f32[2] = splineRead[8];

			break;
		case AnimationKeyframes:
			int frame = (int)(t / animations[animation].duration * boneAnimation.scaleShearKeyframes->keyframes.size());
			int currentFrame = frame % boneAnimation.scaleShearKeyframes->keyframes.size();
			scaleShearAnimation = XMLoadFloat4x4(&boneAnimation.scaleShearKeyframes->keyframes[currentFrame]);
			break;
		}
		/*
		if (animations[animation].boneAnimations[i].keyframes.size() == 0)
		{
			// not animated, use base skeletal transform
			positionAnimation = XMLoadFloat4(&boneList[i].position);
			orientationAnimation = XMLoadFloat4(&boneList[i].orientation);
			scaleShearAnimation = XMLoadFloat4x4(&boneList[i].scaleShear);
		}
		else
		{
			int currentFrame = frame % animations[animation].boneAnimations[i].keyframes.size();

			positionAnimation = XMLoadFloat4(&animations[animation].boneAnimations[i].keyframes[currentFrame].position);
			orientationAnimation = XMLoadFloat4(&animations[animation].boneAnimations[i].keyframes[currentFrame].orientation);
			scaleShearAnimation = XMLoadFloat4x4(&animations[animation].boneAnimations[i].keyframes[currentFrame].scaleShear);
		}
		*/

		XMMATRIX animationPositionTransform = XMMatrixTranslationFromVector(positionAnimation);
		XMMATRIX animationOrientationTransform = XMMatrixRotationQuaternion(orientationAnimation);

		XMMATRIX animatedBoneTransform = scaleShearAnimation * animationOrientationTransform * animationPositionTransform;

		XMMATRIX inverseSkeleton = XMLoadFloat4x4(&boneList[i].inverseWorld);
		XMMATRIX accumulatedBoneTransform = animatedBoneTransform * currentTransform;
		XMMATRIX fullAnimationWithInverseSkeleton = inverseSkeleton * accumulatedBoneTransform;
		fullAnimationWithInverseSkeleton = XMMatrixTranspose(fullAnimationWithInverseSkeleton);

		XMStoreFloat4x4(&boneMatricesTransformed->matrices[i], fullAnimationWithInverseSkeleton);
		XMStoreFloat4x4(&accumulatedBoneTransforms[i], accumulatedBoneTransform);
	}

	renderer->g_pImmediateContext->UpdateSubresource(skinnedModelMatrices, 0, NULL, boneMatricesTransformed, 0, 0);

	// Set vertex buffer
	renderer->g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride2 = sizeof(SkinnedMeshVertex);
	UINT offset2 = 0;

	for (unsigned int i = 0; i < meshParts.size(); i++)
	{
		renderer->g_pImmediateContext->IASetInputLayout(renderer->g_pSkinnedVertexLayoutModel);
		renderer->g_pImmediateContext->IASetVertexBuffers(0, 1, &meshParts[i].vertexBuffer, &stride2, &offset2);
		renderer->g_pImmediateContext->VSSetShader(renderer->g_pSkinnedVertexShaderModel, NULL, 0);
		renderer->g_pImmediateContext->VSSetConstantBuffers(0, 1, renderer->GetViewTransformBuffer());
		renderer->g_pImmediateContext->VSSetConstantBuffers(1, 1, renderer->GetProjectionTransformBuffer());
		renderer->g_pImmediateContext->VSSetConstantBuffers(2, 1, &worldTransformBuffer);
		renderer->g_pImmediateContext->VSSetConstantBuffers(3, 1, &skinnedModelMatrices);
		renderer->g_pImmediateContext->PSSetShader(renderer->g_pSkinnedPixelShaderModel, NULL, 0);
		renderer->g_pImmediateContext->PSSetConstantBuffers(2, 1, &worldTransformBuffer);
		renderer->g_pImmediateContext->PSSetShaderResources(0, 1, &meshParts[i].texture);
		renderer->g_pImmediateContext->PSSetSamplers(0, 1, &renderer->g_pSamplerLinear);
		renderer->g_pImmediateContext->Draw(meshParts[i].triangles.size(), 0);

		renderer->m_rendererdTriangleCount += meshParts[i].triangles.size();
	}
}