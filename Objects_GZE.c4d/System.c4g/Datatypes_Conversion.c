#strict 2

/* Turns e.g. "-23a15" into -2315 */
global func StringToInt(string strNumber) {
	var length = GetLength(strNumber), chr;
	var iNumber = 0;
	
	for (var i = 0; i < length; i++) {
		chr = GetChar(strNumber, i);
		// Do not parse any non numeric character
		if (chr < 48 || chr > 57) {
			iNumber = iNumber/10;
			continue;
		}
		iNumber += Pow(10, length - i - 1)*(chr-48); 
	}
	
	if (GetChar(strNumber, 0) == GetChar("-")) iNumber = -iNumber;
	
	return iNumber;
}
