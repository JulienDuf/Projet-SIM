#pragma once

#include "Singleton.h"
#include "Vecteur3.h"
#include "Chrono.h"

class Physique : public Singleton<Physique>{
private:

	double gravite;
	double constanteDeFriction;
	double champsMagnetique;
	double sensibiliteMagnetique;
	Chrono temps;

	void trouverPointFace(unsigned int numeroFace, Vecteur3d& point1, Vecteur3d& point2, Vecteur3d& point3, float* tabVertices) {

        	for (int j = 0; j < 3; j++) {
           		switch (j) {

            	case 0:
                	point1 = { tabVertices[(j + numeroFace)  3], tabVertices[(j + numeroFace) * 3 + 1], tabVertices[(j + numeroFace) * 3 + 2] };
                	break;

            	case 1:
                	point2 = { tabVertices[(j + numeroFace) * 3], tabVertices[(j + numeroFace) * 3 + 1], tabVertices[(j + numeroFace) * 3 + 2] };
                	break;

           	 case 2:
                	point3 = { tabVertices[(j + numeroFace) * 3], tabVertices[(j + numeroFace) * 3 + 1], tabVertices[(j + numeroFace) * 3 + 2] };
               		break;
            		}
        	}
    	}

    	bool collisionDroiteModele(gfx::Modele& modele, Droite& rayonCollision, Vecteur3d& pointCollision) {

        	double resultat1, resultat2, resultat3;
        	Vecteur3d point1;
       		Vecteur3d point2;
        	Vecteur3d point3;

        	for (unsigned int Nbrface = 0; Nbrface <  modele.obtNbrFaces(); Nbrface++) {

           		trouverPointFace(Nbrface, point1, point2, point3, modele.obtVertices());
            		plan.calculerPlan(point1, point2, point3);
           		pointCollision = plan.insertionDroitePlan(rayonCollision);

            		if (pointCollision != NULL) {

                		resultat1 = positionPointDroite(point1, point2, pointCollision, plan.obtenirNormale);
                		resultat2 = positionPointDroite(point2, point3, pointCollision, plan.obtenirNormale);
                		resultat3 = positionPointDroite(point3, point1, pointCollision, plan.obtenirNormale);

                		if ((resultat1 == 0 || resultat2 == 0 || resultat3 == 0)|| (resultat1 < 0 && resultat2 < 0 && resultat3 < 0) || (resultat1 > 0 && resultat2 > 0 && resultat3 > 0))
                    			return true;
            		}
        	}
       		return false;
    	}
	
public:

	Physique() {
		temps.partir();
		constanteDeFriction = 0.5;
		gravite = -9.8;
		champsMagnetique = 4;
		sensibiliteMagnetique = 0.0072;
	}

	double obtenirForceNormale(double Masse, Vecteur3d& Vitesse, Vecteur3d Normale) {

		return masse * gravite * SDL_cos(Vitesse->angleEntreVecteurs(Normale) - 90);

	}

	void appliquerGravite(Vecteur3d& vecteurVitesseObjet) {
		vecteurVitesseObjet.y += gravite;
	}

