#strict 2

global func ColorGetRed(int rgba) {
	return GetByte(rgba, 1);
}
global func ColorGetGreen(int rgba) {
	return GetByte(rgba, 2);
}
global func ColorGetBlue(int rgba) {
	return GetByte(rgba, 3);
}
global func ColorGetAlpha(int rgba) {
	return GetByte(rgba, 0);
}
global func ColorGetHue(int rgba) {
	var hsla = RGB2HSL(rgba);
	return GetByte(hsla, 1);
}
global func ColorGetSaturation(int rgba) {
	var hsla = RGB2HSL(rgba);
	return GetByte(hsla, 2);
}
global func ColorGetLightness(int rgba) {
	var hsla = RGB2HSL(rgba);
	return GetByte(hsla, 3);
}


global func ColorSetRed(int rgba, int red) {
	return SetByte(rgba, red, 1);
}
global func ColorSetGreen(int rgba, int green) {
	return SetByte(rgba, green, 2);
}
global func ColorSetBlue(int rgba, int blue) {
	return SetByte(rgba, blue, 3);
}
global func ColorSetAlpha(int rgba, int alpha) {
	return SetByte(rgba, alpha, 0);
}
global func ColorSetHue(int rgba, int hue) {
	var hsla = RGB2HSL(rgba);
	hsla = SetByte(hsla, hue, 1);
	
	return HSL2RGB(hsla);
}
global func ColorSetSaturation(int rgba, int saturation) {
	var hsla = RGB2HSL(rgba);
	hsla = SetByte(hsla, saturation, 2);
	return HSL2RGB(hsla);
}
global func ColorSetLightness(int rgba, int lightness) {
	var hsla = RGB2HSL(rgba);
	//Log("  %v  vs   %v", IntToBytes(rgba), IntToBytes(SetByte(hsla, lightness, 3)));
	Log("ColorSetLightness: %v  vs   %v", IntToBytes(hsla), IntToBytes(HSL2RGB(hsla)));
	hsla = SetByte(hsla, lightness, 3);
	
	return HSL2RGB(hsla);
}


/* Functions from System.c4g, but fixed 
 * - RGB2HSL and HSL2RGB discarding alpha channel
 * - clean code style
 */

global func HSL2RGB(hsl) {
	var alpha = GetByte(hsl, 0), hue = GetByte(hsl,1), sat = GetByte(hsl,2), lightness=GetByte(hsl,3);
	var red, green, blue;
	var var1, var2;
	
	if (sat == 0) {
		red = green = blue = lightness;
	} else {
		if (lightness<128) {
			var2 = (lightness*(255 + sat))/255;
		} else {
			var2 = lightness+sat-lightness*sat/255;
		}

		var1 = 2 * lightness-var2;
			
		red = Hue_2_RGB(var1, var2, hue+85);
		green = Hue_2_RGB(var1, var2, hue);
		blue = Hue_2_RGB(var1, var2, hue-85);
	}
  
	return RGBa(red, green, blue, alpha);
}


global func RGB2HSL(rgb) {
	var alpha = GetByte(rgb, 0), red = GetByte(rgb,1), green = GetByte(rgb,2), blue = GetByte(rgb,3);
	var min_val = Min(red, Min(green, blue)), max_val = Max(red, Max(green, blue));
	var diff_val = max_val - min_val;
	var lightness = (max_val + min_val)/2;
	var hue, sat, diff_red, diff_green, diff_blue;

	
	if (diff_val==0) {
		hue=0;
		sat=0;
	} else {
		
		if(lightness<128) {
			sat = (255*diff_val)/(max_val+min_val);
		} else {
			sat = (255*diff_val)/(510-(max_val+min_val));
		}

		diff_red  = ((255*(max_val-red  ))/6 + (255*diff_val)/2)/diff_val;
		diff_green= ((255*(max_val-green))/6 + (255*diff_val)/2)/diff_val;
		diff_blue = ((255*(max_val-blue ))/6 + (255*diff_val)/2)/diff_val;
			
		if	 (red ==max_val) hue = diff_blue-diff_green;
		else if (green == max_val) hue = 255/3+diff_red-diff_blue;
		else if (blue == max_val) hue = 510/3+diff_green-diff_red;
		
		if (hue<0)  hue+=255;
		if (hue>255) hue-=255;
	}
	
	return RGBa(hue, sat, lightness, alpha);
}
