#ifndef GFX_H_
#define GFX_H_

#include "font.h"

#define GFX_MAX_OBJECTS				30

//#define GFX_RGB565(r, g, b)			((((r & 0xF8) >> 3) << 11) | (((g & 0xFC) >> 2) << 5) | ((b & 0xF8) >> 3))
//#define GFX_RGB565_R(color)			((0xF800 & color) >> 11)
//#define GFX_RGB565_G(color)			((0x07E0 & color) >> 5)
//#define GFX_RGB565_B(color)			(0x001F & color)

//#define GFX_ARGB8888(r, g, b, a)	(((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF))
//#define GFX_ARGB8888_R(color)		((color & 0x00FF0000) >> 16)
//#define GFX_ARGB8888_G(color)		((color & 0x0000FF00) >> 8)
//#define GFX_ARGB8888_B(color)		((color & 0x000000FF))
//#define GFX_ARGB8888_A(color)		((color & 0xFF000000) >> 24)

#define GFX_RGB888(r, g, b)	    (((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF))
#define GFX_RGB888_R(color)		((color & 0x00FF0000) >> 16)
#define GFX_RGB888_G(color)		((color & 0x0000FF00) >> 8)
#define GFX_RGB888_B(color)		((color & 0x000000FF))

#define GFX_TRANSPARENT				0x00
#define GFX_OPAQUE					0xFF

typedef enum {
	GFX_PIXEL_FORMAT_RGB565,
	GFX_PIXEL_FORMAT_ARGB8888
} gfx_pixel_format_t;

typedef struct {
	uint16_t			width;
	uint16_t			height;
	gfx_pixel_format_t	pixel_format;
	uint8_t*			pixel_data;
} gfx_image_t;

typedef struct {
	uint16_t source_x;
	uint16_t source_y;
	uint16_t source_w;
	uint16_t source_h;
	uint16_t dest_x;
	uint16_t dest_y;
} gfx_coord_t;

typedef enum {
	GFX_OBJ_IMG,
	GFX_OBJ_TEXT // ToDo: useful?
} gfx_obj_type_t;

typedef struct {
	gfx_obj_type_t	obj_type;
	void*			data;
	gfx_coord_t		coords;
	bool			enabled;
} gfx_obj_t;

void gfx_init (void); 
bool gfx_add_object (gfx_obj_t* obj); 
bool gfx_delete_object (gfx_obj_t* obj); 
void gfx_delete_objects (void); 
void gfx_draw_objects (void); 
void gfx_refresh (void); 
void gfx_clear_screen (uint16_t color); 
void gfx_draw_img (gfx_image_t* img, gfx_coord_t* coord); 
bool gfx_load_img (const char* file_name, gfx_image_t* img); 
void gfx_unload_img (gfx_image_t* img); 
uint8_t gfx_draw_character (const char chr, gfx_image_t* img, uint16_t x0, uint16_t y0, const gfx_font_t* font, uint32_t color); 
void gfx_draw_character_centered (const char chr, gfx_image_t* img, const gfx_font_t* font, gfx_coord_t* coords, uint32_t color); 
void gfx_rotate (gfx_image_t* source, gfx_image_t* dest, float radians, uint32_t back_color); 
void gfx_flip90 (gfx_obj_t* obj); 
void gfx_copy_image (gfx_image_t* source, gfx_image_t* dest); 
void gfx_set_pixel (gfx_image_t* img, uint16_t x, uint16_t y, uint32_t color); 
uint32_t gfx_get_pixel (gfx_image_t* img, uint16_t x, uint16_t ); 
void gfx_draw_line (gfx_image_t* img, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color); 
void gfx_fill_rect (gfx_image_t* img, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color); 
void gfx_draw_img_on_img (gfx_image_t* img_src, gfx_image_t* img_dst, gfx_coord_t* coord); 
void gfx_fill_img (gfx_image_t* img, uint32_t color); 
void gfx_init_img (gfx_image_t* img, uint16_t width, uint16_t height, gfx_pixel_format_t pixel_format, uint32_t color); 
void gfx_init_img_obj (gfx_obj_t* obj, gfx_image_t* img); 
void gfx_init_img_coord (gfx_coord_t* coord, gfx_image_t* img);

extern gfx_obj_t* gfx_objects[GFX_MAX_OBJECTS];

#endif
