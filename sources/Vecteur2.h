#pragma once
#include <vector>

template<typename T>
class Vecteur2{
public:
	T x, y;
	
	Vecteur2(T x = 0, T y = 0) { 
		this->x = x; this->y = y; 
	}
		
	Vecteur2 operator+(Vecteur2& vect) { 
		return Vecteur2(this->x + vect.x, this->y + vect.y); 
	}
		
	void operator+=(Vecteur2& autre) { 
		x += autre.x; y += autre.y; 
	}
		
	Vecteur2 operator-(Vecteur2& vect) { 
		return Vecteur2(this->x - vect.x, this->y - vect.y); 
	}
		
	void operator-=(Vecteur2& autre) { 
		x -= autre.x; y -= autre.y; 
	}
		
	Vecteur2 operator*(float scalaire) { 
		return Vecteur2(this->x * scalaire, this->y * scalaire); 
	}
		
	void operator*=(Vecteur2& autre) { 
		x *= autre.x; y *= autre.y; 
	}
		
	bool operator==(Vecteur2& autre) { 
		return x == autre.x && y == autre.y; 
	}
		
	bool operator!=(Vecteur2 &autre) { 
		return !operator==(autre); 
	}

	float norme() { 
		return sqrt(x * x + y * y); 
	}

	void normaliser() { 
		float module = norme(); 
		this->x /= module; this->y /= module; 
	}
		
		
};
typedef Vecteur2<int> Vecteur2i;
typedef Vecteur2<unsigned int> Vecteur2ui;
typedef Vecteur2<float> Vecteur2f;
typedef Vecteur2<double> Vecteur2d;