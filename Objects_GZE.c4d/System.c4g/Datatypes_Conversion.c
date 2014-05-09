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


global func IntToBytes(int value) {
	return [value >> (3*8) & 255, value >> (2*8) & 255, value >> (1*8) & 255, value & 255];
}

global func BytesToInt(array bytes) {
	return 
		(bytes[3] << (0*8)) |
		(bytes[2] << (1*8)) |
		(bytes[1] << (2*8)) |
		(bytes[0] << (3*8));
}

// Extracts a single byte from a variable (usefull mostly just for ints)
global func GetByte(value, position) {
	if (GetType(value) == C4V_Int) {
		return value >> ((3-position)*8) & 255;
	}
	if (GetType(value) == C4V_String) {
		return GetChar(value, position);
	}
	return -1;
}

global func SetByte(value, newvalue, position) {
	if (GetType(value) == C4V_Int) {
		// Alte Farbe 'löschen'
		value = value & ~(255 << ((3-position)*8));
		// Neue hinzufügen
		return (value|newvalue << ((3-position)*8));
	}
	if (GetType(value) == C4V_String) {
		var chr = newvalue;
		if (GetType(chr) == C4V_Int) chr = Format("%c", chr);
		
		return ConcatStrings(Substring(value, 0, position), chr, Substring(value, position+1));
	}
}
