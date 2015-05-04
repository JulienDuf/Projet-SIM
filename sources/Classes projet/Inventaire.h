#pragma once 
#include <vector>

#include "Case.h"
#include "Vecteur2.h"
class Inventaire{
private:
	std::vector<Case*> sacADos;
	std::vector<Case*> menuAccesRapide;
	int itemSelectionne;
public:
	Inventaire(Vecteur2f taille){
		for (int i = 0; i < taille.x * taille.y; ++i){
			if (i < taille.x)
				menuAccesRapide.push_back(new Case());
			sacADos.push_back(new Case());
		}
		itemSelectionne = 0;
	}
	~Inventaire(){
		while (sacADos.size() > 0){
			delete sacADos.back();
			sacADos.pop_back();
		}
		while (menuAccesRapide.size() > 0){
			delete menuAccesRapide.back();
			menuAccesRapide.pop_back();
		}
	}

	int obtTailleSacADos(){
		return sacADos.size();
	}


	int obtTailleAccesRapide(){
		return menuAccesRapide.size();
	}

	bool ajouterObjet(Item *objet){
		//V�rifions si il reste de la place dans le menu d'acc�s rapide
		for (Case* i : menuAccesRapide){
			if (i->obtObjet() == nullptr){
				i->defObjet(objet);
				return true;
			}
		}
		//Sinon, regardons pour le sac � dos.
		for (Case* i : sacADos){
			if (i->obtObjet() == nullptr){
				i->defObjet(objet);
				return true;
			}
		}
		return false;
	}

	Item* ajouterObjetCaseSacADos(Item *objet, int position){
		if (position >= sacADos.size())
			return nullptr;
		Item* tmp = sacADos[position]->retirerObjet();
		sacADos[position]->defObjet(objet);
		return tmp;
	}

	Item* ajouterObjetCaseAccesRapide(Item *objet, int position){
		if (position >= menuAccesRapide.size())
			return nullptr;
		Item* tmp = menuAccesRapide[position]->retirerObjet();
		menuAccesRapide[position]->defObjet(objet);
		return tmp;
	}

	Item* retirerObjetSacADos(int position){
		return sacADos[position]->retirerObjet();
	}

	Item* retirerObjetAccesRapide(int position){
		return menuAccesRapide[position]->retirerObjet();
	}

	Item* obtObjetSacADos(int position){
		return sacADos[position]->obtObjet();
	}

	Item* obtObjetAccesRapide(int position){
		return menuAccesRapide[position]->obtObjet();
	}

	void defItemSelectionne(int index){
		itemSelectionne = index;
	}

	int obtItemSelectionne(){
		return itemSelectionne;
	}
};