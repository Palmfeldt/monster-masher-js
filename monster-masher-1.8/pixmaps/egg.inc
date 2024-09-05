#include "colors.inc"
#include "stones.inc"

#include "settings.inc"

#declare rs = seed(sval);

#declare crack = 
texture {
   pigment {
      crackle
      turbulence 0.2
      color_map {
	 [0.0    color rgbt <0.0, 0.0, 0.0, 0.0>]
	 [0.025  color rgbt <0.0, 0.0, 0.0, 1.0>]
      }
      scale 3
   }
}

#declare egg_shape =
union {
   difference {
      sphere { 0, 1 }
      plane { y, 0 }
      scale <1.1, 1.7, 1.1>
   }

   difference {
      sphere { 0, 1 }
      plane { -y, 0 }
      scale <1.1, 1.3, 1.1>
   }

   scale 35
   rotate -10
}

#declare egg_texture =
texture { T_Stone29 }   // 7 29 38 40 43 44

#if (clock >= 1/4)
#declare egg_texture =
texture { T_Stone29 }   // 7 29 38 40 43 44
texture { crack  translate <rand(rs), rand(rs), rand(rs)> scale 35}
#end
#if (clock >= 2/4)
#declare egg_texture =
texture { T_Stone29 }   // 7 29 38 40 43 44
texture { crack  translate <rand(rs), rand(rs), rand(rs)> scale 35 }
texture { crack  translate <rand(rs), rand(rs), rand(rs)> scale 35 }
#end
#if (clock >= 3/4)
#declare egg_texture =
texture { T_Stone29 }   // 7 29 38 40 43 44
texture { crack  translate <rand(rs), rand(rs), rand(rs)> scale 35 }
texture { crack  translate <rand(rs), rand(rs), rand(rs)> scale 35 }
texture { crack  translate <rand(rs), rand(rs), rand(rs)> scale 35 }
#end
