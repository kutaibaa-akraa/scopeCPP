PRINT "Newtonian Baffling Calculation Routine"
PRINT
INPUT "Focal Plane Diameter "; FocD
INPUT "2 Inch Focuser Barrel Length "; BL
INPUT "Focuser Barrel Inside Diameter "; BD
INPUT "Diagonal Size "; Diag
INPUT "Focal Plane to Diagonal Length "; L
INPUT "Initial Baffle to Diagonal Distance "; D1
INPUT "Final Baffle to Diagonal Distance "; D2
PRINT

FocBafL = (L - D1)
BaffleOD = (FocD + BD) / BL * (FocBafL - BL) + BD
BaffleID = (Diag - FocD) / L * FocBafL + FocD

PRINT "Pre-baffle below focuser ID = "; BaffleID
PRINT "Pre-baffle below focuser OD = "; BaffleOD

FinalBaffleD = (FocD + BaffleID) / FocBafL * (D1 + D2) + BaffleID

PRINT "Baffle on opposite side of upper end OD = "; FinalBaffleD
PRINT

