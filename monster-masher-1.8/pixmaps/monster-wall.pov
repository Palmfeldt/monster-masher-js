#include "settings.inc"

#include "colors.inc"
#include "stones.inc"
#include "metals.inc"

#declare monster_texture =
texture {
   pigment { color rgb 0.3 }
   finish { F_MetalA }
}

superellipsoid {
   <0.1, 0.25>
   
   texture {
      average
      texture_map {
	 [1 T_Stone28]
	 [10 monster_texture]
      }
   }

   scale 65
}
