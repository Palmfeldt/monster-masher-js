#include "metals.inc"

#include "settings.inc"
#include "monster-blob.inc"

union {
   object {
      monster_blob
      texture {
	 pigment { color rgb <0.70, 0.30, 0.30> }
	 finish { F_MetalA }
      }
   }

   // spikes
   union {
      #declare sps = 0;
      
      #declare i = 0;
      #while (i < 70)
	 #declare rho = 45;
	 #declare phi = pi * rand(sps);
	 #declare theta = 2 * pi * rand(sps);
	 cone {
	    from_spherical(rho, phi, theta), 10 + 5 * rand(sps)
	    from_spherical(rho + 20 + 5 * rand(sps), phi, theta), 0
	 }
	 #declare i = i + 1;
      #end
      texture {
	 T_Chrome_3B
      }
   }
   
   rotate (z + y) * 360 * clock
}
