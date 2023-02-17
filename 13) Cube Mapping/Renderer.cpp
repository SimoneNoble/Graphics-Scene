#include "Renderer.h"
#include "../nclgl/Light.h"
#include "../nclgl/Heightmap.h"
#include "../nclgl/Shader.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshAnimation.h" 
#include "../nclgl/MeshMaterial.h"
#include <algorithm>	// For std::sort..., new for scene node
#include <string> 

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"TestTest2.png");
	waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthTex = SOIL_load_OGL_texture(TEXTUREDIR"grasstest2.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	earthBump = SOIL_load_OGL_texture(TEXTUREDIR"grassbump.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	wallBump = SOIL_load_OGL_texture(TEXTUREDIR"DeFantasy_River_RockWall_Normal_OpenGL.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"forest_east.jpg", TEXTUREDIR"forest_west.jpg", TEXTUREDIR"forest_up.jpg", TEXTUREDIR"forest_down.jpg", 
		TEXTUREDIR"forest_north.jpg", TEXTUREDIR"forest_south.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	rockTex = SOIL_load_OGL_texture(TEXTUREDIR"RockTexture.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	snowTex = SOIL_load_OGL_texture(TEXTUREDIR"SnowTexture.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	//if (!earthTex || !earthBump || !cubeMap || !waterTex || !waterBump || !wallBump || !rockTex || !snowTex) {
	//	return;
	//}

	if (!earthTex || !earthBump || !cubeMap || !waterTex || !waterBump || !wallBump) {
		return;
	}

	SetTextureRepeating(earthTex, true);
	SetTextureRepeating(earthBump, true);
	SetTextureRepeating(waterTex, true);
	SetTextureRepeating(waterBump, true);
	SetTextureRepeating(rockTex, true);
	SetTextureRepeating(snowTex, true);
	//glEnable(GL_DEPTH_TEST);

	WallMesh = Mesh::LoadFromMeshFile("StoneWall_FBX.msh");
	WallMaterial = new MeshMaterial("StoneWall_FBX.mat");

	reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	lightShader1 = new Shader("PerPixelVertex.glsl", "PerPixelFragmentDirectional.glsl");
	lightShader2 = new Shader("PerPixelVertex.glsl", "PerPixelFragmentDirectional.glsl");
	lightShader3 = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	RobotShader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl"); 
	WallShader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");

	texture = SOIL_load_OGL_texture(TEXTUREDIR"DeFantasy_River_RockWall_Normal_OpenGL.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0); // leave as this for now

	if (!reflectShader->LoadSuccess() || !skyboxShader->LoadSuccess() || !lightShader1->LoadSuccess() || !lightShader2->LoadSuccess() || !lightShader3->LoadSuccess() || !RobotShader->LoadSuccess() || !WallShader->LoadSuccess() || !texture) {
		return;
	}

	root = new SceneNode(); // new for scene node

	Vector3 heightmapSize = heightMap->GetHeightmapSize();

	camera = new Camera(-45.0f, 0.0f, heightmapSize * Vector3(0.4f, 0.6f, 0.4f));
	light1 = new Light(Matrix4::Translation(Vector3(2.0f, 2.0f, 2.0f)), Vector4(1, 1, 2, 0.3), heightmapSize.x * 2.0f);
	light2 = new Light(Matrix4::Translation(Vector3(1.0f, 1.0f, 1.0f)), Vector4(1, 1, 1, 0.3), heightmapSize.x * 0.5f);
	light3 = new Light(Matrix4::Translation(Vector3(1.0f, 1.0f, 1.0f)), Vector4(0, 4, 0, 3), heightmapSize.x * 1.0f);
	//Matrix4::Translation(heightmapSize * Vector3(0.8f, 0.11f, 0.6f))* Matrix4::Scale(Vector3(750, 750, 750))* Matrix4::Rotation(0, Vector3(0.0f, 1.0f, 0.0f));
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//glEnable(GL_DEPTH_TEST); // new for scene node

	RobotMesh = Mesh::LoadFromMeshFile("Role_T.msh"); 
	anim = new MeshAnimation("Role_T.anm"); 
	RobotMaterial = new MeshMaterial("Role_T.mat"); 

	//for robot:
	for (int i = 0; i < RobotMesh->GetSubMeshCount(); ++i) { 
		const MeshMaterialEntry* matEntry = RobotMaterial->GetMaterialForLayer(i); 
		const string* filename = nullptr; 
		matEntry->GetEntry("Diffuse", &filename); 
		string path = TEXTUREDIR + *filename; 
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		RobotmatTextures.emplace_back(texID); 
	} 

	// row 1 of walls:
	for (int i = 0; i < 7; ++i) { // new for scene node
		SceneNode* s = new SceneNode(); // new for scene node
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // new for scene node
		s->SetTransform(Matrix4::Translation(Vector3(1050, 10.0f, -100.0f + 100.0f + 165 * i))); // new for scene node
		s->SetModelScale(Vector3(50.0f, 50.0f, 50.0f)); // new for scene node
		s->SetBoundingRadius(100.0f); // new for scene node

		for (int j = 0; j < WallMesh->GetSubMeshCount(); ++j) {
			const MeshMaterialEntry* matEntry = WallMaterial->GetMaterialForLayer(j); // new for scene node
			const string* filename = nullptr; // new for scene node
			matEntry->GetEntry("Diffuse", &filename); // new for scene node
			string path = TEXTUREDIR + *filename; // new for scene node
			GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y); // new for scene node
			WallmatTextures.emplace_back(texID); // new for scene node

			glActiveTexture(GL_TEXTURE0); // new for scene node
			glBindTexture(GL_TEXTURE_2D, WallmatTextures[j]); // new for scene node
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, wallBump);
			WallMesh->DrawSubMesh(j); // new for scene node
			s->SetTexture(WallmatTextures[j]); // new for scene node

		} // new for scene node

		s->SetMesh(WallMesh); // new for scene node
		root->AddChild(s); // new for scene node
	} // new for scene node

	// row 2 of walls:
	for (int i = 0; i < 5; ++i) { // new for scene node
		SceneNode* s = new SceneNode(); // new for scene node
		s->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f)); // new for scene node

		//s->SetTransform(Matrix4::Translation(Vector3(1050, 1.0f, 10.0f + 50))); // new for scene node
		s->SetModelScale(Vector3(50.0f, 50.0f, 50.0f)); // new for scene node
		s->SetBoundingRadius(100.0f); // new for scene node
		s->SetTransform(Matrix4::Translation(Vector3(300.0f + 165 * i, 10.0f, 1060))* Matrix4::Rotation(90, Vector3(0.0f, 1.0f, 0.0f)));
		for (int j = 0; j < WallMesh->GetSubMeshCount(); ++j) {
			const MeshMaterialEntry* matEntry = WallMaterial->GetMaterialForLayer(j); // new for scene node
			const string* filename = nullptr; // new for scene node
			matEntry->GetEntry("Diffuse", &filename); // new for scene node
			string path = TEXTUREDIR + *filename; // new for scene node
			GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y); // new for scene node
			WallmatTextures.emplace_back(texID); // new for scene node

			glActiveTexture(GL_TEXTURE0); // new for scene node
			glBindTexture(GL_TEXTURE_2D, WallmatTextures[j]); // new for scene node
			WallMesh->DrawSubMesh(j); // new for scene node
			s->SetTexture(WallmatTextures[j]); // new for scene node
		} // new for scene node

		s->SetMesh(WallMesh); // new for scene node
		root->AddChild(s); // new for scene node

		//BindShader(WallShader); // robot shader is the same as wall shader

		//glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "lightPos"), 1, (float*) & (*light1).GetPosition());
		//glUniform4fv(glGetUniformLocation(lightShader1->GetProgram(), "lightColour"), 1, (float*) & (*light1).GetColour());
		//glUniform1f(glGetUniformLocation(lightShader1->GetProgram(), "lightRadius"), (*light1).GetRadius());

		//glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "lightPos2"), 1, (float*) & (*light2).GetPosition());
		//glUniform4fv(glGetUniformLocation(lightShader2->GetProgram(), "lightColour2"), 1, (float*) & (*light2).GetColour());
		//glUniform1f(glGetUniformLocation(lightShader2->GetProgram(), "lightRadius2"), (*light2).GetRadius());

		//glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "diffuseTex"), 0);

		//UpdateShaderMatrices();

		//vector<Matrix4> frameMatrices;

		//const Matrix4* invBindPose = WallMesh->GetInverseBindPose();

	} // new for scene node

	////for wall:
	//for (int i = 0; i < WallMesh->GetSubMeshCount(); ++i) { 
	//	const MeshMaterialEntry* matEntry = WallMaterial->GetMaterialForLayer(i); 
	//	const string* filename = nullptr; 
	//	matEntry->GetEntry("Diffuse", &filename); 
	//	string path = TEXTUREDIR + *filename; 
	//	GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y); 
	//	WallmatTextures.emplace_back(texID); 
	//} 

	currentFrame = 0; 
	frameTime = 0.0f;
	init = true; 

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE); //new for robot, deletes water
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = 0.0f;
	waterCycle = 0.0f;

	init = true;
}

