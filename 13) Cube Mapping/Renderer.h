#pragma once
#include "../nclgl/OGLRenderer.h"
#include "..//nclgl/SceneNode.h" // new for scene node
#include "..//nclgl/Frustum.h"  // new for scene node
class Camera;
class Shader;
class HeightMap;
class Light;
class Mesh; 
class MeshAnimation; 
class MeshMaterial; 
class SceneNode; // new for scene node

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene() override;
	void UpdateScene(float dt) override;
	int togCam;
	int startTime;

protected:
	void			DrawHeightmap();
	void			DrawWater();
	void			DrawSkybox();
	void			DrawRobot();
	void			DrawWall();

	Shader*			lightShader1; //1st sun (blue)
	Shader*			lightShader2; //2nd sun (red)
	Shader*			lightShader3; //campfire

	Shader*			reflectShader;
	Shader*			skyboxShader;
	Shader*			RobotShader; 
	Shader*			WallShader;
	Shader*			GrassShader;

	HeightMap*		heightMap;
	Mesh*			quad;
	Mesh*			RobotMesh; 
	Mesh*			WallMesh;
	MeshAnimation*	anim; 
	MeshMaterial*	RobotMaterial; 
	MeshMaterial*	WallMaterial;
	vector<GLuint>	RobotmatTextures; 
	vector<GLuint>	WallmatTextures;

	void			BuildNodeLists(SceneNode* from); // new for scene node
	void			SortNodeLists(); // new for scene node
	void			ClearNodeLists(); // new for scene node
	void			DrawNodes(); // new for scene node
	void			DrawNode(SceneNode* n); // new for scene node
	SceneNode*		root; // new for scene node
	GLuint			texture; // new for scene node

	Frustum				frameFrustum; // new for scene node

	vector<SceneNode*> transparentNodeList; // new for scene node
	vector<SceneNode*> nodeList; // new for scene node

	int				currentFrame; 
	float			frameTime;

	Light*			light1;
	Light*			light2;
	Light*			light3;
	Camera*			camera;

	GLuint			cubeMap;
	GLuint			waterTex;
	GLuint			waterBump;
	GLuint			earthTex;
	GLuint			earthBump;
	GLuint			wallBump;
	GLuint			rockTex;
	GLuint			snowTex;

	float			waterRotate;
	float			waterCycle;
	float			walking;
	float			walkingRotate;
};