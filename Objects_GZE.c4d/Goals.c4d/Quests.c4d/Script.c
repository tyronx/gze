



/*****************************************************************************\
 *                                                                           *
 *                           QUESTS MAIN SCRIPT                              *
 *                                                                           *
\*****************************************************************************/




/*
  iQuestID
    [Type 1 Quests - Objekte sammeln / töten / vernichten ]
    [Type 2 Quests - Objekte herstellen]
    [Type 3 Quests - Lorenladungen machen]
*/

#strict
#include GOAL

// allgemein
local iQuestIDCount;    // Anzahl der zur Verfügung stehenden Quests
local iQuestsLeft;      // noch zu erfüllende Quests

// momentanes Quest
local iQuestID;         // Quest ID
local iQuestType;       // Typ: -1-keins, 0-anderes, 1-vernichten, 2-herstellen, 3-Lore füllen, ...
local idQuestTarget;    // welches Object vernichten/herstellen, etc
local iQuestObjCount;   // Anzahl
local iQuestPrize;      // Belohnung
local strQuestText;     // Quest Beschreibung

protected Initialize:

  // anderes Objekt vorhanden? Selber dazuzählen
  if (Var()=FindObject(GetID()))
    { LocalN("iQuestsLeft",Var())++; return(RemoveObject()); }
  iQuestsLeft=1;
  iQuestIDCount = 26;               // Gesamtanzahl der Quests
  iQuestType = -1;
  iQuestPrize = 0;
  SetPosition(0,0); // sonst Probleme mit offset bei lokalen aufrufen
  return(1);

private Execute:
  if(iQuestsLeft==0)
    return(0);
  if(iQuestType==-1)
    CreateQuest();
  // Quest schon gelöst?
  if(IsQuestFulfilled())
    CreateQuest();
  return(1);

private CreateQuest:
  // Belohnung geben
  if(iQuestPrize)
  {
    for(Var(0)=0;Var(0)<12;Var(0)++)
      DoWealth(Var(0),iQuestPrize);
    iQuestPrize=0;
    iQuestsLeft--;
    if(!iQuestsLeft)
      return(Message("Gratulation! Du hast Alle Quests bestanden!"));
  }
  UpdateTransferZone();
  // neuen Quest stellen
  var Suceed = 0; var try=0;
  while(!Suceed && try<20)
  {
    iQuestID = Random(iQuestIDCount);
//     iQuestID = --iQuestIDCount; Log("Versuche Quest ID %d",iQuestID);   // debug
    Suceed = AttemptNewQuest();
    try++;
  }
  // TODO: Force here
  if(!Suceed)
    iQuestsLeft=0;
  return(1);

// versuchen, ein neues Quest aufzubauen
private AttemptNewQuest:
    Var()=0;

    Var() = Objects_AttemptQuest (Var());
    Var() =   Other_AttemptQuest (Var());
    Var() = Objects_AttemptLoryQuest (Var());
//    Var() =   Other_AttemptLoryQuest (Var());

    // neuen Quest gefunden? Dann ausgeben...
    if(iQuestPrize)
    {
      UpdateQuestText();
      Message(Format("Neue Aufgabe: %s", strQuestText));
//      Log(Format("Neuer Quest: %s", strQuestText)); // debug
      return(1);
    }
  return(0);

private UpdateQuestText:
    Var(0)=0;
    Var() = Objects_GetQuestText(Var());
    Var() =   Other_GetQuestText(Var());
    if(iQuestType==3)
        strQuestText= Format( "Mach eine Lorenladung %s (%d)!", GetName(0, idQuestTarget), iQuestObjCount );
    return(1);

InitQuestType1: // Par() C4ID
    iQuestType = 1;
    idQuestTarget = Par();
    
    if(IsQuestFulfilled())  return (0);
    
    return((GetResourceCount(idQuestTarget)*Sqrt(LandscapeWidth()*LandscapeHeight()))/1000);

