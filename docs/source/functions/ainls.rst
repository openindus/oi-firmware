.. _ainls_s:

Analog Input Low Signal
============================

Description
-----------

The redaction of this page is under progress.

.. Le composant ADS114S08 offre une résolution de 16 bits. Cela correspond à 76 µV (PGA actif, gain = 1), 0,6 µV (PGA actif, gain = 128).
.. Le composant ADS114S08 dispose de 10 entrées analogiques soit 5 entrées analogiques différentielles.
.. Le composant ADS114S08 dispose générateur de courant afin de pouvoir exciter les capteurs passifs tels que les RTD 2 ou 3 fils.
.. Dans le cadre de la mesure de capteur tel que des RTDs, une source de courant constant est nécessaire. 
.. Afin de limiter les erreurs d’acquisition, le courant d’excitation traversera une résistance de précision, la tension aux bornes de cette résistance de précision servira de tension de référence à l’étage de numérisation de l’ADC. On parle de mesure RATIOMETRIQUE.
.. Dans le cas de mesure de paire différentielle, la plage de tension maximale est +/- 2,5 V. Si le PGA intégré dans l’ADC est activé (gain de 1 à 128), la plage de tension maximale peut descendre jusqu’à +/- 20 mV. La résolution restant la même, l’utilisation du PGA dans le cas de très faibles tensions, augmente la précision.
.. Par rapport à la masse, chaque entrée analogique doit être comprise entre -50 mV et 5,05V lorsque le PGA est désactivé. 
.. Lorsque le gain du PGA est de 1 à 16, la plage de tension doit être de 150 mV + Vdiff,max * (Gain – 1) / 2 à 4,85 V – Vdiff,max * (Gain – 1) / 2. 
.. Lorsque le gain du PGA est de 32 à 128, la plage de tension doit être de 150 mV + 15,5 * Vdiff,max à 4,85 V – 15,5 * Vdiff,max.

.. La carte AnalogLS intègre une matrice dont le but est de réalisé les connexions dans les multiplexeurs d’excitations et le bornier capteur.
.. Ces liaisons sont physiques sur la carte et ne sont pas être pilotées.
.. La matrice doit être optimisée afin de rendre la lecture simultanée :
.. -	Soit de 5 thermocouples,
.. -	Soit de 5 RTDs 2 fils,
.. -	Soit de 3 RTDs 3 fils,
.. -	Soit de 4 jauges de contrainte excitées en tension,
.. -	Soit de 2 jauges de contraintes excitées en courant.
.. La lecture de capteurs de type différents doit être possible dans la limite où l’affectation des capteurs sur le bornier est compatible avec la matrice existante.
.. Voir en page suivante le détail de la matrice.


Characteristics
---------------

Code examples
-------------

Software API
------------