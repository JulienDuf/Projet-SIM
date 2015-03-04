#pragma once

class Porte : public Contenu{
private:
	unsigned int ID;
	bool sortable;
	bool verrouillee;
public:
	Porte(Vecteur3<double> positionDepart, unsigned int identifiant, bool sortie, bool verrou){
		//Le modele de la porte est assign�e ici, � faire quand tous les trucs de graphiques requis seront faits
		position = Vecteur<double>(positionDepart.x, positionDepart.y, positionDepart.z);
		ID = Identifiant;
		sortable = sortie;
		verrouillee = verrou;
	}

	unsigned int obtID(){
		return ID;
	}
	bool obtSortable(){
		return sortable;
	}
	void defSortable(bool sortie){
		sortable = sortie;
	}
	bool obtVerrou(){
		return verrouillee;
	}
	void defVerrou(bool verrou){
		verrouillee = verrou;
	}
};