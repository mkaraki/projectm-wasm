#include <stdio.h>

#include <emscripten.h>
#include <SDL.h>
#include <GLES3/gl3.h>

#include <projectM-4/projectM.h>

projectm_handle projectMHandle = NULL;
SDL_GLContext glContext = nullptr;
SDL_Window *window = nullptr;

const int AUDIO_BUFFER_SIZE = 2048;
float audio_buffer[AUDIO_BUFFER_SIZE];

extern "C" {
	EMSCRIPTEN_KEEPALIVE
	float* get_audio_buffer() {
		return audio_buffer;
	}

	EMSCRIPTEN_KEEPALIVE
	void process_audio_buffer(int length) {
		if (projectMHandle == NULL) {
			return;
		}

		projectm_pcm_add_float(projectMHandle, audio_buffer, length, PROJECTM_MONO);
	}

	EMSCRIPTEN_KEEPALIVE
	void on_resize(int width, int height) {
		if (window)
			SDL_SetWindowSize(window, width, height);
		if (projectMHandle)
			projectm_set_window_size(projectMHandle, width, height);
	}
}

void destroy() {
	/* In your shutdown code: */
	projectm_destroy(projectMHandle);
	projectMHandle = NULL;

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int setup() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow(
			"ProjectM Window", 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			640, 480,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
			);

	if (!window) {
		printf("Failed to create SDL Window\n");
		return 1;
	}

	glContext = SDL_GL_CreateContext(window);
	if (!glContext) {
		printf("Failed to create GL Context\n");
		return 1;
	}
	int res = SDL_GL_MakeCurrent(window, glContext);
	if (res < 0) {
		printf("Failed to mark context current: %s\n", SDL_GetError());
		return 1;
	}

	/* In your setup code: */

	/* Create a projectM instance with default settings */
	projectMHandle = projectm_create();
	if (!projectMHandle)
	{
		/* Something went wrong, most probably OpenGL isn't configured. */
		printf("Failed to initialize ProjectM\n");
		return 1;
	}
	else
	{
		printf("ProjectM initialized\n");
	}

	projectm_set_window_size(projectMHandle, 640, 480);

	return 0;
}

void main_loop() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			emscripten_cancel_main_loop();
			return;
		}
	}

	projectm_opengl_render_frame(projectMHandle);
	SDL_GL_SwapWindow(window);
}

int main() {
	int res = setup();
	if (res != 0) {
		printf("Failed to execute setup() program.\n");
		return res;
	}
	emscripten_set_main_loop(main_loop, 0, 1);
	destroy();

	return 0;
}
