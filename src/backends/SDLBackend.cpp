/*
 *  SDLBackend.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Oct 24 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "SDLBackend.h"
#include "openc2e.h"
#include "Engine.h"
#include "creaturesImage.h"

#if defined(__has_include)
#	if __has_include("SDL2/SDL2_gfxPrimitives.h")
#		include "SDL2/SDL2_gfxPrimitives.h"
#	else
#		include "SDL2_gfxPrimitives.h"
#	endif

#	if __has_include("SDL2/SDL_ttf.h")
#		include "SDL2/SDL_ttf.h"
#	else
#		include "SDL_ttf.h"
#	endif
#else
#	include "SDL_ttf.h"
#	include "SDL2_gfxPrimitives.h"
#endif

SDLBackend *g_backend;

SDLBackend::SDLBackend() : mainsurface(this) {

	//std::cout << "SDL up and running" << std::endl;
	
	networkingup = false;
	basicfont = 0;

	// reasonable defaults
	mainsurface.width = 800;
	mainsurface.height = 600;
	mainsurface.texture = 0;
}

int SDLBackend::idealBpp() {
	// shadow surfaces seem to generally be faster (presumably due to overdraw), so get SDL to create one for us
	if (engine.version == 1) return 0;
	else return 16;
}

void SDLBackend::resizeNotify(int _w, int _h) 
{
	if (mainsurface.texture != nullptr) {
		SDL_DestroyTexture(mainsurface.texture);
	}

	mainsurface.width = _w;
	mainsurface.height = _h;
	mainsurface.texture = SDL_CreateTexture(
		mainsurface.renderer,
		SDL_BITSPERPIXEL(idealBpp()),
		SDL_TEXTUREACCESS_STATIC,
		_w, _h
	);

	if (!mainsurface.texture)
		throw creaturesException(std::string("Failed to create SDL texture due to: ") + SDL_GetError());
}

void SDLBackend::init() 
{
	int init = SDL_INIT_VIDEO;

	if (SDL_Init(init) < 0)
		throw creaturesException(std::string("SDL error during initialization: ") + SDL_GetError());

	mainsurface.window = SDL_CreateWindowFrom(handle);
	if (mainsurface.window == nullptr) {
		std::cerr << "Unable to create a window: " << SDL_GetError() << std::endl;
		abort();
	}

	std::string windowtitle;
	if (engine.getGameName().size()) windowtitle = engine.getGameName() + " - ";
	windowtitle += "openc2e";
	std::string titlebar = windowtitle + " (development build)";
	SDL_SetWindowTitle(mainsurface.window, titlebar.c_str());

	mainsurface.renderer = SDL_CreateRenderer(mainsurface.window, -1, 0);

	// TODO SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL) (DEPRECATED);
	// SDL_ShowCursor(false);
	if (TTF_Init() == 0) {
		// TODO: think about font sizing
		basicfont = TTF_OpenFont("VeraSe.ttf", 9);
		if (!basicfont) // TODO: think about font fallbacks/etc
		#ifdef __APPLE__
			basicfont = TTF_OpenFont("/Library/Fonts/Arial.ttf", 9);
		#else
			basicfont = TTF_OpenFont("/usr/share/fonts/truetype/ttf-bitstream-vera/VeraSe.ttf", 9);
		#endif
	}
}

int SDLBackend::networkInit() {
	if (SDLNet_Init() < 0)
		throw creaturesException(std::string("SDL_net error during initialization: ") + SDLNet_GetError());
	networkingup = true;

	listensocket = 0;
	int listenport = 20000;
	while ((!listensocket) && (listenport < 20050)) {
		listenport++;
		IPaddress ip;

		SDLNet_ResolveHost(&ip, 0, listenport);
		listensocket = SDLNet_TCP_Open(&ip);
	}
	
	if (!listensocket)
		throw creaturesException(std::string("Failed to open a port to listen on."));

	return listenport;
}

void SDLBackend::shutdown() {
	if (TTF_WasInit()) {
		if (basicfont) {
			TTF_CloseFont(basicfont);
		}
		TTF_Quit();
	}
	if (networkingup && listensocket)
		SDLNet_TCP_Close(listensocket);
	SDLNet_Quit();
	SDL_Quit();
}

void SDLBackend::handleEvents() {
	if (networkingup)
		handleNetworking();
}

void SDLBackend::handleNetworking() {
	// handle incoming network connections
	while (TCPsocket connection = SDLNet_TCP_Accept(listensocket)) {
		// check this connection is coming from localhost
		IPaddress *remote_ip = SDLNet_TCP_GetPeerAddress(connection);
		unsigned char *rip = (unsigned char *)&remote_ip->host;
		if ((rip[0] != 127) || (rip[1] != 0) || (rip[2] != 0) || (rip[3] != 1)) {
			std::cout << "Someone tried connecting via non-localhost address! IP: " << (int)rip[0] << "." << (int)rip[1] << "." << (int)rip[2] << "." << (int)rip[3] << std::endl;
			SDLNet_TCP_Close(connection);
			continue;
		}
			
		// read the data from the socket
		std::string data;
		bool done = false;
		while (!done) {
			char buffer;
			int i = SDLNet_TCP_Recv(connection, &buffer, 1);
			if (i == 1) {
				data = data + buffer;
				// TODO: maybe we should check for rscr\n like c2e seems to
				if ((data.size() > 3) && (data.find("rscr\n", data.size() - 5) != data.npos)) done = true;
			} else done = true;
		}

		// pass the data onto the engine, and send back our response
		std::string tosend = engine.executeNetwork(data);
		SDLNet_TCP_Send(connection, (void *)tosend.c_str(), tosend.size());
		
		// and finally, close the connection
		SDLNet_TCP_Close(connection);
	}
}

bool SDLBackend::pollEvent(SomeEvent &e) {
	SDL_Event event;
retry:
	if (!SDL_PollEvent(&event)) return false;

	switch (event.type) {
		case SDL_WINDOWEVENT_RESIZED:
			resizeNotify(event.window.data1, event.window.data2);
			e.type = eventresizewindow;
			e.x = event.window.data1;
			e.y = event.window.data2;
			break;

		case SDL_MOUSEMOTION:
			e.type = eventmousemove;
			e.x = event.motion.x;
			e.y = event.motion.y;
			e.xrel = event.motion.xrel;
			e.yrel = event.motion.yrel;
      e.button = 0;
      if (event.motion.state & SDL_BUTTON(1))
        e.button |= buttonleft;
      if (event.motion.state & SDL_BUTTON(2))
        e.button |= buttonmiddle;
      if (event.motion.state & SDL_BUTTON(3))
        e.button |= buttonright;
      if (event.motion.state & SDL_BUTTON(4))
        e.button |= buttonwheelup;
      if (event.motion.state & SDL_BUTTON(5))
        e.button |= buttonwheeldown;
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (event.type == SDL_MOUSEBUTTONDOWN)
				e.type = eventmousebuttondown;
			else
				e.type = eventmousebuttonup;
			switch (event.button.button) {
				case SDL_BUTTON_LEFT: e.button = buttonleft; break;
				case SDL_BUTTON_RIGHT: e.button = buttonright; break;
				case SDL_BUTTON_MIDDLE: e.button = buttonmiddle; break;
				/*case SDL_MOUSEWHEEL: e.button = buttonwheeldown; break;
				case SDL_BUTTON_WHEELUP: e.button = buttonwheelup; break;*/
				default: goto retry;
			}
			e.x = event.button.x;
			e.y = event.button.y;
			break;

        case SDL_KEYUP:
            {
                int key = translateKey(event.key.keysym.sym);
                if (key != -1) {
                    e.type = eventspecialkeyup;
                    e.key = key;
                    return true;
                }
                goto retry;
            }

		case SDL_KEYDOWN:
			if (event.key.keysym.sym >= 32 && event.key.keysym.sym < 128)
			{
				e.type = eventkeydown;
				e.key = event.key.keysym.sym;
				return true;
			} 
			else 
			{ // TODO: should this be 'else'?
				int key = translateKey(event.key.keysym.sym);
				if (key != -1) {
					e.type = eventspecialkeydown;
					e.key = key;
					return true;
				}
			}
			goto retry;
            break;

		case SDL_QUIT:
			e.type = eventquit;
			break;

		default:
			goto retry;
	}
	
	return true;
}

