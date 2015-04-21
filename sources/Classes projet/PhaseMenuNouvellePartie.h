#pragma once
#include "MenuNouvellePartie.h"
#include "Phase.h"

class PhaseMenuNouvellePartie : public Phase {

private:

	MenuNouvellePartie* menuNouvellePartie;


public:

	PhaseMenuNouvellePartie() : Phase() {

		menuNouvellePartie = new MenuNouvellePartie();

	}

	~PhaseMenuNouvellePartie() {

		delete menuNouvellePartie;

	}

	void rafraichir(float frameTime){

		menuNouvellePartie->actualiser();

	}

	void remplir() {

	}
};