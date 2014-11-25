/*-- Baum-Imitator --*/

#strict 2

public func BeTreeStump(object tree)
{
	SetPosition(tree->GetX(), tree->GetY());
	CopyGraphicsFrom(tree, "Stump");
}

private func CopyGraphicsFrom(object tree, string action)
{
	var con = GetCon(tree);
	var def = GetID(tree);
	
	var offY = GetDefOffset(def, 1);
	var height = GetDefHeight(def);

	var actionHeight = GetActMapVal ("Facet", action, def, 3);
	
	var y = offY + height - actionHeight/2; // position it, so that it fits to the tree bottom

	SetPosition(GetX(), GetY() + (con * y)/100);
	
	SetGraphics(0, this, def, 1, GFXOV_MODE_Action, action);
	SetObjDrawTransform(10*con, 0, 0, 0, 10*con, 0, this, 1);
}