void SDLSurface::renderLine(int x1, int y1, int x2, int y2, unsigned int colour) {
	aalineColor(renderer, x1, y1, x2, y2, colour);
}

SDL_Color getColourFromRGBA(unsigned int c) {
	// SDL's functions seem to want a pixelformat, which is more effort to fake than just doing this
	SDL_Color sdlc;
	sdlc.b = c & 0xff;
	sdlc.g = (c >> 8) & 0xff;
	sdlc.r = (c >> 16) & 0xff;
	assert(c >> 24 == 0);
	//sdlc = NULL; // T_T "may be used uninitialized in this function"
	return sdlc;
}

void SDLSurface::renderText(int x, int y, std::string text, unsigned int colour, unsigned int bgcolour) {
	if (!parent->basicfont) return;
	if (text.empty()) return;

	// TODO: Implement later
	// SDL_Color sdlcolour;
	// if (engine.version == 1) sdlcolour = palette[colour];
	// else sdlcolour = getColourFromRGBA(colour);
	
	// SDL_Surface *textsurf;

	// if (bgcolour == 0) { // transparent
	// 	textsurf = TTF_RenderText_Solid(parent->basicfont, text.c_str(), sdlcolour);
	// } else {
	// 	SDL_Color sdlbgcolour;
	// 	if (engine.version == 1) sdlbgcolour = palette[bgcolour];
	// 	else sdlbgcolour = getColourFromRGBA(bgcolour);
	// 	textsurf = TTF_RenderText_Shaded(parent->basicfont, text.c_str(), sdlcolour, sdlbgcolour);
	// }

	// if (!textsurf) return; // thanks, SDL_ttf, we love you too

	// SDL_Rect destrect;
	// destrect.x = x; destrect.y = y;	
	// SDL_BlitSurface(textsurf, NULL, surface, &destrect);
	// SDL_FreeSurface(textsurf);
}

