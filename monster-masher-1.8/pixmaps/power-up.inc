#declare power_up =
difference {
   box { -1 1  scale <2, 2, 1> }
   #declare a = 0;
   #while (a < 360)  
   plane { z (-1.1)  rotate <70, 0, a> }
   plane { z (-0.8)  rotate <-35, 0, a> }
   plane { z (-1.0)  rotate <140, 0, a> }
   #declare a = a + 360 / 8;
   #end

   scale 48
   rotate <-90, 45/2, 0>
}
