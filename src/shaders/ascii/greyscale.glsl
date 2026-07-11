/* greyscale.glsl */


#ifdef PERCEPTUAL_LUMINANCE
//https://stackoverflow.com/a/56678483
float sRGBtoLin(float colourChannel) {
	//Send this function a decimal sRGB gamma encoded color value
	//between 0.0 and 1.0, and it returns a linearized value.

	if (colourChannel <= 0.04045f) {
		return colourChannel / 12.92f;
	} else {
		return pow(
			(colourChannel + 0.055f) / 1.055f,
			2.4f
		);
	}
}

float getLuminance(vec3 colour) {
	return (
		0.2126f * sRGBtoLin(colour.r) +
		0.7152f * sRGBtoLin(colour.g) +
		0.0722f * sRGBtoLin(colour.b)
	);
}

#else

float getLuminance(vec3 colour) {
	return (colour.r + colour.g + colour.b) / 3.0f;
}

#endif
