#pragma once

#include <ncl/gl/Scene.h>
#include <sstream>
#include <iomanip>

using namespace ncl;
using namespace gl;
using namespace std;
using namespace glm;

class SpotLightScene : public Scene {
public:
	SpotLightScene() :Scene("Spot light test", 500, 500) {
		useImplictShaderLoad(true);
	}

	virtual void init() override {
		glClearColor(0, 0, 0, 0);
		glEnable(GL_DEPTH_TEST);

		shader.loadFromstring(GL_VERTEX_SHADER, pass_through_vert_shader);
		shader.loadFromstring(GL_FRAGMENT_SHADER, pass_through_frag_shader);
		shader.createAndLinkProgram();

		font = Font::Arial(12, 0, YELLOW);
		LightSource& light0 = light[0];
		light0.on = true;
		light0.position = { 0.0, 3.0, 0.0, 1.0 };
		light0.spotAngle = spotAngle;
		light0.spotDirection = { 0.0, -1.0, 0.0, 1.0 };
		light0.spotExponent = spotExponent;
		light0.diffuse = light0.specular = vec4(1);
		light0.transform = true;
		lightModel.globalAmbience = { 0.05, 0.05, 0.05, 1.0 };
		lightModel.localViewer = true;
		lightModel.colorMaterial = true;

		sphere = new Sphere(0.5, 20, 16, 81);
		sphere->material().diffuse = RED;
		sphere->material().specular = vec4(1);
		sphere->material().shininess = 50.0f;
		cam.view = lookAt({ 0.0, 4.0, 6.0 }, vec3{ 0 }, { 0.0, 1.0, 0.0 });


		sphere2 = new Sphere(0.2, 20, 20);

		addKeyListener([&](const Key& key) {
			if (Keyboard::get().W.pressed()) {
				if (zMove > -4.0) zMove -= 0.1;
			}
			else if (Keyboard::get().S.pressed()) {
				if (zMove < 4.0) zMove += 0.1;
			}
			if (Keyboard::get().A.pressed())
			{
				if (xMove > -4.0) xMove -= 0.1;
			}
			else if (Keyboard::get().D.pressed())
			{
				if (xMove < 4.0) xMove += 0.1;
			}
			if (Keyboard::get().UP.pressed()) {
				if (spotAngle < 90.0) spotAngle += 1.0;
			}
			else if (Keyboard::get().DOWN.pressed()) {
				if (spotAngle > 0.0) spotAngle -= 1.0;
			}
			if (Keyboard::get().T.pressed()) {
				if (Keyboard::get().T.isLowerCase) {
					if (spotExponent > 0.0) spotExponent -= 0.1;
				}
				else {
					spotExponent += 0.1;
				}
			}

		});

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				unsigned index = i * 9 + j;
				positions[index] = translate(mat4(1), { -4.0 + i, 0.0, -4.0 + j });
				if ((i + j) % 3 == 0) colors[index] = RED;
				else if ((i + j) % 3 == 1) colors[index] = GREEN;
				else colors[index] = BLUE;
			}
		}

		initColorBuffer();
		initMatrixBuffer();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

	}

	void initColorBuffer() {
		color_tbo = new TextureBuffer(colors, sizeof(colors));
		_shader.sendUniform1ui("color_tbo", color_tbo->id());
	}

	void initMatrixBuffer() {
		matrix_tbo = new TextureBuffer(positions, sizeof(positions));
		_shader.sendUniform1ui("matrix_tbo", matrix_tbo->id());

	}

	virtual void display() override {

		ostringstream sbr;
		sbr << "FPS: " << setprecision(3) << fps;
		font->render(sbr.str(), 10, _height - 10);
		sbr.str("");
		sbr.clear();
		sbr << "Spotlight attenuation: " << setprecision(3) << light[0].spotExponent;
		font->render(sbr.str(), 10, height() - 25);

		
		light[0].on = true;
		light[0].position.x = xMove;
		light[0].position.z = zMove;
		light[0].spotAngle = spotAngle;
		light[0].spotExponent = spotExponent;

		mat4& model = cam.model;

		static mat4 idMat = mat4(1);


		_shader.sendComputed(cam);
		_shader.sendUniformLight("light[0]", light[0]);
		sphere->draw(_shader);
		float h = length(light[0].position);
		float r = h * tan(radians(light[0].spotAngle));
		lightSource = new Cone(r, h, 10, 10, WHITE);


		cam.model = translate(mat4(1), vec3(light[0].position));
		cam.model = rotate(cam.model, pi<float>() / 2, { 1, 0, 0 });
		shader.use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		shader.send(cam);
		lightSource->draw(shader);
		//	sphere2->draw(shader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		shader.unUse();
		cam.model = mat4(1);
		cam.model = mat4(1);
		delete lightSource;
	}

	virtual void resized() override {
		font->resize(_width, _height);
		cam.projection = perspective(radians(60.0f), aspectRatio, 1.0f, 20.0f);
	}

private:
	Sphere* sphere;
	Cone* lightSource;
	Sphere* sphere2;
	Shader shader;
	mat4 positions[81];
	vec4 colors[81];
	TextureBuffer* color_tbo;
	TextureBuffer* matrix_tbo;
	GLuint color_buffer;
	GLuint matrix_buffer;
	Font* font;
	float xMove = 0;
	float zMove = 0;
	float spotAngle = 45.0f;
	float spotExponent = 2.0f;
};