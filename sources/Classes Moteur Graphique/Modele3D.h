#pragma once

#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include "Vecteur3.h"
#include "Maths.h"
#include "Modele.h"
#include "Texture.h"
#include "Objet3D.h"
class Modele3D : public Objet3D {
private:
	Modele modele;
	Texture texture;
	Vecteur3f boiteDeCollision[8];
	void calculerBoiteDeCollision(){
		std::vector<float> tmpX;
		std::vector<float> tmpY;
		std::vector<float> tmpZ;
		float xmax, xmin, ymax, ymin, zmax, zmin;
		for (int i = 0; i < modele.obtNbrFaces(); ++i){
			tmpX.push_back(modele.obtVertices()[i * 3]);
			tmpY.push_back(modele.obtVertices()[i * 3 + 1]);
			tmpZ.push_back(modele.obtVertices()[i * 3 + 2]);
		}
		xmax = Maths::obtValeurMax(tmpX);
		xmin = Maths::obtValeurMin(tmpX);
		ymax = Maths::obtValeurMax(tmpY);
		ymin = Maths::obtValeurMin(tmpY);
		zmax = Maths::obtValeurMax(tmpZ);
		zmin = Maths::obtValeurMin(tmpZ);
		boiteDeCollision[0] = Vecteur3f(xmax, ymax, zmax);
		boiteDeCollision[1] = Vecteur3f(xmax, ymax, zmin);
		boiteDeCollision[2] = Vecteur3f(xmax, ymin, zmax);
		boiteDeCollision[3] = Vecteur3f(xmax, ymin, zmin);
		boiteDeCollision[4] = Vecteur3f(xmin, ymax, zmax);
		boiteDeCollision[5] = Vecteur3f(xmin, ymax, zmin);
		boiteDeCollision[6] = Vecteur3f(xmin, ymin, zmax);
		boiteDeCollision[7] = Vecteur3f(xmin, ymin, zmin);
	}

public:

	Modele3D(){
	}

	Modele3D(Modele &modele, Texture &texture){
		this->modele = modele;
		this->texture = texture;
	}

	~Modele3D(){}

	void defModele(Modele &modele){
		this->modele = modele;
	}

	void defTexture(Texture &texture){
		this->texture = texture;
	}

	void defEchelle(float echX, float echY, float echZ){
		for (int i = 0; i < modele.obtNbrFaces(); i++)
		{
			modele.obtVertices()[i * 3] *= echX;
			modele.obtVertices()[i * 3 + 1] *= echY;
			modele.obtVertices()[i * 3 + 2] *= echZ;
		}
		calculerBoiteDeCollision();
		modele.calculerTaille();
	}

	Modele obtModele(){
		return modele;
	}

	Vecteur3f* obtBoiteDeCollision(){
		return boiteDeCollision;
	}

	Texture obtTexture(){
		return texture;
	}

	virtual void rafraichir(){
		glPushMatrix();
		glLoadIdentity();


		glPopMatrix();
	};

	virtual void afficher(){
		glPushMatrix();
		glLoadIdentity();

		glPopMatrix();
	};
};