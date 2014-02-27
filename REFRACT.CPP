#include <math.h>

#include "common.h"
#include "video.hpp"
#include "atimes.hpp"
#include "refract.hpp"

// if need to move to alt=0, instead move to alt=0.5,
// if measured alt from object=0.5, alt to use in calculations = 0;
// refraction handled by routines that translate between scope's
// position (AccumAMs) and sky altitude (Current.Alt)

const int MaxRIx=5;
// to correct for refraction, add interpolation of R1 table to altitude
const double R1[MaxRIx+1][2]=
{
	{ 0/RadToDeg,   0/RadToArcmin},
	{60/RadToDeg, 1.7/RadToArcmin},
	{80/RadToDeg, 5.5/RadToArcmin},
	{84/RadToDeg, 8.6/RadToArcmin},
	{88/RadToDeg,18.1/RadToArcmin},
	{90/RadToDeg,34.5/RadToArcmin}
};

// to remove refraction, subtract interpolation of R2 table from altitude
const double R2[MaxRIx+1][2]=
{
	{ 0/RadToDeg,                    0/RadToArcmin},
	{60/RadToDeg- 1.7/RadToArcmin, 1.7/RadToArcmin},
	{80/RadToDeg- 5.5/RadToArcmin, 5.5/RadToArcmin},
	{84/RadToDeg- 8.6/RadToArcmin, 8.6/RadToArcmin},
	{88/RadToDeg-18.1/RadToArcmin,18.1/RadToArcmin},
	{90/RadToDeg-34.5/RadToArcmin,34.5/RadToArcmin}
};

Refract::Refract(void)
{
	Refract=0;
}

void Refract::CalcRefractScopeToSky(double Alt)
{
	int Ix;
	double ZAng;

	ZAng=M_PI/2-Alt;
	for(Ix=0;ZAng>R1[Ix][0];Ix++)
	;
	// adjust index to extrapolate beyond table if necessary
	if(Alt<0)
		Ix-=2;
	Refract=R1[Ix-1][1]+(ZAng-R1[Ix-1][0])*(R1[Ix][1]-R1[Ix-1][1])
	/(R1[Ix][0]-R1[Ix-1][0]);
}

void Refract::CalcRefractSkyToScope(double Alt)
{
	int Ix;
	double ZAng;
	static double ExtrapolationLimit=R2[MaxRIx][1];

	ZAng=M_PI/2-Alt;
	for(Ix=0;ZAng>R2[Ix][0];Ix++)
	;
	// adjust index to extrapolate beyond table if necessary
	if(Alt<ExtrapolationLimit)
		Ix-=2;
	Refract=R2[Ix-1][1]+(ZAng-R2[Ix-1][0])*(R2[Ix][1]-R2[Ix-1][1])
	/(R2[Ix][0]-R2[Ix-1][0]);
}