SDL_Window * SDLSurface::getQTWindow(SDL_Window * QTwindow)
{
	if (QTwindow != nullptr)
	{
		window = QTwindow;
		return window;
	}
	else
	{
		return nullptr;
	}
}

void SDLSurface::render(shared_ptr<creaturesImage> image, unsigned int frame, int x, int y, bool trans, unsigned char transparency, bool mirror, bool is_background) {
	assert(image);
	assert(image->numframes() > frame);

	// This is a freaking slow function...
	// TODO: Optimize

	// don't bother rendering off-screen stuff
	if (x >= (int)width) return; if (y >= (int)height) return;
	if ((x + image->width(frame)) <= 0) return;
	if ((y + image->height(frame)) <= 0) return;

	// create surface
	SDL_Texture *tex;
	SDL_Color *surfpalette = 0;

	switch (image->format()) {
		case if_paletted: {
			tex = SDL_CreateTexture(
				renderer,
				SDL_BITSPERPIXEL(8),
				SDL_TEXTUREACCESS_STATIC,
				image->width(frame), image->height(frame)
			);
			SDL_UpdateTexture(tex, nullptr, image->data(frame), image->width(frame));
		} break;
		case if_16bit: {
			tex = SDL_CreateTexture(
				renderer,
				SDL_BITSPERPIXEL(16),
				SDL_TEXTUREACCESS_STATIC,
				image->width(frame), image->height(frame)
			);
			SDL_UpdateTexture(tex, nullptr, image->data(frame), image->width(frame) * 2);
		} break;
		default: {
			tex = SDL_CreateTexture(
				renderer,
				SDL_BITSPERPIXEL(24),
				SDL_TEXTUREACCESS_STATIC,
				image->width(frame), image->height(frame)
			);
			SDL_UpdateTexture(tex, nullptr, image->data(frame), image->width(frame) * 3);
		} break;
	}
	
	// set colour-keying/alpha
	// TODO: if (!is_background) SDL_SetColorKey(surf, SDL_TRUE, 0);

	if (trans) SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	
	// do actual blit
	SDL_Rect destrect;
	destrect.x = x; destrect.y = y;

	SDL_RenderCopyEx(renderer, tex, nullptr, &destrect, 0.0, nullptr, (SDL_RendererFlip) (mirror ? SDL_FLIP_HORIZONTAL : 0));

	// free surface
	SDL_DestroyTexture(tex);
}

