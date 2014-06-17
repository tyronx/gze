#strict 2

global func ArrayIndexOf(array &arr, elem1) {
	var type = GetType(elem1);
	var idx = 0;
	for (var elem2 in arr) {
		if (elem1 == elem2) return idx;
		idx++;
	}
	return -1;
}

// Remove the last element from the array, returning the last element
/* Example:
	var values = [1,2,3,4,5];
	Log("%v", ArrayPop(values)); // will print 5
	Log("%v", values) // Will print [1, 2,3, 4]
*/
global func ArrayPop(array &arr) {
	var elem = arr[GetLength(arr) - 1];
	SetLength(arr, Max(0, GetLength(arr) - 1));
	return elem;
}

// Push given element onto end of the array
global func ArrayPush(array &arr, elem) {
	SetLength(arr, GetLength(arr) + 1);
	arr[GetLength(arr)-1] = elem;
}


// Remove element from array at index i, returns the sliced out value
global func ArraySlice(array &arr, int index) {
	var val = arr[index];
	var len = GetLength(arr);
	if (index < 0 || len == 0 || index > len-1) return;
	
	while (index++ < len) {
		arr[index-1] = arr[index];
	}
	SetLength(arr, Max(0, len - 1));
	return val;
}

global func ArrayInsert(array &arr, elem, int index) {
	SetLength(arr, GetLength(arr)+1);
	var i = index;
	var len = GetLength(arr);
	while (i++ < len) {
		arr[i+1] = arr[i];
	}
	arr[index] = elem;
}

// Remove the first element from the array
global func ArrayShift(array &arr) {
	var elem = arr[0];
	ArraySlice(arr, 0);
	return elem;
}

// Push element to beginning of the array
global func ArrayUnshift(array &arr, elem) {
	var i=0, len = GetLength(arr);
	SetLength(arr, len + 1);
	while (i++ < len) {
		arr[i] = arr[i-1];
	}
	arr[0] = elem;
}

// Splits a string into an array with given seperator, e.g. ExplodeString("_", "a_b_c") will return ["a","b","c"]
global func ExplodeString(string seperator, string str) {
	var parts = CreateArray(SubstringCount(seperator, str)+1);
	
	var i = 0, start = 0, end = 0;
	while ((end = IndexOf(seperator, str, start)) != -1) {
		parts[i++] = Substring(str, start, end);
		start = end+1;
	}
	parts[i] = Substring(str, start);
	
	return parts;
}

// Concatenates an array with given concatenator - inverse function of ExplodeString()
global func ImplodeArray(string concatenator, array parts) {
	var str = "", i=0;
	
	for (var part in parts) {
		str = Format("%s%s", str, part);
		if (i++ > 0) str = Format("%s%s", str, concatenator);
	}
	return str;
}