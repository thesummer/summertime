include <sensor_module.scad>
//use <lego_brick_builder.scad>
use <LEGO.scad>;

//LEGO = getFLU(); // Lego base unit              
//rim_dim = [20*LEGO, 15*LEGO, getPcbDimensions().z+2.3];
lego_height = 3.2;
rim_dim = [31.8, 23.8, 4.5];
pcb_offset = [0,(rim_dim.y-getPcbDimensions().y)/2,lego_height] + [3.5,0,0];

rim_offset = [0,0,lego_height];

top_dim = [rim_dim.x, rim_dim.y, 2];
top_offset = rim_offset + [0,0,rim_dim.z];

screw_diameter = 2.0;
thread_offset = 0.1; //screw_diameter - screw_diameter*0.7;
screw_mount_d = 5;

bar_tolerance = 0.2;

cable_d = 4;

$fn=200;

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
//            brick(4,3,1, true);
            block(
                width=4,
                length=3,
                height=1/3,
                type="tile",
                reinforcement =true
            );

            // Add knop to hold the pcb in place
            translate(pcb_offset+get_mh1_offset()) cylinder(h=  getPcbDimensions().z+1, d=2.8);
            translate(pcb_offset+get_mh2_offset()) cylinder(h=getPcbDimensions().z+1, d=2.8);
            // Add rim
            translate(rim_offset) difference()
            {
                cube(rim_dim);
                translate([1,1,0]) cube(rim_dim-[2,2,-1]);
            }
            // Add screw corner: bottom right
            translate([1,1,lego_height]) difference() 
            {
                intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=screw_mount_d, h=rim_dim[2]);
                }
            }    
            // Add screw corner: bottom left
            translate([1,rim_dim.y-1,lego_height]) difference() 
            {
                rotate([0,0,-90]) intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=screw_mount_d, h=rim_dim[2]);
                }
            }
            // Add screw corner: top right
            translate([rim_dim.x-1,1,lego_height]) difference() 
            {
                rotate([0,0,90]) intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=screw_mount_d, h=rim_dim[2]);
                }
            }
            // Add screw corner: top left
            translate([rim_dim.x-1,rim_dim.y-1,lego_height]) difference() 
            {
                rotate([0,0,180]) intersection()
                {
                    cube([2.5,2.5,rim_dim[2]]);
                    cylinder(d=screw_mount_d, h=rim_dim[2]);
                }
            }

        }
        // Create hole in screw corner
        translate([1.5,1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim[2]);
        translate([rim_dim.x-1.5,1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim[2]);
        translate([rim_dim.x-1.5,rim_dim.y-1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim[2]);
        translate([1.5,rim_dim.y-1.5,rim_offset.z]) cylinder(d=screw_diameter-thread_offset, h=rim_dim.z);
        // Create hole for audio cable
        translate([0,rim_dim.y/2,pcb_offset.z+2]) rotate([0,90,0]) cylinder(h=2, d=cable_d);
    }

}

module top()
{
    // Create top
    union()
    {
        difference()
        {
            color("green") cube(top_dim);
            // Create hole in screw corner
            translate([1.5,1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            translate([rim_dim.x-1.5,1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            translate([rim_dim.x-1.5,rim_dim.y-1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            translate([1.5,rim_dim.y-1.5,0]) cylinder(d=screw_diameter, h=top_dim.z);
            // Create hole for sensor
            tolerance = 1;
            sensor_offset = getSensorOffset()+pcb_offset-[tolerance/2,tolerance/2,5];
            translate(sensor_offset) cube(getSensorDimensions()+[tolerance, tolerance,0]);
        }
        // Add bars
        translate([top_dim.x/4,1+bar_tolerance,-1]) cube([9, 1, 1]);
        translate([top_dim.x/4,top_dim.y-2-bar_tolerance,-1]) cube([9, 1, 1]);
        translate([top_dim.x-2-bar_tolerance,top_dim.y/2-4.5,-1]) cube([1, 9, 1]);
        translate([1+bar_tolerance,top_dim.y/2-8.5,-1]) cube([1, 5, 1]);
        translate([1+bar_tolerance,top_dim.y/2+3.5,-1]) cube([1, 5, 1]);
    }
}
