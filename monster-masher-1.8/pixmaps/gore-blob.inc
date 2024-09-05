#declare gore_blob =
blob {
   #declare gbs = seed(sval);
   
   threshold 0.2

   #declare i = 0;
   #while (i < 40)
      #declare r = 10 + 43 * rand(gbs);
      #declare theta = 2 * pi * rand(gbs);
      sphere {
	 <r * cos(theta), r * sin(theta), 5 * (1 - 2 * rand(gbs))>
	 15 + 15 * rand(gbs), 1

	 scale <1, 1, 0.6>
      }
      #declare i = i + 1;
   #end
}
