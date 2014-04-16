/*-- Prevent Wipfs from ruining the landscape --*/

#strict
#appendto WIPF

func StartDig() {
	return(0);
}


/*-- In GZE Wipfs went to underground burrows :D --*/

func GetAnimalPlacement() { return (4); }