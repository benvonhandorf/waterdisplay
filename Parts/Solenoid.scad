module solenoid(insertionClearance=true) {

	//Solenoid dimensions
	tubeDiameter = 6.5;//5.6 measured
	bodyDiameter = 24; //23.5 measured
	lowerRingDiameter = 12;//10.85 measured
//    if(center) {
//        offset = [-bodyDiameter/2, -bodyDiameter/2, -47.5/2];
//    }

    translate([bodyDiameter/2, bodyDiameter/2,0])
    {
          translate([0, -(tubeDiameter/2), 0.0]) cylinder(d=tubeDiameter, h=10.9); //Drain tube
          translate([0, 0, 10.8]) color([.2,.4,0]) cylinder(d=lowerRingDiameter,h=4.4); //Lower Ring
			if(insertionClearance) {
				translate([-(tubeDiameter/2), 0, 10.8]) cube([tubeDiameter, 20.1, 32.5]); //Supply Tube

			} else {
				translate([0, (20/2), 10.8+(tubeDiameter/2)]) rotate([90,0,0]) cylinder(d=tubeDiameter, h=20.1, center=true); //Supply Tube
			}
         translate([0, 0, 10.8+4.3]) cylinder(d=bodyDiameter, h=32.5); //Main body
			translate([0, 0, 10.8+4.3+32.5]) color([0,0,1]) cube([20, 9, 30], center=true); //Clearance for wiring harness
    }
}

solenoid(false);

translate([30,0,0]) color([1.0, 0, 0]) solenoid(true);