Renderer::~Renderer(void) {
	delete root; // new for scene node
	delete camera;
	delete RobotMesh; 
	delete RobotMaterial; 
	delete anim;
	delete RobotShader; 
	delete WallMesh;
	delete WallMaterial;
	delete WallShader;
	delete heightMap;
	delete quad;
	delete reflectShader;
	delete skyboxShader;
	delete lightShader1;
	delete lightShader2;
	delete lightShader3;
	delete light1;
	delete light2;
	delete light3;
	glDeleteTextures(1, &texture); // new for scene node
	glDeleteTextures(1, &waterTex);
	glDeleteTextures(1, &waterBump);
	glDeleteTextures(1, &earthTex);
	glDeleteTextures(1, &waterBump);
	glDeleteTextures(1, &wallBump);
	glDeleteTextures(1, &rockTex);
	glDeleteTextures(1, &snowTex);
}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(startTime, togCam, dt);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix); // new for scene node
	light1->SetPosition(light1->GetPosition() * Matrix4::Rotation(-40.0f * dt, Vector3(6, 6, 6)));
	light2->SetPosition(light2->GetPosition() * Matrix4::Rotation(-20.0f * dt, Vector3(3, 3, 3))); 
	light3->SetPosition(light3->GetPosition() * Matrix4::Rotation(0.0f * dt, Vector3(3, 3, 3)));
	//light3->SetPosition(light3->GetPosition()); //change this?

	waterRotate += dt * 2.0f; // 2 degrees a second
	waterCycle += dt * 0.25f; //10 units a second

	walkingRotate = 0.25f * dt;
	walking = 2.0f * dt; 

	frameTime -= dt;
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % anim->GetFrameCount();
		frameTime += 1.0f / anim->GetFrameRate();
	}

	root->Update(dt); // new for scene node
}

