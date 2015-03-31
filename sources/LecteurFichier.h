#pragma once
#include "BoiteCollision.h"
#include "Info.h"
#include <fstream>

namespace LecteurFichier{
	bool lireBoite(char* cheminAcces, InfoSalle& salle){
		std::ifstream fichier("cheminAcces");
		/*Exemple fichier
		...
		Boite{
		x y z
		x y z
		x y z
		x y z
		x y z
		x y z
		x y z
		x y z
		}
		...
		*/
		if (fichier.is_open()){

			char* ligne;

			while (!fichier.eof()){
				fichier.getline(ligne, 256);
				if (ligne == "Boite{"){
					Vecteur3<double>* tabBoite[8];
					double x, y, z;
					for (int i = 0; i < 8; i++){
						fichier >> x >> y >> z;
						tabBoite[i] = new Vecteur3<double>(x, y, z);
					}
					BoiteCollision boite(*tabBoite);
					salle.boitesCollision.push_back(boite);
				}
			}

			fichier.close();
		}

		else
			return false;
	}


	bool lirePuzzle(char* cheminAcces){
		std::ifstream fichier("cheminAcces");
		/*Exemple fichier
		...
		Puzzle{
		x y z//dimension max
		Objet{
		c \FichierObjet
		s x y z//position, objet au sol
		o x y z//orientation
		}
		Objet{
		c \FichierObjet
		m x y z//position, objet au mur
		o x y z//orientation
		}
		};
		...
		*/
		if (fichier.is_open()){

			char* ligne;

			while (!fichier.eof()){
				fichier.getline(ligne, 256);
				if (ligne == "Puzzle{"){
					double x, y, z;
					fichier >> x >> y >> z;
					Vecteur3<double> dimension(x, y, z);
					Vecteur3<double> *position, *orientation;
					char* cheminObjet;
					fichier.getline(ligne, 256);
					while (ligne != "};"){
						if (ligne == "Objet{"){
							fichier >> ligne >> cheminObjet;
							fichier >> ligne >> x >> y >> z;
							position = new Vecteur3<double>(x, y, z);
							if (ligne == "m"){
								//� coder
								fichier >> ligne >> x >> y >> z;
								orientation = new Vecteur3<double>(x, y, z);
							}
							else if (ligne == "s"){
								//� coder
								fichier >> ligne >> x >> y >> z;
								orientation = new Vecteur3<double>(x, y, z);
							}
						}
						fichier.getline(ligne, 256);
					}
				}
			}

			fichier.close();
		}

		else
			return false;
	}
}