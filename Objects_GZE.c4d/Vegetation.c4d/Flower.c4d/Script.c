/*-- Plants --*/

#strict 2

protected func Initialize(obj) { 
  SetAction(Format("Plant%d", Random(3) + 1));
}

// Bei nahen Explosionen
public func OnShockwaveHit(iLevel,iX,iY)
{
 var con=(40*GetCon())/100;
 iLevel=40+iLevel/2;
 for(var cnt=0;cnt<10+Random(10);cnt++)
  CreateParticle("GrassBlade",RandomX(-con/2,con/2),-1,RandomX(-iLevel/3 ,iLevel/3),RandomX(-2*iLevel/3,-iLevel/3),30+Random(30),RGB(255,255,255),0,0);
 return(RemoveObject());
}

// Kann immer von Schockwellen getroffen werden
public func CanBeHitByShockwaves(){return(true);}
public func BlastObjectsShockwaveCheck(){return(true);}


public func GetVegetationSoil() { return ("Grass"); }
public func GetVegetationIsFlower() { return (1); }
public func GetVegetationRootDepth() { return(2); } 
// Auto rotate the vegetation to fit the landscape between these angles
public func GetVegetationRotates() { return [-30, 30]; }