void SDLSurface::renderDone() 
{
	SDL_RenderPresent(renderer);
}

void SDLSurface::blitSurface(Surface *s, int x, int y, int w, int h) {
	SDLSurface *src = dynamic_cast<SDLSurface*>(s);
	assert(src);

	SDL_Rect r; r.x = x; r.y = y; r.w = w; r.h = h;
	SDL_RenderCopy(renderer, src->texture, nullptr, &r);
}

Surface *SDLBackend::newSurface(unsigned int w, unsigned int h) {
	SDL_Texture *surf = mainsurface.texture;
	SDL_Texture* underlyingsurf = SDL_CreateTexture(
		mainsurface.renderer,
		SDL_BITSPERPIXEL(idealBpp()),
		SDL_TEXTUREACCESS_STATIC,
		w, h
	);
	assert(underlyingsurf);

	SDLSurface *newsurf = new SDLSurface(this);
	newsurf->texture = underlyingsurf;
	newsurf->width = w;
	newsurf->height = h;
	return newsurf;
}

void SDLBackend::freeSurface(Surface *s) {
	SDLSurface *surf = dynamic_cast<SDLSurface *>(s);
	assert(surf);

	SDL_DestroyTexture(surf->texture);
	delete surf;
}

// left out: menu, select, execute, snapshot, numeric keypad, f keys
#define keytrans_size 25
struct _keytrans { int sdl, windows; } keytrans[keytrans_size] = 
{
	{ SDLK_BACKSPACE, 8 },
	{ SDLK_TAB, 9 },
	{ SDLK_CLEAR, 12 },
	{ SDLK_RETURN, 13 },
	{ SDLK_RSHIFT, 16 },
	{ SDLK_LSHIFT, 16 },
	{ SDLK_RCTRL, 17 },
	{ SDLK_LCTRL, 17 },
	{ SDLK_PAUSE, 19 },
	{ SDLK_CAPSLOCK, 20 },
	{ SDLK_ESCAPE, 27 },
	{ SDLK_SPACE, 32 },
	{ SDLK_PAGEUP, 33 },
	{ SDLK_PAGEDOWN, 34 },
	{ SDLK_END, 35 },
	{ SDLK_HOME, 36 },
	{ SDLK_LEFT, 37 },
	{ SDLK_UP, 38 },
	{ SDLK_RIGHT, 39 },
	{ SDLK_DOWN, 40 },
	{ SDLK_PRINTSCREEN, 42},
	{ SDLK_INSERT, 45 },
	{ SDLK_DELETE, 46 },
	{ SDLK_NUMLOCKCLEAR, 144}
};

// TODO: handle f keys (112-123 under windows, SDLK_F1 = 282 under sdl)
 
// TODO: this is possibly not a great idea, we should maybe maintain our own state table
bool SDLBackend::keyDown(int key) 
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	
	for (unsigned int i = 0; i < keytrans_size; i++) 
	{
		if (keytrans[i].windows == key)
			if (keystate[keytrans[i].sdl])
				return true;
	}

	return false;
}

int SDLBackend::translateKey(int key) {
	if (key >= 97 && key <= 122) { // lowercase letters
		return key - 32; // capitalise
	}
	if (key >= 48 && key <= 57) { // numbers
		return key;
	}

	for (unsigned int i = 0; i < keytrans_size; i++) {
		if (keytrans[i].sdl == key)
			return keytrans[i].windows;
	}

	return -1;
}

void SDLBackend::setPalette(uint8 *data) {
	// TODO: we only set the palette on our main surface, so will fail for any C1 cameras!
	for (unsigned int i = 0; i < 256; i++) {
		mainsurface.palette[i].r = data[i * 3];
		mainsurface.palette[i].g = data[(i * 3) + 1];
		mainsurface.palette[i].b = data[(i * 3) + 2];
	}
}

void SDLBackend::delay(int msec) {
	SDL_Delay(msec);
}

unsigned int SDLBackend::textWidth(std::string text) {
	if (!basicfont) return 0;
	if (text.size() == 0) return 0;

	int w, h;

	if (TTF_SizeText(basicfont, text.c_str(), &w, &h))
		return 0; // error
	else
		return w;
}