void Renderer::BuildNodeLists(SceneNode* from) { // new for scene node
	if (frameFrustum.InsideFrustum(*from)) { // new for scene node
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition(); // new for scene node
		from->SetCameraDistance(Vector3::Dot(dir, dir)); // new for scene node

		if (from->GetColour().w < 1.0f) { // new for scene node
			transparentNodeList.push_back(from); // new for scene node
		} // new for scene node
		else { // new for scene node
			nodeList.push_back(from); // new for scene node
		} // new for scene node
	} // new for scene node

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) { // new for scene node
		BuildNodeLists((*i)); // new for scene node
	} // new for scene node
} // new for scene node

void Renderer::SortNodeLists() { // new for scene node
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance); // note the r's! // new for scene node
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance); // new for scene node
} // new for scene node

void Renderer::DrawNodes() { // new for scene node
	for (const auto& i : nodeList) { // new for scene node
		DrawNode(i); // new for scene node
	} // new for scene node
	for (const auto& i : transparentNodeList) { // new for scene node
		DrawNode(i); // new for scene node
	} // new for scene node
} // new for scene node

void Renderer::DrawNode(SceneNode* n) { // new for scene node
	if (n->GetMesh()) { // new for scene node
		Matrix4 model = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale()); // new for scene node

		glUniformMatrix4fv(glGetUniformLocation(WallShader->GetProgram(), "modelMatrix"), 1, false, model.values); // new for scene node

		glUniform4fv(glGetUniformLocation(WallShader->GetProgram(), "nodeColour"), 1, (float*)& n->GetColour()); // new for scene node

		texture = n->GetTexture(); // new for scene node
		glActiveTexture(GL_TEXTURE0); // new for scene node
		glBindTexture(GL_TEXTURE_2D, texture); // new for scene node

		glUniform1i(glGetUniformLocation(WallShader->GetProgram(), "useTexture"), texture); // new for scene node

		n->Draw(*this); // new for scene node
	} // new for scene node
} // new for scene node

