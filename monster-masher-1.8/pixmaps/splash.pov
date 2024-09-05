#include "colors.inc"
#include "stones.inc"


// the various substients
#macro from_spherical(rho, phi, theta)
<rho * sin(phi) * cos(theta), rho * sin(phi) * sin(theta), rho * cos(phi)>
#end

#include "hero.inc"

object {
   hero

   texture {
      normal { bumps 1.5 }
      pigment { color rgb <0.6, 0.2, 0.6> * 1.2 }
      scale 1.5
   }

   rotate <90, -90, 0>
   scale 0.9

   scale 1/150
   translate <1, -0.05, 0.2>
}

#declare Wall =
superellipsoid {
   <0.1, 0.25>

   scale <1, 1.5, 1>
   texture {
      average
      texture_map {
	 [4 T_Stone28]
	 [1 T_Stone44]
      }
   }
   scale 0.5
}

#declare Block =
superellipsoid {
   <0.4, 0.4>

   texture { T_Stone44 }
   scale 0.5
}

#declare rs = seed(0);

#declare LevitationBlock =
superellipsoid {
   <0.4, 0.4>
   
   texture { T_Stone44 }
   #declare i = 0;
   #while (i < 8)
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
	 rotate <rand(rs), rand(rs), rand(rs)> * 360
      }
   }
   #declare i = i + 1;
   #end

   scale 0.5
}

#include "metals.inc"

#include "monster-blob.inc"

#declare Seeker =
union {
   object {
      monster_blob
      texture {
	 pigment { color rgb <0.70, 0.30, 0.30> }
	 finish { F_MetalA }
      }
   }

   // spikes
   union {
      #declare sps = 0;

      #declare i = 0;
      #while (i < 70)
	 #declare rho = 45;
	 #declare phi = pi * rand(sps);
	 #declare theta = 2 * pi * rand(sps);
	 cone {
	    from_spherical(rho, phi, theta), 10 + 5 * rand(sps)
	    from_spherical(rho + 20 + 5 * rand(sps), phi, theta), 0
	 }
	 #declare i = i + 1;
      #end
      texture {
	 T_Chrome_3B
      }
   }

   rotate (z + y) * 360 * clock
   scale 1/140
   translate <0, -0.1, 0>
}

#declare EggLayer =
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
   scale 1/100
   translate <0, -0.1, 0>
}

#declare HardMonster =
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

   rotate (z + y) * 123
   scale 1/130
   translate <0, -0.1, 0>
}

#declare PlainMonster =
object {
   monster_blob

   texture {
      pigment { color rgb 0.3 }
      finish { F_MetalA }
   }


   scale 0.85
   scale 1/130

   rotate (z + y) * 360 * clock
}


// the scene itself as a string
#declare scene_str =
concat(
"WWWWWWWWWWWWWWWWWWWWWW",
"W BB    p     W  W    ",
"W  B    WWWWWWW  W    ",
"W  B  p    p  W  W    ",
"W  B       Bp W  W    ",
"WBB  p  h  B es  W    ",
"W BBBBp    B     W    ",
"W  B       Bh    W    ",
"W  B       BBBBBBW    ",
"W  B  e p        W    ",
"W                WB   ",
"WBBBB    BBBBBBBBh    ",
"W    hp    spp  BhB   ",
"WWWWWWWWBB   Bp BB    ",
"W      B     B  BWWWW ",
"W  WWWWWWWWBBBepB= BB ",
"W     p  p  p  pB=    ",
"W    WWWWWWWWBBBB= p  ",
"W      p    s  s      ",
"W      WWWWWWBBB      ",
"W              Bp     ",
"W              B      ");

#declare scene_width = 22;
#declare scene_height = 22;

#macro equals(s1, s2)
   !strcmp(s1, s2)
#end

#declare i = 0;
#while (i < scene_width * scene_height)
   #local trans = <6, 0, -5> + <-scene_width, 0, scene_height>
   + <mod(i, scene_width), 0, -div(i, scene_width)>;
   #local s = substr(scene_str, i + 1, 1);

   #if (equals(s, "W"))
   object { Wall  translate trans }
   #end
   #if (equals(s, "B"))
   object { Block translate trans }
   #end
   #if (equals(s, "p"))
   object { PlainMonster translate trans }
   #end
   #if (equals(s, "h"))
   object { HardMonster translate trans }
   #end
   #if (equals(s, "s"))
   object { Seeker translate trans }
   #end
   #if (equals(s, "e"))
   object { EggLayer translate trans }
   #end

   #declare i = i + 1;
#end

object { LevitationBlock translate <1, 0, 1.3> }
object { HardMonster translate <1.2, 0, 2.2> }
//plane  { (x + -z)  0 }

#declare RightmostFootprint =
texture {
   pigment { image_map { png "splash-foot.png" once} }
   normal { bump_map { png "splash-foot.png" once}}
   rotate x * 90
   rotate z * 180
   translate x
   scale 0.35
}

#declare LeftmostFootprint =
texture {
   pigment { image_map { png "splash-foot.png" once} }
   normal { bump_map { png "splash-foot.png" once}}
   rotate x * 90
   scale 0.35
}
   
plane  {
   y  (-0.5)
   texture {
      normal { bumps 1.5 }
      pigment { color rgb <0.55, 0.5, 0.5>}
      scale <0.01, 0.005, 0.01>
   }
   texture { LeftmostFootprint  rotate y *  -5 translate <0.7, 0, -0.4> }
   texture { RightmostFootprint rotate y * -40 translate <1.2, 0, -0.7> }
   texture { LeftmostFootprint  rotate y * -45 translate <1.25, 0, -1.2> }
   texture { RightmostFootprint rotate y * -30 translate <1.7, 0, -1.3> }
   //texture { LeftmostFootprint rotate y * -60 translate <1.9, 0, -1.8> }
}

camera {
   up y
//   right x
//   angle 48
   angle 70
   location <3, 3, -3>
   look_at <0, 0.5, 0>
}

light_source { <0.0, 2.0, -2.0> colour rgb 1 fade_distance 3 fade_power 1.3}
light_source { <2.0, 1.0, 2.0> colour rgb 1 fade_distance 3 fade_power 1.3 }
light_source { <-1.5, 0.53, -1.1> colour rgb 0.7 fade_distance 3 fade_power 1.3 }
light_source { <-2.0, 1.0, 1.0> colour rgb 0.3 fade_distance 3 fade_power 1.3 }
light_source { <-1.0, 0.5, 3.0> colour rgb 0.3 fade_distance 3 fade_power 1.3 }

global_settings {
   assumed_gamma 2.2
   max_trace_level 15
}
