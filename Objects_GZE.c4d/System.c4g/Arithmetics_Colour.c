#strict 2

global func GetHSLaValueFromRGBa(int rgba, int sel) {
	return GetRGBaValue(RGB2HSL(rgba), sel);
}

global func GetHFromRGBa(int rgba) {
	return GetHSLaValueFromRGBa(rgba, 1);
}
global func GetSFromRGBa(int rgba) {
	return GetHSLaValueFromRGBa(rgba, 2);
}
global func GetLFromRGBa(int rgba) {
	return GetHSLaValueFromRGBa(rgba, 3);
}




// select 0: a, select 1: H, select 2: S, select 3: L
global func SetHSLaValueFromRGBa(int rgba, int newval, int sel) {
	var hsla = RGB2HSL(rgba);
    hsla = SetRGBaValue(hsla, newval, sel);
	return HSL2RGB(hsla);
}

global func SetHFromRGBa(int rgba, int newval) {
	return SetHSLaValueFromRGBa(rgba, newval, 1);
}

global func SetSFromRGBa(int rgba, int newval) {
	return SetHSLaValueFromRGBa(rgba, newval, 2);
}

global func SetLFromRGBa(int rgba, int newval) {
	return SetHSLaValueFromRGBa(rgba, newval, 3);
}