InitQuestType2: // Par() C4ID
    iQuestType = 2;
    idQuestTarget = Par();

    // Existenz prüfen
    if(IsQuestFulfilled())  return (0);
    var i;
    for(i=0; i<12; i++)
        if(GetHomebaseMaterial(i, Par()))
            return(0);

    // Herstellbarkeit prüfen
    var ProductionPlace=0;
    if(DefinitionCall(Par(),"IsChemicalProduct"))  ProductionPlace = CHEM;
    if(GetDefCoreVal("Category",0,Par()) & C4D_Vehicle())  ProductionPlace = WRKS;

    // Productionplace da oder beschaffbar?
    if(   !ProductionPlace
        || FindObject(ProductionPlace,0,0,0,0,OCF_Fullcon())
        || ( GetAnyPlayerKnowledge(ProductionPlace) && FindAnyBase() )
        || FindObject(RSRC,0,0,0,0,OCF_Fullcon())
        || ( GetAnyPlayerKnowledge(RSRC) && FindAnyBase() )
       )
    {
            // Bauplan da oder beschaffbar?
            if(    GetAnyPlayerKnowledge(Par())
                || FindObject(RSRC,0,0,0,0,OCF_Fullcon())
                || GetAnyPlayerKnowledge(RSRC))
            {
                // Benötigte Materialien vorhanden?
                if(!(!FindObject(CNMT) && (GetCategory(0,Par())&C4D_Structure())))
                {
                    i=0; var comp;
                    while(comp = GetComponent(0, i++, 0, Par()))
                    {
                        if(GetComponent(comp,0,0,Par())>GetResourceCount(comp,2))
                            return(0);
                    }
                }
                return(GetValue(0,Par())/2);
            }
    }

    return(0);


InitQuestType3: // Par(0): C4ID

    if(!FindObject(LORY) && !(FindAnyBase()&&GetAnyHomeBaseMaterial(LORY)))
        return (0);

    iQuestType = 3;
    idQuestTarget = Par();
    iQuestObjCount = 25; // (50*75)/GetMaterialRatio(0,Par());

    // Schon kaufbar?
    var i;
    for(i=0; i<12; i++)
        if( iQuestObjCount<=GetHomebaseMaterial(i, Par()) )
            return(0);

    // gibt es schon eine solche Lorenladung?
    if(IsQuestFulfilled())  return (0);

    // Genug Material da?
    var count = GetResourceCount(Par(),1);
    if( !count || count<iQuestObjCount )
        return(0);
    
    // Belohnung je nach Häufigkeit
    return((Sqrt(LandscapeWidth()*LandscapeHeight()))/GetResourceCount(Par(),1)/100);

IsQuestFulfilled:
  // *** Queststatus prüfen
  // Objekttyp sammeln/entfernen/töten
  if(iQuestType==1)
  {
      // vernichten
      if(!FindObjectOwner(idQuestTarget,-1,0,0,0,0,OCF_Fullcon()))
          return(1);
      // töten
      while(Var()=FindObjectOwner(idQuestTarget,-1,0,0,0,0,OCF_Fullcon(),0,0,Var()))
            if((GetOCF(Var()) & OCF_Living()) && (GetOCF(Var()) & OCF_Alive()))
                return(0);
      // sammeln
      if(ObjectCount(idQuestTarget,0,0,0,0,OCF_Fullcon())<=1)
          return(0);
      Var()=FindObjectOwner(idQuestTarget,-1,0,0,0,0,OCF_Fullcon());
      Var(1)=GetX(Var());
      Var(2)=GetY(Var());
      while(Var()=FindObjectOwner(idQuestTarget,-1,0,0,0,0,OCF_Fullcon(),0,0,Var()))
          if(GetX(Var())<Var(1)-20 || GetX(Var())>Var(1)+20 || GetY(Var())<Var(2)-20 || GetY(Var())>Var(2)+20)
            return(0);
      return(1);

  }
  // Objekttyp herstellen
  if(iQuestType==2)
  {
      Var()=0;
      while(Var()=FindObject(idQuestTarget,0,0,0,0,OCF_Fullcon(),0,0,0,Var()))
        if(GetOwner(Var()) != -1)
          return(1);
  }
  // Lorenladung machen
  if(iQuestType==3)
  {
      Var()=0;
      while(Var()=FindObject(LORY,0,0,0,0,0,0,0,0,Var()))
        if(ContentsCount(idQuestTarget,Var()) >= iQuestObjCount)
          return(1);
  }
  return(0);
  