void Renderer::ClearNodeLists() { // new for scene node
	transparentNodeList.clear(); // new for scene node
	nodeList.clear(); // new for scene node
} // new for scene node

void Renderer::RenderScene() {
	BuildNodeLists(root); // new for scene node
	SortNodeLists(); // new for scene node
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawSkybox();

	BindShader(WallShader);// new for scene node
	UpdateShaderMatrices();// new for scene node

	glUniform1i(glGetUniformLocation(WallShader->GetProgram(), "diffuseTex"), 0); // new for scene node
	DrawNodes(); // new for scene node

	DrawHeightmap();
	DrawWater();
	DrawRobot();
	DrawWall();

	ClearNodeLists(); // new for scene node
}

void Renderer::DrawRobot() { 
	BindShader(RobotShader); 
	//SetShaderLight(*light1);

	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "lightPos"), 1, (float*) & (*light1).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader1->GetProgram(), "lightColour"), 1, (float*) & (*light1).GetColour());
	glUniform1f(glGetUniformLocation(lightShader1->GetProgram(), "lightRadius"), (*light1).GetRadius());

	glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "lightPos2"), 1, (float*) & (*light2).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader2->GetProgram(), "lightColour2"), 1, (float*) & (*light2).GetColour());
	glUniform1f(glGetUniformLocation(lightShader2->GetProgram(), "lightRadius2"), (*light2).GetRadius());

	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(lightShader2->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(lightShader2->GetProgram(), "bumpTex"), 1);

	glUniform1i(glGetUniformLocation(RobotShader->GetProgram(), "diffuseTex"), 0); 
	Vector3 heightmapSize = heightMap->GetHeightmapSize(); 
	//modelMatrix = Matrix4::Translation(heightmapSize*(Vector3{ walking, 0.11f, walking })) * Matrix4::Scale(Vector3(750, 750, 750)) * Matrix4::Rotation(walkingRotate, Vector3(0,0,1)); //makes robot disappear!
	modelMatrix = Matrix4::Translation(heightmapSize * Vector3(0.63f, 0.11f, 0.53f)) * Matrix4::Scale(Vector3(750, 750, 750)) * Matrix4::Rotation(180, Vector3(0.0f, 1.0f, 0.0f)); 
	//textureMatrix = Matrix4::Translation(Vector3{ waterCycle, 0.0f, waterCycle }) * Matrix4::Scale(Vector3{ 10, 10, 10 }) * Matrix4::Rotation(waterRotate, Vector3{ 0,0,1 });

	UpdateShaderMatrices(); 

	vector<Matrix4> frameMatrices; 

	const Matrix4* invBindPose = RobotMesh->GetInverseBindPose();
	const Matrix4* frameData = anim->GetJointData(currentFrame);

	for (unsigned int i = 0; i < RobotMesh->GetJointCount(); ++i) { 
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]); 

		int j = glGetUniformLocation(RobotShader->GetProgram(), "joints"); 
		glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data()); 

		for (int i = 0; i < RobotMesh->GetSubMeshCount(); ++i) { 
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, RobotmatTextures[i]); 
			RobotMesh->DrawSubMesh(i); 
		} 
	} 
} 

