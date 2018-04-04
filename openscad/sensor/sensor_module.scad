
//sensor_module();

mount_hole_radius = 1.5;
function get_mh1_offset() = [0.7+mount_hole_radius, 0.85+mount_hole_radius,0];
function get_mh2_offset() = [0.75+mount_hole_radius, pcb_width-0.90-mount_hole_radius,0];

function    getPcbDimensions() = [pcb_length, pcb_width, pcb_height];
function    getSensorOffset() = sensor_offset;
function    getSensorDimensions() = [sensor_length, sensor_width, sensor_height];

pcb_length = 24.9;
pcb_width = 20;
pcb_height = 1.5;

sensor_width = 18;
sensor_gap   = 10;
sensor_length = 6;
sensor_height = 15.2;
sensor_offset = [pcb_length-1.3-sensor_length,1,pcb_height];

module sensor_module() 
{    
    // Create the circuit board
    difference()
    {
        color("darkgrey") cube([pcb_length, pcb_width, pcb_height]);
    
        // Create mounting holes
        translate(get_mh1_offset())     cylinder(h=pcb_height, r=mount_hole_radius, $fn=50);
        
        translate(get_mh2_offset())     cylinder(h=pcb_height, r=mount_hole_radius, $fn=50);
        
    }

    // Create sensor
    translate(sensor_offset) difference() 
    {
        color("black")cube([sensor_length,sensor_width,sensor_height]);
        translate([0,(sensor_width-sensor_gap)/2,3]) cube([sensor_length, sensor_gap, sensor_height]);
    }

    // Create the schmitt trigger
    translate([sensor_offset.x-5,pcb_width/2,0.75+pcb_height]) color("grey") cube([6.20, 5, 1.5], center=true);
    
}