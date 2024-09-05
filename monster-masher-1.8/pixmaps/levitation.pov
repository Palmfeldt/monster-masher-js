#include "settings.inc"

#include "colors.inc"
#include "stones.inc"

#declare rs = seed(sval);

superellipsoid {
   <0.4, 0.4>
   
   /*texture {
      T_Stone44
   }*/
   texture {
      pigment {
	 crackle
	 turbulence 0.2
	 color_map {
	    [0.0    color rgbt <0.3, 0.3, 1.0, 0.2>]
	    [0.025  color rgbt <0.0, 0.0, 0.0, 1.0>]
	 }
	 scale 3
	 translate <rand(rs), rand(rs), rand(rs)>
      }
   }
   
   scale 65
}


/*
sphere {
   0  1
   hollow

   pigment { color rgbt 1 }
   interior {
	 media {
         emission color rgb <1, 0.5, 0.5> * 0.03
	    density {
	       spherical
	       turbulence 0.4
	    }
	 }
   }
   
   scale 65
}
*/