GetAnyPlayerKnowledge:  // Par() C4ID
    var i,j;
    for(i=0; i<12; i++)
    {
        if(j=GetPlrKnowledge(i,Par()))
            break;
    }
    // negativ...
    if(i==12)  return(0);
    return (j);

FindAnyBase:
    var i,j;
    for(i=0; i<12; i++)
    {
        if(j=FindBase(i))
            break;
    }
    // negativ...
    if(i==12)  return(0);
    return (j);

private func GetAnyHomeBaseMaterial(objectid)
{
    var i,j=0;
    for(i=0; i<12; i++)
        j+=GetHomebaseMaterial(i,objectid);
    return (j);
}

// Resourcen: frei rumlaufend/liegend, aus Material oder Bäumen direkt oder indirekt erschaffbar
// funktioniert mit allen Originalmaterialien (fest&flüssig), generell allen Lebewesen, Metall und Holz
    // idRes:        Resource (OBRL, WOOD, METL)
    // fSourceCheck: 0:direkte Nonplayerobjekte
    //               1:Nonplayerobjekte+Sourcecheck
    //               2:Alle Objekte+Kaufbar+Sourcecheck
// Zukunftsmusik: bei idRes angeben: CST1 etc; und er prüft, ob die Materialien und das Wissen da sind, um das ding zu bauen
private func GetResourceCount(idRes, fSourceCheck)
{

    var iCount=0;

    if( fSourceCheck == 2 )
    {
        iCount = ObjectCount(idRes);
        if(FindAnyBase()) iCount += GetAnyHomeBaseMaterial(idRes);
    }
    else
    {
        Var(0)=0;
        while(Var()=FindObjectOwner(idRes,-1,0,0,0,0,OCF_Fullcon()|OCF_Alive(),0,0,Var()))
          iCount++;
    }

    if( fSourceCheck )
    {
        iCount += Objects_SourceCheck(idRes, fSourceCheck);
    }
    return(iCount);
}

private GetMaterialRatio:   // Par(0) strMatName oder Par(1) C4ID MatObj
    var ratio;
    ratio = Objects_MaterialRatio(Par(),Par(1)); if(ratio) return (ratio);
//  ratio = YourPack_MaterialRatio(Par(),Par(1)); if(ratio) return ratio;
    return(0);

// nicht synchronisierte Inhalte setzen
UpdateTransferZone:
    if(iQuestsLeft==0)
        SetName("Quests");
    else
        SetName(Format("Quests (%d)", iQuestsLeft), this());

    return(1);

protected Activate:
  if(iQuestsLeft)
  {
    UpdateQuestText();
    MessageWindow(Format("%s|Belohnung: %d Clunker", strQuestText,iQuestPrize),Par());
  }
  else
    MessageWindow("Alle Quests erfüllt!",Par());
  return(1);

IsFulfilled:
  if(!iQuestsLeft)
    return (1);
  return (0);




/*****************************************************************************\
 *                                                                           *
 *                      STANDARD OBJECTS QUESTS                              *
 *                                                                           *
\*****************************************************************************/





