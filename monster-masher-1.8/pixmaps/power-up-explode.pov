#include "settings.inc"
#include "power-up.inc"

object {
   power_up

   material {
      texture {
	 pigment { rgbt <1, 0.3, 0.3, 0.6> }
	 finish {
	    specular 1
	    diffuse 0.7
	    roughness 0.001
	    reflection 0.1
	 }
      }
      interior { ior 2.47 }
   }
}
