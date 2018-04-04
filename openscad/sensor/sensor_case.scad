include <sensor_module.scad>
use <lego_brick_builder.scad>
use <fillets.scad>


LEGO = getFLU(); // Lego base unit

pcb_offset = [0,7.5*LEGO-getPcbDimensions().y/2,2*LEGO] + [3,0,0];
rim_dim = [20*LEGO, 15*LEGO, getPcbDimensions().z+2.3];
rim_offset = [0,0,2*LEGO];

top_dim = [20*LEGO, 15*LEGO, 2];
top_offset = rim_offset + [0,0,rim_dim.z];

screw_diameter = 1.0;
thread_offset = 0.25;

bar_tolerance = 0.2;

$fn=50;

//translate([0,30,0]) bottom();

//translate(pcb_offset+[0,0,0.5]) sensor_module();
bottom();

//translate([0,-5,2]) rotate([180,0,0]) top();

//translate([0,-60,0])
//{
//    bottom();
//    translate(pcb_offset+[0,0,0.5]) sensor_module();
//    translate(top_offset) top();
//}

module bottom()
{
    difference()
    {
        union()
        {
            // Createthe lego base plate
            brick(4,3,1, true);
            // Add knop to hold the pcb in place
            translate(pcb_offset+get_mh1_offset()) cylinder(h=  getPcbDimensions().z+1, d=2.8, $fn=50);
            translate(pcb_offset+get_mh2_offset()) cylinder(h=getPcbDimensions().z+1, d=2.8, $fn=50);
            // Add rim
            translate(rim_offset) difference()
            {
                cube(rim_dim);
                translate([1,1,0]) cube(rim_dim-[2,2,-1]);
            }
            // Add screw corner: bottom right
            translate([1,1,LEGO*2]) difference() 
            {
                intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=4.2, h=rim_dim[2]);
                }
            }    
            // Add screw corner: bottom left
            translate([1,15*LEGO-1,LEGO*2]) difference() 
            {
                rotate([0,0,-90]) intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=4.2, h=rim_dim[2]);
                }
            }
            // Add screw corner: top right
            translate([20*LEGO-1,1,LEGO*2]) difference() 
            {
                rotate([0,0,90]) intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=4.2, h=rim_dim[2]);
                }
            }
            // Add screw corner: top left
            translate([20*LEGO-1,15*LEGO-1,LEGO*2]) difference() 
            {
                rotate([0,0,180]) intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=4.2, h=rim_dim[2]);
                }
            }

        }
        // Create hole in screw corner
        translate([1.5,1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim[2]);
        translate([20*LEGO-1.5,1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim[2]);
        translate([20*LEGO-1.5,15*LEGO-1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim[2]);
        translate([1.5,15*LEGO-1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim.z);
    }

}

module top()
{
    // Create top
    union()
    {
        difference()
        {
            cube(top_dim);
            // Create hole in screw corner
            translate([1.5,1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            translate([20*LEGO-1.5,1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            translate([20*LEGO-1.5,15*LEGO-1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            translate([1.5,15*LEGO-1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            // Create hole for sensor
            tolerance = 1;
            sensor_offset = getSensorOffset()+pcb_offset-[tolerance/2,tolerance/2,5];
            translate(sensor_offset) cube(getSensorDimensions()+[tolerance, tolerance,0]);
        }
        // Add bars
        translate([top_dim.x/4,1+bar_tolerance,-1]) cube([9, 1, 1]);
        translate([top_dim.x/4,top_dim.y-2-bar_tolerance,-1]) cube([9, 1, 1]);
        translate([top_dim.x-2-bar_tolerance,top_dim.y/2-4.5,-1]) cube([1, 9, 1]);
        translate([1+bar_tolerance,top_dim.y/2-4.5,-1]) cube([1, 9, 1]);
    }
}
