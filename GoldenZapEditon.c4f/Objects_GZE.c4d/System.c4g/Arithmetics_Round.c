#strict

/* A C4Script Equivalent for rounding numbers. 
 *
 * Parameters:
 * - val: Value to be rounded, will be divided by Pow(10, prec), default is division by 10
 * - prec: Precision - how many decimals you want to round
 *
 * Examples:
 * Round(641, 2) - will return 6
 * Round(661, 2) - will return 7
 * Round(71) - will return 7
 * Round(6444447, 6) - will return 7
 */
global func Round(int val, int prec) {
	if (!prec) prec=1;
	
	var div10;
	while (prec-- > 0) {
		div10 = val / 10;
		val = div10 + (val-div10*10)/5;
	}
	
	return val;
}

/*
global func Cart2Polar(cart) {
	return [Distance(0,0,cart[0],cart[1]), Angle(0,0, cart[0], cart[1])];
}

global func Polar2Cart(polar) {
	return [Cos(polar[1], polar[0]), Sin(polar[1], polar[0])];
}*/