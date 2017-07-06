// SpotLight.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ncl/gl/GlfwApp.h>
#include "SpotLightScene.h"

using namespace std;
using namespace ncl;
using namespace gl;

static ConsoleAppender CONSOLE_APPENDER;
static FileAppender FILE_APPENDER("log.txt");


vector<Appender*> Logger::appenders = { &CONSOLE_APPENDER,  &FILE_APPENDER };
const unsigned int Logger::level = LEVEL_DEBUG;


int main()
{
	Scene* spotLightScene = new SpotLightScene();
	start(spotLightScene);
    return 0;
}

