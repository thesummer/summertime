module audiojack()
{
    BOX_WIDTH=12;
    BOX_HEIGTH=5;

    JACK_OUTER_D=6;
    JACK_INNER_D=3.6;
    JACK_HEIGHT=3;

    union() {
        color("black") cube([BOX_WIDTH,BOX_WIDTH,BOX_HEIGTH], center=true);
        
        translate([-(JACK_HEIGHT+BOX_WIDTH)/2+1,0,0]) rotate([0,90,0]) color("grey")
        difference() {
            cylinder(h=JACK_HEIGHT,d=JACK_OUTER_D,center=true, $fn=100);
            cylinder(h=JACK_HEIGHT+1,d=JACK_INNER_D,center=true, $fn=75);
        }
    }
}

audiojack();