#include "settings.inc"

#include "hero.inc"

object {
   hero
   
   material {
      texture {
	 pigment { color rgbt <0.6, 0.2, 0.6, 0.8> * 1.2 }
	 /*
	 finish {
	    specular 1
	    diffuse 0.7
	    roughness 0.001
	    reflection 0.1
	 }*/
	 normal { bumps 1.5 }
	 scale 1.5
      }
      interior { ior 1.1 }
   }
   
   rotate <0, 0, -hero_angle>
   scale 0.9
   //rotate <90, 90, 0>
}
