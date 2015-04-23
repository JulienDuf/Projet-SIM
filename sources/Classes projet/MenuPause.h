#pragma once
#include "Menu.h"
#include "Sprite2D.h"
#include "PhaseMenuOptions.h"
#include "Bouton.h"
class MenuPause : public Menu {
private:
	Bouton* options;
	Bouton* quitter;

public:
	MenuPause(void) : Menu() {
		this->spriteFond = new gfx::Sprite2D(Vecteur2f(0, 0), &gfx::GestionnaireRessources::obtInstance().obtTexture("fondMenu.png"));
		
		options = new Bouton(std::bind(&MenuPause::enClicOptions, this, std::placeholders::_1),
			std::bind(&MenuPause::survol, this, std::placeholders::_1),
			std::bind(&MenuPause::defaut, this, std::placeholders::_1),
			Vecteur2f(600, 500),
			"Options",
			50);

		quitter = new Bouton(std::bind(&MenuPause::enClicQuitter, this, std::placeholders::_1),
			std::bind(&MenuPause::survol, this, std::placeholders::_1),
			std::bind(&MenuPause::defaut, this, std::placeholders::_1),
			Vecteur2f(450, 200),
			"Quit",
			50);

		this->retour = new Bouton(std::bind(&MenuPause::enClicRetour, this, std::placeholders::_1),
			std::bind(&MenuPause::survol, this, std::placeholders::_1),
			std::bind(&MenuPause::defaut, this, std::placeholders::_1),
			Vecteur2f(300, 500),
			"Back",
			50);
		defPause(true);
	}

	~MenuPause(){
		gfx::Gestionnaire2D::obtInstance().retObjets({ this->spriteFond });
		delete this->spriteFond;
	}

	void survol(Bouton* sender){
		sender->defCouleur({ 255, 0, 0, 255 });
	}

	void defaut(Bouton* sender){
		sender->defCouleur({ 0, 0, 0, 255 });
	}

	void enClicRetour(Bouton* sender){
		gfx::Gestionnaire2D::obtInstance().vider();
		GestionnairePhases::obtInstance().defPhaseActive(1);
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_ShowCursor(SDL_DISABLE);
		gfx::Gestionnaire3D::obtInstance().defFrustum(45, 800.0 / 600.0, 0.99, 1000);
		gfx::Gestionnaire3D::obtInstance().obtCamera()->defPause(false);;
	}

	void enClicOptions(Bouton* sender){
		gfx::Gestionnaire2D::obtInstance().vider();
		GestionnairePhases::obtInstance().defPhaseActive(5);
	}

	void enClicQuitter(Bouton* sender){
		GestionnairePhases::obtInstance().retirerPhase();
	}

	void remplir() {
		gfx::Gestionnaire2D::obtInstance().ajouterObjet(spriteFond);
		options->remplir();
		retour->remplir();
		quitter->remplir();
	}
	void defPause(bool pause) {
		if (pause) {
			this->pause = pause;
			quitter->defEtat(PAUSE);
			options->defEtat(PAUSE);
			retour->defEtat(PAUSE);
		}
		else {
			this->pause = pause;
			quitter->defEtat(DEFAUT);
			options->defEtat(DEFAUT);
			retour->defEtat(DEFAUT);
		}
	}
};