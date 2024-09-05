#include "metals.inc"

#include "settings.inc"
#include "monster-blob.inc"

union {
   object {
      monster_blob
      texture {
	 pigment { color rgb 0.3 }
	 finish { F_MetalA }
      }
   }

   // armour
   union {
      #declare belt =
      difference {
	 cylinder {
	    -x, x, 63
	    scale <12, 1, 1>
	 }
	 #declare i = 0;
	 #while (i < 360)
	    box {
	       -1, 1
	       scale <8, 1, 1>
	       rotate x * 45
	       translate y * 63
	       rotate x * i
	    }
	    #declare i = i + 5;
	 #end
      }
      
      object { belt }
      object { belt rotate y * 90}
      object { belt rotate z * 90}
      
      texture { T_Chrome_4B }
   }
   
   rotate (z + y) * 360 * clock
}
