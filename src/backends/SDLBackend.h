/*
 *  SDLBackend.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Oct 24 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#ifndef _SDLBACKEND_H
#define _SDLBACKEND_H

#if defined(__has_include)
#	if __has_include("SDL2/SDL.h")
#		include "SDL2/SDL.h"
#	else
#		include "SDL.h"
#	endif

#	if __has_include("SDL2/SDL_net.h")
#		include "SDL2/SDL_net.h"
#	else
#		include "SDL_net.h"
#	endif

#else
#	include "SDL.h"
#	include <SDL_net.h>
#endif


#include "Backend.h"

class SDLSurface : public Surface
{
	friend class SDLBackend;

protected:
	class SDLBackend *parent;
	SDL_Texture *texture = nullptr;
	SDL_Renderer *renderer = nullptr;
	SDL_Window *window = nullptr;
	unsigned int width, height;
	SDL_Color palette[256];
	
	SDLSurface(SDLBackend *p) { parent = p; }

public:
	SDL_Window* getQTWindow(SDL_Window* QTwindow);
	void render(shared_ptr<creaturesImage> image, unsigned int frame, int x, int y, bool trans = false, unsigned char transparency = 0, bool mirror = false, bool is_background = false);
	void renderLine(int x1, int y1, int x2, int y2, unsigned int colour);
	void renderText(int x, int y, std::string text, unsigned int colour, unsigned int bgcolour);
	void blitSurface(Surface *src, int x, int y, int w, int h);
	unsigned int getWidth() const { return width; }
	unsigned int getHeight() const { return height; }
	void renderDone();

};

class SDLBackend : public Backend {
	friend class SDLSurface;

protected:
	bool networkingup;

	void* handle;
	SDLSurface mainsurface;
	TCPsocket listensocket;

	struct _TTF_Font *basicfont;

	void handleNetworking();
	void resizeNotify(int _w, int _h);
	int translateKey(int key);

	SDL_Texture *getMainSDLTexture() { return mainsurface.texture; }

	virtual int idealBpp();

public:
	SDLBackend();	
	void init();
	int networkInit();
	void shutdown();

	void resize(unsigned int w, unsigned int h) { resizeNotify(w, h); }

	bool pollEvent(SomeEvent &e);
	
	unsigned int ticks() { return SDL_GetTicks(); }
	
	void handleEvents();

	bool selfRender() { return false; }
	void requestRender() { }

	Surface *getMainSurface() { return &mainsurface; }
	Surface *newSurface(unsigned int width, unsigned int height);
	void freeSurface(Surface *surf);
	unsigned int textWidth(std::string text);
		
	bool keyDown(int key);
	
	void setPalette(uint8 *data);
	void delay(int msec);

	void setHandle(void* hnd) { handle = hnd; }
};

#endif
