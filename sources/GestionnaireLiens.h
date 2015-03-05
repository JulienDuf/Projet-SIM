#pragma once
#include <map>
#include <tuple>
#include <list>
#include "Contenu.h"
#include "Porte.h"
#include "Salle.h"
#include "Fabrique.h"
class GestionnaireLens : public Singleton<GestionnaireLens>{
private:
	std::map<std::tuple<unsigned int, unsigned int, bool>, std::tuple<unsigned int, unsigned int>> connections;
	std::list<Salle> salles;
	std::list<Porte> portes;
public:
	GestionnaireLens(){}
	~GestionnaireLens(){}

	void ajouter(std::tuple<unsigned int, unsigned int, bool> entree, std::tuple<unsigned int, unsigned int> sortie){
		connections[entree] = sortie;
	}
	std::tuple<unsigned int, unsigned int> destination(std::tuple<unsigned int, unsigned int, bool> sortie){
		return connections.find(sortie)->second;
	}
	Porte obtPorte(unsigned int _ID){
		for (auto it : portes){
			if (it.obtID() == _ID)
				return it;
		}
	}
	Salle obtSalle(unsigned int _ID){
		for (auto it : salles){
			if (it.obtID() == _ID)
				return it;
		}
	}
};