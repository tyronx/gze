#strict 2

/* Same as FindObjects() but removes all found objects and returns amount removed */
global func RemoveObjects() {
	var obj, cnt = 0;
	while (obj = FindObject2(Par(0), Par(1), Par(2), Par(3), Par(4), Par(5), Par(6), Par(7), Par(8), Par(9))) {
		RemoveObject(obj);
		cnt++;
	}
	return cnt;
}






global func CreateObjectLine(objid, quantity, vector, checknostuck) {
	var x, y, i=0, pObj, attempts;
	
	while (i < quantity) {
		i++;
		
		x = vector[0] + (vector[2]-vector[0])*i/quantity;
		y = vector[1] + (vector[3]-vector[1])*i/quantity;
		pObj = CreateObject(objid, x, y, -1);
		attempts = 0;
		while (checknostuck && Stuck(pObj) && attempts++ < 100) {
			pObj->SetPosition(GetX()+x, GetY()+y+attempts);
			if (!Stuck(pObj)) break;
			pObj->SetPosition(GetX()+x, GetY()+y-attempts);
			if (!Stuck(pObj)) break;
		}
		if (attempts >= 100) {
			pObj->SetPosition(GetX()+x, GetY()+y);
		}
	}
}