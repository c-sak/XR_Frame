#pragma once
//Å•êF
typedef struct color_t {
	float red, green, blue, alpha;
	color_t() :red(1), green(1), blue(1), alpha(1) {}
	color_t(float red_, float green_, float blue_) :red(red_), green(green_), blue(blue_), alpha(1) {}
	color_t(float red_, float green_, float blue_, float alpha_) :red(red_), green(green_), blue(blue_), alpha(alpha_) {}
} color_t;