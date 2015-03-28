/*-- Palme --*/

#strict 2

#include TRE1

// Etwas leichter zu fällen
private func TreeStrength() { return 100; }
public func GetVegetationSoil() { return "Sand"; }
public func GetVegetationRotates() { return [-45, 45]; }

public func ShakeStrength() { return 0; } // don't shake because currently bugged