// versuchen, einen Quest aufzubauen
private Objects_AttemptQuest:
    Var() = Par();

    /*
     *  Type 1: Remove Quests
     */

    // Muscheln sammeln
    if(iQuestID==Var()++)
    {
      iQuestPrize = InitQuestType1(SHEL);
    }
    // Monsterjagd
    if(iQuestID==Var()++ && !FindObject(MNTK))
    {
      iQuestPrize = InitQuestType1(MONS)*3;
    }
    // Feuermonsterjagd
    if(iQuestID==Var()++ && !FindObject(MNTK))
    {
      iQuestPrize=InitQuestType1(FMNS)*4;
    }
    // Monstereier
    if(iQuestID==Var()++ && !FindObject(MNTK))
    {
      iQuestPrize = InitQuestType1(MEGG);
    }
    // Feuermonstereier
    if(iQuestID==Var()++ && !FindObject(MNTK))
    {
      iQuestPrize = InitQuestType1(FMEG);
    }
    // Zapnester
    if(iQuestID==Var()++)
    {
      iQuestPrize = InitQuestType1(ZAPN);
    }
    // Schlangenjagd
    if(iQuestID==Var()++)
    {
      iQuestPrize=InitQuestType1(SNKE)*2;
    }
    // Fische fangen
    if(iQuestID==Var()++)
    {
      iQuestPrize=InitQuestType1(FISH);
    }
    // Wipfe retten
    if(iQuestID==Var()++ && !FindObject(WPHT))
    {
      iQuestPrize=InitQuestType1(WIPF);
    }

    /*
     *  Type 2: Create Quests
     */

    if(iQuestID==Var()++)  iQuestPrize = InitQuestType2(EFLN);
    if(iQuestID==Var()++)  iQuestPrize = InitQuestType2(FBMP);

    if(iQuestID==Var()++)  iQuestPrize = InitQuestType2(STMG);
    if(iQuestID==Var()++)  iQuestPrize = InitQuestType2(WAGN);
    if(iQuestID==Var()++)  iQuestPrize = InitQuestType2(BLMP);

    if(iQuestID==Var()++
        && !(FindObject(CST2,0,0,0,0,OCF_Fullcon())
             ||FindObject(CST3,0,0,0,0,OCF_Fullcon())))
                           iQuestPrize = InitQuestType2(CST1);
    if(iQuestID==Var()++
        && !(FindObject(CST1,0,0,0,0,OCF_Fullcon())
             ||FindObject(CST3,0,0,0,0,OCF_Fullcon())))
                           iQuestPrize = InitQuestType2(CST2);
    if(iQuestID==Var()++
        && !(FindObject(CST1,0,0,0,0,OCF_Fullcon())
             ||FindObject(CST2,0,0,0,0,OCF_Fullcon())))
                           iQuestPrize = InitQuestType2(CST3);
    //if(iQuestID==Var()++)  iQuestPrize = InitQuestType2(IGLO);

    return(Var());

private Objects_AttemptLoryQuest:
    Var()=Par();

    /*
     *  Type 3: Lory Loads
     */

    if(iQuestID==Var()++)  iQuestPrize = InitQuestType3(CRYS);
    if(iQuestID==Var()++ && !FindObject(GLDM))  iQuestPrize = InitQuestType3(GOLD);
    if(iQuestID==Var()++)  iQuestPrize = InitQuestType3(SPHR);
    if(iQuestID==Var()++)  iQuestPrize = InitQuestType3(COAL);
    if(iQuestID==Var()++ && !FindObject(OREM))  iQuestPrize = InitQuestType3(ORE1);
    if(iQuestID==Var()++)  iQuestPrize = InitQuestType3(METL);
    if(iQuestID==Var()++)  iQuestPrize = InitQuestType3(WOOD);

    return(Var());


