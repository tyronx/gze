#strict

local time;

Initialize:
  time=FrameCounter()/35;
  SetAction("Static");
  return(1);

CheckRespawn:
  if(FrameCounter()/35-time>8) return(Respawn());
  return(1);
  
Respawn:
  Var(0)=CreateObject(KNIG,0,0,GetOwner());
  MakeCrewMember(Var(0),GetOwner());
	/*if(GetOwner()%2) SetColor(1,Var(0));
		else SetColor(0,Var(0));*/
  // Inhalt
//  if(Var(1)=FindObjectOwner(FLAG,Par(0))) RemoveObject(Var(1));
//	CreateContents(SPER,Var(0));
//	CreateContents(FARP,Var(0));
//	CreateContents(BOW1,Var(0));
  // Volle Energie
  DoEnergy(GetPhysical("Energy",0,Var(0))/1000-50,Var(0));
  CastParticles("PxSpark", 10,100, GetX(Var(0))-GetX(),GetY(Var(0))-GetY(), 100, 200, RGBa(128,128,255,0), RGBa(255,255,255,127));
  return(RemoveObject());