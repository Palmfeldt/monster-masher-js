#include "metals.inc"

#include "settings.inc"
#include "gore-blob.inc"

// from shapes2.inc
#declare Dodecahedron = 
 intersection 
  {plane {-z, 1 rotate <-26.56505117708,    0, 0>}
   plane {-z, 1 rotate <-26.56505117708,  -72, 0>}
   plane {-z, 1 rotate <-26.56505117708, -144, 0>}
   plane {-z, 1 rotate <-26.56505117708, -216, 0>}
   plane {-z, 1 rotate <-26.56505117708, -288, 0>}
   
   plane {-z, 1 rotate <26.56505117708,  -36, 0>}
   plane {-z, 1 rotate <26.56505117708, -108, 0>}
   plane {-z, 1 rotate <26.56505117708, -180, 0>}
   plane {-z, 1 rotate <26.56505117708, -252, 0>}
   plane {-z, 1 rotate <26.56505117708, -324, 0>}
   
   plane { y, 1}
   plane {-y, 1}
   
   bounded_by {sphere {0, 1.2585}}
  }
   


#declare rs = seed(sval + 1);

union {
   object {
      gore_blob
      texture {
	 pigment { color rgbt <0.3, 0.3, 0.3, 0.5> }
	 finish { F_MetalC }
      }
   }

   // shrapnels
   #declare i = 0;
   #while (i < 20)
      object {
	 Dodecahedron
	 texture { T_Chrome_5B }

	 #declare r = 10 + 55 * rand(rs);
	 #declare theta = 2 * pi * rand(rs);
	 scale <rand(rs) * 15 + 3, rand(rs) * 15 + 3, rand(rs) * 15 + 3>
	 translate <r * cos(theta), r * sin(theta), 5 * (1 - 2 * rand(rs))>
	 
      }
      #declare i = i + 1;
   #end
}
