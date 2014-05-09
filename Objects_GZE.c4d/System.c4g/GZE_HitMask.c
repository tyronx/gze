#strict 2
#appendto *

/* Getting Hit reduces object speed */

func CatchBlow(int iLevel, object pObj) {
	if (GetCategory(pObj) & C4D_Object && GetOCF(pObj) & OCF_Collectible && ObjectDistance(pObj)*10 < 8*Sqrt(GetDefWidth(GetID())*GetDefHeight(GetID())) ) {
		pObj->~Hit();
		// Reduces object speed by 90%
		if (pObj) {
			pObj->SetXDir(GetXDir()*5, 0, 100);
			pObj->SetYDir(GetYDir()*5, 0, 100);
		}
	}
	return _inherited(iLevel, pObj);
}