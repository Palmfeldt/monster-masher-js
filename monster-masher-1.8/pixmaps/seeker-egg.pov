#include "egg.inc"
#include "metals.inc"

object {
   egg_shape
   
   texture {
      average
      texture_map {
	 [1 egg_texture]
	 [1 pigment { color rgb <0.70, 0.30, 0.30> } finish { F_MetalA } ]
      }
   }
}