void Renderer::DrawWall() {
	BindShader(lightShader1);

	//SetShaderLight(*light1);
	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "lightPos"), 1, (float*) & (*light1).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader1->GetProgram(), "lightColour"), 1, (float*) & (*light1).GetColour());
	glUniform1f(glGetUniformLocation(lightShader1->GetProgram(), "lightRadius"), (*light1).GetRadius());

	/*glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "lightPos2"), 1, (float*) & (*light2).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader2->GetProgram(), "lightColour2"), 1, (float*) & (*light2).GetColour());
	glUniform1f(glGetUniformLocation(lightShader2->GetProgram(), "lightRadius2"), (*light2).GetRadius());*/

	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	/*glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());*/

	glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "diffuseTex"), 0);
	//glUniform1i(glGetUniformLocation(lightShader2->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "bumpTex"), 1);
	//glUniform1i(glGetUniformLocation(lightShader2->GetProgram(), "bumpTex"), 1);

	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	modelMatrix = Matrix4::Translation(Vector3(1150.0, 1.0, 0)) * Matrix4::Scale(Vector3(50, 50, 50)) * Matrix4::Rotation(90, Vector3(0.0f, 0.5f, 0.0f));

	UpdateShaderMatrices();

	vector<Matrix4> frameMatrices;

	const Matrix4* invBindPose = WallMesh->GetInverseBindPose();

	for (int i = 0; i < WallMesh->GetSubMeshCount(); ++i) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, WallmatTextures[i]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, wallBump);
		WallMesh->DrawSubMesh(i);
	}
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(skyboxShader);
	UpdateShaderMatrices();

	quad->Draw();

	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
	BindShader(lightShader1);

	//light 1:
	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "lightPos"), 1, (float*)& (*light1).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader1->GetProgram(), "lightColour"), 1, (float*)& (*light1).GetColour());
	glUniform1f(glGetUniformLocation(lightShader1->GetProgram(), "lightRadius"), (*light1).GetRadius());

	//light 2:
	glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "lightPos2"), 1, (float*)& (*light2).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader2->GetProgram(), "lightColour2"), 1, (float*)& (*light2).GetColour());
	glUniform1f(glGetUniformLocation(lightShader2->GetProgram(), "lightRadius2"), (*light2).GetRadius());

	//light 3:
	glUniform3fv(glGetUniformLocation(lightShader3->GetProgram(), "lightPos3"), 1, (float*) & (*light3).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader3->GetProgram(), "lightColour3"), 1, (float*) & (*light3).GetColour());
	glUniform1f(glGetUniformLocation(lightShader3->GetProgram(), "lightRadius3"), (*light3).GetRadius());

	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform3fv(glGetUniformLocation(lightShader3->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(lightShader2->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(lightShader3->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(lightShader2->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(lightShader3->GetProgram(), "bumpTex"), 1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, earthBump);

	glUniform1i(glGetUniformLocation(lightShader1->GetProgram(), "diffuseTex2"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, rockTex);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();

	UpdateShaderMatrices();

	heightMap->Draw();
}

void Renderer::DrawWater() {
	BindShader(reflectShader);
	//BindShader(lightShader1);

	glUniform3fv(glGetUniformLocation(reflectShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(reflectShader->GetProgram(), "cubeTex"), 2);

	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "lightPos"), 1, (float*) & (*light1).GetPosition());
	glUniform4fv(glGetUniformLocation(lightShader1->GetProgram(), "lightColour"), 1, (float*) & (*light1).GetColour());
	glUniform1f(glGetUniformLocation(lightShader1->GetProgram(), "lightRadius"), (*light1).GetRadius());

	glUniform3fv(glGetUniformLocation(lightShader1->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform3fv(glGetUniformLocation(lightShader2->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());
	glUniform3fv(glGetUniformLocation(lightShader3->GetProgram(), "cameraPos"), 1, (float*)& camera->GetPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, waterTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, waterBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	Vector3 hSize = heightMap->GetHeightmapSize();

	modelMatrix = Matrix4::Translation(hSize * 0.13f) * Matrix4::Scale(hSize * 0.13f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3{ waterCycle, 0.0f, waterCycle }) * Matrix4::Scale(Vector3{ 10, 10, 10 }) * Matrix4::Rotation(waterRotate, Vector3{ 0,0,1 });

	UpdateShaderMatrices();
	//SetShaderLight(*light1); // no lighting in this shader!
	quad->Draw();
}
