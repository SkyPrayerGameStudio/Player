#include "bitmap.h"
#include "SDL_Image.h"

Bitmap::Bitmap(int width, int height)
{
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, SCREEN_BPP, rmask, gmask, bmask, amask);
	disposed = false;
	
	id = count;
	add_bitmap(id, this);
	count++;
}

Bitmap::Bitmap(std::string filename)
{
	SDL_Surface* temp = NULL;
	temp = IMG_Load(filename.c_str());
	if(temp == NULL) {
		// Error
		SDL_FreeSurface(temp);
	}
	surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	disposed = false;
}

Bitmap::~Bitmap()
{
	remove_bitmap(id);
}

std::map<int, Bitmap*> Bitmap::bitmaps;
int Bitmap::count = 0;

void Bitmap::dispose()
{
	if(!disposed) {
		SDL_FreeSurface(surface);
		disposed = true;
	}
}

bool Bitmap::is_disposed()
{
	return disposed;
}

int Bitmap::width()
{
	return surface->w;
}

int Bitmap::height()
{
	return surface->h;
}

Rect* Bitmap::rect()
{
	return new Rect(0, 0, surface->w, surface->h);
}

void Bitmap::blt(int x, int y, Bitmap *src_bitmap, Rect *src_rect)
{
	
}

void Bitmap::blt(int x, int y, Bitmap *src_bitmap, Rect *src_rect, int opacity)
{
	
}

void Bitmap::stretch_blt(Rect *dest_rect, Bitmap *src_bitmap, Rect *src_rect)
{
	
}

void Bitmap::stretch_blt(Rect *dest_rect, Bitmap *src_bitmap, Rect *src_rect, int opacity)
{
	
}

void Bitmap::fill_rect(int x, int y, int width, int height, Color *color)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	SDL_FillRect(surface, &rect, color->get_uint32());
}

void Bitmap::fill_rect(Rect *rect, Color *color)
{
	//SDL_FillRect(surface, &rect->get_sdlrect(), (*color).get_uint32());
}

void Bitmap::clear()
{
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = surface->w;
	rect.h = surface->h;
	SDL_FillRect(surface, &rect, 0);
}

Color Bitmap::get_pixel(int x, int y)
{
	Uint8* pixel = (Uint8*)surface->pixels + (x + y * surface->w) * 4;
	Color color;
	//Uint8 r, g, b, a;
	//SDL_GetRGBA(pixel[x + y * surface->w], Graphics::get_pixelformat(), &r, &g, &b, &a);
	color.set_r(pixel[0]);
	color.set_g(pixel[1]);
	color.set_b(pixel[2]);
	color.set_a(pixel[3]);
	return color;
}

void Bitmap::set_pixel(int x, int y, Color* color)
{
	Uint32* pixel = reinterpret_cast<Uint32*>(surface->pixels) + (x + y * surface->w) * 4;
	pixel[0] = (*color).get_uint32();
}

void Bitmap::hue_change(int hue){
	
}

void Bitmap::draw_text(int x, int y, int width, int height, std::string str)
{
	
}

void Bitmap::draw_text(int x, int y, int width, int height, std::string str, int align)
{
	
}

void Bitmap::draw_text(Rect* rect, std::string str)
{
	
}

void Bitmap::draw_text(Rect* rect, std::string str, int align)
{
	
}

int Bitmap::text_size(std::string str)
{
	return 0;
}

Font* Bitmap::get_font()
{
	return font;
}

void Bitmap::set_font(Font* nfont)
{
	font = nfont;
}

void Bitmap::add_bitmap(int id, Bitmap* bitmap)
{
	bitmaps[id] = bitmap;
}

void Bitmap::remove_bitmap(int id)
{
	bitmaps.erase(id);
}
