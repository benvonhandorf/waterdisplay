use <solenoid.scad>

additionalSpacing = 0.8;
solenoidCount = 5;
partialStart = true; //if partial, no spacing will be provided at the ends but an alignment hole set will
partialEnd = true;
alignmentPinDiameter = 3;
alignmentPinDepth = 6;

spacing = 24 + additionalSpacing;

blockLength = (spacing * (solenoidCount + (partialStart ? 0 : 0.5) + (partialEnd ? 0 : 0.5) ) );

difference() {
	union() {
		cube([blockLength, 30, 30]);
	}

	for ( counter = [0 : solenoidCount - 1] ) {
		translate([(partialStart ? additionalSpacing / 2 : 24/2) + (spacing * counter), 2.5, 0])solenoid(true);
	}

	if(partialEnd) {
		//Provide two alignment pins at the end
		translate([blockLength, 27, 27])
			rotate([0,90,0])
				cylinder(d=alignmentPinDiameter, h=alignmentPinDepth, center=true, $fn=10);
		translate([blockLength, 3, 3])
			rotate([0,90,0])
				cylinder(d=alignmentPinDiameter, h=alignmentPinDepth, center=true, $fn=10);
	}

	if(partialStart) {
		//Provide two alignment pins at the beginning
		translate([0, 27, 27])
			rotate([0,90,0])
				cylinder(d=alignmentPinDiameter, h=alignmentPinDepth, center=true, $fn=10);
		translate([0, 3, 3])
			rotate([0,90,0])
				cylinder(d=alignmentPinDiameter, h=alignmentPinDepth, center=true, $fn=10);
	}
}

echo("Total Width: ", spacing * (solenoidCount + 1), "mm");