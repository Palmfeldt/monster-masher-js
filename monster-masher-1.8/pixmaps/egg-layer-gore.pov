#include "metals.inc"

#include "settings.inc"
#include "gore-blob.inc"

object {
   gore_blob
   
   texture {
      pigment {
	 bozo
	 color_map {
	    [ 0.4  color rgbt <0.30, 0.30, 0.70, 0.6> ]
	    [ 0.7  color rgbt <0.30 * 1.4, 0.30 * 1.4, 0.70 * 1.4, 0.6> ]
	 }
	 scale 15
      }
      finish { F_MetalC }
   }
}