private Objects_GetQuestText:
    Var()=Par();

    if(iQuestID==Var(0)++) strQuestText = Format("Muscheln sammeln (%d)!",ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Monsterjagd, alle Monster töten (%d)! Verschone die Feuermonster.", ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Monsterjagd, alle Feuermonster töten (%d)!", ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Finde alle Monstereier (%d)! Lass die Feuermonstereier.",ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Finde alle Feuermonstereier (%d)!",ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Finde alle Zapnester (%d)!", ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Schlangenjagd, alle Schlangen wegbringen oder töten (%d)!", ObjectCount(idQuestTarget,0,0,0,0,OCF_Alive()));
    if(iQuestID==Var(0)++) strQuestText = Format("Fische fangen (%d)!", ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Wipfe retten (%d)!", ObjectCount(idQuestTarget));

    if(iQuestID==Var(0)++) strQuestText = "Mache einen Terraflint!";
    if(iQuestID==Var(0)++) strQuestText = "Baue eine Brandbombe!";
    if(iQuestID==Var(0)++) strQuestText = "Erbaue eine Dampfmaschine!";
    if(iQuestID==Var(0)++) strQuestText = "Baue einen Wagon!";
    if(iQuestID==Var(0)++) strQuestText = "Baue ein Luftschiff!";
    if(iQuestID==Var(0)++) strQuestText = "Errichte eine kleine Burg!";
    if(iQuestID==Var(0)++) strQuestText = "Errichte eine Burg!";
    if(iQuestID==Var(0)++) strQuestText = "Errichte ein Schloß!";
    //if(iQuestID==Var(0)++) strQuestText = "Bau dir ein Iglu!";

    // Beschreibungen für Lory Loads werden automatisch erzeugt!

    return(Var());


private Objects_SourceCheck:
    var iCount=0;
    var idRes; idRes=Par();
    var fSourceCheck; fSourceCheck=Par(1);
    // Objekte aus Materialien
    if(idRes==CRYS)  iCount += GetMaterialCount(Material("Crystal"))/(GetMaterialRatio("Crystal")+15);  // ratio + 15
    if(idRes==GOLD)  iCount += GetMaterialCount(Material("Gold"))/(GetMaterialRatio("Gold")+15);
    if(idRes==ROCK)  iCount += GetMaterialCount(Material("Rock"))/(GetMaterialRatio("Rock")+15);
    if(idRes==SPHR)  iCount += GetMaterialCount(Material("Sulphur"))/(GetMaterialRatio("Sulphur")+15);
    if(idRes==COAL)  iCount += GetMaterialCount(Material("Coal"))/(GetMaterialRatio("Coal")+15);
    if(idRes==ORE1)  iCount += GetMaterialCount(Material("Ore"))/(GetMaterialRatio("Ore")+15);
    if(idRes==ERTH)  iCount += GetMaterialCount(Material("Earth"))/(GetMaterialRatio("Earth")+15);
    if(idRes==SAND)  iCount += GetMaterialCount(Material("Sand"))/(GetMaterialRatio("Sand")+15);
    //if(idRes==SNOW)  iCount += GetMaterialCount(Material("Snow"))/(GetMaterialRatio("Snow")+15);
    if(idRes==ICE1)  iCount += GetMaterialCount(Material("Ice"))/(GetMaterialRatio("Ice")+15);
    if(idRes==WBRL)  iCount += GetMaterialCount(Material("Water"))/(GetMaterialRatio("Water")+15);
    if(idRes==OBRL)  iCount += GetMaterialCount(Material("Oil"))/(GetMaterialRatio("Oil")+15);
    if(idRes==LBRL)  iCount += GetMaterialCount(Material("DuroLava"))/(GetMaterialRatio("DuroLava")+15);
    if(idRes==ABRL)  iCount += GetMaterialCount(Material("Acid"))/(GetMaterialRatio("Acid")+15);

    if(idRes==EFLN)  iCount += (GetMaterialCount(Material("Sulphur"))/(GetMaterialRatio("Sulphur")+15))/5;
    if(idRes==SFLN)  iCount += (GetMaterialCount(Material("Sulphur"))/(GetMaterialRatio("Sulphur")+15))/2;
    if(idRes==FLNT)  iCount += GetMaterialCount(Material("Sulphur"))/(GetMaterialRatio("Sulphur")+15);

    // WOOD: aus Bäumen
    if(idRes==WOOD)
    {
        Var()=0;
        // Bedingung geht davon aus, daß ohne SAWM||HomeBase+Material kein Holz gemacht werden kann (obwohl das nicht immer stimmt)
        // GetAnyHomeBaseMaterial(WOOD/METL)+ObjectCount(WOOD/METL)>=? muß hier direkt geprüft werden, um Endlosschleife zu verhindern
        if(FindObject(SAWM) || (FindAnyBase() && (GetAnyHomeBaseMaterial(CNKT)+ObjectCount(CNKT)) && GetAnyHomeBaseMaterial(WOOD)+ObjectCount(WOOD)>=6 && GetAnyHomeBaseMaterial(METL)+ObjectCount(METL)>=2))
            while(Var()=FindObject(0,0,0,0,0,0,0,0,0,Var()))
                if(ObjectCall(Var(),"IsTree"))
                    iCount += GetComponent(WOOD,0,Var());
    }
    
    // METL: aus Eisenerz und Kohle/Holz/Öl
    if(idRes==METL)
    {
        // Bedingung geht davon aus, daß ohne FNDR||HomeBase+Material kein Metall gemacht werden kann (obwohl das nicht immer stimmt)
        if(FindObject(FNDR) || (FindAnyBase() && (GetAnyHomeBaseMaterial(CNKT)+ObjectCount(CNKT)) && GetResourceCount(ROCK,2)>=6 && GetResourceCount(WOOD,2)>=3))
        {
            Var(1) = GetResourceCount(ORE1, fSourceCheck);
            Var(2) =   GetResourceCount(COAL, fSourceCheck)
                     + GetResourceCount(WOOD, fSourceCheck)/2
                     + GetResourceCount(OBRL, fSourceCheck);
            iCount += Min( Var(1),Var(2) );
        }
    }
    return (iCount);


private Objects_MaterialRatio:
    if( Par() eq "Crystal" || Par(1) == CRYS )  return(100);
    if( Par() eq "Gold"    || Par(1) == GOLD )  return(75);
    if( Par() eq "Rock"    || Par(1) == ROCK )  return(75);
    if( Par() eq "Sulphur" || Par(1) == SPHR )  return(200);
    if( Par() eq "Coal"    || Par(1) == COAL )  return(300);
    if( Par() eq "Ore"     || Par(1) == ORE1 )  return(100);
    if( Par() eq "Earth"   || Par(1) == ERTH )  return(200);
    if( Par() eq "Sand"    || Par(1) == SAND )  return(200);
//  if( Par() eq "Snow"    || Par(1) == SNOW )  return(200);
    if( Par() eq "Ice"     || Par(1) == ICE1 )  return(400);
    if( Par() eq "Water"   || Par(1) == WBRL )  return(200);
    if( Par() eq "Oil"     || Par(1) == OBRL )  return(200);
    if( Par() eq "DuroLava"|| Par(1) == LBRL )  return(200);
    if( Par() eq "Acid"    || Par(1) == ABRL )  return(200);
    
    // Kein echtes Material
    if( Par(1) == WOOD )  return(75);
    if( Par(1) == METL )  return(200);
    return (0);




/*****************************************************************************\
 *                                                                           *
 *                        OTHER OBJECTS QUESTS                               *
 *                                                                           *
\*****************************************************************************/






// versuchen, einen Quest aufzubauen
private Other_AttemptQuest:

    Var()=Par();

    /*
     *  Typ 1: Objekte vernichten
     */

    // Das Groms
    if(iQuestID==Var()++)
    {
      iQuestPrize = InitQuestType1(GRMS);
    }

    // Dorf zerstören
    if(iQuestID==Var()++)
    {
      if(!FindObject(JHUT,0,0,0,0,OCF_Fullcon()))
        return(0);
      InitQuestType1(JHUT);
      iQuestPrize=10*ObjectCount(JHUT,0,0,0,0,OCF_Fullcon());
      iQuestPrize+=15*ObjectCount(JCLK,0,0,0,0,OCF_Alive());
      iQuestPrize+=20*ObjectCount(JCKH,0,0,0,0,OCF_Alive());
    }

    return(Var());

private Other_GetQuestText:
    Var()=Par();
    if(iQuestID==Var(0)++) strQuestText = Format("Finde alle Groms und sammle, aktiviere oder verkaufe sie (%d)!",ObjectCount(idQuestTarget));
    if(iQuestID==Var(0)++) strQuestText = Format("Greife die Siedlung der Jungel-Clonks an! Zerstöre ihre Hütten (%d)!",ObjectCount(idQuestTarget,0,0,0,0,OCF_Fullcon()));

    return(Var());
