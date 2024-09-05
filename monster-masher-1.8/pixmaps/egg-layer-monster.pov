#include "metals.inc"

#include "settings.inc"
#include "monster-blob.inc"

object {
   monster_blob
   texture {
      pigment {
	 bozo
	 color_map {
	    [ 0.4  color rgb <0.30, 0.30, 0.70> ]
	    [ 0.7  color rgb <0.30, 0.30, 0.70> * 1.4 ]
	 }
	 scale 15
      }
      finish { F_MetalA }
   }

   scale 1.1
   
   rotate (z + y) * 360 * clock
}
