#strict 2

//example: HighlightRect([-50, -100, 100, 200])
global func HighlightRect(rect) {
	CreateObjectLine(DSPK, rect[2]/2, [
		rect[0], 
		rect[1],
		rect[0]+rect[2],
		rect[1]
	]);
	CreateObjectLine(DSPK, rect[2]/2, [
		rect[0]+rect[2], 
		rect[1],
		rect[0]+rect[2],
		rect[1]+rect[3]
	]);
	CreateObjectLine(DSPK, rect[2]/2, [
		rect[0]+rect[2], 
		rect[1]+rect[3],
		rect[0],
		rect[1]+rect[3]
	]);
	CreateObjectLine(DSPK, rect[2]/2, [
		rect[0], 
		rect[1]+rect[3],
		rect[0],
		rect[1]
	]);
}