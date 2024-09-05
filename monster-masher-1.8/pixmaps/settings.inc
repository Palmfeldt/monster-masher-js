camera {
   orthographic
   up y
   right x
   angle 48
   location <0, 0, -150>
   look_at 0
}

light_source { <100.0, 300.0, -200.0> colour rgb 1 }
light_source { <200.0, -150.0, -200.0> colour rgb 1 }
light_source { <-200.0, 80.0, -200.0> colour rgb 1 }
light_source { <-100.0, -200.0, -200.0> colour rgb 1 }

global_settings {
   assumed_gamma 2.2
   max_trace_level 15
}

#macro from_spherical(rho, phi, theta)
<rho * sin(phi) * cos(theta), rho * sin(phi) * sin(theta), rho * cos(phi)>
#end

