#strict

/* Same as FindObjects() but removes all found objects and returns amount removed */
global func RemoveObjects() {
	var obj, cnt = 0;
	while (obj = FindObject2(Par(0), Par(1), Par(2), Par(3), Par(4), Par(5), Par(6), Par(7), Par(8), Par(9))) {
		RemoveObject(obj);
		cnt++;
	}
	return cnt;
}
