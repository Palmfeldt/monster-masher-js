#declare monster_blob = blob {
   #declare bs = seed(0);

   threshold 0.2

   sphere {
      0, 32, 1
   }
   
   #declare i = 0;
   #while (i < 80)
      sphere {
	 from_spherical((20 + 20 * rand(bs)), pi * rand(bs), 2 * pi * rand(bs)),
	 30 + 10 * rand(bs), 1
      }
      #declare i = i + 1;
   #end

   scale 0.9
}
