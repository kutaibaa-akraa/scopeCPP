#ifndef REFRACT_HPP
#define REFRACT_HPP

class Refract
{
	public:
		// in radians
		double Refract;

		Refract(void);
		// this function calcs refraction to add to altitude
		void CalcRefractScopeToSky(double Alt);
		// this function calcs refraction to remove from altitude
		void CalcRefractSkyToScope(double Alt);
};

#endif