	void AppliquerVent(Vecteur3d vecteurVitesseVent, float tableauNormales[], unsigned int nombreFace, float tableauVertices[], Vecteur3d& vecteurVitesseObjet, double& masseObjet) {

		double accelerationSelonForceVent = 0.5 * 1.204 * pow(vecteurVitesseVent.norme(), 2);

		double coefficientTrainer = 0;
		double surface = 0;

		nombreFace *= 3;

		unsigned int nombreFaceSousPression = 0;

		// Boucle sur toutes les faces de l'objet
		for (unsigned int parcoursFace = 0; parcoursFace < nombreFace;) {

			// Vecteur normal de la face selon une moyenne de ceux des vertices.
			Vecteur3d vecteurNormale = { (tableauNormales[parcoursFace] + tableauNormales[parcoursFace + 3] + tableauNormales[parcoursFace + 6]) / 3,
				(tableauNormales[parcoursFace + 1] + tableauNormales[parcoursFace + 4] + tableauNormales[parcoursFace + 7]) / 3,
				(tableauNormales[parcoursFace + 2] + tableauNormales[parcoursFace + 5] + tableauNormales[parcoursFace + 8]) / 3 };

			double angleEntreVecteursVentNormale = (vecteurVitesseVent.produitScalaire(vecteurNormale)) / (vecteurVitesseVent.norme() * vecteurNormale.norme());

			// Si l'angle entre le vecteur normal et le vecteur du vent est n�gatif
			if (angleEntreVecteursVentNormale < 0) {

				coefficientTrainer += abs(angleEntreVecteursVentNormale);
				// Calcul de la surface selon la formule d'h�ron: sqrt(s(s-a)(s-b)(s-c))

				// a : Point 1 � point 2;
				double a = sqrt(pow(tableauVertices[parcoursFace] - tableauVertices[parcoursFace + 3], 2) +
					pow(tableauVertices[parcoursFace + 1] - tableauVertices[parcoursFace + 4], 2) +
					pow(tableauVertices[parcoursFace + 2] - tableauVertices[parcoursFace + 5], 2));

				// a : Point 1 � point 3;
				double b = sqrt(pow(tableauVertices[parcoursFace] - tableauVertices[parcoursFace + 6], 2) +
					pow(tableauVertices[parcoursFace + 1] - tableauVertices[parcoursFace + 7], 2) +
					pow(tableauVertices[parcoursFace + 2] - tableauVertices[parcoursFace + 8], 2));

				// a : Point 2 � point 3;
				double c = sqrt(pow(tableauVertices[parcoursFace + 3] - tableauVertices[parcoursFace + 6], 2) +
					pow(tableauVertices[parcoursFace + 4] - tableauVertices[parcoursFace + 7], 2) +
					pow(tableauVertices[parcoursFace + 5] - tableauVertices[parcoursFace + 8], 2));

				// Demi-p�rim�tre;
				double s = 0.5 * (a + b + c);

				surface += sqrt(s*(s - a)*(s - b)*(s - c));

				nombreFaceSousPression++;

			}
			parcoursFace += 9; // Prochaine face(3vertice*xyz)
		}

		coefficientTrainer /= nombreFaceSousPression;

		// Fin du calcul...
		accelerationSelonForceVent *= (coefficientTrainer * surface / masseObjet);

		// Mise en proportion pour l'addition...
		vecteurVitesseVent.normaliser();

		// N�c�ssite l'ajout d'un division par le temps...
		Vecteur3d vecteurVitesseAppliquer = { accelerationSelonForceVent * vecteurVitesseVent.x, accelerationSelonForceVent * vecteurVitesseVent.y, accelerationSelonForceVent * vecteurVitesseVent.z };

		vecteurVitesseObjet += vecteurVitesseAppliquer;
	}
	
	void appliquerFrottement(Objet& objet) {
		objet.vecteurVitesse -= constanteDeFriction * obtenirForceNormale(objet.masse, objet.vecteurPosition);
	}

	// Proc�dure qui applique la force d'attraction magn�tique sur un objet
	// (La force du champs et la sensibilit� magn�tique de l'objet sont constant).
	void appliquerMagnetisme(double& masseObjet, Vecteur3d& positionObjet, Vecteur3d& vecteurVitesseObjet, Vecteur3d& positionAimant) {

	double distanceObjetAimant = distanceEntreDeuxPoints(positionAimant, positionObjet);
	double accelerationMagnetique = (6 * sensibiliteMagnetique * champsMagnetique) / (masseObjet * distanceObjetAimant);

	Vecteur3d vecteurProportionnel = { positionAimant.x - positionObjet.x, positionAimant.y - positionObjet.y, positionAimant.z - positionObjet.z };
	vecteurProportionnel.normaliser();

	Vecteur3d vecteurAcceleration = { accelerationMagnetique, accelerationMagnetique, accelerationMagnetique };

	vecteurAcceleration.prodruitParUnVecteur(vecteurProportionnel);

	vecteurVitesseObjet += vecteurAcceleration;

	}
	
	double obtenirAnglePenduleSimple(double angleMaximal, double omega) {
		return angleMaximal * SDL_cos(omega * temps.obtenirTemps());
	}

	double distanceEntreDeuxPoints(Vecteur3d& point1, Vecteur3d& point2) {
		return SDL_sqrt(SDL_pow((point2.x - point1.x), 2) + SDL_pow((point2.y - point1.y), 2) + SDL_pow((point2.z - point1.z), 2));
	}

	double positionPointDroite(Vecteur3d& droite1, Vecteur3d& droite2, Vecteur3d& point, Vecteur3D& normale) {
		
		normale.normaliser();

		if (normale.x > normale.y && normale.x > normale.z) {
			
			return (droite2.y - droite1.y) * (point.z - droite1.z) - (droite2.z - droite1.z) * (point.y - droite1.y);
		}
		
		if (normale.y > normale.x && normale.y > normale.z) {

			return (droite2.x - droite1.x) * (point.z - droite1.z) - (droite2.z - droite1.z) * (point.x - droite1.x);
		}

		if (normale.z > normale.x && normale.z > normale.y) {
			
			return (droite2.x - droite1.x) * (point.y - droite1.y) - (droite2.y - droite1.y) * (point.x - droite1.x);
		}	
	}

	double obtenirEnergieCinetique(double masse, Vecteur3d& vecteurVitesseObjet) {
		return 0.5 * masse * SDL_pow(vecteurVitesseObjet.norme(), 2);
	}	
};
