#include "metals.inc"

#include "settings.inc"
#include "monster-blob.inc"

object {
   monster_blob

   texture {
      pigment { color rgb 0.3 }
      finish { F_MetalA }
   }

   
   scale 0.85
   
   rotate (z + y) * 360 * clock
}
