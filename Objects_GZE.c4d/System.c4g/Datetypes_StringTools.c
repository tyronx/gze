//  Partially taken from the Clonk Infinity Project - debugged, extended and simplified ;)

#strict 2

global func ConcatString(string str1, string str2) {
	return Format("%s%s", str1, str2);
}

// end may be ommitted
// start may be negative to cut out from the end of the string. e.g. Substring("test.", -1) will return '.'
global func Substring(string str, int start, int end) {
	var result = "";
	
	if (start < 0) {
		start = GetLength(str) + start;
	}
	
	if (!end) end = GetLength(str);
	else end = Min(end, GetLength(str));

	for(var i = Max(0, start); i < end; i++) {
		result = Format("%s%c", result, GetChar(str, i));
	}
	return result;
}

// Returns -1 if not found
global func IndexOf(string needle, string haystack, int start) {
	var haystacklen = GetLength(haystack), needlelen = GetLength(needle);
	
	for(var i = start; i <= haystacklen - needlelen; i++) {
		var matching = true;
		for (var j = 0; j < needlelen && matching; j++) {
			matching = matching && GetChar(haystack, i + j) == GetChar(needle, j);
		}
		if(matching) {
			return i;
		}
	}
	return -1;
}

// Counts occurences of a substring, e.g. SubstringCount("_","a_b_c_") will return 3
global func SubstringCount(string needle, string haystack) {
	var cnt = 0, pos = 0;
	while ((pos = IndexOf(needle, haystack, pos)) != -1) {
		cnt++;
		pos++;
	}
	return cnt;
}
