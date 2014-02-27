#ifndef COORD_HPP
#define COORD_HPP

#define CurrentFile "CURRENT.DAT"

// Ra, Dec, Alt, Az kept in radians
struct Position
{
	double Ra;
	HMS RaHMS;
	double Dec;
	DMS DecDMS;
	double Alt;
	double Az;
	TimeStruct SidT;
};

class Coord : public ATimes
{
	public:
		Coord(const double LongitudeDeg,int Tz);
		~Coord(void);
	public:
		// current coordinates
		Position Current;

		void SaveCurrentAltaz();
		void LoadCurrentAltaz();
		/* this function overrides ATimes::NewTime and sets Current.SidT
		to ATimes::SidT */
		int NewTime(void);
		void ValidRa(Position& Pos);
		void ValidDec(Position& Pos);
		void ValidAz(Position& Pos);
		void SetCoordDeg(Position& Pos,double RaDeg,double DecDeg,double AltDeg,
		double AzDeg,double SidTDeg);
		virtual void ShowCoordDeg(Position& Pos);
		// this function displays coordinates in traditional format
		virtual void ShowCoord(Position& Pos);
		void CalcDisplayRa(Position& Pos);
		void CalcDisplayDec(Position& Pos);
		void DisplayAltDeg(Position& Pos);
		void DisplayAzDeg(Position& Pos);
		void DisplayAltArcsec(Position& Pos);
		void DisplayAzArcsec(Position& Pos);
};

#endif