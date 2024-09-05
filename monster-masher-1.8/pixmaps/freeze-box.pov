#include "settings.inc"

box {
   -43, 43

   material {
      texture {
	 pigment { rgbt <0.8, 0.8, 1, 0.8> }
	 finish {
	    specular 1
	    diffuse 0.8
	    roughness 0.001
	    reflection 0.1
	 }
      }
      interior { ior 2.47 }
   }
   rotate 30
}
