#declare rounded_sphere = 
blob {
   sphere { 0 1  1 }
   cylinder { 0 <-1, 0, 0>  1  1}
}

#declare hero =
union {
   // the hood
   difference {
      object { rounded_sphere }
      plane { -x  (-0.2) }

      scale <70, 60, 60>
      translate -z * 45
      translate x * 18
   }

   // main part of the cowl
   intersection {
      cylinder {
	 <0, 0, 70> <0, 0, -40>  60
	 scale <0.5, 1, 1>
      }

      union {
	 cylinder {
	    <-100, 0, 0> <100, 0, 0>  60
	    scale <0.5, 1, 0.5>
	 }

	 plane { -z  0 }
	 
	 translate <0, 0, -40 + 60 * 0.5>
      }
   }

   // the sleeves
   union {
      #declare sleeve =
      difference {
	 object { rounded_sphere }
	 plane { -x  0 }
	 scale <50, 30, 30>
	 translate <40, 0, 20>
      }
      object { sleeve translate y * 40 }
      object { sleeve translate y * -40 }
      
   }
}
