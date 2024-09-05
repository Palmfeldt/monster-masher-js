#include "settings.inc"

#include "hero.inc"

object {
   hero
   
   texture {
      normal { bumps 1.5 }
      pigment { color rgb <0.6, 0.2, 0.6> * 1.2 }
      scale 1.5
   }
   
   rotate <0, 0, -hero_angle>
   scale 0.9
   //rotate <90, 90, 0>
}
